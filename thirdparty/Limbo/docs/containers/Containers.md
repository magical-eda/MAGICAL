Limbo.Containers {#Containers}
---------

[TOC]

# Introduction {#Containers_Introduction}

Some specially designed containers for specific applications, such as disjoint set, multiple-level set, etc. 

# Examples {#Containers_Examples}

test_disjointset.cpp
~~~~~~~~~~~~~~~~{.cpp}
#include <iostream>
#include <limbo/containers/DisjointSet.h>

int main()
{
    std::vector<int> vParent (10);
    std::vector<int> vRank (vParent.size());

    typedef limbo::containers::DisjointSet disjoint_set_type;
    disjoint_set_type::SubsetHelper<int, int> gp (vParent, vRank);

    disjoint_set_type::union_set(gp, 2, 3);
    disjoint_set_type::union_set(gp, 4, 6);
    disjoint_set_type::union_set(gp, 5, 8);

    std::cout << "number of sets = " << disjoint_set_type::count_sets(gp) << std::endl; 
    std::cout << "parent of subset of element 4 = " << disjoint_set_type::find_set(gp, 4) << std::endl; 

    disjoint_set_type::union_set(gp, 2, 4); 

    std::cout << "number of sets = " << disjoint_set_type::count_sets(gp) << std::endl; 
    std::cout << "parent of subset of element 4 = " << disjoint_set_type::find_set(gp, 4) << std::endl; 

    return 0; 
}
~~~~~~~~~~~~~~~~

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_disjointset test_disjointset.cpp -I $LIMBO_DIR/include
./test_disjointset
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
number of sets = 7
parent of subset of element 4 = 4
number of sets = 6
parent of subset of element 4 = 2
~~~~~~~~~~~~~~~~

# References {#Containers_References}

- [limbo/containers/DisjointSet.h](@ref DisjointSet.h)
- [limbo/containers/FastMultiSet.h](@ref FastMultiSet.h)
