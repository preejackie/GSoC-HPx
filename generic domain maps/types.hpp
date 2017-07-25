#include<hpx/config.hpp>

// this is a range of possible types of distribution
// that the user can deal with
namespace hpx{
namespace domain_maps{
namespace internals{
  enum  distribution_type
    {

      UNDEFINED  = 0,
      NONE,
      BLOCKED,
      BLOCKCYCLIC,
      CYCLIC,
      R_BIJECTION,
    };
}
}
}
