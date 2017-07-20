#include <hpx/config.hpp>
//#include <hpx/traits/is_domain.hpp>


#include <utility>
#include <vector>

namespace hpx{
namespace reimplement{
struct domain{
  public:
    domain()
    {
      HPX_ASSERT(false);
    }

    explicit  domain(std::size_t const& first_ , std::size_t const & last_ )
      :
        boundary (std::make_pair(first_, last_)),
        range(last_ - first_)
        {
          static_assert(last_ > first_ ,"Wrong domain range is specified");
        }


    explicit domain(std::size_t && first_, std::size_t && last_ ) :
      :
        boundary(std::make_pair(first_, last_ )),
        range(last_ - first_)
        {
          static_assert(last_ > first_, "Wrong domain range is specified");
        }


    explicit  domain(std::pair<std::size_t,std::size_t> const& pair)
      :
        boundary(pair),
        range(pair.second - pair.first)
        {
          static_assert(pair.second > pair.first, "wrong domain range is specified");
        }


    bool  find(const std::size_t&value) const
            {
              if(value >= boundary.first && value <= boundary.second)
                return true;
              else
                return false;
            }

    unsigned int  first() const
            {
              return boundary.first;
            }
    unsigned int last() const
            {
              return boundary.second;
            }
    unsigned int next(const unsigned int a) const
            {
               if(find(a) && a != last())
                  return a++;
               else
                HPX_ASSERT_MSG(false,"Next operation exceeds boundary");
            }


  std::size_t size() const
    {
      return range;
    }

  hpx::id_type residing_locality() const
    {
      return hpx::find_here();
    }
};
}
namespace reimplement{
  struct block_partitioned_domain
  {
    struct block_sub_domain
    {
      std::pair<unsigned int, unsigned int> part;
      std::size_t size;
      block_sub_domain()
      {

      }
      block_sub_domain(unsigned int const& start, unsigned int const& end)
        :
          part(std::make_pair(start,end)),
          size(end - start)
          {}
    };
    std::size_t size() const
    {
      return holder.size();
    }
    private:
        std::vector<block_sub_domain> holder;

  };
  struct block_cyclic_partitioned_domain
  {
    struct block_cyclic_domain
    {
      std::vector<std::pair<unsigned int, unsigned int> > part;
      block_cyclic_domain()
      {}
      block_cyclic_domain(std::pair<unsigned int, unsigned int> const& pairs)
      :
        part(pairs)
        {}
      void add_pairs(std::pair<unsigned  int, unsigned int> const& pairs)
      {
        part.emplace_back(pairs);
      }
    };
    std::size_t size() const
    {
      return holder.size();
    }
    private:
            std::vector<block_cyclic_domain> holder;
  };
  struct cyclic_partitioned_domain
  {
    struct cyclic_domain
    {
      std::vector<unsigned int> part;
      cyclic_domain()
      {}
      cyclic_domain(unsigned int const& index)
      :
        part(index)
        {}
      void add_index(unsigned int const& idx)
        {
          part.emplace_back(idx);
        }
    };
      private:
        std::vector<cyclic_domain> holder;
      public:
        std::size_t size() const
        {
          return holder.size();
        }
  };
}
