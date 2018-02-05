
#include <hpx/parallel/algorithms/sort.hpp>
#include <hpx/parallel/execution_policy.hpp>
#include <hpx/parallel/algorithms/find.hpp>
#include <hpx/traits/is_domain.hpp>
// Standard

#include <initializer_list>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace hpx{
namespace domain{

template<typename index_type>
struct domain
{


};

template<>
struct domain<unsigned int>
{
  public:
    explicit  domain(const unsigned int& first_ , const unsigned int& last_ ) : first(first_),
                  range{0}
                            {
                              static_assert(first_ != last_, "Domains needs to be unique" );
                              first(first_);
                              last(last_);
                              size(last_ - first_);
                              var(false);
                            }

    explicit domain(unsigned int && first_, unsigned int && last_ ) :

                  range{0}
                          {
                            static_assert(first_ != last_, "Domains needs to be unique" );
                            first(std::move(first_));
                            last(std::move(last_));
                            size(std::move(last_ - first_));
                            var(false);
                          }

    explicit  domain(std::initializer_list<unsigned int>& range_)
                  :

                              {
                                hpx::parallel::sort(par, range_.begin(), range_.end());
                                auto It = std::unique(range_.begin(),range_.end());
                                static_assert(It != range_.end(), "Domains needs to be unique");

                                size(range_.size());
                                range(range_);
                                first(range_.front());
                                second(range_.back());
                                var(true);
                                expand_var(true);
                              }

    explicit domain(std::initializer_list<unsigned int>&& range_)
                  :
                    first(-1),
                    second(-1)
                      {
                        hpx::parallel::sort(par,range_.begin(),range_.end()); // check move
                        auto It = std::unique(range_.begin(),range_.end());
                        static_assert(It != range_.end(), "Domains needs to be unique");

                        size(range_.size());
                        range(std::move(range_));
                        var(true);
                        expand_var(true);
                      }

    bool  find(const unsigned int& value) const
            {
              if(var == true)
                {
                  auto Itr = hpx::parallel::find(par, range.begin(), range.end());
                  return (Itr == range.end() ? false : true);
                }
              return ((value >= first && value <= last)  ? true : false);
            }

    unsigned int first() const
            {
              if(var == true)
                return range.front();

              return first;
            }
    unsigned int last() const
            {
              if(var == true)
                return  range.back();

              return last;
            }

    void expand(const std::size_t& stride = 1) const
    {

       expand_impl(stride);
       expand_var(true);
    }

   bool is_expanded() const
    {
      return expand_var;
    }

  std::size_t size() const
    {
      return size;
    }

  bool expand_var;
  private:
    void  expand_impl(d::size_t& stride) const
      {
        for(std::size_t i = first; i < last; i+= stride)
          range.emplace_back(i);
      }


  private:
      int first;
      int last;
      std::size_t size;
      bool var;
      std::vector<unsigned int> range;

};
}}

namespace hpx
 {

 using hpx::domain::domain;
  namespace traits
    {

      template<>
      struct is_domain<domain> : std::true_type
       {};

      template<>
      struct is_integer_domain< domain<unsigned int> > : std::true_type
      {};

    }
 }
