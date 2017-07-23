#include <hpx/config.hpp>
#include "types.hpp"

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
            distribution_type type;
            std::size_t       block_size;

          using internals = hpx::domain_maps::distribution_type;
    public:
            distribution()
              :
                  type(internals::NONE),
                  block_size(0)
                  {}

            distribution(internals  type_t, std::size_t size)
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
            template<typename indextype, typename size_type>
              indextype max_block_size(indextype range, size_type num_localities) const
              {
                switch (type) {
                  case internals::NONE:
                            return range;

                  case internals::BLOCKED:
                            return ((range + num_localities) - 1 / num_localities);

                  case internals::BLOCKCYCLIC:
                            return std::min<size_type>(range, block_size);

                  case internals::CYCLIC:
                            return 1;

                  /*
                    case internals_t::R_BIJECTION:
                            return something
                  */

                  default :
                            throw std::runtime_error("Distribution type is ill-formed");
                }
              }

            template<typename indextype, typename size_type>
            indextype local_index_to_block_index(
                indextype loc_id_key,
                indextype local_index,
                size_type nlocs)
              {
                std::size_t off_;
                switch (type) {
                  case internals::NONE:
                          return 0;

                  case internals::BLOCKED:
                          return loc_id_key;

                  case internals::BLOCKCYCLIC:
                        {
                            off_  = local_index / block_size;

                            return (off_ * nlocs) + loc_id_key;
                        }
                  case internals::CYCLIC:
                        return (local_index * nlocs) + loc_id_key;
                }
              }


          bool operator == (const my_type & other) const
          {
            if(type == other.type && block_size == other.block_size)
            return true;
            else
            return false;
          }

          bool operator != (const my_type & other) const
          {
             if(type == other.type && block_size == other.block_size)
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
}}
