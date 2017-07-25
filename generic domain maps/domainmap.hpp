#include <hpx/config.hpp>


#include "dimensional.hpp"
#include "batch.hpp"

#include <stdexcept>
namespace hpx{
namespace domain_maps{


  // It defines how the global indices are mapped to the units and to their local indexes
  // by following distribution policy per dimension
  // One here is 1 dimensional containers index space domain map

  template<typename indextype,
          std::size_t numdimensions>
 class domain_map
  {

    private:

        typedef typename  std::make_unsigned<indextype>::type size_type;
        typedef           policy<numdimensions>              policy_spec;
        typedef std::array<indextype, numdimensions> coordinate;

    private:
        size_type       extent_1;
        size_type       size;
        policy_spec     policy_of;
        batch           in_batch;
        size_type       block_size;
        size_type       no_of_blocks;
        size_type       nlocs;
        size_type       local_size;
        size_type       nlocalblocks;
        size_type       local_capacity;

    public:
    // Avoid default constructor
    domain_map() = delete;

    domain_map(size_type linear_extent, policy_spec pol, batch  set_)
     :
      extent_1(linear_extent),
      size     (linear_extent),
      policy_of(pol),
      in_batch(set_),
      block_size(assign_blocksize(linear_extent, pol, set_.size())),

      no_of_blocks(assign_nblocks
      (block_size, extent_1)),

      nlocs(in_batch.size()),

      local_size(assign_local_size(0, in_batch.position(0))),

      nlocalblocks(assign_nlocal_blocks(block_size, local_size)),

      local_capacity(assign_local_capacity())
    {}


    domain_map(const domain_map& other)
    {
      extent_1    = other.extent_1;
      size        = other.size;
      policy_of   = other.policy_of;
      in_batch    = other.in_batch;
      block_size  = other.block_size;
      no_of_blocks= other.no_of_blocks;
      nlocs       = other.nlocs;
      local_size  = other.local_size;
      nlocalblocks = other.nlocalblocks;
      local_capacity = other.local_capacity;
    }

    domain_map& operator=(const domain_map& other)
    {
      extent_1    = other.extent_1;
      size        = other.size;
      policy_of   = other.policy_of;
      in_batch    = other.in_batch;
      block_size  = other.block_size;
      nlocs       = other.nlocs;
      local_size  = local_size;
      nlocalblocks = other.nlocalblocks;
      local_capacity = local_capacity;
      no_of_blocks= other.no_of_blocks;

      return *this;
    }





    public:
      typedef struct
        {
          hpx::id_type loc_id;
          indextype    index;
        } local_index;

      typedef struct
        {
          hpx::id_type  loc_id;
          std::array<indextype,1> coords;
        } local_coords;

      // get respective locality id from given


      // 1. get locality_id where the global_coordinate points to

      hpx::id_type locality_at(const coordinate& g_co_ord) const
      {
          int loc_key     =  (g_co_ord[0] / block_size) % nlocs;
          return in_batch.at(loc_key);
      }


     //  2. given global linear index --> associated locality_id
      hpx::id_type locality_at(const indextype& global_index ) const
      {
        int loc_key   =  (global_index /block_size) % nlocs;
        return in_batch.at(loc_key);
      }

        // Given no of elements in the given dimension
      std::size_t extent(std::size_t ndim) const
      {
        if(ndim > 0)
          {
            throw std::logic_error("Requested extent for invalid dimension second or larger dimensions for one dimension space \n");

          }
        else
          return extent_1;
      }

      // local_extent  ---> Note need to do

      // converts given local_coordinate to linear local index

      indextype local_offset(const coordinate& local_co) const
        {
          return local_co[0];
        }


     // converts global coordinates into their associative locality
     //  local coordinates
      coordinate get_local_coordinate(const coordinate& global_co) const
      {

        std::array<indextype, 1> local_coordinate;

                local_coordinate[0] = static_cast<indextype>
                  (
                     (((global_co[0] / block_size) / nlocs) * block_size )
                      + (global_co[0] % block_size)
                  );

        return local_coordinate;


      }

      // converts global coordinates to their associative loc id and local coordinates

    local_coords get_local(const coordinate& global_co) const
        {
          return local_coords
          {
            locality_at(global_co),
            get_local_coordinate(global_co)
          };
        }
  /*
    to do
        converts global_index to  locality id and its respective  local index

        local_index get_local_index(const indextype& global_index) const
        {

        }

       converts global index to their associative locality id and local index
        {

        }

  */


    /*
        Converts local_coordinates into it's associative global_coordinate
        note parameter : locality id_key it will change later
    */
        coordinate  get_global(int loc_id, const coordinate& local_coords) const
        {

          if(nlocs > 2)
            {
                std::array<indextype, 1> global_coordinate;

                auto val_ = (policy_of[0].local_index_to_block_index(
                    static_cast<indextype>(loc_id),
                    local_coords[0],
                    nlocs
                  ) * block_size);

                   global_coordinate[0] = static_cast<indextype>(
                                            val_  + (local_coords[0] % block_size)
                                          );

                    return global_coordinate;

            }
          else

              return local_coords;

        }

      bool is_local(hpx::id_type loc_id, indextype indx) const
        {
            return locality_at(indx) == loc_id;
        }

      size_type get_no_of_localities() const
        {
            return nlocs;
        }

      size_type get_local_size() const
        {
            return local_size;
        }

      size_type  get_local_capacity() const
        {
           return local_capacity;
        }
      std::size_t get_block_size() const
        {
          return block_size;
        }
          std::size_t get_no () const
            {
              return no_of_blocks;
            }




    private:


    size_type assign_blocksize(size_type size, policy_spec dist, size_type nlocs) const
    {
      if(nlocs == 0 || size == 0)
        return 0;
      distribution rule  = dist.entity_[0];
      return rule.max_block_size(size, nlocs);
    }

    size_type assign_nblocks(size_type block_size, size_type to_allocate) const
      {
        if(to_allocate == 0 || block_size == 0)
            return 0;
        else
        return ((to_allocate + block_size) - 1)/block_size;
      }

    size_type assign_nlocal_blocks(size_type block_size, size_type local_size) const
    {
      size_type nlocs = local_size;
      if(block_size > 0)
        {
           return ( (nlocs + block_size) -1 / block_size);
        }
        else
          return nlocs;
    }

    size_type assign_local_capacity() const
      {
        if(nlocs == 0)
          return 0;
        else
          return ((no_of_blocks + nlocs - 1) / nlocs ) * block_size;
      }

    size_type assign_local_size(size_type loc_key, hpx::id_type id) const
      {
         if(nlocs == 0)
            return 0;
          else if (no_of_blocks == 1 &&  nlocs == 1)
            return size;
          else
          return   ((no_of_blocks / nlocs) * block_size  + (loc_key < (no_of_blocks % nlocs) ? block_size : 0));
      }


  };
}
}
