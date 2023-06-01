/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "tx_precoding_and_beamforming_pdu.h"
#include "field_checkers.h"

using namespace srsran;
using namespace fapi;

/// This validator checks a DL_TTI.request message.
static constexpr message_type_id msg_type = message_type_id::dl_tti_request;

/// Validates the number of PRGs property of the transmission precoding and beamforming PDU, as per SCF-222 v4.0
/// section 3.4.2.5.
static bool validate_num_prbs(unsigned value, validator_report& report, unsigned pdu_type)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "Number of PRGs", msg_type, pdu_type, report);
}

/// Validates the PRG size property of the transmission precoding and beamforming PDU, as per SCF-222 v4.0
/// section 3.4.2.5.
static bool validate_prg_size(unsigned value, validator_report& report, unsigned pdu_type)
{
  static constexpr unsigned MIN_VALUE = 1;
  static constexpr unsigned MAX_VALUE = 275;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "PRG size", msg_type, pdu_type, report);
}

/// Validates the number of beam indexes for each PRG of the transmission precoding and beamforming PDU, as per SCF-222
/// v4.0 section 3.4.2.5.
static bool
validate_nof_beam_indexes_per_prg(unsigned value, unsigned expected, validator_report& report, unsigned pdu_type)
{
  return validate_field_eq(expected, value, "Number of beam indexes", msg_type, pdu_type, report);
}

bool srsran::fapi::validate_tx_precoding_and_beamforming_pdu(const tx_precoding_and_beamforming_pdu& pdu,
                                                             validator_report&                       report,
                                                             dl_pdu_type                             pdu_type)
{
  bool result = true;

  const unsigned pdu_type_value = static_cast<unsigned>(pdu_type);

  // NOTE: TRP scheme property will not be validated as its range is not defined.
  result &= validate_num_prbs(pdu.prgs.size(), report, pdu_type_value);
  result &= validate_prg_size(pdu.prg_size, report, pdu_type_value);
  // NOTE: digBFInterfaces property will not be validated as it uses the variable whole range.
  // NOTE: PMidx property will not be validated as it uses the variable whole range.
  // NOTE: beamIdx property will not be validated as it uses the variable whole range.
  for (const auto& prg : pdu.prgs) {
    result &= validate_nof_beam_indexes_per_prg(prg.beam_index.size(), pdu.dig_bf_interfaces, report, pdu_type_value);
  }

  return result;
}
