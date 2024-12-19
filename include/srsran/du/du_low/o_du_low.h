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

namespace srsran {

namespace fapi {
class slot_data_message_notifier;
class slot_error_message_notifier;
class slot_last_message_notifier;
class slot_message_gateway;
class slot_time_message_notifier;
} // namespace fapi

namespace srs_du {

class du_low;

/// O-RAN DU low interface. O-DU low groups the DU low with the PHY-FAPI adaptor.
class o_du_low
{
public:
  /// Default destructor.
  virtual ~o_du_low() = default;

  /// Returns the upper PHY of this DU low.
  virtual du_low& get_du_low() = 0;

  /// Returns a reference to the FAPI slot-based message gateway for the given cell of this O-RAN DU low.
  virtual fapi::slot_message_gateway& get_slot_message_gateway(unsigned cell_id) = 0;

  /// Returns a reference to the FAPI slot-based last message notifier for the given cell of this O-RAN DU low.
  virtual fapi::slot_last_message_notifier& get_slot_last_message_notifier(unsigned cell_id) = 0;

  /// Configures the FAPI slot-based, time-specific message notifier for the given cell to the given one.
  virtual void set_slot_time_message_notifier(unsigned                          cell_id,
                                              fapi::slot_time_message_notifier& fapi_time_notifier) = 0;

  /// Configures the FAPI slot-based, error-specific message notifier for the given cell to the given one.
  virtual void set_slot_error_message_notifier(unsigned                           cell_id,
                                               fapi::slot_error_message_notifier& fapi_error_notifier) = 0;

  /// Configures the FAPI slot-based, data-specific message notifier for the given cell to the given one.
  virtual void set_slot_data_message_notifier(unsigned                          cell_id,
                                              fapi::slot_data_message_notifier& fapi_data_notifier) = 0;
};

} // namespace srs_du
} // namespace srsran
