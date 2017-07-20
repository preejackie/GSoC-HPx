#include <hpx/config.hpp>
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/runtime/components/server/locking_hook.hpp>
#include <hpx/runtime/components/server/component_base.hpp>
#include <hpx/runtime/components/server/component.hpp>

#include <array>
#include <map>

namespace hpx
{
 namespace containers
 {
  namespace details{
   template <typename T,std::size_t N, typename Data = std::array<T,N> >
   struct local_arrays : public hpx::components::locking_hook<
                                component_base< local_arrays<T,N,Data> >
                                                             >
    {
      public:
              typedef Data datatype;
              typedef typename  datatype::allocator_type allocator_type;
              typedef typename  datatype::size_type      size_type;
              typedef typename  datatype::iterator       iterator_type;
              typedef typename  datatype::const_iterator const_iterator_type;
              typedef typename  hpx::components::locking_hook<
                                  component_base< local_arrays<T,N,Data> >
                                                             >  base_type;

              datatype parray_local_;

    };
}
}
}

namespace hpx
{
  namespace containers
  {
    template<typename T, std::size_t V>
    struct parrays
      {
        private:
                std::map< co_domain<unsigned int>, hpx::id_type
        public:
            explicit  parrays(const std::map< co_domain<unsigned int>,
                               hpx::id_type >& list)
            :
      };
  }
}
