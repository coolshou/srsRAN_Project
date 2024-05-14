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

#include "cu_up_unit_logger_config.h"
#include "srsran/ran/five_qi.h"

namespace srsran {

/// Metrics configuration.
struct cu_up_unit_metrics_config {
  /// Statistics report period in seconds.
  unsigned cu_up_statistics_report_period = 1;
  struct pdcp_metrics {
    unsigned report_period = 0; // PDCP report period in ms
  } pdcp;
};

struct cu_up_unit_amf_config {
  std::string ip_addr                = "127.0.0.1";
  uint16_t    port                   = 38412;
  std::string bind_addr              = "127.0.0.1";
  std::string n2_bind_addr           = "auto";
  std::string n2_bind_interface      = "auto";
  std::string n3_bind_addr           = "auto";
  std::string n3_bind_interface      = "auto";
  std::string n3_ext_addr            = "auto";
  int         sctp_rto_initial       = 120;
  int         sctp_rto_min           = 120;
  int         sctp_rto_max           = 500;
  int         sctp_init_max_attempts = 3;
  int         sctp_max_init_timeo    = 500;
  bool        sctp_nodelay           = false;
  int         udp_rx_max_msgs        = 256;
  bool        no_core                = false;
};

/// QoS configuration.
struct cu_up_unit_qos_config {
  five_qi_t five_qi = uint_to_five_qi(9);
};

/// CU-UP application unit configuration.
struct cu_up_unit_config {
  unsigned gtpu_queue_size          = 2048;
  unsigned gtpu_reordering_timer_ms = 0;
  bool     warn_on_drop             = false;
  /// AMF configuration.
  cu_up_unit_amf_config amf_cfg;
  /// Metrics.
  cu_up_unit_metrics_config metrics;
  /// Loggers.
  cu_up_unit_logger_config loggers;
  /// QoS configuration.
  std::vector<cu_up_unit_qos_config> qos_cfg;
};

} // namespace srsran
