#include <hpx/config.hpp>
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/runtime/components/server/locking_hook.hpp>
#include <hpx/runtime/components/server/component_base.hpp>
#include <hpx/runtime/components/server/component.hpp>

#include <vector>


 /*
    Individual components represents the parts of the hpx distributed  containers
    it should have the required put and get operations with them for accesing their
    objects from global data structure through the domain map.
 */

  namespace hpx
  {
    namespace domain_maps
    {

      // It is a light wrapper around the STL container

      template <typename T, typename Data>
      struct parts :  public hpx::components::locking_hook<
                        hpx::components::component_base<parts<T, Data> >
                                                          >
        {
          private:
                Data            structure;
                //std::size_t     part_size;

          public:
                typedef Data      part;
                typedef typename  part::allocator_type allocator_type;
                typedef typename  part::size_type      size_type;
                typedef typename  part::iterator       iterator_type;
                typedef typename  part::const_iterator const_iterator_type;

                typedef hpx::components::locking_hook<
                        hpx::components::component_base<parts<T, Data> >
                                                      > base;
            /*
                No default constructor because each part of the container want
                to know the exact size
            */
                parts()
                {
                  HPX_ASSERT(false);
                }

                explicit parts(const size_type& size_)
                :
                  structure(size_),
                  part_size(size_)
                  {}

                explicit parts(size_type && size_)
                :
                  structure(std::move(size_)),
                  part_size(std::move(size_))
                  {}

                parts(size_type size, T const& val)
                :
                  structure(size, val)
                  {}

                parts(size_type size, T const& val, allocator_type alloc)
                :
                  structure(size, val, alloc)
                  {}

                /*
                   copy and move should have no effect
                   but in partitioned_vector, Individual partitions has copy constructor,
                   move constructor, == && != operator overload I didn't find any use cases for them
                   my claim is that we should have them in the global view of  data structure, because
                   the containers are always following some sort of distribution, copying, moving
                   didn't have meaning i guess
                */

                // returns a copy of part
                part get_value() const
                  {
                    return structure;
                  }

                part const& get_value() const
                  {
                    return structure;
                  }

                size_type size() const
                  {
                    return structure.size();
                  }

                size_type max_size() const
                  {
                    return structure.max_size();
                  }

                size_type capacity() const
                  {
                    return structure.capacity();
                  }

                bool empty() const
                  {
                    return structure.empty()
                  }

                void resize(size_type n, T const& val)
                  {
                    structure.resize(n, val);
                  }

                void reserve(size_type n)
                  {
                    structure.reserve(n);
                  }

                /*
                  Individual partitions iterators should omitted
                  because iterators are built on the top of domain map only
                  not on the actual container
                 */

                /*
                    Element access operations
                      put && get operations
                */

                // Note
                // A simple flow of this function
                /*
                    Global view of container in hpx
                    hpx::batch all();
                      In all localities.
                    hpx::domain_map map (12,BLOCKED, all);
                      12       - denotes size
                      BLOCKED  - specifies the distribution of 12 elements to the localities owned
                                  by batch
                      all      - localities to distribute the elements

                    hpx::vector<T> some_vector(map);
                    creates a vector of type T over the domain map

                    some_vector[pos] ---> the domain map mappes the given pos to locality_id and the local pos
                                          using mapping function provided in the domain map impl.
                                          After mapping the locality_id, the global view container
                                          calls this(below) function with argument passed "pos" and
                                          retrives the Element by copy.
                */
                T get_value(size_type pos) const
                  {
                     return structure[pos];
                  }

                // same description applies here

                void set_value(size_type pos, T const& val)
                  {
                    structure[pos]  = val;
                  }
                // in case of dynamic container
                void add(T const& val)
                  {
                    structure.push_back(val);
                  }

                void clear()
                {
                  structure.clear()
                }
        };

    }
  }
