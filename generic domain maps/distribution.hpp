#include <hpx/config.hpp>
#include "internals/types.hpp"

#include <stdexcept>

/// Distribution specifies how to distribute the elements in the given dimension
/// to the localities in batch.
/// Predefined distribution types are
  /* UNDEFINED,
      NONE,
      BLOCKED,
      BLOCKCYCLIC,
      CYCLIC,
      R_BIJECTION,
  */

namespace hpx{
  namespace domain_maps{
  class distribution
  {
    private:
            using my_type = hpx::domain_maps::distribution;
    public:
            hpx::domain_maps::internals::types::distribution_type type;
            std::size_t                                           block_size;

            using internals_t = hpx::domain_maps::internals::types::distribution_type;
    public:
            distribution()
              :
                  type(internals::types::distribution_type::NONE),
                  block_size(0)
                  {}

            distribution(internals::types::distribution_type type_t = internals::types::distribution_type::NONE
                              , std::size_t size)
              :
                 type(type_t),
                 block_size(size)
                 {}

            distribution(const my_type& other)
              :
                type(other.type),
                block_size(other.block_size)
                {}

            my_type& operator= (const my_type& other)
                {
                  if(this != &other)
                    {
                      type          =    other.type;
                      block_size    =    other.block_size;
                      return *this;
                    }
                  else
                    return *this;
                }

            // It returns the maximum size of block which is used in the distribution
            // of elements in the specific dimension
            template<typename indextype, template size_type>
              indextype max_block_size(indextype range, size_type num_localities) const
              {
                switch (type) {
                  case internals_t::NONE:
                            return range;

                  case internals_t::BLOCKED:
                            return ((range + num_localities) - 1 / num_localities);

                  case internals_t::BLOCKCYCLIC:
                            return std::min<size_type>(range, block_size);

                  case internals_t::CYCLIC:
                            return 1;

                  /*
                    case internals_t::R_BIJECTION:
                            return something
                  */

                  default :
                            throw std::runtime_error("Distribution type is ill-formed");
                }
              }

          bool operator == (const my_type & other) const
          {
            if(type == other.type && block_size = other.block_size)
            return true;
            else
            return false;
          }

          bool operator != (const my_type & other) const
          {
             if(type == other.type && block_size = other.block_size)
                return false;
            else
                return true;
          }
  };

extern const distribution NONE;

extern const distribution BLOCKED;

extern const distribution BLOCKCYCLIC(int block_size);

extern const distribution CYCLIC;

// extern const distribution R_BIJECTION;
