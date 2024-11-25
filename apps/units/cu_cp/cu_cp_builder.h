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

#include "apps/services/application_command.h"
#include "apps/services/e2/e2_metric_connector_manager.h"
#include "cu_cp_wrapper.h"
#include "srsran/cu_cp/cu_cp.h"
#include "srsran/e2/e2_cu_metrics_connector.h"

namespace srsran {

class dlt_pcap;
class io_broker;
struct cu_cp_unit_config;
struct worker_manager;

namespace srs_cu_cp {
class n2_connection_client;
}

template <typename ConnectorType, typename NotifierType, typename InterfaceType>
class e2_metric_connector_manager;
class e2_connection_client;
class e2_gateway_remote_connector;

/// CU-CP build dependencies.
struct cu_cp_build_dependencies {
  task_executor*        cu_cp_executor = nullptr;
  task_executor*        cu_cp_e2_exec  = nullptr;
  timer_manager*        timers         = nullptr;
  dlt_pcap*             ngap_pcap      = nullptr;
  io_broker*            broker         = nullptr;
  e2_connection_client* e2_gw          = nullptr;
};

/// Wraps the CU-CP and its supported application commands.
struct cu_cp_unit {
  std::unique_ptr<cu_cp_wrapper>                                  unit;
  std::vector<std::unique_ptr<app_services::application_command>> commands;
};

/// Builds a CU-CP object with the given configuration.
cu_cp_unit build_cu_cp(const cu_cp_unit_config& cu_cp_unit_cfg, cu_cp_build_dependencies& dependencies);

} // namespace srsran
