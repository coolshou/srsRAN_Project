/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../ngap_asn1_utils.h"
#include "../ue_context/ngap_ue_context.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

class ngap_pdu_session_resource_release_procedure
{
public:
  ngap_pdu_session_resource_release_procedure(const cu_cp_pdu_session_resource_release_command& command_,
                                              const ngap_ue_ids&                                ue_ids_,
                                              ngap_du_processor_control_notifier& du_processor_ctrl_notif_,
                                              ngap_message_notifier&              amf_notif_,
                                              srslog::basic_logger&               logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "PDU Session Resource Release Procedure"; }

private:
  // results senders
  void send_pdu_session_resource_release_response();

  cu_cp_pdu_session_resource_release_command  command;
  const ngap_ue_ids                           ue_ids;
  cu_cp_pdu_session_resource_release_response response;
  ngap_du_processor_control_notifier&         du_processor_ctrl_notifier;
  ngap_message_notifier&                      amf_notifier;
  srslog::basic_logger&                       logger;
};

} // namespace srs_cu_cp
} // namespace srsran
