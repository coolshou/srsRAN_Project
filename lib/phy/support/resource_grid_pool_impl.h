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

#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/support/resource_grid_pool.h"
#include "srsran/phy/support/shared_resource_grid.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/event_tracing.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// \brief Implements a resource grid pool.
///
/// It zeroes the resource grids asynchronously upon their scope release if it is given an asynchronous executor.
/// Otherwise, it does not zero the resource grid.
class resource_grid_pool_impl : public resource_grid_pool, private shared_resource_grid::pool_interface
{
public:
  using resource_grid_ptr = std::unique_ptr<resource_grid>;

  /// \brief Constructs a resource grid pool.
  /// \param async_executor_ Asynchronous housekeeping executor.
  /// \param grids_          Resource grids.
  resource_grid_pool_impl(task_executor* async_executor_, std::vector<std::unique_ptr<resource_grid>> grids_);

  /// \brief The destructor checks that all resource grids have been returned to the pool.
  ~resource_grid_pool_impl();

  // See resource_grid_pool interface for documentation.
  shared_resource_grid allocate_resource_grid(const resource_grid_context& context) override;

private:
  /// Reference counter value to indicate the availability of a resource grid.
  static constexpr unsigned ref_counter_available = std::numeric_limits<unsigned>::max();

  // See shared_resource_grid::pool_interface for documentation.
  resource_grid& get(unsigned identifier) override;

  // See shared_resource_grid::pool_interface for documentation.
  void notify_release_scope(unsigned identifier) override;

  /// PHY logger.
  srslog::basic_logger& logger;
  /// Actual pool of resource grids.
  std::vector<resource_grid_ptr> grids;
  /// Counts the resource grid requests.
  unsigned counter = 0;
  /// Resource grid scope count. Zero means they are available.
  std::vector<std::atomic<unsigned>> grids_scope_count;
  /// Pool of resource grid zero set string for tracing.
  std::vector<std::string> grids_str_zero;
  /// Pool of resource grid reservation string for tracing.
  std::vector<std::string> grids_str_reserved;
  /// Asynchronous task executor.
  task_executor* async_executor;
};

} // namespace srsran
