#include <hpx/config.hpp>




#include <array>
#include <stdexcept>

#include "distribution.hpp"
/*
  Base class for Any type that follows dimensional property
  like distribution,etc
*/
namespace hpx{
namespace domain_maps{
 template<typename elementtype, std::size_t numdimensions>
 class dimensional
  {
    public:

    template<typename ...Values>
    dimensional(elementtype  val, Values ... values)
    : entity_{ {val, (elementtype)values... }}
    {
      static_assert(sizeof...(values) == numdimensions - 1,
        "invalid number of default constructor arguments passed ");
    }

    dimensional(std::array<elementtype, numdimensions> const& entity)
    : entity_(entity)
    {}

    dimensional(const dimensional& other)
     :
      entity_(other.entity_)
    {}


    dimensional& operator=(const dimensional& other)
    {
       entity_ = other.entity_;
       return *this;
    }

    auto get_entity() const
    {
      return entity_;
    }

    const elementtype at_dimensiom(std::size_t dim) const
    {
      if(dim >= numdimensions)
        return entity_[0];
      else
        return entity_[dim];
    }

    const elementtype operator[](std::size_t dim) const
    {
      if(dim >= numdimensions)
        throw std::runtime_error(" out of bounds ");
      else
        return entity_[dim];
    }

    bool operator== (dimensional const& other) const
    {
      return entity_ == other.entity_;
    }

    bool operator!= (dimensional const& other) const
    {
      return entity_ !=  other.entity_;
    }

    std::size_t rank() const
    {
      return numdimensions;
    }


    std::array<elementtype, numdimensions> entity_;
  };


template <std::size_t numdimensions>
class policy : public dimensional<distribution, numdimensions>
  {
    public:
    policy()
    {
      this->entity_[0]  = BLOCKED;
      for(std::size_t i = 1; i < numdimensions; i++)
        {
          this->entity_[i] = NONE;
        }
    }


    template<typename ... Values>
    policy(distribution dist, Values ... values)
    : dimensional<distribution, numdimensions>::dimensional(dist, values ...)
    {

    }


    policy(const std::array<distribution, numdimensions> & policies)
    : dimensional<distribution, numdimensions>::dimensional(policies)
    {}

    /*policy(const policy& pol)
    {
      this->entity_ = pol.entity_;
    }
    */

  };

}
}
