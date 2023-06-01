/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_ota_symbol_dispatcher.h"

using namespace srsran;
using namespace ofh;

void ota_symbol_dispatcher::on_new_symbol(slot_point slot, unsigned symbol_index)
{
  // Handle the new symbol.
  for (auto* handler : symbol_handlers) {
    handler->handle_new_symbol(slot, symbol_index);
  }

  // Call OTA notifiers.
  for (auto* notifier : ota_notifiers) {
    notifier->on_new_symbol(slot, symbol_index);
  }

  // First incoming slot, notify always.
  if (!current_slot.valid()) {
    current_slot = slot;
    logger.debug("First slot is {}, notifying slot={}", slot, slot + nof_slot_offset_du_ru);
    time_notifier->on_tti_boundary(slot + nof_slot_offset_du_ru);

    return;
  }

  // Skip if the slot did not change.
  if (slot == current_slot) {
    return;
  }

  // At this point, notify the change of slot.
  slot_point expected_slot = current_slot + 1;
  if (expected_slot != slot) {
    logger.warning("Detected a non-consecutive slot change. current_slot={} , new_slot={}", current_slot, slot);
  }

  if (symbol_index == half_slot_symbol) {
    time_notifier->on_ul_half_slot_boundary(slot);
  }

  if (symbol_index == full_slot_symbol) {
    time_notifier->on_ul_full_slot_boundary(slot);
  }

  current_slot = slot;
  time_notifier->on_tti_boundary(slot + nof_slot_offset_du_ru);
}

ota_symbol_dispatcher::ota_symbol_dispatcher(unsigned                            nof_slot_offset_du_ru_,
                                             unsigned                            nof_symbols_per_slot,
                                             srslog::basic_logger&               logger_,
                                             std::unique_ptr<timing_notifier>    timing_notifier_,
                                             span<symbol_handler*>               symbol_handlers_,
                                             span<ota_symbol_boundary_notifier*> ota_notifiers_) :
  nof_slot_offset_du_ru(nof_slot_offset_du_ru_),
  half_slot_symbol(nof_symbols_per_slot / 2U - 1U),
  full_slot_symbol(nof_symbols_per_slot - 1U),
  logger(logger_),
  time_notifier(std::move(timing_notifier_)),
  symbol_handlers(symbol_handlers_.begin(), symbol_handlers_.end()),
  ota_notifiers(ota_notifiers_.begin(), ota_notifiers_.end())
{
}
