/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace fapi {

class logging_slot_gateway_decorator : public slot_message_gateway
{
public:
  logging_slot_gateway_decorator(srslog::basic_logger& logger, slot_message_gateway& gateway) :
    logger(logger), gateway(gateway)
  {
  }

  // See interface for documentation.
  void dl_tti_request(const dl_tti_request_message& msg) override;

  // See interface for documentation.
  void ul_tti_request(const ul_tti_request_message& msg) override;

  // See interface for documentation.
  void ul_dci_request(const ul_dci_request_message& msg) override;

  // See interface for documentation.
  void tx_data_request(const tx_data_request_message& msg) override;

private:
  /// FAPI logger.
  srslog::basic_logger& logger;
  slot_message_gateway& gateway;
};

} // namespace fapi
} // namespace srsgnb