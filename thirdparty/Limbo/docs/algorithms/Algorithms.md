Limbo.Algorithms {#Algorithms}
---------

[TOC]

# Introduction {#Algorithms_Introduction}

This package provides various algorithms that are often used in optimization. 

## Graph Partition {#Algorithms_Introduction_Partition}

Partitioning algorithms are included such as FM partitioning \cite PAR_DAC1982_FM and its multiway extensions. 

## Graph Coloring {#Algorithms_Introduction_Coloring}

Various graph coloring algorithms are included such as 
integer linear programming (ILP) based coloring \cite TPL_TCAD2015_Yu, 
        semidefinite programming (SDP) based coloring \cite TPL_TCAD2015_Yu, 
        iterative linear programming (LP) based coloring \cite TPL_SPIE2016_Lin , 
        greedy approach \cite MPL_CACM1979_Brelaz, etc. 
It also provides graph simplification algorithms for the coloring problem, which can also be applied to other graph algorithms. 

## Graph Misc {#Algorithms_Introduction_Misc}

Implementation of graph utilities and some other graph algorithms such as maximum clique and maximum independent set. 

## Placement {#Algorithms_Introduction_Placement}

Useful VLSI placement strategies. 

# Examples {#Algorithms_Examples}

## FM Partition {#Algorithms_Examples_FM}

See documented version: [test/algorithms/test_FM.cpp](@ref test_FM.cpp)
\include test/algorithms/test_FM.cpp

Compiling and running commands (assuming LIMBO_DIR and BOOST_DIR are well defined). 
~~~~~~~~~~~~~~~~
g++ -o test_FM test_FM.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include -L $BOOST_DIR/lib -lboost_regex
# test FM algorithm with a simple graph 
./test_FM 
~~~~~~~~~~~~~~~~

Output
~~~~~~~~~~~~~~~~
------- partitions -------
{d c b a | h g f e }
------- connections -------
a - b - c
b - d - e - f
c - f - g
a - g
d - e - h
f - h
######### iteration #0 #########
======= trial phase =======
======= actual phase w. target_cnt = 2 =======
######### iteration #1 #########
======= trial phase =======
======= actual phase w. target_cnt = 0 =======
------- partitions -------
{g c b a | h f e d }
------- connections -------
a - b - c
b - d - e - f
c - f - g
a - g
d - e - h
f - h
~~~~~~~~~~~~~~~~

## All Examples {#Algorithms_Examples_All}

Possible dependencies: 
[Boost](http://www.boost.org), 
[Gurobi](https://www.gurobi.com), 
[Cbc](https://projects.coin-or.org/Cbc), 
[Lemon](https://lemon.cs.elte.hu), 
[Csdp](@ref ThirdParty), 
[OpenBLAS](@ref ThirdParty). 

- [test/algorithms/test_FM.cpp](@ref test_FM.cpp)
- [test/algorithms/test_ChromaticNumber.cpp](@ref test_ChromaticNumber.cpp)
- [test/algorithms/test_GraphSimplification.cpp](@ref test_GraphSimplification.cpp)
- [test/algorithms/test_ILPColoring.cpp](@ref test_ILPColoring.cpp)
- [test/algorithms/test_SDPColoring.cpp](@ref test_SDPColoring.cpp)
- [test/algorithms/test_LPColoring.cpp](@ref test_LPColoring.cpp)

# References {#Algorithms_References}

## Graph Partition {#Algorithms_References_Partition}

- [limbo/algorithms/partition/FM.h](@ref FM.h)
- [limbo/algorithms/partition/FMMultiWay.h](@ref FMMultiWay.h)

## Graph Coloring {#Algorithms_References_Coloring}

- [limbo/algorithms/coloring/Coloring.h](@ref Coloring.h)
- [limbo/algorithms/coloring/BacktrackColoring.h](@ref BacktrackColoring.h)
- [limbo/algorithms/coloring/ChromaticNumber.h](@ref ChromaticNumber.h)
- [limbo/algorithms/coloring/GraphSimplification.h](@ref GraphSimplification.h)
- [limbo/algorithms/coloring/GreedyColoring.h](@ref GreedyColoring.h)
- [limbo/algorithms/coloring/ILPColoring.h](@ref ILPColoring.h)
- [limbo/algorithms/coloring/ILPColoringLemonCbc.h](@ref ILPColoringLemonCbc.h)
- [limbo/algorithms/coloring/LPColoring.h](@ref LPColoring.h)
- [limbo/algorithms/coloring/SDPColoringCsdp.h](@ref SDPColoringCsdp.h)

## Graph Misc {#Algorithms_References_Misc}

- [limbo/algorithms/GraphUtility.h](@ref GraphUtility.h)
- [limbo/algorithms/MaxClique.h](@ref MaxClique.h)
- [limbo/algorithms/MaxIndependentSet.h](@ref MaxIndependentSet.h)

## Placement {#Algorithms_Reference_Placement}

- [limbo/algorithms/placement/GreedySearch.h](@ref GreedySearch.h)
