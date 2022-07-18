/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "bwp_configuration.h"
#include "prb_grant.h"
#include "sched_consts.h"
#include "scheduler_dci.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/modulation.h"
#include "srsgnb/ran/ofdm_symbol_range.h"
#include "srsgnb/ran/pci.h"
#include "srsgnb/ran/prach/prach_preamble_format.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include "srsgnb/scheduler/dmrs.h"
#include <cstddef>

namespace srsgnb {

/// Maximum grants per slot. Implementation-specific.
const size_t MAX_GRANTS = 16;
/// Maximum Logical channels per TB. Implementation-specific.
const size_t MAX_LC_GRANTS = 4;
/// Maximum SSB opportunity per slot. This can be derived from the candidate ODFM symbols indices within the ranges
/// 0-13, 14-27, 28-41, 42-55, etc.. from TS 38.213, Section 4.1
const size_t MAX_SSB_PER_SLOT = 2;
/// [Implementation defined] This corresponds to "Number of search space sets per slot" in Table 13-11, TS 38.213.
constexpr size_t MAX_SIB1_PDUS_PER_SLOT = 2;
/// [Implementation defined] This corresponds to maximum number of RARs that can be scheduled per slot.
constexpr size_t MAX_RAR_PDUS_PER_SLOT = 16;
/// Maximum number of PRACH occasions within a slot as per TS38.211, Tables 6.3.3.2-[2-4].
const static size_t MAX_NOF_PRACHS_PER_SLOT = 7;

struct beamforming_info {
  // TODO
};

struct tx_power_pdcch_information {
  // TODO
};

struct dmrs_information {
  /// Bitmap of DM-RS position symbols.
  dmrs_symbol_mask dmrs_symb_pos;
  dmrs_config_type config_type;
  /// \brief DMRS-Scrambling-ID (see TS 38.211 sec 7.4.1.1.1) as provided by parameter \f$N^{n_{SCID}}_{ID}\f$.
  /// Values: (0..65535).
  unsigned dmrs_scrambling_id;
  /// PHY shall disregard this parameter if lowPaprDmrs=0.
  unsigned dmrs_scrambling_id_complement;
  /// False means that dmrs_scrambling_id == dmrs_scrambling_id_complement.
  bool low_papr_dmrs;
  /// \brief DMRS sequence initialization (see TS 38.221 sec 7.4.1.1.2), as provided by parameter n_{SCID}.
  /// Values: false -> 0, true -> 1.
  bool n_scid;
  /// Values: (1..3).
  uint8_t num_dmrs_cdm_grps_no_data;
  /// \brief Bitmap of antenna ports. Bit 0 corresponds to antenna port 1000 and bit 11 to antenna port 1011, and
  /// each bit=1 mean DM-RS port used.
  bounded_bitset<12> dmrs_ports;
};

struct dci_dl_context_information {
  const bwp_configuration*     bwp_cfg;
  const coreset_configuration* coreset_cfg;
  /// RNTI used to identify the destination of this DCI (e.g. UE, RA-RNTI, SI, Paging).
  rnti_t rnti;
  /// Parameter \f$n_{ID}\f$ used for PDCCH Data scrambling as per 3GPP TS 38.211 [2], sec 7.3.2.3. Values: (0..65535).
  /// For a UE-specific search space it equals the higherlayer parameter PDCCH-DMRS-Scrambling-ID if configured,
  /// otherwise it should be set to the phy cell ID.
  unsigned n_id_pdcch_data;
  /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling, as per 3GPP TS 38.211 [2], sec 7.3.2.3.
  /// Values: (0..65535). For a UE-specific search space where PDCCH-DMRSScrambling-ID is configured, this param
  /// equals the CRNTI. Otherwise, it should be set to 0.
  unsigned n_rnti_pdcch_data;
  /// CCE position of the allocated PDCCH.
  cce_position cces;
  /// Precoding and beamforming info used for this DCI.
  beamforming_info bf;
  /// Transmission power information used for this DCI.
  tx_power_pdcch_information tx_pwr;
};

/// PDCCH DL allocation.
struct pdcch_dl_information {
  /// Context associated with PDCCH allocation.
  dci_dl_context_information ctx;
  /// DL DCI unpacked content.
  dci_dl_info dci;
};

/// PDCCH UL allocation.
struct pdcch_ul_information {
  /// Context associated with PDCCH allocation.
  dci_dl_context_information ctx;
  /// UL DCI unpacked content.
  dci_ul_info dci;
};

/// MCS index table for PDSCH.
/// \remark See TS 38.214, Section 5.1.3.1.
enum class mcs_pdsch_table { qam64 = 0, qam256 = 1, qam64LowSE = 2 };

/// PDSCH codeword.
/// \remark See FAPI PDSCH PDU.
struct pdsch_codeword {
  /// Information bits per 1024 encoded bits. If the code rate is 379/1024, target_code_rate is set as 379. Refer to
  /// Tables 5.1.3.1-1, 5.1.3.1-2, 5.1.3.1-3, TS 38.214.
  float          target_code_rate;
  qam_modulation qam_mod;
  /// MCS index [3GPP TS 38.214, sec 5.1.3.1], should match value sent in DCI. Values (0..31).
  uint8_t         mcs_index;
  mcs_pdsch_table mcs_table;
  /// Redundancy version index [TS 38.212, Table 5.4.2.1-2 and TS 38.214, Table 5.1.2.1-2].
  uint8_t rv_index;
  /// Transport block size (in bytes) [TS 38.214, sec 5.1.3.2].
  uint32_t tb_size_bytes;
};

/// Information related to a PDSCH allocation.
/// \remark See FAPI PDSCH PDU.
struct pdsch_information {
  rnti_t                           rnti;
  const bwp_configuration*         bwp_cfg;
  prb_grant                        prbs;
  ofdm_symbol_range                symbols;
  static_vector<pdsch_codeword, 2> codewords;
  dmrs_information                 dmrs;
  /// Parameter n_ID, used for scrambling, as per TS 38.211, Section 7.3.1.1.
  unsigned n_id;
};

struct dl_msg_lc_info {
  /// LCID {0..32}
  lcid_t lcid;
  /// Number of scheduled bytes for this specific logical channel. {0..65535}
  unsigned sched_bytes;
};

struct dl_msg_tb_info {
  /// List of allocated logical channels
  static_vector<dl_msg_lc_info, MAX_LC_GRANTS> lc_lst;
};

/// Dedicated DL Grant for UEs
struct dl_msg_alloc {
  rnti_t                                        crnti;
  pdsch_information                             pdsch_cfg;
  static_vector<dl_msg_tb_info, MAX_NOF_LAYERS> tbs;
};

struct pusch_information {
  rnti_t                   rnti;
  const bwp_configuration* bwp_cfg;
  prb_grant                prbs;
  ofdm_symbol_range        symbols;
};

/// RAR grant.
/// \remark See TS 38.213, Table 8.2-1.
struct rar_ul_grant {
  // MAC subheader.
  uint16_t rapid;

  // RAR payload.
  uint16_t ta;
  rnti_t   temp_crnti;

  // RAR Payload.
  bool         freq_hop_flag;
  uint8_t      time_resource_assignment;
  prb_interval prbs;
  int8_t       mcs;
  int8_t       tpc;
  bool         csi_req;
};

/// See ORAN WG8, 9.2.3.3.10 - RAR information.
struct rar_information {
  pdcch_dl_information*                   pdcch_cfg;
  pdsch_information                       pdsch_cfg;
  static_vector<rar_ul_grant, MAX_GRANTS> grants;
};

/// Stores the information associated to an SSB.
struct ssb_information {
  unsigned          ssb_index;
  crb_interval      crbs;
  ofdm_symbol_range symbols;
};

/// Stores the information associated to an SIB1 or other SI allocation.
struct sib_information {
  enum si_indicator_type { sib1, other_si } si_indicator;
  unsigned                    nof_txs;
  const pdcch_dl_information* pdcch_cfg;
  pdsch_information           pdsch_cfg;
};

/// See ORAN WG8, 9.2.3.3.12 - Downlink Broadcast Allocation.
struct dl_broadcast_allocation {
  static_vector<ssb_information, MAX_SSB_PER_SLOT>       ssb_info;
  static_vector<sib_information, MAX_SIB1_PDUS_PER_SLOT> sibs;
};

struct dl_sched_result {
  /// Allocated DL PDCCHs. Includes both SIB, RAR and Data PDCCHs.
  static_vector<pdcch_dl_information, MAX_GRANTS> dl_pdcchs;

  /// Allocated UL PDCCHs.
  static_vector<pdcch_ul_information, MAX_GRANTS> ul_pdcchs;

  /// Allocation of SSB and SIBs
  dl_broadcast_allocation bc;

  /// Allocation of dedicated RARs
  static_vector<rar_information, MAX_GRANTS> rar_grants;

  /// Allocation of dedicated UE messages
  static_vector<dl_msg_alloc, MAX_GRANTS> ue_grants;
};

struct ul_sched_info {
  rnti_t            crnti;
  pusch_information pusch_cfg;
};

struct prach_occasion_info {
  /// Number of time-domain PRACH occasions (N^{RAslot}_t), as per TS38.211 Tables 6.3.3.2-[2-4].
  uint8_t nof_prach_occasions;
  /// RACH format information for the PRACH occasions.
  preamble_format format;
  /// Frequency domain occasion indexes, which ranges from 0 to M-1, where M is the higher-layer parameter msg1-FDM,
  /// which can take the values {1,2,4,8}. Possible values: {0,...7}.
  /// \remark See TS 38.211, sec 6.3.3.2.
  interval<uint8_t> fd_ra_resources;
  /// Starting symbol for the first PRACH TD occasion.
  /// \remark See TS38.211, sec 6.3.3.2 and Tables 6.3.3.2-2 and 6.3.3.2-4. Possible values: {0,...,13}.
  uint8_t start_symbol;
  /// N-CS configuration as per TS38.211, Table 6.3.3.1-5. Possible values: {0,...,419}.
  uint16_t nof_cs;
  /// Values: {0,...,255}.
  uint8_t prach_config_index;
  /// Start of preamble logical index to monitor the PRACH occasions in this slot. Values: {0,...63}.
  uint8_t start_preamble_index;
  /// Number of preamble logical indices. Values: {1,...,64}.
  uint8_t nof_preamble_indexes;
};

struct ul_sched_result {
  /// PUSCH grants allocated in the current slot.
  static_vector<ul_sched_info, MAX_GRANTS> puschs;
  /// PRACH occasions within the given slot.
  static_vector<prach_occasion_info, MAX_NOF_PRACHS_PER_SLOT> prachs;
};

/// Scheduler decision made for DL and UL in a given slot.
struct sched_result {
  dl_sched_result dl;
  ul_sched_result ul;
};

class scheduler_slot_handler
{
public:
  virtual ~scheduler_slot_handler()                                                         = default;
  virtual const sched_result* slot_indication(slot_point sl_tx, du_cell_index_t cell_index) = 0;
};

} // namespace srsgnb
