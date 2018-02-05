#include <hpx/lcos/future.hpp>
#include <vector>
#include <map>
#include <utility>


namespace hpx {
namespace partition {
namespace details{

template<typename T>
struct co_domain
 {

 };

template<>
struct co_domain<unsigned int>
 {
    public:
    std::vector<unsigned int> indexes;
    std::size_t size;

    explicit co_domain() : indexes(0), size(0) {}
    void add(const unsigned int& index)
    {
      indexes.emplace_back(index);
      size++;
    }

 };

 // partitioned_domain should be container

template<typename T>
struct partitioned_domain
{

};

template<>
struct partitioned_domain<co_domain<unsigned int> >
  {
    public:
    typedef co_domain<unsigned int>  value_type;

    partitioned_domain()
    {}

    private:

    std::vector<value_type> collection;
    std::size_t size;

  }
}
}

}

namespace hpx{
namespace partition {
template <typename T>
 struct partition
 {

 };

template<>
 struct partition<domain<unsigned int> >
  {

    typedef domain<unsigned int>  domain_type;

    partition()
      {
        static_assert(false,"partition should be called with domain");
      }

    partition(const domain<unsigned int>& domain )
      :
        ref_domain(domain)
         {}


     // map function should map the indexes of domain to co_domain
     //  map function will replaced by the EDSL
     template <typename mapper>
     partitioned_domain operator() (std::size_t buckets, mapper&& map) const
      {
        ref_domain.expand();
        partitioned_domain values.reserve(buckets);
        std::size_t limit(ref_domain.size());
        std::size_t co_domain_index;
        std::size_t check(0);

        for(std::size_t i = 0;i < limit; i++)
          {
            check = map(ref_domain.range[i]);
            HPX_ASSERT_MSG(check < buckets,"Map function will not map to invalid buckets");
            co_domain_index = check;
            values[co_domain_index].add(i);
          }

        return  values;
      }

  private:
      domain_type ref_domain;
  };

}}
namespace hpx{
namespace domain_map{
  struct domain_map
  {
    public:
        domain_map()
        {
          HPX_ASSERT_MSG(false,"Domain_map should be initialized with partitioned_domain");
        }
      domain_map(const partitioned_domain& imp, hpx::id_type loc_ = hpx::find_here()) :
              ref_partitioned_domain(imp),
              localities_(loc_) {}
      domain_map(const partitioned_domain& imp, hpx::id_type loc1, hpx::id_type loc2)
                :
                  ref_partitioned_domain(imp),
                  localities_(loc1)
                  {
                    localities_.emplace_back(loc2);
                  }
      domain_map(const partitioned_domain& imp, std::vector<hpx::id_type> loc_)
              :
                ref_partitioned_domain(imp),
                localities_(loc_)
                {}

      hpx::future<void> construct_locality_map()
      {
        std::size_t index(0);
        for(auto &id : localities_)
          {
            locality_map.emplace(std::make_pair(index,id));
            index++;
          }
      }
      // Mapper will be replaced by the EDSL as in partition

      template<typename Mapper>
      auto operator() (Mapper&& map)
      {

        std::size_t limit = ref_partitioned_domain.size();
        std::size_t check(0);
        hpx::id_type target_loc;
        for(std::size_t i = 0;i < limit; i++)
          {
            check = map(i);
            loc_  = locality_map[check];
            domain_map.emplace(std::)

          }
      }
    private:
          partitioned_domain ref_partitioned_domain;
          std::vector<hpx::id_type> localities_;
          std::map<int,hpx::id_type>  locality_map;
  }
}
}
