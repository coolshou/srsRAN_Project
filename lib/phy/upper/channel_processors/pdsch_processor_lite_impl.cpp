/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_processor_lite_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/ran/dmrs.h"

using namespace srsran;

/// \brief Computes the number of RE used for mapping PDSCH data.
///
/// The number of RE excludes the elements described by \c pdu as reserved and the RE used for DM-RS.
///
/// \param[in] pdu Describes a PDSCH transmission.
/// \return The number of resource elements.
static unsigned compute_nof_data_re(const pdsch_processor::pdu_t& pdu)
{
  // Copy reserved RE and merge DM-RS pattern.
  re_pattern_list reserved_re = pdu.reserved;
  reserved_re.merge(pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask));

  // Generate allocation mask.
  bounded_bitset<MAX_RB> prb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Calculate the number of RE allocated in the grid.
  unsigned nof_grid_re = pdu.freq_alloc.get_nof_rb() * NRE * pdu.nof_symbols;

  // Calculate the number of reserved resource elements.
  unsigned nof_reserved_re = reserved_re.get_inclusion_count(pdu.start_symbol_index, pdu.nof_symbols, prb_mask);

  // Subtract the number of reserved RE from the number of allocated RE.
  srsran_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re;
}

void pdsch_block_processor::configure_new_transmission(span<const uint8_t>           data,
                                                       unsigned                      i_cw,
                                                       const pdsch_processor::pdu_t& pdu)
{
  // The number of layers is equal to the number of ports.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = compute_nof_data_re(pdu);

  // Init scrambling initial state.
  scrambler.init((static_cast<unsigned>(pdu.rnti) << 15U) + (i_cw << 14U) + pdu.n_id);

  // Select codeword specific parameters.
  unsigned rv = pdu.codewords[i_cw].rv;
  modulation  = pdu.codewords[i_cw].modulation;

  // Prepare segmenter configuration.
  segmenter_config encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = pdu.tbs_lbrm_bytes * 8;
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = nof_re_pdsch * nof_layers;

  // Clear the buffer.
  d_segments.clear();

  // Segmentation (it includes CRC attachment for the entire transport block and each individual segment).
  segmenter.segment(d_segments, data, encoder_config);

  // Initialize the codeblock counter.
  next_i_cb = 0;
}

void pdsch_block_processor::new_codeblock()
{
  // Temporary data storage.
  std::array<uint8_t, MAX_SEG_LENGTH.value()>     temp_unpacked_cb;
  std::array<uint8_t, 3 * MAX_SEG_LENGTH.value()> buffer_cb;

  srsran_assert(next_i_cb < d_segments.size(),
                "The codeblock index (i.e., {}) exceeds the number of codeblocks (i.e., {})",
                next_i_cb,
                d_segments.size());

  // Select segment description.
  const described_segment& descr_seg = d_segments[next_i_cb];

  // CB payload number of bits.
  unsigned cb_length = descr_seg.get_data().size();

  // Rate Matching output length.
  unsigned rm_length = descr_seg.get_metadata().cb_specific.rm_length;

  // Resize internal buffer to match data from the segmenter to the encoder (all segments have the same length).
  span<uint8_t> tmp_data = span<uint8_t>(temp_unpacked_cb).first(cb_length);

  // Resize internal buffer to match data from the encoder to the rate matcher (all segments have the same length).
  span<uint8_t> tmp_encoded = span<uint8_t>(buffer_cb).first(descr_seg.get_metadata().cb_specific.full_length);

  // Unpack segment.
  srsvec::bit_unpack(tmp_data, descr_seg.get_data());

  // Set filler bits.
  span<uint8_t> filler_bits = tmp_data.last(descr_seg.get_metadata().cb_specific.nof_filler_bits);
  std::fill(filler_bits.begin(), filler_bits.end(), ldpc::FILLER_BIT);

  // Encode the segment into a codeblock.
  encoder.encode(tmp_encoded, tmp_data, descr_seg.get_metadata().tb_common);

  // Rate match the codeblock.
  encoded_bit_buffer = span<uint8_t>(temp_cb_unpacked_bits).first(rm_length);
  rate_matcher.rate_match(encoded_bit_buffer, tmp_encoded, descr_seg.get_metadata());

  // Increment codeblock counter.
  ++next_i_cb;
}

span<const cf_t> pdsch_block_processor::pop_symbols(unsigned block_size)
{
  // Get number of bits per symbol.
  unsigned nof_bits_per_symbol = get_bits_per_symbol(modulation);

  // Create a view to the symbol buffer.
  span<cf_t> symbols = span<cf_t>(temp_symbol_buffer).first(block_size);

  // Process symbols until the symbol buffer is depleted.
  while (!symbols.empty()) {
    // Process new codeblock if the RM buffer is empty.
    if (encoded_bit_buffer.empty()) {
      new_codeblock();
    }

    // Calculate the number of bits to process.
    unsigned nof_bits = std::min(encoded_bit_buffer.size(), symbols.size() * nof_bits_per_symbol);

    // Calculate the number symbols to process.
    srsran_assert(nof_bits_per_symbol > 0 && nof_bits % nof_bits_per_symbol == 0, "Invalid number of bits.");
    unsigned nof_symbols = nof_bits / nof_bits_per_symbol;

    // Pop the bits to process.
    span<uint8_t> encoded_bit = encoded_bit_buffer.first(nof_bits);
    encoded_bit_buffer        = encoded_bit_buffer.last(encoded_bit_buffer.size() - nof_bits);

    // Pack the bits.
    temp_packed_bits.resize(nof_bits);
    srsvec::bit_pack(temp_packed_bits, encoded_bit);

    // Apply scrambling sequence in-place.
    scrambler.apply_xor(temp_packed_bits, temp_packed_bits);

    // Pop the symbols to modulate.
    span<cf_t> modulated = symbols.first(nof_symbols);
    symbols              = symbols.last(symbols.size() - nof_symbols);

    // Modulate.
    modulator.modulate(modulated, temp_packed_bits, modulation);
  }

  return span<const cf_t>(temp_symbol_buffer).first(block_size);
}

void pdsch_processor_lite_impl::process(resource_grid_mapper&                                        mapper,
                                        static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                                        const pdsch_processor::pdu_t&                                pdu)
{
  assert_pdu(pdu);

  // Configure new transmission.
  subprocessor.configure_new_transmission(data[0], 0, pdu);

  // Get the PRB allocation mask.
  const bounded_bitset<MAX_RB> prb_allocation_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // First symbol used in this transmission.
  unsigned start_symbol_index = pdu.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = pdu.start_symbol_index + pdu.nof_symbols;

  srsran_assert(end_symbol_index <= MAX_NSYMB_PER_SLOT,
                "The time allocation of the transmission [{}, {}) exceeds the slot boundary.",
                start_symbol_index,
                end_symbol_index);

  // PDSCH OFDM symbol mask.
  symbol_slot_mask symbols;
  symbols.fill(start_symbol_index, end_symbol_index);

  // Allocation pattern for the mapper.
  re_pattern_list allocation;
  re_pattern      pdsch_pattern;

  // Reserved REs, including DM-RS and CSI-RS.
  re_pattern_list reserved(pdu.reserved);

  // Get DM-RS RE pattern.
  re_pattern dmrs_pattern = pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask);

  // Merge DM-RS RE pattern into the reserved RE patterns.
  reserved.merge(dmrs_pattern);

  // Set PDSCH allocation pattern.
  pdsch_pattern.prb_mask = prb_allocation_mask;
  pdsch_pattern.re_mask  = ~re_prb_mask();
  pdsch_pattern.symbols  = symbols;
  allocation.merge(pdsch_pattern);

  // Map PDSCH.
  mapper.map(subprocessor, allocation, reserved, pdu.precoding);

  // Process DM-RS.
  process_dmrs(mapper, pdu);
}

void pdsch_processor_lite_impl::assert_pdu(const pdsch_processor::pdu_t& pdu) const
{
  // Deduce parameters from the PDU.
  unsigned         nof_layers       = pdu.precoding.get_nof_layers();
  unsigned         nof_codewords    = (nof_layers > 4) ? 2 : 1;
  unsigned         nof_symbols_slot = get_nsymb_per_slot(pdu.cp);
  dmrs_config_type dmrs_config = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2;

  srsran_assert(pdu.dmrs_symbol_mask.size() == nof_symbols_slot,
                "The DM-RS symbol mask size (i.e., {}), must be equal to the number of symbols in the slot (i.e., {}).",
                pdu.dmrs_symbol_mask.size(),
                nof_symbols_slot);
  srsran_assert(pdu.dmrs_symbol_mask.any(), "The number of OFDM symbols carrying DM-RS RE must be greater than zero.");
  srsran_assert(
      static_cast<unsigned>(pdu.dmrs_symbol_mask.find_lowest(true)) >= pdu.start_symbol_index,
      "The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to or greater than the first symbol "
      "allocated to transmission (i.e., {}).",
      pdu.dmrs_symbol_mask.find_lowest(true),
      pdu.start_symbol_index);
  srsran_assert(static_cast<unsigned>(pdu.dmrs_symbol_mask.find_highest(true)) <
                    (pdu.start_symbol_index + pdu.nof_symbols),
                "The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than or equal to the last "
                "symbol allocated to transmission (i.e., {}).",
                pdu.dmrs_symbol_mask.find_highest(true),
                pdu.start_symbol_index + pdu.nof_symbols - 1);
  srsran_assert((pdu.start_symbol_index + pdu.nof_symbols) <= nof_symbols_slot,
                "The transmission with time allocation [{}, {}) exceeds the slot boundary of {} symbols.",
                pdu.start_symbol_index,
                pdu.start_symbol_index + pdu.nof_symbols,
                nof_symbols_slot);
  srsran_assert(pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb),
                "Invalid BWP configuration [{}, {}) for the given frequency allocation {}.",
                pdu.bwp_start_rb,
                pdu.bwp_start_rb + pdu.bwp_size_rb,
                pdu.freq_alloc);
  srsran_assert(pdu.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(pdu.freq_alloc.is_contiguous(), "Only contiguous allocation is currently supported.");
  srsran_assert(pdu.nof_cdm_groups_without_data <= get_max_nof_cdm_groups_without_data(dmrs_config),
                "The number of CDM groups without data (i.e., {}) must not exceed the maximum supported by the DM-RS "
                "type (i.e., {}).",
                pdu.nof_cdm_groups_without_data,
                get_max_nof_cdm_groups_without_data(dmrs_config));
  srsran_assert(nof_layers != 0, "No transmit layers are active.");
  srsran_assert(nof_layers <= 4, "Only 1 to 4 layers are currently supported. {} layers requested.", nof_layers);

  srsran_assert(pdu.codewords.size() == nof_codewords,
                "Expected {} codewords and got {} for {} layers.",
                nof_codewords,
                pdu.codewords.size(),
                nof_layers);
  srsran_assert(pdu.tbs_lbrm_bytes > 0 && pdu.tbs_lbrm_bytes <= ldpc::MAX_CODEBLOCK_SIZE / 8,
                "Invalid LBRM size ({} bytes). It must be non-zero, less than or equal to {} bytes",
                pdu.tbs_lbrm_bytes,
                ldpc::MAX_CODEBLOCK_SIZE / 8);
}

void pdsch_processor_lite_impl::process_dmrs(resource_grid_mapper& mapper, const pdu_t& pdu)
{
  bounded_bitset<MAX_RB> rb_mask_bitset = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned dmrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdu_t::PRB0) {
    dmrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

  // Prepare DM-RS configuration.
  dmrs_pdsch_processor::config_t dmrs_config;
  dmrs_config.slot                 = pdu.slot;
  dmrs_config.reference_point_k_rb = dmrs_reference_point_k_rb;
  dmrs_config.type                 = pdu.dmrs;
  dmrs_config.scrambling_id        = pdu.scrambling_id;
  dmrs_config.n_scid               = pdu.n_scid;
  dmrs_config.amplitude            = convert_dB_to_amplitude(-pdu.ratio_pdsch_dmrs_to_sss_dB);
  dmrs_config.symbols_mask         = pdu.dmrs_symbol_mask;
  dmrs_config.rb_mask              = rb_mask_bitset;
  dmrs_config.precoding            = pdu.precoding;

  // Put DM-RS.
  dmrs->map(mapper, dmrs_config);
}