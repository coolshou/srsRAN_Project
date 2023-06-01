/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_generic_impl.h"

using namespace srsran;

ru_generic_impl::ru_generic_impl(ru_generic_impl_config&& config) :
  phy_err_printer(std::move(config.phy_err_printer)),
  ru_rx_adapter(std::move(config.ru_rx_adapter)),
  ru_time_adapter(std::move(config.ru_time_adapter)),
  radio(std::move(config.radio)),
  low_phy(std::move(config.low_phy)),
  ru_ctrl(low_phy->get_controller(), *radio, config.srate_MHz),
  ru_downlink_hdlr(low_phy->get_rg_handler()),
  ru_uplink_request_hdlr(low_phy->get_request_handler())
{
  srsran_assert(phy_err_printer, "Invalid PHY error printer");
  srsran_assert(ru_rx_adapter, "Invalid Radio Unit receiver adapter");
  srsran_assert(ru_time_adapter, "Invalid Radio Unit time adapter");
  srsran_assert(radio, "Invalid radio session");
  srsran_assert(low_phy, "Invalid lower PHY");
}

ru_controller& ru_generic_impl::get_controller()
{
  return ru_ctrl;
}

ru_downlink_plane_handler& ru_generic_impl::get_downlink_plane_handler()
{
  return ru_downlink_hdlr;
}

ru_uplink_plane_handler& ru_generic_impl::get_uplink_plane_handler()
{
  return ru_uplink_request_hdlr;
}
