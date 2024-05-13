/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/ngap/ngap_test_message_validators.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_reestablishment_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_reestablishment_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, create_mock_amf()})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    auto ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));
  }

  /// Run RRC Reestablishment.
  /// \return Returns true if Reestablishment is successful, and false if the gNB fallbacked to RRC Setup.
  bool run_rrc_reestablishment(gnb_du_ue_f1ap_id_t new_du_ue_id, rnti_t new_rnti, rnti_t old_rnti_, pci_t old_pci_)
  {
    // Generate RRC Reestablishment Request.
    byte_buffer rrc_container =
        pack_ul_ccch_msg(create_rrc_reestablishment_request(old_rnti_, old_pci_, "0011000101110000"));

    // Send Initial UL RRC Message to CU-CP.
    f1ap_message f1ap_init_ul_rrc_msg =
        test_helpers::create_init_ul_rrc_message_transfer(new_du_ue_id, new_rnti, {}, std::move(rrc_container));
    get_du(du_idx).push_ul_pdu(f1ap_init_ul_rrc_msg);

    // Wait for DL RRC message transfer
    f1ap_message f1ap_pdu;
    bool         result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu);
    report_fatal_error_if_not(result, "F1AP DL RRC Message Transfer with Msg4 not sent to DU");

    // Check that the received message is a DL RRC Message Transfer with RRC Reestablishment.
    report_fatal_error_if_not(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu), "Invalid Msg4");

    auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
    EXPECT_EQ(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.gnb_du_ue_f1ap_id), new_du_ue_id);

    // It's either RRC setup or Reestablishment.
    return dl_rrc_msg.srb_id == 1;
  }

  void ue_sends_rrc_setup_complete(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    // Generate RRC Setup Complete.
    byte_buffer pdu = pack_ul_dcch_msg(create_rrc_setup_complete());

    // Prepend PDCP header and append MAC.
    EXPECT_TRUE(pdu.prepend(std::array<uint8_t, 2>{0x00U, 0x00U}));
    EXPECT_TRUE(pdu.append(std::array<uint8_t, 4>{}));

    // Send UL RRC Message to CU-CP.
    f1ap_message f1ap_ul_rrc_msg =
        test_helpers::create_ul_rrc_message_transfer(du_ue_id, cu_ue_id, srb_id_t::srb1, std::move(pdu));
    get_du(du_idx).push_ul_pdu(f1ap_ul_rrc_msg);
  }

  void ue_sends_rrc_reest_complete(gnb_du_ue_f1ap_id_t du_ue_id, gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    // Generate RRC Reestablishment Complete.
    byte_buffer pdu = pack_ul_dcch_msg(create_rrc_reestablishment_complete());

    // Prepend PDCP header and append MAC.
    EXPECT_TRUE(pdu.prepend(std::array<uint8_t, 2>{0x00U, 0x00U}));
    EXPECT_TRUE(pdu.append(std::array<uint8_t, 4>{0x01, 0x1d, 0x37, 0x38}));

    // Send UL RRC Message to CU-CP.
    f1ap_message f1ap_ul_rrc_msg =
        test_helpers::create_ul_rrc_message_transfer(du_ue_id, cu_ue_id, srb_id_t::srb1, std::move(pdu));
    get_du(du_idx).push_ul_pdu(f1ap_ul_rrc_msg);
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t old_du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              old_crnti    = to_rnti(0x4601);
  pci_t               old_pci      = 0;
};

TEST_F(cu_cp_reestablishment_test, when_old_ue_does_not_exist_then_reestablishment_fails)
{
  // Connect UE 0x4601.
  EXPECT_TRUE(connect_new_ue(du_idx, old_du_ue_id, old_crnti));

  // Reestablishment Request to RNTI that does not exist.
  ASSERT_FALSE(run_rrc_reestablishment(int_to_gnb_du_ue_f1ap_id(1), to_rnti(0x4602), to_rnti(0x4603), old_pci))
      << "RRC setup should have been sent";

  // UE sends RRC Setup Complete
  this->ue_sends_rrc_setup_complete(int_to_gnb_du_ue_f1ap_id(1), int_to_gnb_cu_ue_f1ap_id(1));

  // old UE should not be removed.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 2);
}

TEST_F(cu_cp_reestablishment_test, when_old_ue_has_no_ngap_context_then_reestablishment_fails_and_old_ue_is_removed)
{
  // Connect UE 0x4601.
  EXPECT_TRUE(connect_new_ue(du_idx, old_du_ue_id, old_crnti));

  ASSERT_FALSE(run_rrc_reestablishment(int_to_gnb_du_ue_f1ap_id(1), to_rnti(0x4602), old_crnti, old_pci))
      << "RRC setup should have been sent";

  // old UE should not be removed at this stage.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 2) << "Old UE should not be removed yet";

  // UE sends RRC Setup Complete
  this->ue_sends_rrc_setup_complete(int_to_gnb_du_ue_f1ap_id(1), int_to_gnb_cu_ue_f1ap_id(1));

  // Given that the old UE still has no AMF-UE-ID, the CU-CP removes the UE context without sending the
  // NGAP UE Context Release Request to the AMF.
  f1ap_message ue_ctxt_rem_msg;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, ue_ctxt_rem_msg))
      << "F1AP UEContextReleaseCommand should have been sent for old UE";
  this->get_du(du_idx).push_ul_pdu(test_helpers::generate_ue_context_release_complete(ue_ctxt_rem_msg));

  // Verify old UE is removed.
  ASSERT_TRUE(this->tick_until(std::chrono::milliseconds{10}, [this, &report]() {
    report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
    return report.ues.size() == 1;
  })) << "Old UE was not successfully removed";
}

TEST_F(cu_cp_reestablishment_test,
       when_old_ue_has_no_drb_then_reestablishment_fails_and_release_request_is_sent_for_old_ue)
{
  // Connect UE 0x4601.
  EXPECT_TRUE(connect_new_ue(du_idx, old_du_ue_id, old_crnti));
  EXPECT_TRUE(authenticate_ue(du_idx, old_du_ue_id, uint_to_amf_ue_id(0)));
  EXPECT_TRUE(setup_ue_security(du_idx, old_du_ue_id));

  // Run Reestablishment.
  gnb_du_ue_f1ap_id_t new_du_ue_id = int_to_gnb_du_ue_f1ap_id(1);
  rnti_t              new_crnti    = to_rnti(0x4602);
  ASSERT_FALSE(reestablish_ue(du_idx, new_du_ue_id, new_crnti, old_crnti, old_pci)) << "Reestablishment failed";

  // STATUS: Given that the old UE has an AMF-UE-ID, the CU-CP should request its release.
  ngap_message ngap_pdu;
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu)) << "NGAP UEContextReleaseRequest should have been sent for old UE";
  ASSERT_TRUE(test_helpers::is_valid_ue_context_release_request(ngap_pdu));
  ASSERT_EQ(ngap_pdu.pdu.init_msg().value.ue_context_release_request()->amf_ue_ngap_id, 0);

  // STATUS: old UE should not be removed at this stage (Still waiting for AMF UE CONTEXT RELEASE COMMAND).
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 2) << "Old UE should not be removed yet";
}

TEST_F(cu_cp_reestablishment_test,
       when_old_ue_has_drb_then_reestablishment_succeeds_and_no_ngap_release_request_is_sent_for_old_ue)
{
  // Attach UE 0x4601.
  EXPECT_TRUE(attach_ue(du_idx, old_du_ue_id, old_crnti, uint_to_amf_ue_id(0)));

  // Send RRC Reestablishment Request and DU receives RRC Reestablishment.
  gnb_du_ue_f1ap_id_t new_du_ue_id = int_to_gnb_du_ue_f1ap_id(1);
  rnti_t              new_crnti    = to_rnti(0x4602);
  ASSERT_TRUE(reestablish_ue(du_idx, new_du_ue_id, new_crnti, old_crnti, old_pci)) << "Reestablishment failed";

  // old UE should not be removed at this stage.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1) << "Old UE should not be removed yet";
}
