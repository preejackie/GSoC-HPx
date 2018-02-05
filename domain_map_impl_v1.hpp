// partitioned_domain to co-domain (list of localities)
   #include <hpx/config.hpp>

   #include <vector>
   #include <map>
  namespace hpx{
  namespace reimplement{
  struct domain_map_factory
    {
      domain_map_factory(const block_partitioned_domain& t, const std::vector<hpx::id_type>& loc_)
        :
          ref_partition(t),
          targets(loc_)
          {}
      domain_map_factory(const block_partitioned_domain& t, const hpx::id_type& loc_)
        :
          ref_partition(t),
          targets(loc_)
          {}

      domain_map_factory(const block_cyclic_partitioned_domain& t, const std::vector<hpx::id_type> loc_)
        :
          ref_partition(t),
          targets(loc_) {}

      domain_map_factory(const block_cyclic_partitioned_domain& t, const hpx::id_type& loc_)
        :
              ref_partition(t),
              targets(loc_) {}

      domain_map_factory(const cyclic_partitioned_domain &t, const std::vector<hpx::id_type>& loc_)
        :
          ref_partition(t),
          targets(loc_) {}
      domain_map_factory(const cyclic_partitioned_domain &t, const hpx::id_type& loc_)
        :
          ref_partition(t),
          targets(loc_) {}

      template<typename Mapper>
      void operator() (Mapper&& map)
      {
        unsigned int target_index;
        for(std::size_t i=0; i < ref_partition.size(); i++)
          {
            target_index = map(i);
            domain_map.emplace_back(i,target_index);
          }
      }
      private:
        std::map<int,int> domain_map;
        std::vector<hpx::id_type> targets;

    };
}
}
