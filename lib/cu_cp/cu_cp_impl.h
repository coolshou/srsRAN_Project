/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adapters/cell_meas_manager_adapters.h"
#include "adapters/du_processor_adapters.h"
#include "adapters/e1ap_adapters.h"
#include "adapters/mobility_manager_adapters.h"
#include "adapters/ngap_adapters.h"
#include "adapters/rrc_du_adapters.h"
#include "adapters/rrc_ue_adapters.h"
#include "cu_cp_controller/cu_cp_controller.h"
#include "cu_cp_impl_interface.h"
#include "cu_up_processor/cu_up_processor_repository.h"
#include "du_processor/du_processor_repository.h"
#include "ngap_repository.h"
#include "ue_manager/ue_manager_impl.h"
#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ran/plmn_identity.h"
#include <memory>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

class cu_cp_common_task_scheduler : public common_task_scheduler
{
public:
  cu_cp_common_task_scheduler() : main_ctrl_loop(128) {}

  bool schedule_async_task(async_task<void> task) override { return main_ctrl_loop.schedule(std::move(task)); }

private:
  // cu-cp task event loop
  fifo_async_task_scheduler main_ctrl_loop;
};

class cu_cp_impl final : public cu_cp,
                         public cu_cp_impl_interface,
                         public cu_cp_ng_handler,
                         public cu_cp_command_handler
{
public:
  explicit cu_cp_impl(const cu_cp_configuration& config_);
  ~cu_cp_impl() override;

  bool start() override;
  void stop() override;

  // NGAP interface
  ngap_message_handler* get_ngap_message_handler(const plmn_identity& plmn) override;

  bool amfs_are_connected() override;

  // CU-UP handler
  void handle_bearer_context_inactivity_notification(const cu_cp_inactivity_notification& msg) override;

  // cu_cp_rrc_ue_interface
  rrc_ue_reestablishment_context_response
                   handle_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) override;
  async_task<bool> handle_rrc_reestablishment_context_modification_required(ue_index_t ue_index) override;

  void             handle_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) override;
  void             handle_rrc_reestablishment_complete(ue_index_t old_ue_index) override;
  async_task<bool> handle_ue_context_transfer(ue_index_t ue_index, ue_index_t old_ue_index) override;
  async_task<void> handle_ue_context_release(const cu_cp_ue_context_release_request& request) override;

  // cu_cp_ue_context_manipulation_handler
  async_task<bool> handle_handover_reconfiguration_sent(ue_index_t target_ue_index, uint8_t transaction_id) override;
  void             handle_handover_ue_context_push(ue_index_t source_ue_index, ue_index_t target_ue_index) override;

  // cu_cp_ngap_handler
  bool handle_handover_request(ue_index_t ue_index, security::security_context sec_ctxt) override;
  async_task<expected<ngap_init_context_setup_response, ngap_init_context_setup_failure>>
  handle_new_initial_context_setup_request(const ngap_init_context_setup_request& request) override;
  async_task<cu_cp_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) override;
  async_task<cu_cp_pdu_session_resource_modify_response>
  handle_new_pdu_session_resource_modify_request(const cu_cp_pdu_session_resource_modify_request& request) override;
  async_task<cu_cp_pdu_session_resource_release_response>
  handle_new_pdu_session_resource_release_command(const cu_cp_pdu_session_resource_release_command& command) override;
  async_task<cu_cp_ue_context_release_complete>
  handle_ue_context_release_command(const cu_cp_ue_context_release_command& command) override;
  async_task<ngap_handover_resource_allocation_response>
                   handle_ngap_handover_request(const ngap_handover_request& request) override;
  async_task<bool> handle_new_handover_command(ue_index_t ue_index, byte_buffer command) override;
  ue_index_t       handle_ue_index_allocation_request(const nr_cell_global_id_t& cgi) override;
  void             handle_n2_disconnection() override;

  // cu_cp_measurement_handler
  std::optional<rrc_meas_cfg>
       handle_measurement_config_request(ue_index_t                  ue_index,
                                         nr_cell_identity            nci,
                                         std::optional<rrc_meas_cfg> current_meas_config = {}) override;
  void handle_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results) override;

  // cu_cp_measurement_config_handler
  bool handle_cell_config_update_request(nr_cell_identity nci, const serving_cell_meas_config& serv_cell_cfg) override;

  // cu_cp_mobility_manager_handler
  async_task<cu_cp_inter_du_handover_response>
  handle_inter_du_handover_request(const cu_cp_inter_du_handover_request& request,
                                   du_index_t&                            source_du_index,
                                   du_index_t&                            target_du_index) override;

  // cu_cp_ue_removal_handler
  async_task<void> handle_ue_removal_request(ue_index_t ue_index) override;
  void             handle_pending_ue_task_cancellation(ue_index_t ue_index) override;

  cu_cp_mobility_command_handler& get_mobility_command_handler() override { return *mobility_mng; }
  metrics_handler&                get_metrics_handler() override { return *metrics_hdlr; }

  // cu_cp public interface
  cu_cp_f1c_handler&                     get_f1c_handler() override { return controller->get_f1c_handler(); }
  cu_cp_e1_handler&                      get_e1_handler() override { return controller->get_e1_handler(); }
  cu_cp_e1ap_event_handler&              get_cu_cp_e1ap_handler() override { return *this; }
  cu_cp_ng_handler&                      get_ng_handler() override { return *this; }
  cu_cp_ngap_handler&                    get_cu_cp_ngap_handler() override { return *this; }
  cu_cp_command_handler&                 get_command_handler() override { return *this; }
  cu_cp_rrc_ue_interface&                get_cu_cp_rrc_ue_interface() override { return *this; }
  cu_cp_measurement_handler&             get_cu_cp_measurement_handler() override { return *this; }
  cu_cp_measurement_config_handler&      get_cu_cp_measurement_config_handler() override { return *this; }
  cu_cp_mobility_manager_handler&        get_cu_cp_mobility_manager_handler() override { return *this; }
  cu_cp_ue_removal_handler&              get_cu_cp_ue_removal_handler() override { return *this; }
  cu_cp_ue_context_manipulation_handler& get_cu_cp_ue_context_handler() override { return *this; }

private:
  // Handling of DU events.
  void handle_rrc_ue_creation(ue_index_t ue_index, rrc_ue_interface& rrc_ue) override;

  byte_buffer handle_target_cell_sib1_required(du_index_t du_index, nr_cell_global_id_t cgi) override;

  async_task<void> handle_transaction_info_loss(const f1_ue_transaction_info_loss_event& ev) override;

  // NGAP UE creation handler
  ngap_cu_cp_ue_notifier* handle_new_ngap_ue(ue_index_t ue_index) override;

  // cu_cp_task_scheduler_handler
  bool schedule_ue_task(ue_index_t ue_index, async_task<void> task) override;

  void on_statistics_report_timer_expired();

  cu_cp_configuration cfg;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP");

  // Components
  ue_manager ue_mng;

  std::unique_ptr<mobility_manager> mobility_mng;

  cell_meas_manager cell_meas_mng; // cell measurement manager

  cu_cp_common_task_scheduler common_task_sched;

  // DU repository to Node Manager adapter.
  du_processor_cu_cp_connection_adapter conn_notifier;

  // Cell Measurement Manager to mobility manager adapters
  cell_meas_mobility_manager_adapter cell_meas_ev_notifier;

  // E1AP to CU-CP adapter
  e1ap_cu_cp_adapter e1ap_ev_notifier;

  // NGAP to CU-CP adapters
  ngap_cu_cp_adapter ngap_cu_cp_ev_notifier;

  // Mobility manager to CU-CP adapter
  mobility_manager_adapter mobility_manager_ev_notifier;

  // RRC DU to CU-CP adapters
  rrc_du_cu_cp_adapter rrc_du_cu_cp_notifier;

  // DU connections being managed by the CU-CP.
  du_processor_repository du_db;

  // CU-UP connections being managed by the CU-CP.
  cu_up_processor_repository cu_up_db;

  // Handler of paging messages.
  paging_message_handler paging_handler;

  // AMF connections beeing managed by the CU-CP.
  ngap_repository ngap_db;

  // RRC UE to NGAP adapter
  rrc_ue_ngap_adapter rrc_ue_ngap_notifier;

  // Handler of the CU-CP connections to other remote nodes (e.g. AMF, CU-UPs, DUs).
  std::unique_ptr<cu_cp_controller> controller;

  std::unique_ptr<metrics_handler> metrics_hdlr;

  unique_timer statistics_report_timer;

  std::atomic<bool> stopped{false};
};

} // namespace srs_cu_cp
} // namespace srsran
