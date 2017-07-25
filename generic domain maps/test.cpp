#include "domainmap.hpp"
#include <iostream>
#include <hpx/hpx_main.hpp>
int main()
{
  using namespace hpx::domain_maps;
  batch all;
  distribution obj(hpx::domain_maps::internals::distribution_type::BLOCKED);
  policy<1> polic(obj);
  domain_map<int,1>   dom(200, polic, all);


        std::cout<< " \n BLOCKED DISTRIBUTION ";
    for(int i = 0; i < 200; i++)
    {
        std::cout<<"\n in loc" <<dom.locality_at(i);
    }


    std::cout <<"Extent in domains" << dom.extent(0);
    std::cout<<"NO of localities "<<dom.get_no_of_localities();
    std::cout<<"\n Blocksize " <<dom.get_block_size();
    std::cout<<"\n no of blocks" <<dom.get_no();
    std::cout<<"\n local size"<<dom.get_local_size();
    std::cout<<"\n local capacity" <<dom.get_local_capacity();

    batch all_cy;
      distribution obj_cy(hpx::domain_maps::internals::distribution_type::CYCLIC);
      policy<1> polic_cy(obj_cy);
      domain_map<int,1>   dom_cy(200, polic_cy, all_cy);


            std::cout<< " \n CYCLIC DISTRIBUTION ";
        for(int i = 0; i < 200; i++)
        {
            std::cout<<"\n in loc" <<dom_cy.locality_at(i);
        }


        std::cout <<"Extent in domains" << dom_cy.extent(0);
        std::cout<<"NO of localities "<<dom_cy.get_no_of_localities();
        std::cout<<"\n Blocksize " <<dom_cy.get_block_size();
        std::cout<<"\n no of blocks" <<dom_cy.get_no();
        std::cout<<"\n local size"<<dom_cy.get_local_size();
        std::cout<<"\n local capacity" <<dom_cy.get_local_capacity();

        batch all_b;
         distribution obj_b(hpx::domain_maps::internals::distribution_type::BLOCKCYCLIC,10);
         policy<1> polic_b(obj_b);
         domain_map<int,1>   dom_b(200, polic_b, all_b);


               std::cout<< " \n BLOCKED CYCLIC DISTRIBUTION ";
           for(int i = 0; i < 200; i++)
           {
               std::cout<<"\n in loc" <<dom_b.locality_at(i);
           }


           std::cout <<"Extent in domains" << dom_b.extent(0);
           std::cout<<"NO of localities "<<dom_b.get_no_of_localities();
           std::cout<<"\n Blocksize " <<dom_b.get_block_size();
           std::cout<<"\n no of blocks" <<dom_b.get_no();
           std::cout<<"\n local size"<<dom_b.get_local_size();
           std::cout<<"\n local capacity" <<dom_b.get_local_capacity();



         return 0;
       }
