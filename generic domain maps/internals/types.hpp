#include<hpx/config.hpp>

// this is a range of possible types of distribution
// that the user can deal with
namespace hpx{
namespace domain_maps{
namespace internals{
namespace types
{
  typedef enum struct distribution_type : int
    {

      UNDEFINED  = 0,
      NONE,
      BLOCKED,
      BLOCKCYCLIC,
      CYCLIC,
      R_BIJECTION,
    } distribution_type;
}
}
}
}
