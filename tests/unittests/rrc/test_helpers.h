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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/rrc/rrc_du.h"
#include "srsran/rrc/rrc_ue.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"

namespace srsran {
namespace srs_cu_cp {

class dummy_rrc_f1ap_pdu_notifier : public rrc_pdu_f1ap_notifier
{
public:
  dummy_rrc_f1ap_pdu_notifier() = default;

  void on_new_rrc_pdu(const srb_id_t srb_id, const byte_buffer& pdu) override
  {
    last_rrc_pdu = pdu.copy();
    last_srb_id  = srb_id;
  }

  byte_buffer last_rrc_pdu;
  srb_id_t    last_srb_id;
};

class dummy_rrc_ue_ngap_adapter : public rrc_ue_nas_notifier, public rrc_ue_control_notifier
{
public:
  void set_ue_context_release_outcome(bool outcome) { ue_context_release_outcome = outcome; }

  void on_initial_ue_message(const cu_cp_initial_ue_message& msg) override
  {
    logger.info("Received Initial UE Message");
    initial_ue_msg_received = true;
  }

  void on_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) override
  {
    logger.info("Received UL NAS Transport message");
  }

  void on_inter_cu_ho_rrc_recfg_complete_received(const ue_index_t           ue_index,
                                                  const nr_cell_global_id_t& cgi,
                                                  const unsigned             tac) override
  {
    logger.info("ue={}: Received inter CU handover related RRC Reconfiguration Complete", ue_index);
  }

  bool initial_ue_msg_received = false;

private:
  bool                  ue_context_release_outcome = false;
  srslog::basic_logger& logger                     = srslog::fetch_basic_logger("TEST");
};

class dummy_rrc_ue_cu_cp_adapter : public rrc_ue_context_update_notifier, public rrc_ue_measurement_notifier
{
public:
  void add_ue_context(rrc_ue_reestablishment_context_response context) { reest_context = context; }

  bool next_ue_setup_response = true;

  bool on_ue_setup_request() override { return next_ue_setup_response; }

  rrc_ue_reestablishment_context_response on_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti) override
  {
    logger.info("old_pci={} old_c-rnti={}: Received RRC Reestablishment Request", old_pci, old_c_rnti);

    return reest_context;
  }

  async_task<bool> on_rrc_reestablishment_context_modification_required() override
  {
    logger.info("Received Reestablishment Context Modification Required");

    return launch_async([](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  void on_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) override
  {
    logger.info("ue={}: Received RRC Reestablishment failure notification", request.ue_index);
  }

  void on_rrc_reestablishment_complete(ue_index_t old_ue_index) override
  {
    logger.info("ue={}: Received RRC Reestablishment complete notification", old_ue_index);
  }

  async_task<bool> on_ue_transfer_required(ue_index_t old_ue_index) override
  {
    logger.info("Requested a UE context transfer from old_ue={}.", old_ue_index);
    return launch_async([](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(true);
    });
  }

  async_task<void> on_ue_removal_required() override
  {
    logger.info("UE removal requested");
    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) override
  {
    logger.info("ue={}: Requested a UE release", request.ue_index);
    last_cu_cp_ue_context_release_request = request;

    return launch_async([](coro_context<async_task<void>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN();
    });
  }

  optional<rrc_meas_cfg> on_measurement_config_request(nr_cell_id_t           nci,
                                                       optional<rrc_meas_cfg> current_meas_config = {}) override
  {
    optional<rrc_meas_cfg> meas_cfg;
    return meas_cfg;
  }

  void on_measurement_report(const rrc_meas_results& meas_results) override {}

  cu_cp_ue_context_release_request last_cu_cp_ue_context_release_request;

private:
  rrc_ue_reestablishment_context_response reest_context = {};
  srslog::basic_logger&                   logger        = srslog::fetch_basic_logger("TEST");
};

class dummy_rrc_du_cu_cp_adapter : public rrc_du_measurement_config_notifier
{
public:
  bool on_cell_config_update_request(nr_cell_id_t nci, const serving_cell_meas_config& serv_cell_cfg) override
  {
    return true;
  }
};

struct dummy_ue_task_scheduler : public rrc_ue_task_scheduler {
public:
  dummy_ue_task_scheduler(timer_manager& timers_, task_executor& exec_) : timer_db(timers_), exec(exec_) {}
  void           schedule_async_task(async_task<void> task) override { ctrl_loop.schedule(std::move(task)); }
  unique_timer   make_unique_timer() override { return timer_db.create_unique_timer(exec); }
  timer_factory  get_timer_factory() override { return timer_factory{timer_db, exec}; }
  task_executor& get_executor() override { return exec; }

  void tick_timer() { timer_db.tick(); }

private:
  fifo_async_task_scheduler ctrl_loop{16};
  timer_manager&            timer_db;
  task_executor&            exec;
};

} // namespace srs_cu_cp
} // namespace srsran
