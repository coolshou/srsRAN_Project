/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/du/f1u_gateway.h"

namespace srsran {
namespace srs_du {

class dummy_f1u_du_gateway_bearer_rx_notifier : public f1u_du_gateway_bearer_rx_notifier
{
public:
  ~dummy_f1u_du_gateway_bearer_rx_notifier() override = default;

  void on_new_pdu(nru_dl_message msg) override {}
};

/// \brief Dummy F1-U bearer for the purpose of benchmark.
class f1u_gw_dummy_bearer : public f1u_du_gateway_bearer
{
public:
  void on_new_pdu(nru_ul_message msg) override {}
  void stop() override {}
};

/// \brief Simulator of the CU-UP from the perspective of the DU.
class cu_up_simulator : public f1u_du_gateway
{
public:
  std::vector<srs_du::f1u_du_gateway_bearer_rx_notifier*> created_du_notifs;
  std::vector<up_transport_layer_info>                    registered_dl_tnls;

  optional<uint32_t> last_ue_idx;
  optional<drb_id_t> last_drb_id;

  std::unique_ptr<f1u_du_gateway_bearer> create_du_bearer(uint32_t                                   ue_index,
                                                          drb_id_t                                   drb_id,
                                                          srs_du::f1u_config                         config,
                                                          const up_transport_layer_info&             dl_up_tnl_info,
                                                          const up_transport_layer_info&             ul_up_tnl_info,
                                                          srs_du::f1u_du_gateway_bearer_rx_notifier& du_rx,
                                                          timer_factory                              timers,
                                                          task_executor& ue_executor) override
  {
    created_du_notifs.push_back(&du_rx);
    registered_dl_tnls.push_back(dl_up_tnl_info);
    last_ue_idx = ue_index;
    last_drb_id = drb_id;
    auto bearer = std::make_unique<f1u_gw_dummy_bearer>();
    return bearer;
  }

  void remove_du_bearer(const up_transport_layer_info& dl_tnl) override
  {
    for (unsigned i = 0; i != registered_dl_tnls.size(); ++i) {
      if (dl_tnl == registered_dl_tnls[i]) {
        registered_dl_tnls.erase(registered_dl_tnls.begin() + i);
        created_du_notifs.erase(created_du_notifs.begin() + i);
        break;
      }
    }
  }
};

} // namespace srs_du
} // namespace srsran
