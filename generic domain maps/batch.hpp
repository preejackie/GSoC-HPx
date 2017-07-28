#include <hpx/config.hpp>
#include <hpx/dataflow.hpp>
#include <hpx/async.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/lcos/when_all.hpp>
#include <hpx/runtime/find_localities.hpp>
#include <hpx/parallel/algorithms/all_any_none.hpp>
#include <hpx/parallel/algorithms/find.hpp>
#include <hpx/parallel/execution_policy.hpp>

#include <unordered_map>
#include <vector>
#include <utility>
#include <stdexcept>

namespace hpx{
namespace domain_maps{
/*
   An instance of batch represents a group of computational elements
    (processors with memory) and GPU's

   It is used to represent which localities are actually involving in the domain_maps
   data distribution.

  It is one of the most important part of the project...
   Allocators which are built on top of those domain_maps actually uses this instance
   to dynamically create and destroy in the active global address space.

  This also tells where the elements are distributed in global address space

  As far now, I implementing batch class as a wrapper over  unordered map between integer type and locality id's
  This will be changed as project proceeds

   I need your most help for this part. Can we jointly make this ??



*/
class batch
{
  private:
          std::unordered_map<int, hpx::id_type> batch_map;
          //std::vector< std::pair<int, hpx::id_type>> batch_map;

  public:
          batch()

                  {

                    std::vector<hpx::id_type> ids = hpx::find_all_localities();
                    int i = 0;
                    for(auto it = ids.begin(); it != ids.end(); it++)
                      {
                        batch_map.emplace(std::make_pair(i,*it));
                        i++;
                      }
                 }

          batch(const std::vector<hpx::id_type>& localities)
            {
              int i = 0;
              for(auto id = localities.begin(); id != localities.end(); id++)
                {
                  batch_map.emplace(std::make_pair(i, *id));
                  i++;
                }
            }
  private:
            // want to restrict the function : )
           template<typename Iterator>
           batch(Iterator first, Iterator last)
           {
              int i = 0;
              for(;first != last; first++)
                {
                  batch_map.emplace(std::make_pair(i,first));
                }
           }
         public:
                   //hpx::future<bool> empty() const
                   bool empty()
                   {
                     //return //hpx::make_future<bool>(batch_map.empty());
                       return batch_map.empty();
                   }

                  std::vector<hpx::id_type>   get()
                  {
                    std::size_t num_localities = batch_map.size();
                    std::vector<hpx::id_type> locs;
                    locs.reserve(num_localities);

                     for(std::size_t i = 0; i < num_localities; i++)
                     {
                        const hpx::id_type ids  = batch_map[i];
                        locs.emplace_back(ids);
                     }
                    return locs;
                  }
                 /*
                   It divides the batch of localities into nparts batch of localities
                   and returns a future container
                 */
                 bool is_all()
                         {
                            if(batch_map.empty())
                             {
                               //return hpx::make_future<bool>(false);
                                 return false;
                             }
                           else
                             {
                               std::size_t num_localities = batch_map.size();
                               std::vector<hpx::id_type> locs;
                               locs.reserve(num_localities);

                                 for(std::size_t i = 0; i < num_localities; i++)
                                 {
                                    const hpx::id_type ids  = batch_map[i];
                                    locs.emplace_back(ids);
                                 }


                             /*  for(auto itr = batch_map.begin(); itr != batch_map.end(); itr++)
                                 {
                                   locs.emplace_back(itr.second);
                                 }
                             */


                                 auto all_  = hpx::find_all_localities();
                               using namespace hpx::parallel;
                               return hpx::parallel::all_of (par,all_.begin(), all_.end(),
                                                             [&](hpx::id_type id_t)
                                                             {
                                                               auto result = hpx::parallel::find(par,locs.begin(),locs.end(), id_t);
                                                               if(result != locs.end())
                                                                 return true;
                                                               else
                                                                 return false;
                                                             }
                                                             );

                             }
                           }
                             hpx::id_type operator[](int of_id)
                              {
                                return batch_map[of_id];
                              }

                          /*  const hpx::id_type operator[](int of_id) const
                              {
                                return batch_map[of_id];
                              }
                              const  hpx::id_type operator[](int && of_id) const
                                   {
                                     return batch_map[std::move(of_id)];
                                   }
                             */
                                 hpx::id_type operator[](int && of_id)
                                   {
                                     return batch_map[std::move(of_id)];
                                   }

                                   hpx::id_type at(int i)
                                   {
                                       return batch_map.at(i);
                                   }

                                   const hpx::id_type at(int i) const
                                     {
                                       return batch_map.at(i);
                                     }

                             /*      bool is_member(hpx::id_type & loc_id)
                                     {
                                       std::size_t num_localities = batch_map.size();
                                       std::vector<hpx::id_type> locs;
                                       locs.reserve(num_localities);
                                       for(std::size_t i = 0; i < num_localities; i++)
                                         {
                                             const hpx::id_type id   =  batch_map[i];
                                             locs.emplace_back(id);
                                         }

                                         auto  result = hpx::parallel::find(locs.begin(),
                                                                            locs.end(), loc_id);

                                         if(result != std::end(locs))
                                           true;
                                         else
                                         false;
                                                }
                                        */
                                                std::size_t size() const
                                                {
                                                  return batch_map.size();
                                                }

                                                hpx::id_type position(int key)
                                                {
                                                  return batch_map[key];
                                                }
                                              };


                                              }
                                              }
