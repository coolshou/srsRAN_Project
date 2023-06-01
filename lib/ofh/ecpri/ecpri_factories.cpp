/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ofh/ecpri/ecpri_factories.h"
#include "ecpri_packet_builder_impl.h"
#include "ecpri_packet_decoder_impl.h"

using namespace srsran;
using namespace ecpri;

std::unique_ptr<packet_builder> srsran::ecpri::create_ecpri_packet_builder()
{
  return std::make_unique<packet_builder_impl>();
}

std::unique_ptr<packet_decoder> srsran::ecpri::create_ecpri_packet_decoder(srslog::basic_logger& logger)
{
  return std::make_unique<packet_decoder_impl>(logger);
}
