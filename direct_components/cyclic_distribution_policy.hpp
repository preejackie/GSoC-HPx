#include <algorithm>
#include <cstddef>
#include <hpx/config.hpp>
#include <hpx/dataflow.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/runtime/components/stubs/stub_base.hpp>
#include <hpx/runtime/launch_policy.hpp>
#include <hpx/runtime/naming/id_type.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>
namespace hpx {
namespace components {
struct cyclic_distribution_policy {
public:
  cyclic_distribution_policy() {}

  cyclic_distribution_policy operator()(std::vector<hpx::id_type> const &locs) {
    return cyclic_distribution_policy(locs);
  }

  cyclic_distribution_policy operator()(std::vector<hpx::id_type> &&locs) {
    return cyclic_distribution_policy(std::move(locs));
  }

  cyclic_distribution_policy operator()(hpx::id_type &locs) {
    return cyclic_distribution_policy(locs);
  }

  typedef std::pair<hpx::id_type, std::vector<hpx::id_type>>
      bulk_locality_result;

  template <typename Component, typename... params>

  hpx::future<std::vector<bulk_locality_result>>
  bulk_create(size_t count, params &&... vs, size_t runs) {
    HPX_ASSERT(runs != 0);
    using components::stub_base;

    if (localities_.empty() || localities_.size() == 1) {
      hpx::id_type loc_id =
          localities_.empty() ? hpx::find_here() : localities_.front();
      hpx::future<std::vector<hpx::id_type>> objs;
      objs = (stub_base<Component>::bulk_create_async(
          loc_id, count * runs, std::forward<params>(vs)...));

      return objs.then([&loc_id](hpx::future<std::vector<hpx::id_type>> &&f)
                           -> std::vector<bulk_locality_result> {
        std::vector<bulk_locality_result> result;

        result.emplace_back(loc_id, f.get());
      }

      );
    }

    std::vector<hpx::future<std::vector<hpx::id_type>>> objs;
    objs.reserve(localities_.size());

    std::map<hpx::id_type, std::size_t> look_up;
    std::size_t i = 0;
    for (auto &i_loc : localities_) {
      look_up.emplace(i_loc, i);
      i++;
    }

    for (size_t j = 0; j < runs; j++) {
      for (auto const &locality : localities_) {
        if (j == 0)
          objs.push_back(stub_base<Component>::bulk_create_async(
              locality, count, std::forward<params>(vs)...));
        else {
          auto &match = objs[look_up.at(locality)].get();
          auto temp = stub_base<Component>::bulk_create_async(
              locality, count, std::forward<params>(vs)...);
          match.insert(match.end(), std::move((temp.get()).begin()),
                       std::move((temp.get()).end()));
        }

        return hpx::dataflow(
            hpx::launch::sync,
            [=](std::vector<hpx::future<std::vector<hpx::id_type>>> &&in)
                -> std::vector<bulk_locality_result> {
              std::vector<bulk_locality_result> result;
              result.reserve(in.size());
              for (size_t k = 0; k < in.size(); k++) {
                result.emplace_back(std::move(localities_[k]),
                                    std::move(in[k].get()));
              }
              return result;
            },
            std::move(objs));
      }
    }
  }

protected:
  cyclic_distribution_policy(std::vector<hpx::id_type> const &locs)
      : localities_(locs) {}
  cyclic_distribution_policy(std::vector<hpx::id_type> &&locs)
      : localities_(std::move(locs)) {}
  cyclic_distribution_policy(hpx::id_type &locs) {
    localities_.push_back(locs);
  }
  std::vector<hpx::id_type> localities_;
};
} // namespace components
} // namespace hpx
