
template <typename T>
struct domain
{

};

struct dimension {};
template <>
struct domain<dimension>
 {
    std::vector<std::pair<int,int> > boundary;
    std::size_t dimension;
    domain(std::initializer_list<int>& v1)
     {
        boundary.reserve((v1.size()/2));
        for(auto & id : v1)
          {

          }
     }

 };
