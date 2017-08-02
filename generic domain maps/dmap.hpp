#include <hpx/config.hpp>
//#include <hpx

#include <vector>
#include <iostream>

template<typename Indextype>
struct dmap
  {
    public:

    using indextype  = Indextype;
    using size_type  = std::size_t;
    typedef struct local_position
      {
        hpx::id_type loc_id;
        indextype    local_index;
      } local_position;

    public:
    // constructors

    dmap ()  = delete;

    dmap(Indextype ... limits, string layout_type = "row_major", std::vector<hpx::id_type> loc_)
    : extents{limits ...}, layout{layout_type}, rank{sizeof (limits)...}, localities(loc_)
      {}

    dmap(const & dmap other) = default;

    dmap(dmap && other) = default;

    template <typename ...indices>
    indextype linearization(indices ... index) const
    {
      static_assert(sizeof (index)... == rank, "invalid index");

      std::vector<indextype> index_{index ...};

      indextype linear_index = 0;
      size_type subs;
      size_type base = 1;

      if(layout == "row_major")
        {
          for(size_type i = 0; i < rank; i++)
              {
                for(size_type j = i + 1; j < rank; j++)
                    {
                      base = base * extents[j];
                    }
                linear_index += base * index_[i];
              }
            return linear_index;
        }
      else if(layout == "column_major")
        {
          for(size_type i = 0; i < rank; i++)
            {
              for(size_type size_type j = 0; j < i; j++)
                {
                   base = base * extents[j];
                }
                linear_index += base * index_[i];
            }
          return linear_index;
        }
      else
        {
            std::cout<< "\n not implemented";
        }
    }

    indextype linearization(indextype index) const
    {
      return index;
    }

    template <typename ...indices>
    local_position operator()(indices ... index) const
    {
      indextype global_linear_index = linearization(index ...);
      
    }

    private:

    std::vector<indextype> extents;
    string layout;
    size_type rank;

  };
