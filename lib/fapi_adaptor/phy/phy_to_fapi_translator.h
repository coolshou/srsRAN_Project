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

#include "srsgnb/fapi/slot_time_message_notifier.h"
#include "srsgnb/ran/slot_point.h"
#include <functional>

namespace srsgnb {
namespace fapi_adaptor {

/// \brief This class translates all the messages and events from the PHY to FAPI.
class phy_to_fapi_translator
{
public:
  phy_to_fapi_translator();

  /// \brief Handles the new slot by creating a SLOT.indication with the given slot and sending it through the notifier.
  void handle_new_slot(slot_point slot);

  /// \brief Sets the \c slot_time_message_notifier for the translator.
  /// \param[in] fapi_time_notifier Notifier to be set.
  void set_fapi_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_notifier);

private:
  std::reference_wrapper<fapi::slot_time_message_notifier> time_notifier;
};

} // namespace fapi_adaptor
} // namespace srsgnb
