/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_ofh_rx_symbol_handler_impl.h"

using namespace srsran;

void ru_ofh_rx_symbol_handler_impl::on_new_uplink_symbol(const ofh::uplane_rx_symbol_context& context,
                                                         const resource_grid_reader&          grid)
{
  ru_uplink_rx_symbol_context ru_context;
  //: TODO: set sector field when we manage multiple sectors.
  ru_context.sector    = 0;
  ru_context.slot      = context.slot;
  ru_context.symbol_id = context.symbol;

  notifier.on_new_uplink_symbol(ru_context, grid);
}

void ru_ofh_rx_symbol_handler_impl::on_new_prach_window_data(const prach_buffer_context& context,
                                                             const prach_buffer&         buffer)
{
  notifier.on_new_prach_window_data(context, buffer);
}
