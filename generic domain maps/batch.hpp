#include <hpx/config.hpp>
#include <hpx/dataflow.hpp>
#include <hpx/async.hpp>
#include <hpx/lcos/future.hpp>
#include <hpx/lcos/when_all.hpp>

#include <unordered_map>
#include <vector>
#include <utility>
#include <stdexcept>

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

  Parsa I need your most help for this part. Can we jointly make this ??

*/

class batch
{
  private:
          std::unordered_map<int, hpx::id_type> batch_map;

  public:
          batch()
                : batch_map()
                  {}

          batch(const std::vector<hpx::id_type>& localities)
            {
              int i = 0;
              for(auto id : localities)
                {
                  batch_map.emplace(std::make_pair(i,id));
                  i++;
                }
            }
            // want to restrict the function : )
           template<typename Iterator>
           batch(Iterator first, Iterator last)
           {
              int i = 0;
              for(;first != last; first++)
                {
                  batch_map.emplace_back(std::make_pair(i,first));
                }
           }

          bool empty() const
          {
            return batch_map.empty();
          }
        /*
          It divides the batch of localities into nparts batch of localities
          and returns a future container 
        */

        auto split(unsigned int nparts) const
        {
          if(batch_map.empty())
            {
              throw std::runtime_error("An attempt to split empty batch");
            }
          else
            {
              std::size_t batch_size = batch_map.size();
              auto        part_size  = ((batch_size + nparts) - 1 / nparts);

              std::vector<hpx::future<batch> >
                          result_type;
              result_type.reserve(nparts);

              std::vector<hpx::id_type> goes_in;
              goes_in.reserve(part_size);

              std::vector<hpx::id_type> temp_space;
              temp_space.reserve(batch_size);

              for(auto itr = batch_map.begin(); itr != batch_map.end(); itr++)
                {
                  temp_space.emplace_back(itr.second);
                }

              for(auto itr = result_type.begin(); itr != result_type.end(); itr + part_size + 1)
                {
                  result_type.emplace_back(hpx::async(&batch,itr, itr + part_size));
                }
              return when_all(result_type.begin(), result_type.end());
            }

        }





};
