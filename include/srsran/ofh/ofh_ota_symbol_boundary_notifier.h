/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// This interface notifies the timing boundary of an OFDM symbol over the air.
class ota_symbol_boundary_notifier
{
public:
  virtual ~ota_symbol_boundary_notifier() = default;

  /// \brief Notifies a new OTA symbol boundary event.
  ///
  /// Notifies that the beginning of a new OTA symbol has started.
  ///
  /// \param[in] slot         Current slot point.
  /// \param[in] symbol_index Current symbol index within the slot.
  virtual void on_new_symbol(slot_point slot, unsigned symbol_index) = 0;
};

} // namespace ofh
} // namespace srsran
