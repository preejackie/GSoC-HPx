#include <hpx/config.hpp>
#include <hpx/runtime/components/server/migration_support.hpp>
#include <hpx/runtime/components/migrate_component.hpp>
#include <hpx/runtime/components/new.hpp>
#include <hpx/async.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/lcos/when_all.hpp>

#include "batch.hpp"
#include "local_parts.hpp"

#include <vector>
#include <utility>
/*
  allocator
  brief:
       It creates the components(remote objects) in the localities specificed
       by the batch member of domain_map
      input parameters :
        domain_map
*/

template<typename T>
struct proxy_allocator
  {

    private:
            std::vector<hpx::id_type> part_gids;
            batch                     batch_inst;
            std::size_t               local_size;
            std::size_t               num_parts;

    public:

      proxy_allocator()
        :
        part_gids(),
        num_parts(0),
        batch_inst(),
        local_size(0)
        {}

      proxy_allocator(const batch& locs, const std::size_t& remote_size)
        :
          part_gids(),
          num_parts(0),
          batch_inst(locs),
          local_size(remote_size)
          {}

        // All operations are async's
       void create()
          {
            hpx::future<std::vector<hpx::id_type>>  localities = hpx::async(batch_inst.get());
            part_gids =
             localities.then(
                            [this,&]()
                            {
                            hpx::future<std::vector<hpx::id_type>> gids;
                            for(auto id : localities){
                            gids.emplace_back(hpx::new_<parts<T,std::vector<T>>
                                                  (id,local_size));
                            return when_all(gids.begin(),gids.end());
                            }).get();
                            }
          }

         std::unordered_map<hpx::id_type, hpx::id_type>
         make_registry()
          {
            std::unordered_map<hpx::id_type, hpx::id_type> registry;
            hpx::future<std::vector<hpx::id_type>> localities = hpx::async(batch_inst.get());
            return localities.then([this,&]
            ()
            {
              std::size_t size  = localities.size();
              for(std::size_t i = 0;  i < size; i++)
                {
                  registry.emplace(std::make_pair(localities[i], part_gids[i]));
                }
            }
            ).get();
          }
                    
  };
