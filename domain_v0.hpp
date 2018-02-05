#include <hpx/include/parallel_find.hpp>
#include <hpx/include/parallel_set_operations.hpp>
#include <hpx/dataflow.hpp>
#include <hpx/include/parallel_is_sorted.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include <type_traits>
#include <cmath>
#include <functional>
   // change the return types & parameter types of domains as
   //  domain<T>

 // Associative domains
template <typename T>
struct domain
{
  private:
      std::vector<T> index_set;
      std::size_t set_size;
  /*

  */
    // Constructors should not  visible outside
      domain(const std::vector<T>& result) : index_set(result), set_size(result.size())
       {}

  public:
       // Constructors
     domain() : index_set(),
                set_size(0) {}

     template<typename ...Ts>
     domain(Ts&... Args) :
            index_set{Args...},
            set_size{sizeof...(Args)}
            {

            }  // Is_unique

      template<typename ...Ts>
      domain(Ts&&... Args) :
             index_set{std::forward<Ts> (Args)...},
             set_size{sizeof...{Args}}
              {}

      // Copy & Move
        domain(const domain& other ) noexcept
          : index_set(other.index_set),
            set_size(other.set_size)
              {}
        domain(domain&& other) noexcept
          : index_set(std::move(other.index_set)),
            set_size(std::move(other.set_size))
              {}
        /// Copy & move assignable
            domain& operator= ( domain& other)
                {
                  index_set = other.index_set;
                  set_size  = other.set_size;
                  return *this;
                }
            domain& operator= (domain&& other)
                {
                  index_set = std::move(other.index_set);
                  set_size  = std::move(other.set_size);
                  return *this;
                }

            bool operator== (const domain& other)
                {
                    if(std::is_same<decltype(index_set),decltype(other.index_set)>::value&&
                       set_size == other.set_size)
                      return true;
                    else
                       return false;
                }

                bool operator!= (const domain& other)
                    {
                        if(std::is_same<decltype(index_set),decltype(other.index_set)>::value&&
                           set_size == other.set_size)
                          return false;
                        else
                           return true;
                    }
            bool clear() noexcept
            {
               index_set.clear();
                set_size = 0;
            }
            bool empty() const noexcept
              {
                 if(index_set.empty() || set_size == 0)
                    return true;
                 else
                    return false;
              }
            std::size_t get_size() const noexcept
              {
                return set_size;
              }
            bool is_associative_domain()
              {
                return true; // notice
              }
              /// hpx::parallel_find algorithm is very much useful
              /// when doing operations on Associative domains
              /// +=, -= , Is_member etc

              /// T is must moveinsertable and EmplaceConstructible
              /// Associative domain should not contain duplicate indexes
              /// Before adding indexes check whether it is already available
              /// Returns
              ///     True  : If the element is added to the index_set
              ///     false : If the element is not added to the index_set (duplicate)
            bool operator+= ( T& value)
              {

                 using namespace hpx::parallel;
                 auto ref = hpx::parallel::find(execution::par(execution::task),index_set.begin(),index_set.end(),value);
                 if(ref.get() == index_set.end())
                 {
                   index_set.emplace_back(value);
                   set_size++;
                   return true;
                 }
                  return false;
              }
            bool operator+= (T&& value)
              {
                /// Before adding indexes check whether it is already available
                using namespace hpx::parallel;
                auto ref = hpx::parallel::find(execution::par(execution::task),
                index_set.begin(),index_set.end(),std::move(value));
                if(ref.get() == index_set.end())
                {
                  index_set.emplace_back(std::move(value));
                  set_size++;
                  return true;
                }
                  return false;
              }
            /// This removes if the index is present in the set
            /// Returns true if the index exists and it is removed, False on missing
            /// Find & remove
            bool operator-= ( T& value)
            {
              using namespace hpx::parallel;
              auto fut_remove_index = hpx::parallel::find(execution::par(execution::task),
              index_set.begin(),index_set.end(),value);
              auto remove_index = fut_remove_index.get();
              if( remove_index != index_set.end())
                {
                  index_set.erase(remove_index);
                  set_size--;
                  return true;
                }
                return false;
            }

            bool operator-= (T&& value)
            {
              using namespace hpx::parallel;
              auto fut_remove_index = hpx::parallel::find(execution::par(execution::task),
              index_set.begin(),index_set.end(),std::move(value));
              auto remove_index = fut_remove_index.get();
              if(remove_index != index_set.end())
                {
                  index_set.erase(remove_index);
                  set_size--;
                  return true;
                }
                return false;
            }
           bool is_member(T& value)
            {
              using namespace hpx::parallell;
              auto remove_index = hpx::parallel::find(execution::par(execution::task),
              index_set.begin(),index_set.end(),value).get();
                if(remove_index != index_set.end())
                    return true;
                else
                    return false;
            }
           bool is_member(T&& value)
            {
              using namespace hpx::parallell;
              auto remove_index = hpx::parallel::find(execution::par(execution::task),
              index_set.begin(),index_set.end(),std::move(value)).get();
                if(remove_index != index_set.end())
                    return true;
                else
                    return false;
            }

           /// Operations special to Associative domains
           ///  Union, intersection, Difference, Symmetric Difference
           /// Please note that
          /*
              These Operations will rearrange the indexes to the diffrent positions within the set.
              However they are equal !
              i.e
                  A = B are equal if and only if
                                    for all x [ x belongs to A  <-> x belongs to B]
                  order of elements does not matters
          */
          // Not checked possibilty of errors
          domain<T> operator| (const domain& second)
              {
                std::vector<T> dest;
                using namespace hpx::parallel;
                hpx::future<bool> flag_1 = hpx::parallel::is_sorted(execution::par(execution::task),
                second.index_set.begin(), second.index_set.end());
                if(flag_1.get() == 0)
                {
                  auto fut_random_it_2 = hpx::parallel::sort(execution::par(execution:::task),second.index_set.begin()
                  second.index_set());
                }
                hpx::future<bool> flag_2 = hpx::parallel::is_sorted(execution::par(execution::task),
                index_set.begin(), index_set.end());
                if(flag_2.get() == 0)
                {
                  auto fut_random_it_1 = hpx::parallel::sort(execution::par(execution::task),index_set.begin(),
                    index_set.end());
                }
                return hpx::dataflow(hpx::launch::sync,
                [this, &second, &dest](auto &&value_1, auto &&value_2)
                  {
                     using namespace hpx::parallel;

                     auto temp = hpx::parallell::set_union(execution::par(execution::task),
                     this->index_set.begin(),this->index_set.end(),
                     second.index_set.begin(),second.index_set.end(),
                     std::back_inserter(dest));  // check correctness
                     return domain(dest);
                  }, std::move(fut_random_it_1), std::move(fut_random_it_2)
                );
              }

           domain<T> operator& (const domain<T>& second)
            {
              std::vector<T> dest;
              using namespace hpx::parallel;
              hpx::future<bool> flag_1 = hpx::parallel::is_sorted(execution::par(execution::task),
              second.index_set.begin(), second.index_set.end());
              if(flag_1 == 0)
              {
                auto fut_random_it_2 = hpx::parallel::sort(execution::par(execution:::task),second.index_set.begin()
                second.index_set());
              }
              hpx::future<bool> flag_2 = hpx::parallel::is_sorted(execution::par(execution::task),
              index_set.begin(), index_set.end());
              if(flag_2 == 0)
              {
                auto fut_random_it_1 = hpx::parallel::sort(execution::par(execution::task),index_set.begin(),
                index_set.end());
              }
              return hpx::dataflow(hpx::launch::sync,
              [this, &second, &dest](auto &&value_1, auto &&value_2)
                {
                   using namespace hpx::parallel;

                   auto temp  = hpx::parallell::set_intersection(execution::par(execution::task),
                   this->index_set.begin(),this->index_set.end(),
                   second.index_set.begin(),second.index_set.end(),
                   std::back_inserter(dest));
                   return domain(dest);
                }, std::move(fut_random_it_1), std::move(fut_random_it_2)
              );
            }

           domain<T> operator- (const domain<T>& second)
            {
              std::vector<T> dest;
              using namespace hpx::parallel;
              hpx::future<bool> flag_1 = hpx::parallel::is_sorted(execution::par(execution::task),
              second.index_set.begin(), second.index_set.end());
              if(flag_1 == 0)
              {
                auto fut_random_it_2 = hpx::parallel::sort(execution::par(execution:::task),second.index_set.begin()
                second.index_set());
              }
              hpx::future<bool> flag_2 = hpx::parallel::is_sorted(execution::par(execution::task),
              index_set.begin(), index_set.end());
              if(flag_2 == 0)
              {
                auto fut_random_it_1 = hpx::parallel::sort(execution::par(execution::task),index_set.begin(),
                index_set.end());
              }
              return hpx::dataflow(hpx::launch::sync,
              [this, &second, &dest](auto &&value_1, auto &&value_2)
                {
                   using namespace hpx::parallel;

                   auto temp  = hpx::parallell::set_difference(execution::par(execution::task),
                   this->index_set.begin(),this->index_set.end(),
                   second.index_set.begin(),second.index_set.end(),
                   std::back_inserter(dest));
                   return domain(dest);
                }, std::move(fut_random_it_1), std::move(fut_random_it_2)
              );
            }

            domain<T> operator^ (const domain<T> & second)
            {
              std::vector<T> dest;
              using namespace hpx::parallel;
              hpx::future<bool> flag_1 = hpx::parallel::is_sorted(execution::par(execution::task),
              second.index_set.begin(), second.index_set.end());
              if(flag_1 == 0)
              {
                auto fut_random_it_2 = hpx::parallel::sort(execution::par(execution:::task),second.index_set.begin()
                second.index_set());
              }
              hpx::future<bool> flag_2 = hpx::parallel::is_sorted(execution::par(execution::task),
              index_set.begin(), index_set.end());
              if(flag_2 == 0)
              {
                auto fut_random_it_1 = hpx::parallel::sort(execution::par(execution::task),index_set.begin(),
                index_set.end());
              }
              return hpx::dataflow(hpx::launch::sync,
              [this, &second, &dest](auto &&value_1, auto &&value_2)
                {
                   using namespace hpx::parallel;

                   auto temp  = hpx::parallell::set_symmetric_difference(execution::par(execution::task),
                   this->index_set.begin(),this->index_set.end(),
                   second.index_set.begin(),second.index_set.end(),
                   std::back_inserter(dest));
                   return domain(dest);
                }, std::move(fut_random_it_1), std::move(fut_random_it_2)
              );
            }

           T& first()
              {
                return index_set.front();
              }
            T& last()
              {
                return index_set.back();
              }
            ~domain() {}

};

/*domain<string>  D;
domain<string>  D("d","de","ds","ds");
string str = "pat";
string str = "parsa";
domain<string>  D(str,str);
domain<int>  something;
*/



  //  Arithmetic domains
  //  Dimension specialization gives user's a higher level view for Arithmetic Domains
  //  and to get far away from native types

template <>
struct domain<dimension>
  {
     private:
        std::vector<std::pair<int,int> > boundary;
        std::size_t dimension;
        domain(std::vector<std::pair<int,int>const &v1)
        : boundary(v1.boundary) , dimension(v1.dimension) {}

     public:
        // Constructors
        domain(std::initializer_list<std::pair<int,int>>& val)
        : boundary{val} , dimension(boundary.size()) {}

        domain(std::initializer_list<std::pair<int,int>>&& val)
        : boundary{std::move(val)} ,dimension(boundary.size()) {}

        //copy Constructors & move Constructors

        domain(domain<dimension>& other)
        : boundary(other.boundary), dimension(other.dimension) {}

        domain(domain<dimension>&& other)
        : boundary(std::move(other.boundary)) , dimension(std::move(other.dimension)) {}

        // copy & move assignable

        domain<dimension>&  operator= (domain<dimension> & other)
        {
          boundary   = other.boundary;
          dimension  = other.dimension;
          return *this;
        }

         domain<dimension>&  operator= (domain<dimension>&& other)
          {
            boundary  = std::move(other.boundary);
            dimension = std::move(other.dimension);
            return *this;
          }

        void expand(std::vector<int> const & exp)
          {
            HPX_ASSERT(dimension == exp.size())
            for(auto it_1 = boundary.begin(), it_2 = exp.begin();
                it_1 != boundary.end() && it_2 != exp.end() ;it_1++,it_2++)
                {
                  (*it_1).second += *it_2;
                }
          }
        domain<dimension> translate(std::vector<int> const& exp)
          {
              HPX_ASSERT(dimension == exp.size())
              std::vector<std::pair<int,int>> s;
              s.reserve(boundary.size());
              for(auto it_2 = s.begin(),it_1 = boundary.begin(),it_3 = exp.begin() ;
                  it_1 != s.end() && it_2 != boundary.end() && it_3 != exp.end(); it_1 ++ ,it_2 ++, it_3++)
              {
                  (*it_2).second  = (*it_1).second + *it_3;
              }
            return  domain(s);
          }
        std::vector<std::size_t> shape()
          {
            std::vector<std::size_t>& shape_;
            for(auto i : boundary)
              {
                if(i.first < 0 || i.second < 0)
                shape_.emplace_back((std::abs(i.first)) + std::abs(i.second) + 1);
                else
                shape_.emplace_back(std::abs(i.first-i.second) + 1);
              }
            return shape_;
          }
        void clear()
          {
             boundary.clear();
             dimension = 0;
          }
        bool is_cartesian_domain()
          {
            return true;
          }
        bool operator==( const domain<dimension>& other)
          {
            if(dimension == other.dimension)
              {
                for(std::size_t i = 0;  i < dimension; i++)
                    {
                      if(boundary[i].first == other.boundary[i].first &&
                         boundary[i].second == other.boundary[i].second)
                       else
                          return false;
                    }
                return true;
              }
            else
              {
                return false;
              }
          }
          bool operator!=( const domain<dimension>& other)
            {
              if(dimension == other.dimension)
                {
                  for(std::size_t i = 0;  i < dimension; i++)
                      {
                        if(boundary[i].first == other.boundary[i].first &&
                           boundary[i].second == other.boundary[i].second)
                         else
                            return true;
                      }
                  return false;
                }
              else
                {
                  return true;
                }
            }
         std::size_t num_indices()
          {
            std::vector<std::size_t> &values = shape();
            std::size_t result = std::accumulate(values.begin(), values.end(), 1,
                                 std::multiplies<std::size_t>());
            return result;
          }





  };
