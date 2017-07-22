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

    template<typename ...values>
    dimensional(elementtype  val, (values)...)
    : entity_{ {val, (elementtype)(values)... }}
    {
      static_assert(sizeof...(values) == numdimensions - 1,
        "invalid number of default constructor arguments passed ");
    }

    dimensional(std::array<elementtype, numdimensions> const& entity)
    : entity_(entity)
    {}

    dimensional(const dimensional& other)
    {
      entity_ = other.entity_;
    }

    dimensional& operator=(const dimensional& other)
    {
       entity_ = other.entity_;
       return *this;
    }

    auto entity() const
    {
      return entity_;
    }

    elementtype at_dimensiom(std::size_t dim) const
    {
      if(dim >= numdimensions)
        return entity_[0];
      else
        return entity_[dim];
    }

    elementtype operator[](std::size_t dim) const
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


    private:
    std::array<elementtype, numdimensions> entity_;
  };
}
}
