#include <hpx/config.hpp>
#include <hpx/util/tuple.hpp>
namespace hpx
{
  template<typename Index_Type>
  struct oneDdomain
   {

   public:

      typedef Index_Type index_type;
      typedef std::size_t     size_type;
      typedef index_type element_type;

    oneDdomain():
            upper(0),
            lower(1),
            is_container_domain(false) {}

    oneDdomain(size_type size, bool is_ = false)
           {
             if(size > 0)
              {
                lower = 0;
                upper = size - 1;
              }
              else
                {
                  lower = 1;upper = 0;
                }
              is_container_domain = is_;
           }

      oneDdomain(oneDdomain const& other)
          {
            lower = other.lower;
            upper = other.upper;
            is_container_domain = other.is_container_domain;
          }

      oneDdomain(oneDdomain && other)
          {
            lower = std::move(other.lower);
            upper = std::move(other.upper);
            is_container_domain = std::move(other.is_container_domain);
          }

      oneDdomain(index_type start, index_type end, bool is_ = false)
        : lower(start), upper(end), is_container_domain(is_) {}


    std::size_t size() const
      {
        if(lower > upper) return 0;
        else
          return (upper - lower + 1);
      }

    element_type first() const
      {
         return lower;
      }
    element_type last() const
      {
         return upper;
      }

    bool contains(index_type const& idx) const
      {
        if( idx >= lower && idx <= upper)
            return true;
        else
            return false;
      }

    index_type advance(index_type idx, std::size_t n) const
      {
        return idx + n;
      }

    std::size_t distance(index_type & it1,index_type & it2) const
      {
        if(it1 >= it2) return it1 - it2;
        else
            return it2 - it1;
      }

    bool is_container_domain() const
      {
        return is_container_domain;
      }

    bool empty() const
    {
      return lower > upper;
    }

    template <typename T>
    oneDdomain operator&(T const& other) const
      {

        T::index_type  other_lower = other.first();
        T::index_type  other_upper = other.last();
          if(empty())
            return oneDdomain(other_lower, other_upper);

          if (other.empty())
            return oneDdomain(lower, upper);

          if(other_lower > upper)
            return oneDdomain(-1);

          if(lower > other_upper)
            return oneDdomain(-1);

          if(other.contains(upper) == true && other.contains(lower) == true)
              return oneDdomain(lower, upper);

           if(other.contains(upper) == true && other.contains(lower) == false)
              return oneDdomain(other_lower, upper);

          if(other.contains(lower) == true && other.contains(upper) == false)
              return oneDdomain(lower, other_upper);
      }

      template <typename T>
      oneDdomain operator|(T const& other) const
      {
        T::index_type  other_lower  = other.first();
        T::index_type  other_upper  = other.last();
        index_type t_lower = index_type();
        index_type t_upper = index_type();
        if(other_lower >= lower)
            t_lower = lower;
        else
            t_lower = other_lower;

        if(other_upper >= upper)
            t_upper = other_upper;
        else
            t_upper =  upper;

        oneDdomain(t_lower,t_upper, false);
      }
      void expand_by(std::size_t by)
        {
          static_assert(by >= 0," must be greater than zero");
          if(empty())
          {
            lower = index_type();
            upper = by;
          }else
            upper += by;
        }

      void contract_by(std::size_t by)
        {
          static_assert(by >=0 || !empty(), "must be greater than zero or trying to contract an empty domain");
          upper -= by;
        }

      oneDdomain translate(std::size_t val)
        {
          index_type temp =  upper + val;
          oneDdomain(lower,temp,false);
        }

      bool operator == (oneDdomain const& other) const
        {
          if(upper == other.last() && lower == other.first())
            return true;
          else
            return false;
        }

        bool operator != (oneDdomain const& other) const
          {
            if(upper != other.last() || lower != other.first())
              return true;
            else
              return false;
          }

        std::size_t rank() const
          {
            return 1;
          }

      void swap(oneDdomain const& other)
          {
            if(size() == other.size())
              {
                 index_type t_l, t_u;
                  t_l  =  other.first();
                  t_u  =  other.last();
                  other.set_lower(first());
                  other.set_upper(last());
                  set_lower(t_l);
                  set_upper(t_u);
              }
            else
              HPX_ASSERT(false);
          }


    template<typename T>
    friend std::ostream& operator<<(std::ostream& os, oneDdomain<T> const& to_p);


    private:
      index_type upper;
      index_type lower;
      bool       is_container_domain;
   };

   template<typename T>
    std::ostream& operator<<(std::ostream& print, oneDdomain<T> const& to_p)
    {
      if(to_p.empty())
        print << "Empty";
      else
        {
          print << "[ " << to_p.first() << ","
          << to_p.last() << " ]";
        }
      return print;
    }

    template<typename T,typename dom = oneDdomain<T>>
    struct dom1D
    {
      public:

      typedef dom                             domain_t;
      typedef hpx::util::tuple<domain_t>      domain_type;
      typedef typename domain_t::index_type   index_type;
      typedef typename domain_t::size_type    size_type;
      typedef typename domain_t::element_type element_type;
      private:
              domain_type  region;
            //  size_type    region_size;
      public:
            dom1D()
            {
              region(domain_t());
            //  region_size = 0;
            }

            dom1D(domain_t dom)
            {
              region = domain_type(dom);
              //region_size = dom.size();
            }

            dom1D(dom1D const& other)
            {
              region = other.region;
            //  region_size = other.region_size;
            }

            dom1D(dom1D && other)
            {
              region = std::move(other.region);
            //  region_size = std::move(other.region_size);
            }

           dom1D(index_type lower, index_type upper,bool is_container_domain = false)
           : region(domain_t(lower, upper, is_container_domain)){}
                //region_size((upper - lower)+1) {}

           dom1D(std::size_t n)
           : region(domain_t(n)) {}
                //region_size(n) {}

          std::size_t size() const
            {
              return get<0>(region).size();
            }

          element_type first() const
            {
              return element_type(get<0>(this->region).first());
            }

          element_type last() const
            {
                return element_type(get<0>(this->region).last());
            }

          bool contains(index_type idx) const
            {
              return get<0>(this->region).contains(idx);
            }

          index_type advance(index_type idx, std::size_t n) const
            {
              return get<0>(this->region).advance(idx,n);
            }

          std::size_t distance(index_type & it1,index_type & it2) const
            {
              return get<0>(this->region).distance(it1,it2);
            }

          bool is_container_domain()const
            {
              return get<0>(this->region).is_container_domain();
            }

          bool empty() const
            {
              return get<0>(this->region).empty();
            }


        dom1D operator&(dom1D const& other) const
        {
          return dom1D(get<0>.(this->region) & get<0>.(other.region));
        }

      /* template <typename ODom>
        dom1D operator&(ODom const& other)
        {
          return intersect(get<0>(this->m_domain), other.template get_domain<0>());
        }
      */
        dom1D operator|(dom1D const& other) const
        {
          return dom1D(get<0>.(this->region) | get<0>.(other.region));
        }

      void expand_by(std::size_t by)
      {
        return get<0>.(this->region).expand_by(by);
      }

      void contract_by(std::size_t by)
      {
        return get<0>.(this->region).contract_by(by);
      }

      void translate(std::size_t by)
      {
        return get<0>.(this->region).translate(by);
      }

      bool operator == (dom1D const& other) const
      {
        return ( get<0>.(this->region) == get<0>.(other.region) );
      }

      bool operator != (dom1D const& other) const
      {
        return (get<0>.(this->region)  != get<0>.(other.region));
      }

      std::size_t rank() const
      {
        return get<0>.(this->region).rank();
      }

      void swap(dom1D const& other)
      {
        return get<0>.(this->region).swap(other.region);
      }

    /*  template<int N>
  typename tuple_element<N,domain_type>::type get_domain() const
  { return get<N>(m_domain); }
    */
    template<typename T>
    friend std::ostream& operator<<(std::ostream& print, dom1D<T> const& to_p);

    };

    template<typename T>
  std::ostream & operator<<(std::ostream &print, dom1D<T> const& to_p)
  {
    print << get<0>(d.m_domain);
    return print;
  }

}
