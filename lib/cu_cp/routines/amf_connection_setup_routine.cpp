/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_setup_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_setup_routine::amf_connection_setup_routine(ngap_repository&   ngap_db_,
                                                           std::atomic<bool>& amf_connected_) :
  ngap_db(ngap_db_),
  amf_connected(amf_connected_),
  amf_index(ngap_db_.get_ngaps().begin()->first),
  ngap(ngap_db_.get_ngaps().begin()->second),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void amf_connection_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not ngap->handle_amf_tnl_connection_request()) {
    CORO_EARLY_RETURN(false);
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, ngap->handle_ng_setup_request(/*max_setup_retries*/ 1));

  success = std::holds_alternative<ngap_ng_setup_response>(result_msg);

  // Handle result of NG setup.
  handle_connection_setup_result();

  if (success) {
    // Update PLMN lookups in NGAP repository after successful NGSetup.
    ngap_db.update_plmn_lookup(amf_index);

    std::string plmn_list;
    for (const auto& plmn : ngap->get_ngap_context().get_supported_plmns()) {
      plmn_list += plmn.to_string() + " ";
    }

    logger.info("Connected to AMF. Supported PLMNs: {}", plmn_list);
  } else {
    logger.error("Failed to connect to AMF");
    CORO_EARLY_RETURN(false);
  }

  CORO_RETURN(success);
}

void amf_connection_setup_routine::handle_connection_setup_result()
{
  // Update AMF connection handler state.
  amf_connected = success;
}
