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

namespace srsran {

/// NR Duplex mode.
enum class duplex_mode {
  /// Paired FDD.
  FDD = 0,
  /// Unpaired TDD.
  TDD,
  /// Supplementary DownLink (Unpaired).
  SDL,
  /// Supplementary UpLink (Unpaired).
  SUL,
  INVALID
};

} // namespace srsran