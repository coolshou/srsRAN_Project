/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_sdr_factories.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_du/split_helpers/flexible_du_configs.h"
#include "ru_sdr_config_translator.h"
#include "srsran/ru/ru_generic_factory.h"

using namespace srsran;

std::unique_ptr<radio_unit> srsran::create_sdr_radio_unit(const ru_sdr_unit_config&          ru_cfg,
                                                          const flexible_du_ru_config&       ru_config,
                                                          const flexible_du_ru_dependencies& ru_dependencies)
{
  ru_generic_configuration config = generate_ru_sdr_config(ru_cfg, ru_config.du_cells, ru_config.max_processing_delay);

  config.rf_logger                   = &srslog::fetch_basic_logger("RF");
  config.radio_exec                  = ru_dependencies.workers.radio_exec;
  config.statistics_printer_executor = ru_dependencies.workers.ru_printer_exec;
  config.timing_notifier             = &ru_dependencies.timing_notifier;
  config.symbol_notifier             = &ru_dependencies.symbol_notifier;

  for (unsigned i = 0, e = config.lower_phy_config.size(); i != e; ++i) {
    lower_phy_configuration& low_phy_cfg = config.lower_phy_config[i];
    low_phy_cfg.logger                   = &srslog::fetch_basic_logger("PHY");
    low_phy_cfg.tx_task_executor         = ru_dependencies.workers.lower_phy_tx_exec[i];
    low_phy_cfg.rx_task_executor         = ru_dependencies.workers.lower_phy_rx_exec[i];
    low_phy_cfg.dl_task_executor         = ru_dependencies.workers.lower_phy_dl_exec[i];
    low_phy_cfg.ul_task_executor         = ru_dependencies.workers.lower_phy_ul_exec[i];
    low_phy_cfg.prach_async_executor     = ru_dependencies.workers.lower_prach_exec[i];

    low_phy_cfg.logger->set_level(ru_cfg.loggers.phy_level);
  }

  return create_generic_ru(config);
}
