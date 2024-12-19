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

#include "srsran/du/o_du_factory.h"
#include "o_du_impl.h"
#include "srsran/du/du_high/o_du_high_factory.h"
#include "srsran/du/du_low/o_du_low_factory.h"

using namespace srsran;
using namespace srs_du;

std::unique_ptr<o_du> srsran::srs_du::make_o_du(const o_du_config& du_cfg, o_du_dependencies&& dependencies)
{
  o_du_impl_dependencies o_du_deps;

  // Create DU low.
  o_du_deps.du_lo = make_o_du_low(du_cfg.du_low_cfg, du_cfg.du_high_cfg.du_hi.ran.cells);

  // Fill O-DU high dependencies.
  srsran_assert(du_cfg.du_low_cfg.du_low_cfg.cells.size() == dependencies.du_high_deps.sectors.size(),
                "DU low number of cells '{}' does not match the number of cells of the DU high dependencies '{}'",
                du_cfg.du_low_cfg.du_low_cfg.cells.size(),
                dependencies.du_high_deps.sectors.size());
  for (unsigned i = 0, e = du_cfg.du_low_cfg.du_low_cfg.cells.size(); i != e; ++i) {
    o_du_high_sector_dependencies& deps = dependencies.du_high_deps.sectors[i];
    deps.gateway                        = &o_du_deps.du_lo->get_slot_message_gateway(i);
    deps.last_msg_notifier              = &o_du_deps.du_lo->get_slot_last_message_notifier(i);
  }

  o_du_deps.du_hi = make_o_du_high(du_cfg.du_high_cfg, std::move(dependencies.du_high_deps));

  srslog::fetch_basic_logger("DU").info("O-DU created successfully");

  return std::make_unique<o_du_impl>(std::move(o_du_deps));
}

std::unique_ptr<o_du> srs_du::make_o_du(std::unique_ptr<o_du_high> odu_hi, std::unique_ptr<o_du_low> odu_lo)
{
  srsran_assert(odu_hi, "Invalid O-DU high");
  srsran_assert(odu_lo, "Invalid O-DU low");

  srslog::fetch_basic_logger("DU").info("O-DU created successfully");

  o_du_impl_dependencies o_du_deps;
  o_du_deps.du_lo = std::move(odu_lo);
  o_du_deps.du_hi = std::move(odu_hi);

  return std::make_unique<o_du_impl>(std::move(o_du_deps));
}
