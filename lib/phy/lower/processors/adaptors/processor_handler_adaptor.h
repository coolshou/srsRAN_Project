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

#include "srsran/phy/lower/lower_phy_request_handler.h"
#include "srsran/phy/lower/lower_phy_rg_handler.h"
#include "srsran/phy/lower/processors/downlink/pdxch/pdxch_processor_request_handler.h"
#include "srsran/phy/lower/processors/uplink/prach/prach_processor_request_handler.h"
#include "srsran/phy/lower/processors/uplink/puxch/puxch_processor_request_handler.h"

namespace srsran {

class shared_resource_grid;

/// Adapts the lower physical layer handlers to the internal processors request handlers.
class processor_handler_adaptor
{
public:
  /// Constructs the lower physical layer handler adaptor from the internal processor request handlers.
  processor_handler_adaptor(pdxch_processor_request_handler& pdxch_handler,
                            prach_processor_request_handler& prach_handler,
                            puxch_processor_request_handler& puxch_handler) :
    rg_handler(pdxch_handler), request_handler(prach_handler, puxch_handler)
  {
  }

  /// Gets resource grid handler.
  lower_phy_rg_handler& get_rg_handler() { return rg_handler; }

  /// Gets demodulate request handler.
  lower_phy_request_handler& get_request_handler() { return request_handler; }

private:
  /// Implements the lower physical layer resource grid handler.
  class rg_handler_adaptor : public lower_phy_rg_handler
  {
  public:
    /// Connects the adaptor with the PDxCH processor handler.
    rg_handler_adaptor(pdxch_processor_request_handler& pdxch_handler_) : pdxch_handler(pdxch_handler_) {}

    // See interface for documentation.
    void handle_resource_grid(const resource_grid_context& context, const shared_resource_grid& grid) override;

  private:
    /// Actual PDxCH processor request handler.
    pdxch_processor_request_handler& pdxch_handler;
  };

  /// Implements the lower physical layer request handler.
  class request_handler_adaptor : public lower_phy_request_handler
  {
  public:
    /// Connects the adaptor with the PRACH and PUxCH processor handlers.
    request_handler_adaptor(prach_processor_request_handler& prach_handler_,
                            puxch_processor_request_handler& puxch_handler_) :
      prach_handler(prach_handler_), puxch_handler(puxch_handler_)
    {
    }

    // See interface for documentation.
    void request_prach_window(const prach_buffer_context& context, prach_buffer& buffer) override;

    // See interface for documentation.
    void request_uplink_slot(const resource_grid_context& context, const shared_resource_grid& grid) override;

  private:
    /// Actual PRACH processor request handler.
    prach_processor_request_handler& prach_handler;
    /// Actual PUxCH processor request handler.
    puxch_processor_request_handler& puxch_handler;
  };

  rg_handler_adaptor      rg_handler;
  request_handler_adaptor request_handler;
};

} // namespace srsran