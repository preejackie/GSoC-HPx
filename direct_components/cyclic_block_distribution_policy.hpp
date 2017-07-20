//  Copyright (c) 2017 Praveen velliengiri
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file cyclic_block_distribution_policy.hpp
#include <hpx/config.hpp>
#include <hpx/dataflow.hpp>
#include <hpx/include/parallel_execution_policy.hpp>
#include <hpx/include/parallel_sort.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/performance_counters/performance_counter.hpp>
#include <hpx/runtime/components/stubs/stub_base.hpp>
#include <hpx/runtime/components/unique_component_name.hpp>
#include <hpx/runtime/launch_policy.hpp>
#include <hpx/runtime/naming/id_type.hpp>
#include <hpx/runtime/naming/name.hpp>
#include <hpx/util/unwrapped.hpp>

#include <algorithm>
#include <cstddef>
#include <map>
#include <type_traits>
#include <utility>
#include <vector>
namespace hpx {
namespace components {
    static char const* const default_cyclic_counter_name =
        "runtime/{locality/total}/count/component@";
    namespace internals {
        /// get counter_data of each locality i.e the no :of: components already in
        /// that locality of given type

        hpx::future<std::vector<std::uint64_t>> extract_values(
            std::vector<performance_counters::performance_counter> const&
                in_counters)
        {
            using namespace hpx::performance_counters;
            std::vector<hpx::future<std::uint64_t>> wrapped_result;
            wrapped_result.reserve(in_counters.size());
            for (auto const& counter : in_counters)
                wrapped_result.push_back(counter.get_value<std::uint64_t>());

            hpx::dataflow(hpx::launch::sync,
                hpx::util::unwrapped(
                    [](std::vector<std::uint64_t>&& wrapped_result) {
                        return wrapped_result;
                    }),
                std::move(wrapped_result));
        }

        template <typename component_type>
        hpx::future<std::vector<std::uint64_t>> get_counter_data(
            component_type component_name, std::string const& counter,
            std::vector<id_type> const& assigned_localities_)
        {
            using namespace hpx::performance_counters;
            std::vector<performance_counter> counters;
            counters.reserve(assigned_localities_.size());
            if (counter[counter.size() - 1] == '@')
            {
                counter = counter + component_name;
                for (auto const& id : assigned_localities_)
                    // performance_counter returns the future instance
                    counters.push_back(performance_counter(counter, id));
            }
            else
            {
                for (auto const& id : assigned_localities_)
                    counters.push_back(performance_counter(counter, id));
            }
            return hpx::dataflow(&extract_values, std::move(counters));
        }
        template <typename Component>
        struct create_helper
        {
            create_helper(std::vector<hpx::id_type> const& localities)
              : inter_localities(localities)
            {
            }
            /// helper class  : Create one Component on one of the localities associated to
            /// this policy. It creates Component on the localities which is
            /// having minimum number of components already
            template <typename... Ts>
            hpx::future<hpx::id_type> operator()(
                hpx::future<std::vector<std::uint64_t>>&& values,
                Ts&&... vs) const
            {
                std::vector<std::uint64_t> table = values.get();
                auto const& position =
                    std::min_element(table.begin(), table.end());
                hpx::id_type const& best_locality =
                    inter_localities[std::distance(table.begin(), position)];

                return stub_base<Component>::create_async(
                    best_locality, std::forward<Ts>(vs)...);
            }

            std::vector<hpx::id_type> const& inter_localities;
        };

        template <typename Component>
        struct bulk_create_helper
        {
            typedef std::pair<id_type, std::vector<id_type>>
                bulk_locality_result;

            std::vector<id_type> inter_localities;

            /// helper class : create the given number of components on the
            /// locality associated to this policy
            /// working

            /// It will sort the values returned by performance_counters
            /// and allocate the "block_size" no:of:components on the all associated
            /// localities until "count " no:of:components is created.Of course last locality may get
            /// less no:of:components than the block_size
            /// the locality which have less no:of:components will be a start index

            bulk_create_helper(std::vector<id_type> const& localities)
              : inter_localities(localities)
            {
            }

            template <typename... params>
            hpx::future<std::vector<bulk_locality_result>> operator()(
                hpx::future<std::vector<std::uint64_t>>&& values, size_t count,
                size_t block_size, params&&... vs) const
            {
                using namespace hpx::parallel;
                using hpx::components::stub_base;
                size_t i = 0;
                size_t to_create = count;
                size_t remains = count;
                size_t created = 0;
                std::vector<std::pair<std::uint64_t, hpx::id_type>> container;
                std::vector<hpx::future<std::vector<id_type>>> result;
                result.reserve(inter_localities.size());
                std::vector<std::uint64_t> const& table = values.get();
                // counter data missing
                HPX_ASSERT(table.size() == inter_localities.size());
                container.reserve(table.size());
                for (auto const& ids : inter_localities)
                {
                    container.emplace_back(std::make_pair(table[i], ids));
                    i++;
                }
                sort(par, container.begin(), container.end());

                for (size_t cycle = 1; remains != 0; cycle++)
                {
                    for (size_t idx = 0; idx < container.size(); idx++)
                    {
                        if (cycle == 1)
                        {
                            to_create =
                                (remains >= block_size) ? block_size : remains;
                            result.push_back(
                                stub_base<Component>::bulk_create_async(
                                    container[idx].second, to_create, vs...));
                            created += to_create;
                            remains = count - created;
                            if (created == count)
                                break;    // exits both loops
                        }
                        else
                        {
                            to_create = (remains >= block_size) ?
                                block_size :
                                (count - created);
                            auto const& match = result[idx].get();
                            auto const temp =
                                stub_base<Component>::bulk_create_async(
                                    container[idx].second, to_create, vs...)
                                    .get();
                            match.insert(match.end(), temp.begin(), temp.end());
                            created += to_create;
                            remains = count - created;
                            if (created == count)
                                break;    // exits both loops
                        }
                    }
                }
                return hpx::dataflow(
                    [=](std::vector<hpx::future<std::vector<id_type>>>&& j) {
                        std::vector<bulk_locality_result> objs;
                        for (size_t v = 0; v < j.size(); v++)
                        {
                            objs.emplace_back(std::move(container[v].second),
                                std::move(j[v].get()));
                        }
                        return objs;
                    },
                    std::move(result));
            }
        };
    }    // namespace internals
    struct cyclic_block_distribution_policy
    {
    public:
        cyclic_block_distribution_policy()
          : counter_name_(default_cyclic_counter_name)
        {
        }
        cyclic_block_distribution_policy operator()(
            std::vector<id_type> const& locs,
            char const* counter_name = default_cyclic_counter_name) const
        {
#if defined(HPX_DEBUG)
            for (id_type const& loc : locs)
            {
                HPX_ASSERT(naming::is_locality(loc));
            }
#endif
            return cyclic_block_distribution_policy(locs, counter_name);
        }

        cyclic_block_distribution_policy operator()(std::vector<id_type>&& locs,
            char const* counter_name = default_cyclic_counter_name) const
        {
#if defined(HPX_DEBUG)
            for (id_type const& loc : locs)
            {
                HPX_ASSERT(naming::is_locality(loc));
            }
#endif
            return cyclic_block_distribution_policy(
                std::move(locs), counter_name);
        }

        cyclic_block_distribution_policy operator()(id_type const& loc,
            char const* counter_name = default_cyclic_counter_name) const
        {
            HPX_ASSERT(naming::is_locality(loc));
            return cyclic_block_distribution_policy(loc, counter_name);
        }

        template <typename Component, typename... params>

        /// create one component on one of the localities
        /// vs... thing passed to the constructor

        hpx::future<id_type> create(params&&... vs) const
        {
            using hpx::components::stub_base;
            if (localities_.empty() || localities_.size() == 1)
            {
                id_type id_loc = localities_.empty() ? hpx::find_here() :
                                                       localities_.front();
                return stub_base<Component>::create_async(
                    id_loc, std::forward<params>(vs)...);
            }
            else
            {
                hpx::future<std::vector<std::uint64_t>> values =
                    internals::get_counter_data(
                        hpx::components::unique_component_name<
                            hpx::components::component_factory<
                                typename Component::wrapping_type>>::call(),
                        counter_name_, localities_);

                using hpx::util::placeholders::_1;
                return values.then(hpx::util::bind(
                    internals::create_helper<Component>(localities_), _1,
                    std::forward<params>(vs)...));
            }
        }

        /// create  count number of components on the localities of this policy
        /// count -> total number no:of:components to be created
        /// block_size -> the no:of:componets should be created on the locality once it is selected
        typedef std::pair<id_type, hpx::future<std::vector<id_type>>>
            bulk_locality_result;
        template <typename Component, typename... params>
        hpx::future<std::vector<bulk_locality_result>> bulk_create(
            size_t count, params&&... vs, size_t block_size) const
        {
            using hpx::components::stub_base;
            // necessary information not available
            HPX_ASSERT(count != 0 || block_size != 0);
            if (localities_.empty() || localities_.size() == 1)
            {
                id_type alloc_id = localities_.empty() ? hpx::find_here() :
                                                         localities_.front();
                hpx::future<std::vector<id_type>> result =
                    stub_base<Component>::bulk_create_async(
                        alloc_id, count, std::forward<params>(vs)...);

                result.then(hpx::launch::sync,
                    [alloc_id](hpx::future<std::vector<id_type>>&& v)
                        -> std::vector<bulk_locality_result> {
                        std::vector<bulk_locality_result> result;
                        result.reserve(1);
                        result.emplace_back(alloc_id, v.get());
                        return result;
                    });
            }
            else
            {
                hpx::future<std::vector<std::uint64_t>> values =
                    internals::get_counter_data(
                        hpx::components::unique_component_name<
                            hpx::components::component_factory<
                                typename Component::wrapping_type>>::call(),
                        counter_name_, localities_);

                using hpx::util::placeholders::_1;
                return values.then(hpx::util::bind(
                    internals::bulk_create_helper<Component>(localities_), _1,
                    count, block_size, std::forward<params>(vs)...));
            }
        }

    protected:
        cyclic_block_distribution_policy(
            std::vector<id_type> const& localities, char const* counter_name)
          : localities_(localities)
          , counter_name_(counter_name)
        {
        }

        cyclic_block_distribution_policy(
            std::vector<id_type>&& localities, char const* counter_name)
          : localities_(std::move(localities))
          , counter_name_(counter_name)
        {
        }

        cyclic_block_distribution_policy(
            id_type const& locality, char const* counter_name)
          : counter_name_(counter_name)
        {
            localities_.push_back(locality);
        }

        /*friend class hpx::serialization::access;

    template <typename Archive>
    void serialize(Archive& ar, unsigned int const)
    {
        ar & counter_name_ & localities_;
    }
    */

        std::vector<id_type> localities_;    // localities to create things on
        std::string counter_name_;
    };
    static cyclic_block_distribution_policy const block_cyclic;
}    // namespace components
}    // namespace hpx
namespace hpx {
using hpx::components::cyclic_block_distribution_policy;
using hpx::components::block_cyclic;
namespace traits {
    template <>
    struct is_distribution_policy<components::cyclic_block_distribution_policy>
      : std::true_type
    {
    };

}    // namespace traits
}    // namespace hpx
