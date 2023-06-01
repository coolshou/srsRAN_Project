/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "iq_compression_death_impl.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ofh;

void iq_compression_death_impl::compress(span<compressed_prb>         compressed_prbs,
                                         span<const cf_t>             iq_data,
                                         const ru_compression_params& params)
{
  report_error("Compression type {} is not supported", params.type);
}

void iq_compression_death_impl::decompress(span<cf_t>                   iq_data,
                                           span<const compressed_prb>   compressed_prbs,
                                           const ru_compression_params& params)
{
  report_error("Decompression type {} is not supported", params.type);
}
