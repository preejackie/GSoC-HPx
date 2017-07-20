#include <hpx/config.hpp>

#include <cstddef>
#include <type_traits>

namespace hpx
 {
   namespace traits
    {
      template<typename T>
       struct is_domain : std::false_type
        {};

      template<typename T>
       struct is_integer_domain : std::false_type
        {};


    }
 }
