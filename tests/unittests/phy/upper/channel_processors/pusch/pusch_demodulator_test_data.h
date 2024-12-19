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

// This file was generated using the following MATLAB class on 08-11-2024 (seed 0):
//   + "srsPUSCHDemodulatorUnittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/support/file_tensor.h"

namespace srsran {

enum class ch_dims : unsigned { subcarrier = 0, symbol = 1, rx_port = 2, tx_layer = 3, nof_dims = 4 };

struct context_t {
  float                            noise_var;
  float                            sinr_dB;
  pusch_demodulator::configuration config;
};

struct test_case_t {
  context_t                                                            context;
  file_vector<resource_grid_reader_spy::expected_entry_t>              symbols;
  file_tensor<static_cast<unsigned>(ch_dims::nof_dims), cf_t, ch_dims> estimates;
  file_vector<uint8_t>                                                 scrambling_seq;
  file_vector<log_likelihood_ratio>                                    codeword;
};

static const std::vector<test_case_t> pusch_demodulator_test_data = {
    // clang-format off
  {{0.016194, 20.1985, {6393, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 128, 1, true, {0}}}, {"test_data/pusch_demodulator_test_input_symbols0.dat"}, {"test_data/pusch_demodulator_test_input_estimates0.dat", {288, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq0.dat"}, {"test_data/pusch_demodulator_test_output0.dat"}},
  {{0.023644, 18.2904, {29714, {{1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 401, 1, true, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols2.dat"}, {"test_data/pusch_demodulator_test_input_estimates2.dat", {96, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq2.dat"}, {"test_data/pusch_demodulator_test_output2.dat"}},
  {{0.01932, 15.3112, {35832, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 2, 7, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 881, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols4.dat"}, {"test_data/pusch_demodulator_test_input_estimates4.dat", {144, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq4.dat"}, {"test_data/pusch_demodulator_test_output4.dat"}},
  {{0.0006635, 32.9662, {7320, {{1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 0, 12, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 347, 1, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols6.dat"}, {"test_data/pusch_demodulator_test_input_estimates6.dat", {60, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq6.dat"}, {"test_data/pusch_demodulator_test_output6.dat"}},
  {{0.0022768, 26.4585, {52579, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 1, 11, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 9, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols8.dat"}, {"test_data/pusch_demodulator_test_input_estimates8.dat", {240, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq8.dat"}, {"test_data/pusch_demodulator_test_output8.dat"}},
  {{0.0066064, 23.9641, {47998, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 1, 12, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 893, 1, false, {0}}}, {"test_data/pusch_demodulator_test_input_symbols10.dat"}, {"test_data/pusch_demodulator_test_input_estimates10.dat", {240, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq10.dat"}, {"test_data/pusch_demodulator_test_output10.dat"}},
  {{0.011268, 19.8845, {2342, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 0, 8, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 693, 1, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols12.dat"}, {"test_data/pusch_demodulator_test_input_estimates12.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq12.dat"}, {"test_data/pusch_demodulator_test_output12.dat"}},
  {{0.0092416, 15.8933, {26738, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 1, 11, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 870, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols14.dat"}, {"test_data/pusch_demodulator_test_input_estimates14.dat", {300, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq14.dat"}, {"test_data/pusch_demodulator_test_output14.dat"}},
  {{0.00033254, 35.9197, {41693, {{1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 1, 8, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 752, 1, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols16.dat"}, {"test_data/pusch_demodulator_test_input_estimates16.dat", {60, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq16.dat"}, {"test_data/pusch_demodulator_test_output16.dat"}},
  {{0.0045049, 24.2641, {29758, {{1, 1}}, modulation_scheme::QPSK, 2, 9, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 176, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols18.dat"}, {"test_data/pusch_demodulator_test_input_estimates18.dat", {24, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq18.dat"}, {"test_data/pusch_demodulator_test_output18.dat"}},
  {{0.052477, 14.9325, {57306, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 0, 12, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 768, 1, false, {0}}}, {"test_data/pusch_demodulator_test_input_symbols20.dat"}, {"test_data/pusch_demodulator_test_input_estimates20.dat", {240, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq20.dat"}, {"test_data/pusch_demodulator_test_output20.dat"}},
  {{0.049126, 15.3987, {18491, {{1, 1}}, modulation_scheme::QAM16, 2, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 802, 1, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols22.dat"}, {"test_data/pusch_demodulator_test_input_estimates22.dat", {24, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq22.dat"}, {"test_data/pusch_demodulator_test_output22.dat"}},
  {{0.0065728, 20.9563, {26726, {{1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 0, 8, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 1007, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols24.dat"}, {"test_data/pusch_demodulator_test_input_estimates24.dat", {72, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq24.dat"}, {"test_data/pusch_demodulator_test_output24.dat"}},
  {{0.0052086, 23.9511, {12591, {{1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 0, 12, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 870, 1, true, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols26.dat"}, {"test_data/pusch_demodulator_test_input_estimates26.dat", {72, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq26.dat"}, {"test_data/pusch_demodulator_test_output26.dat"}},
  {{0.0018408, 27.7353, {45087, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 0, 12, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 1, 597, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols28.dat"}, {"test_data/pusch_demodulator_test_input_estimates28.dat", {120, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq28.dat"}, {"test_data/pusch_demodulator_test_output28.dat"}},
  {{0.025555, 18.2237, {44634, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM64, 1, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 528, 1, true, {0}}}, {"test_data/pusch_demodulator_test_input_symbols30.dat"}, {"test_data/pusch_demodulator_test_input_estimates30.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq30.dat"}, {"test_data/pusch_demodulator_test_output30.dat"}},
  {{0.0045558, 24.9776, {29606, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM64, 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 669, 1, true, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols32.dat"}, {"test_data/pusch_demodulator_test_input_estimates32.dat", {120, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq32.dat"}, {"test_data/pusch_demodulator_test_output32.dat"}},
  {{0.0023783, 23.9836, {30045, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM64, 1, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 795, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols34.dat"}, {"test_data/pusch_demodulator_test_input_estimates34.dat", {192, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq34.dat"}, {"test_data/pusch_demodulator_test_output34.dat"}},
  {{0.00043207, 35.0645, {42119, {1}, modulation_scheme::QAM64, 2, 10, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 1, 912, 1, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols36.dat"}, {"test_data/pusch_demodulator_test_input_estimates36.dat", {12, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq36.dat"}, {"test_data/pusch_demodulator_test_output36.dat"}},
  {{0.036098, 15.0866, {43254, {{1, 1, 1, 1, 1}}, modulation_scheme::QAM64, 1, 10, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 491, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols38.dat"}, {"test_data/pusch_demodulator_test_input_estimates38.dat", {60, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq38.dat"}, {"test_data/pusch_demodulator_test_output38.dat"}},
  {{0.0053621, 25.4388, {63880, {{1, 1}}, modulation_scheme::QAM256, 2, 8, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 250, 1, true, {0}}}, {"test_data/pusch_demodulator_test_input_symbols40.dat"}, {"test_data/pusch_demodulator_test_input_estimates40.dat", {24, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq40.dat"}, {"test_data/pusch_demodulator_test_output40.dat"}},
  {{0.017859, 17.8897, {7448, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM256, 0, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 998, 1, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols42.dat"}, {"test_data/pusch_demodulator_test_input_estimates42.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq42.dat"}, {"test_data/pusch_demodulator_test_output42.dat"}},
  {{0.004853, 10.3186, {64276, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM256, 2, 12, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 825, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols44.dat"}, {"test_data/pusch_demodulator_test_input_estimates44.dat", {144, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq44.dat"}, {"test_data/pusch_demodulator_test_output44.dat"}},
  {{0.0020736, 28.0302, {33013, {{1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM256, 2, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 88, 1, true, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols46.dat"}, {"test_data/pusch_demodulator_test_input_estimates46.dat", {72, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq46.dat"}, {"test_data/pusch_demodulator_test_output46.dat"}},
  {{0.0035508, 25.0019, {3768, {1}, modulation_scheme::QAM256, 0, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 931, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols48.dat"}, {"test_data/pusch_demodulator_test_input_estimates48.dat", {12, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq48.dat"}, {"test_data/pusch_demodulator_test_output48.dat"}},
  {{0.010174, 22.2436, {48990, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 2, 9, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 888, 1, false, {0}}}, {"test_data/pusch_demodulator_test_input_symbols50.dat"}, {"test_data/pusch_demodulator_test_input_estimates50.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq50.dat"}, {"test_data/pusch_demodulator_test_output50.dat"}},
  {{0.0061508, 24.5003, {22589, {{1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 2, 8, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 856, 1, true, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols52.dat"}, {"test_data/pusch_demodulator_test_input_estimates52.dat", {48, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq52.dat"}, {"test_data/pusch_demodulator_test_output52.dat"}},
  {{0.00086413, 29.3548, {23085, {1}, modulation_scheme::PI_2_BPSK, 2, 10, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 983, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols54.dat"}, {"test_data/pusch_demodulator_test_input_estimates54.dat", {12, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq54.dat"}, {"test_data/pusch_demodulator_test_output54.dat"}},
  {{0.0018395, 27.9189, {31255, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 630, 1, true, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols56.dat"}, {"test_data/pusch_demodulator_test_input_estimates56.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq56.dat"}, {"test_data/pusch_demodulator_test_output56.dat"}},
  {{0.00091017, 30.4726, {58724, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::PI_2_BPSK, 1, 11, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 2, 840, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols58.dat"}, {"test_data/pusch_demodulator_test_input_estimates58.dat", {216, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq58.dat"}, {"test_data/pusch_demodulator_test_output58.dat"}},
  {{0.053869, 15.3704, {63790, {{1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 2, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 579, 1, true, {0}}}, {"test_data/pusch_demodulator_test_input_symbols60.dat"}, {"test_data/pusch_demodulator_test_input_estimates60.dat", {96, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq60.dat"}, {"test_data/pusch_demodulator_test_output60.dat"}},
  {{0.01954, 19.0331, {63937, {{1, 1, 1}}, modulation_scheme::QPSK, 2, 12, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 474, 1, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols62.dat"}, {"test_data/pusch_demodulator_test_input_estimates62.dat", {36, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq62.dat"}, {"test_data/pusch_demodulator_test_output62.dat"}},
  {{0.017253, 17.1841, {29327, {{1, 1}}, modulation_scheme::QPSK, 2, 7, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 3, 930, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols64.dat"}, {"test_data/pusch_demodulator_test_input_estimates64.dat", {24, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq64.dat"}, {"test_data/pusch_demodulator_test_output64.dat"}},
  {{0.00036783, 34.7887, {188, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 0, 9, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 717, 1, true, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols66.dat"}, {"test_data/pusch_demodulator_test_input_estimates66.dat", {288, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq66.dat"}, {"test_data/pusch_demodulator_test_output66.dat"}},
  {{0.0014576, 28.8097, {2024, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QPSK, 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 102, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols68.dat"}, {"test_data/pusch_demodulator_test_input_estimates68.dat", {180, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq68.dat"}, {"test_data/pusch_demodulator_test_output68.dat"}},
  {{0.067817, 14.1592, {6517, {{1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 1, 11, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 90, 1, true, {0}}}, {"test_data/pusch_demodulator_test_input_symbols70.dat"}, {"test_data/pusch_demodulator_test_input_estimates70.dat", {96, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq70.dat"}, {"test_data/pusch_demodulator_test_output70.dat"}},
  {{0.015425, 20.413, {21290, {1}, modulation_scheme::QAM16, 2, 9, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 554, 1, true, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols72.dat"}, {"test_data/pusch_demodulator_test_input_estimates72.dat", {12, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq72.dat"}, {"test_data/pusch_demodulator_test_output72.dat"}},
  {{0.0054558, 22.1841, {7525, {{1, 1}}, modulation_scheme::QAM16, 2, 8, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 3, 5, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols74.dat"}, {"test_data/pusch_demodulator_test_input_estimates74.dat", {24, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq74.dat"}, {"test_data/pusch_demodulator_test_output74.dat"}},
  {{0.0033254, 25.9483, {62190, {{1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 1, 9, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 997, 1, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols76.dat"}, {"test_data/pusch_demodulator_test_input_estimates76.dat", {60, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq76.dat"}, {"test_data/pusch_demodulator_test_output76.dat"}},
  {{0.0022862, 26.3743, {9953, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM16, 0, 11, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 3, 788, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols78.dat"}, {"test_data/pusch_demodulator_test_input_estimates78.dat", {216, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq78.dat"}, {"test_data/pusch_demodulator_test_output78.dat"}},
  {{0.016961, 20.1405, {64150, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM64, 1, 11, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 2, 337, 1, false, {0}}}, {"test_data/pusch_demodulator_test_input_symbols80.dat"}, {"test_data/pusch_demodulator_test_input_estimates80.dat", {144, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq80.dat"}, {"test_data/pusch_demodulator_test_output80.dat"}},
  {{0.01954, 18.9169, {20207, {{1, 1, 1}}, modulation_scheme::QAM64, 1, 13, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 477, 1, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols82.dat"}, {"test_data/pusch_demodulator_test_input_estimates82.dat", {36, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq82.dat"}, {"test_data/pusch_demodulator_test_output82.dat"}},
  {{0.0017253, 27.0822, {24481, {{1, 1}}, modulation_scheme::QAM64, 2, 8, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 2, 775, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols84.dat"}, {"test_data/pusch_demodulator_test_input_estimates84.dat", {24, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq84.dat"}, {"test_data/pusch_demodulator_test_output84.dat"}},
  {{0.00068004, 33.07, {55959, {{1, 1, 1}}, modulation_scheme::QAM64, 2, 11, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 169, 1, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols86.dat"}, {"test_data/pusch_demodulator_test_input_estimates86.dat", {36, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq86.dat"}, {"test_data/pusch_demodulator_test_output86.dat"}},
  {{0.0028782, 25.0366, {13040, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM64, 0, 8, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 384, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols88.dat"}, {"test_data/pusch_demodulator_test_input_estimates88.dat", {216, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq88.dat"}, {"test_data/pusch_demodulator_test_output88.dat"}},
  {{0.053655, 15.1483, {13654, {{1, 1, 1, 1}}, modulation_scheme::QAM256, 1, 11, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 712, 1, false, {0}}}, {"test_data/pusch_demodulator_test_input_symbols90.dat"}, {"test_data/pusch_demodulator_test_input_estimates90.dat", {48, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq90.dat"}, {"test_data/pusch_demodulator_test_output90.dat"}},
  {{0.0043393, 25.1588, {10041, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM256, 1, 9, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 707, 1, true, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols92.dat"}, {"test_data/pusch_demodulator_test_input_estimates92.dat", {144, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq92.dat"}, {"test_data/pusch_demodulator_test_output92.dat"}},
  {{0.015006, 16.5095, {51140, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM256, 0, 9, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 2, 718, 2, false, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols94.dat"}, {"test_data/pusch_demodulator_test_input_estimates94.dat", {192, 14, 2, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq94.dat"}, {"test_data/pusch_demodulator_test_output94.dat"}},
  {{0.0010778, 30.8774, {9132, {{1, 1, 1}}, modulation_scheme::QAM256, 0, 7, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 484, 1, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols96.dat"}, {"test_data/pusch_demodulator_test_input_estimates96.dat", {36, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq96.dat"}, {"test_data/pusch_demodulator_test_output96.dat"}},
  {{0.0058027, 22.8169, {55366, {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}, modulation_scheme::QAM256, 2, 12, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 2, 112, 2, false, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols98.dat"}, {"test_data/pusch_demodulator_test_input_estimates98.dat", {180, 14, 4, 2}}, {"test_data/pusch_demodulator_test_output_scrambling_seq98.dat"}, {"test_data/pusch_demodulator_test_output98.dat"}},
    // clang-format on
};

} // namespace srsran
