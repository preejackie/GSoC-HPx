Google Summer of Code 2017 - Distributed Component Placement 

Organization - STE||AR GROUP (HPX Runtime System)

The GSoC'17 project proposal is [here](https://github.com/pree-jackie/GSoC-HPx/blob/master/Final_gsoc%20_proposal.pdf)

HPX (High Performance Parallelx) - Modern C++ General Purpose Runtime System for applications of any scale with standard Concurrency and Parallelism concepts. Applications written in HPX out-perform OpenMP/MPI.

HPX is an innovative mixture of 
  - Global System-wide Address Space (Active Global Address Space).
  - Fine grain Parallelism & lightweight synchronization between user-level threads.
  - Work queue based, Message driven computation.
  - Elegant semantics for working with Distributed Systems.
  - Support for different types of Hardwares such as accelerators(GPGPU),FPGA's,CPU.

HPX has 5 major subsystems 
  1) Threading Subsystem 
  2) Local Control Objects
  3) Parcelport Layer
  4) Performance Counter
  5) Active Global Address Space (AGAS)
  
  
  
**THREADING SUBSYSTEM** 

This subsystem represent the user-level hpx threads in the application not an OS thread. It is similar to std::thread! 
It is really Lightweight because constructing,executing,destructing a user-level thread takes only a 800-900 nanoseconds.
Constructing a user-level thread in HPX
 ```c++
 void function_to_be_done(int parameter)
 {
   hpx::cout << " function called with "<<parameter;
 }
 int main()
  {
    hpx::thread thread_f(&function_to_be_done,100);
    thread_f.join();
    return 0;
  }
  ```
HPX Runtime place the User-Level Threads into the work queue and then the user-level threads are executed on the OS threads with the 
help of task scheduler!

**LOCAL CONTROL OBJECTS**

This Subsystem represent constructs for task-launching in HPX runtime and the synchronization between them. 
Constructs such as hpx::apply, hpx::async, hpx::future, hpx::promise, hpx::await, hpx::broadcast, hpx::when_any,
hpx::when_all, hpx::when_some etc

eg 
```c++
int calculate_mean( std::vector<int> & vector_values)
{
  return std::accumulate(vector_values.begin(), vector_values.end(), 0);   // RVO
}  

int main()
{
hpx::id_type target = hpx::find_here();
std::vector<int>  values{1,2,2,3,4,5,6};
hpx::future<int> future_object = hpx::async(&calculate_mean, target, values)
                                  .then([](hpx::future<int> value)
                                       {
                                        return value * 2
                                       }).then([](hpx::future<int> value_1)
                                       { 
                                        return value_1 * 2;
                                       });
                                                                                                                                     hpx::apply(&calculate_mean,values);
auto result   =   future_object.get();
}
  ```
hpx::find_here() - Returns the current(local) locality id.
hpx::async       - Asynchronously launches the task in the target(locality) with their function parameters.
                   It returns the **future object** whose value is retrived using **.get()** method.

hpx::apply       - Asynchronously launches the task and doesn't return it's result.

**.then() continuation** executes only after the future becomes available. By .then() we can support **dataflow**.


**PARCELPORT LAYER**

 HPX currently uses ethernet,MPI as their network layer in distributed systems. Although other communication layer can also be
 used with HPX.

**PERFORMANCE COUNTERS**

 HPX supports dynamic adaptation. During the runtime of your application you can query about the no of threads in the system,amount of memory consumed,throughput of the particular compute node.With this mechanism we can dynamically load balance the cluster in order to get better performance.

**ACTIVE GLOBAL ADDRESS SPACE**

It is key feature that separates HPX from other HPC Libraries. It unifies the whole memory of the cluster as single common address 
space.Each object you allocate in HPX has an id. AGAS is global table which maps each id to it's allocated locality. 
Identity of the object doesn't change when you move the object from one locality to another.


**GSoC Project Description - Distributed Component Placement**

Implement a feature similar to [chapel programming language Domain maps](https://chapel-lang.org/tutorials/SC10/M10-6-DomainMaps.pdf) in HPX to support Distribution of Objects in the cluster of computer nodes using data distribution policies like cyclic,block-cyclic.


**First,things first** 

In one line - "Basically we are going to construct a **distributed array** in HPX" :)-

**Domains** in **Chapel** programming language are inspired by **ZPL(Language) Regions**. 

From Chapel:
> **Domain Maps are "recipes" that instruct the compiler how to map the global view of a computation to the target locales' memory and processors**

Domain Maps answers
1) How are arrays laid out in memory ?
2) How are arrays are stored by the locales ?

Domains represent the index-set of an array. It is a way of expressing **Data Parallelism** directly in the code.

Domain Types
1) Dense 
2) Sparse
3) Strided
4) Unstructed (for graphs)
5) Associative (for maps)

Domain Maps tells **how to arrange these indexes in the memory (row-major || column-major)** if the array is stored locally (single-node or multi-processor) and **how to distribute the domain indices to different localities** if the array is stored in distributed manner (multi compute nodes).

**Distribution Policies**

Local(single node)
 > row_major_layout, column_major_layout, compressed_row_format (for sparse domains, experimental).
 
Distributed (cluster of compute nodes)
 > block_dist_policy, cyclic_dist_policy, block_cyclic_dist_policy. 


**Domain_Map_Factory Class**

**Properties**

- Takes domain and distribution policy (local, by default) as it's arguments in constructor.

- If the distribution policy is local, the indices of the domain are mapped to the current(local) node in either row_major or column-major (based on specification).

- In this case, Domain_Map_Factory class creates the **mapped_domain_instance**.

- If the distribution policy is distributed, the indices of the domain are mapped to cluster of nodes in blockwise or cyclic or blockcyclic manner.

  Incase of distributed nodes, Domain_Map_Factory class creates a special map like data structure whose **keys** are       **hpx::locality_ids** and values are **range of domain indices** mapped to that particular locality.


**Operations**

 - Create_Mapped_Domain() 
 > creates an instance of Mapped_Domain.
 - query_localities() 
 > returns a list of localities where the indices are mapped.
 - query_domain()
 > returns a reference to the domain.
 
**Mapped_Domain** 
This type acts as a factory class to create arrays. 

**Properties**

- Maintains a list of locality_ids and range of domain indices mapped to each locality.

- Layout pattern such as row_major, column_major, compressed_row_format.

- This class creates an array, array is stored in the mapped_localities in the specified layout. In distributed case, the single global array instance contains multiple local array instances.

**Behaviours**

- operator[] () (Random access operator)

> Based on the index requested, this function finds the locality_id and the index in the local array in which the element is located and returns a reference to it if **array[] is LHS of =** and copy if **array[] is RHS of =**.

- get_iterator() 

> returns an instance of iterator over the array. Iterator traverses over the distributed global array by finding the indexed element locality and it's index in local array.

- Create_Array_Factory 

> This function creates an array, with mapped_domain in the selected localities.

- Get_Domain

> Returns the reference to the domain.

- Get_Localities

> Returns the vector of hpx::locality_id's which is associated with the Mapped_domain



I have learned a lot many new things during my GSoC 2017 Project. It is truly a unique experience a computer science passionate students can get in their lifetime. I highly encourage and motivate any computer science aspirant(students) to apply for Google Summer of Code program. 
