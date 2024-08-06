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

#include "srsran/security/integrity_engine.h"
#include "srsran/security/security.h"

namespace srsran {
namespace security {

class integrity_engine_generic final : public integrity_engine
{
public:
  integrity_engine_generic(sec_128_key         k_128_int_,
                           uint8_t             bearer_id_,
                           security_direction  direction_,
                           integrity_algorithm integ_algo_);
  ~integrity_engine_generic() = default;

  security_result protect_integrity(byte_buffer buf, uint32_t count) override;
  security_result verify_integrity(byte_buffer buf, uint32_t count) override;

private:
  sec_128_key         k_128_int;
  uint8_t             bearer_id;
  security_direction  direction;
  integrity_algorithm integ_algo;
};

} // namespace security
} // namespace srsran
