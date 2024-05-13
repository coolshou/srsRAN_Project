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

#include "../test_utils/config_generators.h"
#include "../test_utils/dummy_test_components.h"
#include "lib/scheduler/config/sched_config_manager.h"
#include "lib/scheduler/pdcch_scheduling/pdcch_resource_allocator_impl.h"
#include "lib/scheduler/pucch_scheduling/pucch_allocator_impl.h"
#include "lib/scheduler/uci_scheduling/uci_allocator_impl.h"
#include "lib/scheduler/ue_scheduling/ue.h"
#include "lib/scheduler/ue_scheduling/ue_cell_grid_allocator.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/pdcch/search_space.h"
#include <gtest/gtest.h>

using namespace srsran;

class ue_grid_allocator_tester : public ::testing::Test
{
protected:
  ue_grid_allocator_tester() :
    expert_cfg([this]() {
      auto ue_expert_cfg                   = sched_cfg.ue;
      ue_expert_cfg.max_ul_grants_per_slot = 2;
      ue_expert_cfg.max_pucchs_per_slot    = 2;
      return ue_expert_cfg;
    }()),
    cell_cfg(*[this]() {
      cfg_builder_params.dl_arfcn       = 536020;
      cfg_builder_params.channel_bw_mhz = bs_channel_bandwidth_fr1::MHz20;
      auto* cfg = cfg_mng.add_cell(test_helpers::make_default_sched_cell_configuration_request(cfg_builder_params));
      srsran_assert(cfg != nullptr, "Cell configuration failed");
      return cfg;
    }())
  {
    // Initialize resource grid.
    res_grid.slot_indication({cfg_builder_params.scs_common, 0});
    pdcch_alloc.slot_indication({cfg_builder_params.scs_common, 0});
    pucch_alloc.slot_indication({cfg_builder_params.scs_common, 0});
    uci_alloc.slot_indication({cfg_builder_params.scs_common, 0});

    alloc.add_cell(to_du_cell_index(0), pdcch_alloc, uci_alloc, res_grid);
  }

  ue& add_ue(du_ue_index_t ue_index, const std::initializer_list<lcid_t>& lcids_to_activate)
  {
    sched_ue_creation_request_message ue_creation_req =
        test_helpers::create_default_sched_ue_creation_request(cfg_builder_params);
    ue_creation_req.ue_index = ue_index;
    ue_creation_req.crnti    = to_rnti(0x4601 + (unsigned)ue_index);
    for (lcid_t lcid : lcids_to_activate) {
      ue_creation_req.cfg.lc_config_list->push_back(config_helpers::create_default_logical_channel_config(lcid));
    }

    return add_ue(ue_creation_req);
  }

  ue& add_ue(const sched_ue_creation_request_message& ue_creation_req)
  {
    auto ev = cfg_mng.add_ue(ue_creation_req);
    ues.add_ue(std::make_unique<ue>(
        ue_creation_command{ev.next_config(), ue_creation_req.starts_in_fallback, harq_timeout_handler}));
    return ues[ue_creation_req.ue_index];
  }

  scheduler_expert_config                 sched_cfg = config_helpers::make_default_scheduler_expert_config();
  scheduler_ue_expert_config              expert_cfg{sched_cfg.ue};
  sched_cfg_dummy_notifier                mac_notif;
  scheduler_ue_metrics_dummy_notifier     metrics_notif;
  scheduler_ue_metrics_dummy_configurator metrics_ue_handler;
  scheduler_harq_timeout_dummy_handler    harq_timeout_handler;

  cell_config_builder_params cfg_builder_params;
  sched_config_manager       cfg_mng{scheduler_config{sched_cfg, mac_notif, metrics_notif}, metrics_ue_handler};
  const cell_configuration&  cell_cfg;

  cell_resource_allocator res_grid{cell_cfg};

  pdcch_resource_allocator_impl pdcch_alloc{cell_cfg};
  pucch_allocator_impl pucch_alloc{cell_cfg, expert_cfg.max_pucchs_per_slot, expert_cfg.max_ul_grants_per_slot};
  uci_allocator_impl   uci_alloc{pucch_alloc};

  ue_repository          ues;
  ue_cell_grid_allocator alloc{expert_cfg, ues, srslog::fetch_basic_logger("SCHED")};
};

TEST_F(ue_grid_allocator_tester, when_coreset0_grant_inside_coreset0_rb_lims_then_allocation_is_successful)
{
  ue& u = add_ue(to_du_ue_index(0), {});

  ue_pdsch_grant grant{.user           = &u,
                       .cell_index     = to_du_cell_index(0),
                       .h_id           = to_harq_id(0),
                       .ss_id          = to_search_space_id(0),
                       .time_res_index = 0,
                       .crbs           = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs(),
                       .aggr_lvl       = aggregation_level::n4};

  ASSERT_EQ(alloc.allocate_dl_grant(grant), alloc_outcome::success);
}

TEST_F(ue_grid_allocator_tester,
       when_grant_inside_coreset_start_and_coreset0_end_rb_lims_for_css_then_allocation_is_successful)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change SS type to common.
  (*ue_creation_req.cfg.cells)[0]
      .serv_cell_cfg.init_dl_bwp.pdcch_cfg->search_spaces[0]
      .set_non_ss0_monitored_dci_formats(search_space_configuration::common_dci_format{.f0_0_and_f1_0 = true});
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);

  ue& u = add_ue(ue_creation_req);

  const crb_interval crbs =
      get_coreset_crbs((*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdcch_cfg.value().coresets.back());

  ue_pdsch_grant grant{
      .user           = &u,
      .cell_index     = to_du_cell_index(0),
      .h_id           = to_harq_id(0),
      .ss_id          = to_search_space_id(2),
      .time_res_index = 0,
      .crbs           = {crbs.start(),
                         crbs.start() + cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().length()},
      .aggr_lvl       = aggregation_level::n4};

  ASSERT_EQ(alloc.allocate_dl_grant(grant), alloc_outcome::success);
}

TEST_F(ue_grid_allocator_tester, when_using_fallback_dci_format_only_64_qam_mcs_table_is_used)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  (*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = srsran::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                       = to_du_ue_index(0);
  ue_creation_req.crnti                                                          = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#1 uses fallback DCI format hence the 64QAM MCS table must be used.
  const ue_pdsch_grant grant{.user           = &u,
                             .cell_index     = to_du_cell_index(0),
                             .h_id           = to_harq_id(0),
                             .ss_id          = to_search_space_id(0),
                             .time_res_index = 0,
                             .crbs     = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs(),
                             .aggr_lvl = aggregation_level::n4};

  ASSERT_EQ(alloc.allocate_dl_grant(grant), alloc_outcome::success);
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam64);
}

TEST_F(ue_grid_allocator_tester, when_using_non_fallback_dci_format_use_mcs_table_set_in_pdsch_cfg)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  // Change PDSCH MCS table to be used when using non-fallback DCI format.
  (*ue_creation_req.cfg.cells)[0].serv_cell_cfg.init_dl_bwp.pdsch_cfg->mcs_table = srsran::pdsch_mcs_table::qam256;
  ue_creation_req.ue_index                                                       = to_du_ue_index(0);
  ue_creation_req.crnti                                                          = to_rnti(0x4601);

  const ue& u = add_ue(ue_creation_req);

  // SearchSpace#2 uses non-fallback DCI format hence the MCS table set in dedicated PDSCH configuration must be used.
  const ue_pdsch_grant grant{.user           = &u,
                             .cell_index     = to_du_cell_index(0),
                             .h_id           = to_harq_id(0),
                             .ss_id          = to_search_space_id(2),
                             .time_res_index = 0,
                             .crbs     = cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs(),
                             .aggr_lvl = aggregation_level::n4};

  ASSERT_EQ(alloc.allocate_dl_grant(grant), alloc_outcome::success);
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.codewords.back().mcs_table,
            srsran::pdsch_mcs_table::qam256);
}

TEST_F(ue_grid_allocator_tester, remaining_dl_rbs_are_allocated_if_max_pucch_per_slot_is_reached)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);
  ue_creation_req.ue_index = to_du_ue_index(1);
  ue_creation_req.crnti    = to_rnti(0x4602);
  const ue& u2             = add_ue(ue_creation_req);

  const crb_interval   grant1_crbs = {cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().start(),
                                      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().start() +
                                          2};
  const ue_pdsch_grant grant1{.user           = &u1,
                              .cell_index     = to_du_cell_index(0),
                              .h_id           = to_harq_id(0),
                              .ss_id          = to_search_space_id(2),
                              .time_res_index = 0,
                              .crbs           = grant1_crbs,
                              .aggr_lvl       = aggregation_level::n4};

  // Successfully allocates RBs corresponding to the grant.
  ASSERT_EQ(alloc.allocate_dl_grant(grant1), alloc_outcome::success);
  ASSERT_EQ(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().length(), grant1_crbs.length());

  const crb_interval grant2_crbs = {
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().start() + 2,
      cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0->coreset0_crbs().start() + 4};
  const ue_pdsch_grant grant2{.user           = &u2,
                              .cell_index     = to_du_cell_index(0),
                              .h_id           = to_harq_id(0),
                              .ss_id          = to_search_space_id(2),
                              .time_res_index = 0,
                              .crbs           = grant2_crbs,
                              .aggr_lvl       = aggregation_level::n4};

  // Allocates all remaining RBs to UE2.
  ASSERT_EQ(alloc.allocate_dl_grant(grant2), alloc_outcome::success);
  ASSERT_GT(res_grid[0].result.dl.ue_grants.back().pdsch_cfg.rbs.type1().length(), grant2_crbs.length());
}

TEST_F(ue_grid_allocator_tester, remaining_ul_rbs_are_allocated_if_max_ul_grant_per_slot_is_reached)
{
  sched_ue_creation_request_message ue_creation_req =
      test_helpers::create_default_sched_ue_creation_request(this->cfg_builder_params);
  ue_creation_req.ue_index = to_du_ue_index(0);
  ue_creation_req.crnti    = to_rnti(0x4601);
  const ue& u1             = add_ue(ue_creation_req);
  ue_creation_req.ue_index = to_du_ue_index(1);
  ue_creation_req.crnti    = to_rnti(0x4602);
  const ue& u2             = add_ue(ue_creation_req);

  const crb_interval   grant1_crbs = {cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start(),
                                      cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start() + 2};
  const ue_pusch_grant grant1{.user       = &u1,
                              .cell_index = to_du_cell_index(0),
                              .h_id       = to_harq_id(0),
                              .crbs       = grant1_crbs,
                              .ss_id      = to_search_space_id(2),
                              .aggr_lvl   = aggregation_level::n4,
                              .mcs        = sch_mcs_index{24}};

  // Successfully allocates RBs corresponding to the grant.
  ASSERT_EQ(alloc.allocate_ul_grant(grant1), alloc_outcome::success);
  unsigned k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
                    ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
                    .k2;
  ASSERT_EQ(res_grid[k2].result.ul.puschs.back().pusch_cfg.rbs.type1().length(), grant1_crbs.length());

  const crb_interval   grant2_crbs = {cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start() + 2,
                                      cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.start() + 4};
  const ue_pusch_grant grant2{.user       = &u2,
                              .cell_index = to_du_cell_index(0),
                              .h_id       = to_harq_id(0),
                              .crbs       = grant1_crbs,
                              .ss_id      = to_search_space_id(2),
                              .aggr_lvl   = aggregation_level::n4,
                              .mcs        = sch_mcs_index{24}};

  // Allocates all remaining RBs to UE2.
  ASSERT_EQ(alloc.allocate_ul_grant(grant2), alloc_outcome::success);
  k2 = cell_cfg.ul_cfg_common.init_ul_bwp.pusch_cfg_common
           ->pusch_td_alloc_list[res_grid[0].result.dl.ul_pdcchs.back().dci.c_rnti_f0_1.time_resource]
           .k2;
  ASSERT_GT(res_grid[k2].result.ul.puschs.back().pusch_cfg.rbs.type1().length(), grant2_crbs.length());
}
