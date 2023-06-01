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

#include "../support/uplink_cplane_context_repository.h"
#include "srsran/adt/expected.h"
#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/ofh/ofh_uplane_message_decoder.h"

namespace srsran {
namespace ofh {

/// User-Plane uplink packet handler configuration.
struct uplane_uplink_packet_handler_config {
  uplane_uplink_packet_handler_config(srslog::basic_logger& logger_, uplink_cplane_context_repository& cplane_repo_) :
    logger(logger_), cplane_repo(cplane_repo_)
  {
  }

  /// Logger.
  srslog::basic_logger& logger;
  /// PRACH Control-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// Uplink Control-Plane context repository.
  uplink_cplane_context_repository& cplane_repo;
  /// VLAN ethernet frame parameters.
  ether::vlan_frame_params vlan_params;
  /// RU PRACH port.
  unsigned ru_prach_port;
  /// RU uplink data port.
  unsigned ru_ul_data_port;
  /// User-Plane message decoder.
  std::unique_ptr<uplane_message_decoder> uplane_decoder;
  /// eCPRI packet decoder.
  std::unique_ptr<ecpri::packet_decoder> ecpri_decoder;
  /// Ethernet frame decoder.
  std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
};

/// Open Fronthaul uplink packet handler.
class uplane_uplink_packet_handler
{
public:
  explicit uplane_uplink_packet_handler(uplane_uplink_packet_handler_config&& config);

  // Decodes the given packet and returns the results.
  expected<uplane_message_decoder_results> decode_packet(span<const uint8_t> packet);

private:
  /// Returns true if the ethernet frame represented by the given eth parameters should be filtered, otherwise false.
  bool should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const;

  /// Returns true if the eCPRI packet represented by the given eCPRI parameters should be filtered, otherwise false.
  bool should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const;

  /// Returns true if the User-Plane packet represented by the given User-Plane results should be filtered, otherwise
  /// false.
  bool should_uplane_packet_be_filtered(const uplane_message_decoder_results& uplane_results) const;

private:
  srslog::basic_logger&                      logger;
  const bool                                 is_prach_cp_enabled;
  uplink_cplane_context_repository&          cplane_repo;
  ether::vlan_frame_params                   vlan_params;
  const unsigned                             ru_prach_port;
  const unsigned                             ru_ul_data_port;
  std::unique_ptr<uplane_message_decoder>    uplane_decoder;
  std::unique_ptr<ecpri::packet_decoder>     ecpri_decoder;
  std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
};

} // namespace ofh
} // namespace srsran
