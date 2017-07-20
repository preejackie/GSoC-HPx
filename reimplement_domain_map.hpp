#include <hpx/config.hpp>
#include <reimplement_domain.hpp>


namespace hpx{
namespace reimplement{
struct partition
{
  partition()
    {HPX_ASSERT(false);}

  partition(const domain& to_work)
   : work_item(to_work)
    {}

  partition(domain&& to_work)
    : work_item(std::move(to_work))
    {}

  void change_domain(const domain& to_work)
      {
        work_item = to_work;
      }
  void change_domain(domain&& to_work)
      {
        work_item(std::move(to_work));
      }
  block_partitioned_domain block(const std::size_t& buckets) const
    {
      std::size_t domain_size = work_item.size();
      std::size_t block_size = domain_size / buckets;

      unsigned int initial = work_item.first();
      unsigned int end     = work_item.last();

      unsigned int start_range = initial;
      unsigned int end_range   = initial + block_size;
      block_partitioned_domain value;
      if(domain_size > buckets)
      {
      for(std::size_t i = 0; i < buckets; i++)
        {
          value.holder[i](start_range,end_range);
          temp = end_range;
          start_range = temp++;
          end_range   = end_range + block_size;
          if(i == buckets - 1)
            {
              temp = domain_size - start_range;
              end_range = start_range + temp;
            }
        }
      }
      else
        {
          value.holder[0](initial,end);
        }
      return value;
    }
  block_cyclic_partitioned_domain block_cyclic(const std::size_t buckets, std::size_t block_size) const
    {
      block_cyclic_partitioned_domain value;
      value.holder.reserve(buckets);
      unsigned int initial = work_item.first();
      unsigned int end     = work_item.last();
      unsigned int to_alloc = work_item.size();

      unsigned int start_range = initial;
      unsigned int end_range   = (initial + block_size) -1;
      unsigned int keep_track=0;
      if(block_size > to_alloc)
        {
          HPX_ASSERT_MSG(false,"block size exceeds domain size");
        }
      //for(std::size_t i = initial; i <= end; i+block_size)
        else if(buckets * block_size > to_alloc)
        {
          for(std::size_t cnt = 0; cnt < to_alloc; cnt + block_size)
            {
              value[i].holder(start_range,end_range);
              temp = end_range;
              start_range = temp++;
              end_range   = end_range + block_size;
            }
        }
        else
        {
          for(std::size_t cnt = 0; cnt < to_alloc; cnt + keep_track)
          {
            if(cnt==0)
            {
            for(std::size_t idx = 0; idx < buckets; idx++)
              {
                value[idx].holder(start_range,end_range);
                keep_track  = end_range - start_range;
                temp = end_range;
                start_range = temp++;
                end_range   = end_range + block_size;
              }
            }
            else
            {
              for(std::size_t idx = 0; idx < buckets; idx++)
                {
                  value[idx].holder.add_pairs(start_range,end_range);
                  keep_track  = end_range - start_range;
                  temp = end_range;
                  start_range = temp++;
                  end_range   = end_range + block_size;
                  if(keep_track > to_alloc)
                    break;
                }
            }

          }
        }return value;
    }
    cyclic_partitioned_domain cyclic(std::size_t buckets) const
      {
        cyclic_partitioned_domain value;
        value.holder.reserve(buckets);
        unsigned int initial = work_item.first();
        unsigned int end     = work_item.last();
        unsigned int to_alloc = work_item.size();
        unsigned int keep_track(0);
        if(buckets > to_alloc)
          {
          for(std::size_t i=0; i < buckets; i++)
            {
              if(initial > end)break;
              value[i].holder(initial);
              initial ++;
            }
          }
        else
          {
            for(std::size_t cnt_ = 0; cnt_ < to_alloc; cnt_ + buckets){
            if(cnt_ == 0)
            {
             for(std::size_t j=0; j < buckets; j++)
               {
                value[i].holder(initial);
                initial++;
               }
                keep_track = buckets;
            }
            else
              {
                for(std::size_t k=0;k < buckets; k++)
                  {
                    if(keep_track > to_alloc) break;
                    value[k].holder.add_index(initial);
                    keep_track ++;
                  }
              }
            }
          }
              return value;
      }
  template<typename Mapper>
  cyclic_partitioned_domain operator() (const std::size_t &buckets,Mapper && map)
      {
        HPX_ASSERT_MSG(std::is_same<decltype(map()), std::size_t>::value,
                      "Map functions should return only size_t or unsigned int");
        cyclic_partitioned_domain value.reserve(buckets);
        std::size_t last = work_item.last();
        std::size_t target;
        for(auto i = work_item.first(); i < last; i = work_item.next(i))
          {
                target  =  map(i);
                HPX_ASSERT_MSG(target < buckets, "Given Map function return value exceeds number of buckets");
                value[target].holder.add_index(i);
          }
        return value;
      }


};
}
}
