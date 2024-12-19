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

#include "ul_power_controller.h"
#include "../config/ue_configuration.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ul_power_controller::ul_power_controller(const ue_cell_configuration&    ue_cell_cfg_,
                                         const ue_channel_state_manager& ch_state_manager) :
  ue_cell_cfg(ue_cell_cfg_),
  channel_state_manager(ch_state_manager),
  pusch_sinr_target_dB(ue_cell_cfg_.cell_cfg_common.expert_cfg.ue.target_pusch_sinr),
  ref_path_loss_for_target_sinr(ue_cell_cfg_.cell_cfg_common.expert_cfg.ue.path_loss_for_target_pusch_sinr),
  tpc_adjust_prohibit_time_sl([this]() -> unsigned {
    return tpc_adjust_prohibit_time_ms << to_numerology_value(
               ue_cell_cfg.cell_cfg_common.ul_cfg_common.init_ul_bwp.generic_params.scs);
  }()),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  // Initialize PUSCH PRB grid.
  std::fill(pusch_pw_ctrl_grid.begin(), pusch_pw_ctrl_grid.end(), pusch_pw_ctrl_data{slot_point{}, 0});

  // Dummy casts only needed to prevent Clang from complaining about unused variables.
  static_cast<void>(channel_state_manager);
  static_cast<void>(ref_path_loss_for_target_sinr);
  static_cast<void>(pusch_sinr_target_dB);
}

SRSRAN_WEAK_SYMB void ul_power_controller::handle_phr(const cell_ph_report& phr)
{
  latest_phr.emplace(ue_phr_report{phr, std::nullopt});
}

void ul_power_controller::update_pusch_pw_ctrl_state(slot_point slot_rx, unsigned nof_prbs)
{
  const int latest_f_cl_pw_control =
      latest_pusch_pw_control.has_value() ? latest_pusch_pw_control.value().f_cl_pw_control : 0;
  pusch_pw_ctrl_grid[slot_rx.to_uint()] = {slot_rx, nof_prbs, latest_f_cl_pw_control};
}

SRSRAN_WEAK_SYMB unsigned ul_power_controller::adapt_pusch_prbs_to_phr(unsigned nof_prbs) const
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
  return nof_prbs;
}

SRSRAN_WEAK_SYMB uint8_t ul_power_controller::compute_tpc_command(slot_point pusch_slot)
{
  // Dummy function. This feature is only available in the SRSRAN 5G Enterprise version.
  static constexpr uint8_t default_tpc = 1;
  return default_tpc;
}
