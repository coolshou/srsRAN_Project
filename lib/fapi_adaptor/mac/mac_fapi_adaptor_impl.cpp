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

#include "mac_fapi_adaptor_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi_adaptor;

mac_fapi_adaptor_impl::mac_fapi_adaptor_impl(const mac_fapi_adaptor_config&  config,
                                             mac_fapi_adaptor_dependencies&& dependencies) :
  mac_translator(srslog::fetch_basic_logger("FAPI"),
                 *dependencies.gateway,
                 *dependencies.last_msg_notifier,
                 std::move(dependencies.pm_mapper),
                 std::move(dependencies.part2_mapper),
                 config.cell_nof_prbs),
  fapi_data_translator(config.scs, config.sector_id),
  fapi_time_translator(config.scs),
  fapi_error_translator(config.scs)
{
}

fapi::slot_time_message_notifier& mac_fapi_adaptor_impl::get_slot_time_notifier()
{
  return fapi_time_translator;
}

fapi::slot_error_message_notifier& mac_fapi_adaptor_impl::get_slot_error_notifier()
{
  return fapi_error_translator;
}

fapi::slot_data_message_notifier& mac_fapi_adaptor_impl::get_slot_data_notifier()
{
  return fapi_data_translator;
}

mac_cell_result_notifier& mac_fapi_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

void mac_fapi_adaptor_impl::set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  fapi_time_translator.set_cell_slot_handler(mac_slot_handler);
  fapi_error_translator.set_cell_slot_handler(mac_slot_handler);
}

void mac_fapi_adaptor_impl::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  fapi_data_translator.set_cell_rach_handler(mac_rach_handler);
}

void mac_fapi_adaptor_impl::set_cell_pdu_handler(mac_pdu_handler& handler)
{
  fapi_data_translator.set_cell_pdu_handler(handler);
}

void mac_fapi_adaptor_impl::set_cell_crc_handler(mac_cell_control_information_handler& handler)
{
  fapi_data_translator.set_cell_crc_handler(handler);
}
