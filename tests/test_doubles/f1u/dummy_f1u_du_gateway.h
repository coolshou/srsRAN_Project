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

#pragma once

#include "srsran/f1u/du/f1u_gateway.h"

namespace srsran {
namespace srs_du {

/// \brief Dummy F1-U bearer for the purpose of benchmark.
class f1u_dummy_bearer : public f1u_bearer,
                         public f1u_rx_pdu_handler,
                         public f1u_tx_delivery_handler,
                         public f1u_tx_sdu_handler
{
public:
  f1u_rx_pdu_handler&      get_rx_pdu_handler() override { return *this; }
  f1u_tx_delivery_handler& get_tx_delivery_handler() override { return *this; }
  f1u_tx_sdu_handler&      get_tx_sdu_handler() override { return *this; }
  void                     stop() override {}

  void handle_pdu(nru_dl_message msg) override {}
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override {}
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override {}
  void handle_sdu(byte_buffer_chain sdu) override {}
};

/// \brief Simulator of the CU-UP from the perspective of the DU.
class cu_up_simulator : public f1u_du_gateway
{
public:
  f1u_dummy_bearer                          bearer;
  std::vector<srs_du::f1u_rx_sdu_notifier*> created_du_notifs;
  std::vector<up_transport_layer_info>      registered_dl_tnls;

  optional<uint32_t> last_ue_idx;
  optional<drb_id_t> last_drb_id;

  std::unique_ptr<f1u_bearer> create_du_bearer(uint32_t                       ue_index,
                                               drb_id_t                       drb_id,
                                               srs_du::f1u_config             config,
                                               const up_transport_layer_info& dl_tnl,
                                               const up_transport_layer_info& ul_tnl,
                                               srs_du::f1u_rx_sdu_notifier&   du_rx,
                                               timer_factory                  timers,
                                               task_executor&                 ue_executor) override
  {
    created_du_notifs.push_back(&du_rx);
    registered_dl_tnls.push_back(dl_tnl);
    last_ue_idx = ue_index;
    last_drb_id = drb_id;
    return std::make_unique<f1u_dummy_bearer>();
  }

  void remove_du_bearer(const up_transport_layer_info& dl_tnl) override
  {
    for (unsigned i = 0; i != registered_dl_tnls.size(); ++i) {
      if (dl_tnl == registered_dl_tnls[i]) {
        registered_dl_tnls.erase(registered_dl_tnls.begin() + i);
        created_du_notifs.erase(created_du_notifs.begin() + i);
        break;
      }
    }
  }
};

} // namespace srs_du
} // namespace srsran
