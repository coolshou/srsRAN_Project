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

#include "srsran/phy/upper/uplink_processor.h"

namespace srsran {

class uplink_processor_spy : public uplink_processor
{
  bool has_process_prach_method_called = false;
  bool has_process_pusch_method_called = false;
  bool has_process_pucch_method_called = false;
  bool has_process_srs_method_called   = false;

public:
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override
  {
    has_process_prach_method_called = true;
  }

  void process_pusch(span<uint8_t>                      data,
                     unique_rx_buffer                   rm_buffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const shared_resource_grid&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    has_process_pusch_method_called = true;
  }

  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const shared_resource_grid&    grid,
                     const pucch_pdu&               config) override
  {
    has_process_pucch_method_called = true;
  }

  void
  process_srs(upper_phy_rx_results_notifier& notifier, const shared_resource_grid& grid, const srs_pdu& pdu) override
  {
    has_process_srs_method_called = true;
  }

  bool is_process_prach_method_called() const { return has_process_prach_method_called; }

  bool is_process_pusch_method_called() const { return has_process_pusch_method_called; }

  bool is_process_pucch_method_called() const { return has_process_pucch_method_called; }
};

} // namespace srsran
