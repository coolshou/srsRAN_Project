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

#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "logging_pdsch_processor_decorator.h"
#include "pdsch_encoder_hw_impl.h"
#include "pdsch_encoder_impl.h"
#include "pdsch_modulator_impl.h"
#include "pdsch_processor_asynchronous_pool.h"
#include "pdsch_processor_concurrent_impl.h"
#include "pdsch_processor_impl.h"
#include "pdsch_processor_lite_impl.h"
#include "pdsch_processor_pool.h"
#include "pdsch_processor_validator_impl.h"
#include <memory>

using namespace srsran;

namespace {

class pdsch_encoder_factory_sw : public pdsch_encoder_factory
{
private:
  std::shared_ptr<ldpc_encoder_factory>      encoder_factory;
  std::shared_ptr<ldpc_rate_matcher_factory> rate_matcher_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory;

public:
  explicit pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config) :
    encoder_factory(std::move(config.encoder_factory)),
    rate_matcher_factory(std::move(config.rate_matcher_factory)),
    segmenter_factory(std::move(config.segmenter_factory))
  {
    srsran_assert(encoder_factory, "Invalid LDPC encoder factory.");
    srsran_assert(rate_matcher_factory, "Invalid LDPC RM factory.");
    srsran_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
  }

  std::unique_ptr<pdsch_encoder> create() override
  {
    return std::make_unique<pdsch_encoder_impl>(
        segmenter_factory->create(), encoder_factory->create(), rate_matcher_factory->create());
  }
};

/// HW-accelerated PDSCH encoder factory.
class pdsch_encoder_factory_hw : public pdsch_encoder_factory
{
private:
  std::shared_ptr<crc_calculator_factory>                crc_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory>             segmenter_factory;
  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory;

public:
  explicit pdsch_encoder_factory_hw(const pdsch_encoder_factory_hw_configuration& config) :
    crc_factory(std::move(config.crc_factory)),
    segmenter_factory(std::move(config.segmenter_factory)),
    hw_encoder_factory(std::move(config.hw_encoder_factory))
  {
    srsran_assert(crc_factory, "Invalid CRC factory.");
    srsran_assert(segmenter_factory, "Invalid LDPC segmenter factory.");
    srsran_assert(hw_encoder_factory, "Invalid hardware accelerator factory.");
  }

  std::unique_ptr<pdsch_encoder> create() override
  {
    pdsch_encoder_hw_impl::sch_crc crc = {
        crc_factory->create(crc_generator_poly::CRC16),
        crc_factory->create(crc_generator_poly::CRC24A),
        crc_factory->create(crc_generator_poly::CRC24B),
    };
    return std::make_unique<pdsch_encoder_hw_impl>(crc, segmenter_factory->create(), hw_encoder_factory->create());
  }
};

class pdsch_modulator_factory_sw : public pdsch_modulator_factory
{
private:
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pdsch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                             std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(std::move(modulator_factory_)), prg_factory(std::move(prg_factory_))
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pdsch_modulator> create() override
  {
    return std::make_unique<pdsch_modulator_impl>(modulator_factory->create_modulation_mapper(), prg_factory->create());
  }
};

class pdsch_processor_factory_sw : public pdsch_processor_factory
{
private:
  std::shared_ptr<pdsch_encoder_factory>        encoder_factory;
  std::shared_ptr<pdsch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory;

public:
  pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory_,
                             std::shared_ptr<pdsch_modulator_factory>      modulator_factory_,
                             std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory_) :
    encoder_factory(std::move(encoder_factory_)),
    modulator_factory(std::move(modulator_factory_)),
    dmrs_factory(std::move(dmrs_factory_))
  {
    srsran_assert(encoder_factory, "Invalid encoder factory.");
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(dmrs_factory, "Invalid DM-RS factory.");
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    return std::make_unique<pdsch_processor_impl>(
        encoder_factory->create(), modulator_factory->create(), dmrs_factory->create());
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override
  {
    return std::make_unique<pdsch_processor_validator_impl>();
  }
};

class pdsch_processor_concurrent_factory_sw : public pdsch_processor_factory
{
public:
  pdsch_processor_concurrent_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                                        std::shared_ptr<ldpc_encoder_factory>            encoder_factory,
                                        std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory,
                                        std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                        std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                        std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory,
                                        task_executor&                                   executor_,
                                        unsigned                                         nof_concurrent_threads) :
    prg_factory(std::move(prg_factory_)), executor(executor_)
  {
    srsran_assert(crc_factory, "Invalid CRC calculator factory.");
    srsran_assert(encoder_factory, "Invalid encoder factory.");
    srsran_assert(rate_matcher_factory, "Invalid rate matcher factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(dmrs_factory, "Invalid DM-RS factory.");
    srsran_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");

    // Create vector of codeblock processors.
    std::vector<std::unique_ptr<pdsch_codeblock_processor>> cb_processors;
    for (unsigned i_encoder = 0; i_encoder != nof_concurrent_threads; ++i_encoder) {
      cb_processors.emplace_back(
          std::make_unique<pdsch_codeblock_processor>(crc_factory->create(crc_generator_poly::CRC24A),
                                                      crc_factory->create(crc_generator_poly::CRC24B),
                                                      crc_factory->create(crc_generator_poly::CRC16),
                                                      encoder_factory->create(),
                                                      rate_matcher_factory->create(),
                                                      prg_factory->create(),
                                                      modulator_factory->create_modulation_mapper()));
    }

    // Create pool of codeblock processors. It is common for all PDSCH processors.
    cb_processor_pool =
        std::make_shared<pdsch_processor_concurrent_impl::codeblock_processor_pool>(std::move(cb_processors));

    // Create vector of PDSCH DM-RS generators.
    std::vector<std::unique_ptr<dmrs_pdsch_processor>> dmrs_generators;
    for (unsigned i_encoder = 0; i_encoder != nof_concurrent_threads; ++i_encoder) {
      dmrs_generators.emplace_back(dmrs_factory->create());
    }

    // Create pool of PDSCH DM-RS generators. It is common for all PDSCH processors.
    dmrs_generator_pool =
        std::make_shared<pdsch_processor_concurrent_impl::pdsch_dmrs_generator_pool>(std::move(dmrs_generators));
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    return std::make_unique<pdsch_processor_concurrent_impl>(
        cb_processor_pool, prg_factory->create(), dmrs_generator_pool, ptrs_generator_pool, executor);
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override
  {
    return std::make_unique<pdsch_processor_validator_impl>();
  }

private:
  std::shared_ptr<pseudo_random_generator_factory>                            prg_factory;
  task_executor&                                                              executor;
  std::shared_ptr<pdsch_processor_concurrent_impl::codeblock_processor_pool>  cb_processor_pool;
  std::shared_ptr<pdsch_processor_concurrent_impl::pdsch_dmrs_generator_pool> dmrs_generator_pool;
  std::shared_ptr<pdsch_processor_concurrent_impl::pdsch_ptrs_generator_pool> ptrs_generator_pool;
};

class pdsch_processor_lite_factory_sw : public pdsch_processor_factory
{
private:
  std::shared_ptr<ldpc_segmenter_tx_factory>       segmenter_factory;
  std::shared_ptr<ldpc_encoder_factory>            encoder_factory;
  std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory;
  std::shared_ptr<pseudo_random_generator_factory> scrambler_factory;
  std::shared_ptr<channel_modulation_factory>      modulator_factory;
  std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory;

public:
  pdsch_processor_lite_factory_sw(std::shared_ptr<ldpc_segmenter_tx_factory>       segmenter_factory_,
                                  std::shared_ptr<ldpc_encoder_factory>            encoder_factory_,
                                  std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory_,
                                  std::shared_ptr<pseudo_random_generator_factory> scrambler_factory_,
                                  std::shared_ptr<channel_modulation_factory>      modulator_factory_,
                                  std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory_) :
    segmenter_factory(std::move(segmenter_factory_)),
    encoder_factory(std::move(encoder_factory_)),
    rate_matcher_factory(std::move(rate_matcher_factory_)),
    scrambler_factory(std::move(scrambler_factory_)),
    modulator_factory(std::move(modulator_factory_)),
    dmrs_factory(std::move(dmrs_factory_))
  {
    srsran_assert(segmenter_factory, "Invalid segmenter factory.");
    srsran_assert(encoder_factory, "Invalid encoder factory.");
    srsran_assert(rate_matcher_factory, "Invalid rate matcher factory.");
    srsran_assert(scrambler_factory, "Invalid scrambler factory.");
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(dmrs_factory, "Invalid DM-RS factory.");
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    return std::make_unique<pdsch_processor_lite_impl>(segmenter_factory->create(),
                                                       encoder_factory->create(),
                                                       rate_matcher_factory->create(),
                                                       scrambler_factory->create(),
                                                       modulator_factory->create_modulation_mapper(),
                                                       dmrs_factory->create());
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override
  {
    return std::make_unique<pdsch_processor_validator_impl>();
  }
};

class pdsch_processor_asynchronous_pool_factory : public pdsch_processor_factory
{
public:
  pdsch_processor_asynchronous_pool_factory(std::shared_ptr<pdsch_processor_factory> factory_,
                                            unsigned                                 max_nof_processors_) :
    factory(std::move(factory_)), max_nof_processors(max_nof_processors_)
  {
    srsran_assert(factory, "Invalid PDSCH processor factory.");
    srsran_assert(max_nof_processors >= 1,
                  "The number of processors (i.e., {}) must be greater than or equal to one.",
                  max_nof_processors);
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    // Create processors.
    std::vector<std::unique_ptr<pdsch_processor>> processors(max_nof_processors);
    for (std::unique_ptr<pdsch_processor>& processor : processors) {
      processor = factory->create();
    }

    return std::make_unique<pdsch_processor_asynchronous_pool>(processors, false);
  }

  std::unique_ptr<pdsch_processor> create(srslog::basic_logger& logger, bool enable_logging_broadcast) override
  {
    // Create processors with logging.
    std::vector<std::unique_ptr<pdsch_processor>> processors(max_nof_processors);
    for (std::unique_ptr<pdsch_processor>& processor : processors) {
      processor = factory->create(logger, enable_logging_broadcast);
    }

    return std::make_unique<pdsch_processor_asynchronous_pool>(processors, false);
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<pdsch_processor_factory> factory;
  unsigned                                 max_nof_processors;
};

class pdsch_processor_pool_factory : public pdsch_processor_factory
{
public:
  pdsch_processor_pool_factory(std::shared_ptr<pdsch_processor_factory> factory_, unsigned max_nof_processors_) :
    factory(std::move(factory_)), max_nof_processors(max_nof_processors_)
  {
    srsran_assert(factory, "Invalid PDSCH processor factory.");
    srsran_assert(max_nof_processors != 0, "The number of processors must not be zero.");
  }

  std::unique_ptr<pdsch_processor> create() override
  {
    // Creates the processors without logging.
    if (!processors) {
      // Create PDSCH processsor instances.
      std::vector<std::unique_ptr<pdsch_processor>> instances(max_nof_processors);
      std::generate(instances.begin(), instances.end(), [this]() { return factory->create(); });

      // Create pool.
      processors = std::make_shared<pdsch_processor_pool::pdsch_processor_pool_type>(std::move(instances));
    }

    return std::make_unique<pdsch_processor_pool>(processors);
  }

  std::unique_ptr<pdsch_processor> create(srslog::basic_logger& logger, bool enable_logging_broadcast) override
  {
    // Creates the processors with logging.
    if (!processors) {
      // Create PDSCH processor instances.
      std::vector<std::unique_ptr<pdsch_processor>> instances(max_nof_processors);
      std::generate(instances.begin(), instances.end(), [this, &logger, &enable_logging_broadcast]() {
        return factory->create(logger, enable_logging_broadcast);
      });

      // Create pool.
      processors = std::make_shared<pdsch_processor_pool::pdsch_processor_pool_type>(std::move(instances));
    }

    return std::make_unique<pdsch_processor_pool>(processors);
  }

  std::unique_ptr<pdsch_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<pdsch_processor_factory>                         factory;
  std::shared_ptr<pdsch_processor_pool::pdsch_processor_pool_type> processors;
  unsigned                                                         max_nof_processors;
};
} // namespace

std::shared_ptr<pdsch_encoder_factory>
srsran::create_pdsch_encoder_factory_sw(pdsch_encoder_factory_sw_configuration& config)
{
  return std::make_shared<pdsch_encoder_factory_sw>(config);
}

std::shared_ptr<pdsch_encoder_factory>
srsran::create_pdsch_encoder_factory_hw(const pdsch_encoder_factory_hw_configuration& config)
{
  return std::make_shared<pdsch_encoder_factory_hw>(config);
}

std::shared_ptr<pdsch_modulator_factory>
srsran::create_pdsch_modulator_factory_sw(std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                          std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pdsch_modulator_factory_sw>(std::move(modulator_factory), std::move(prg_factory));
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>        encoder_factory,
                                          std::shared_ptr<pdsch_modulator_factory>      modulator_factory,
                                          std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_factory)
{
  return std::make_shared<pdsch_processor_factory_sw>(
      std::move(encoder_factory), std::move(modulator_factory), std::move(dmrs_factory));
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_concurrent_processor_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                                                     std::shared_ptr<ldpc_encoder_factory>            ldpc_enc_factory,
                                                     std::shared_ptr<ldpc_rate_matcher_factory>       ldpc_rm_factory,
                                                     std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                                     std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                                     std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory,
                                                     task_executor&                                   executor,
                                                     unsigned nof_concurrent_threads)
{
  return std::make_shared<pdsch_processor_concurrent_factory_sw>(std::move(crc_factory),
                                                                 std::move(ldpc_enc_factory),
                                                                 std::move(ldpc_rm_factory),
                                                                 std::move(prg_factory),
                                                                 std::move(modulator_factory),
                                                                 std::move(dmrs_factory),
                                                                 executor,
                                                                 nof_concurrent_threads);
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_lite_processor_factory_sw(std::shared_ptr<ldpc_segmenter_tx_factory>       segmenter_factory,
                                               std::shared_ptr<ldpc_encoder_factory>            encoder_factory,
                                               std::shared_ptr<ldpc_rate_matcher_factory>       rate_matcher_factory,
                                               std::shared_ptr<pseudo_random_generator_factory> scrambler_factory,
                                               std::shared_ptr<channel_modulation_factory>      modulator_factory,
                                               std::shared_ptr<dmrs_pdsch_processor_factory>    dmrs_factory)
{
  return std::make_shared<pdsch_processor_lite_factory_sw>(std::move(segmenter_factory),
                                                           std::move(encoder_factory),
                                                           std::move(rate_matcher_factory),
                                                           std::move(scrambler_factory),
                                                           std::move(modulator_factory),
                                                           std::move(dmrs_factory));
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_processor_asynchronous_pool(std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory,
                                                 unsigned                                 max_nof_processors)
{
  return std::make_shared<pdsch_processor_asynchronous_pool_factory>(std::move(pdsch_proc_factory), max_nof_processors);
}

std::shared_ptr<pdsch_processor_factory>
srsran::create_pdsch_processor_pool(std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory,
                                    unsigned                                 max_nof_processors)
{
  return std::make_shared<pdsch_processor_pool_factory>(std::move(pdsch_proc_factory), max_nof_processors);
}

std::unique_ptr<pdsch_processor> pdsch_processor_factory::create(srslog::basic_logger& logger,
                                                                 bool                  enable_logging_broadcast)
{
  return std::make_unique<logging_pdsch_processor_decorator>(logger, enable_logging_broadcast, create());
}