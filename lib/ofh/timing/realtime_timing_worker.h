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

#include "srsran/ofh/ofh_controller.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"
#include "srsran/support/executors/task_executor.h"
#include <atomic>

namespace srsran {
namespace ofh {

class ota_symbol_boundary_notifier;

/// Configuration for the realtime timing worker.
struct realtime_worker_cfg {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// GPS Alpha.
  unsigned gps_Alpha;
  /// GPS Beta.
  int gps_Beta;
};

/// Realtime worker that generates OTA symbol notifications.
class realtime_timing_worker : public controller
{
  srslog::basic_logger&                          logger;
  ota_symbol_boundary_notifier&                  notifier;
  task_executor&                                 executor;
  const subcarrier_spacing                       scs;
  const unsigned                                 nof_symbols_per_slot;
  const unsigned                                 nof_symbols_per_sec;
  const std::chrono::duration<double, std::nano> symbol_duration;
  const std::chrono::nanoseconds                 sleep_time;
  unsigned                                       previous_symb_index = 0;
  std::atomic<bool>                              is_stop_requested{false};

public:
  realtime_timing_worker(srslog::basic_logger&         logger_,
                         ota_symbol_boundary_notifier& notifier_,
                         task_executor&                executor_,
                         const realtime_worker_cfg&    cfg);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  /// Main timing loop.
  void timing_loop();

  /// Polls the system time checking for the start of a new OTA symbol.
  void poll();
};

} // namespace ofh
} // namespace srsran
