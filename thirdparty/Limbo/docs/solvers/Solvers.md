Limbo.Solvers {#Solvers}
---------

[TOC]

# Introduction {#Solvers_Introduction}

Solvers and API for specialized problems, such as solving special linear programming problems with min-cost flow algorithms. 
It also wraps solvers like semidefinite programming solver [Csdp](https://projects.coin-or.org/Csdp "Csdp") and convex optimization solver [Gurobi](https://www.gurobi.com "Gurobi"). 

# Examples {#Solvers_Examples}

## Primal Min-Cost Flow Solvers {#Solvers_Examples_Primal_MCF}

The analysis and background of using primal min-cost flow to solve linear programming problem can be found 
in the detailed description of class @ref limbo::solvers::MinCostFlow. 

See documented version: [test/solvers/test_MinCostFlow.cpp](@ref test_MinCostFlow.cpp)
\include test/solvers/test_MinCostFlow.cpp

Compiling and running commands (assuming LIMBO_DIR, BOOST_DIR and LEMON_DIR are well defined). 
@ref Parsers_LpParser is required for @ref limbo::solvers::MinCostFlow to read input files in .lp format. 
~~~~~~~~~~~~~~~~
g++ -o test_MinCostFlow test_MinCostFlow.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include -I $LEMON_DIR/include -L $LEMON_DIR/lib -lemon -L $LIMBO_DIR/lib -llpparser
# test primal min-cost flow for linear programming problem 
./test_MinCostFlow lpmcf/benchmarks/mcf.lp
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
# debug.lgf 
@nodes
label	supply	name	potential	
0	0	node_constr_0	-4	
1	0	node_constr_1	-4	
2	-1	subg_constr_0	-1	
3	-1	subg_constr_1	0	
4	1	weight_constr	-4	
5	1	st	-8	
@arcs
		label	name	capacity_lower	capacity_upper	cost	flow	
0	2	0	x0_0	0	1	5	0	
0	3	1	x0_1	0	1	4	0	
1	2	2	x1_0	0	1	3	1	
1	3	3	x1_1	0	1	7	0	
5	3	4	xd1	0	1	8	1	
5	2	5	xd0	0	1	8	0	
4	0	6	x0	0	1	0	0	
4	1	7	x1	0	1	0	1	
~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~
Minimize
5.437 x0_0 + 4.689 x0_1 + 3.223 x1_0 + 7.654 x1_1
 + 8.7657 xd1 + 8.32079 xd0

Subject To
node_constr_0: 1 x0_0 + 1 x0_1 + -1 x0 = 0
node_constr_1: 1 x1_0 + 1 x1_1 + -1 x1 = 0
subg_constr_0: 1 x0_0 + 1 x1_0 + 1 xd0 = 1
subg_constr_1: 1 x0_1 + 1 x1_1 + 1 xd1 = 1
weight_constr: 1 x0 + 1 x1 = 1
Bounds
0 <= x0_0 <= 1
0 <= x0_1 <= 1
0 <= x1_0 <= 1
0 <= x1_1 <= 1
0 <= xd1 <= 1
0 <= xd0 <= 1
0 <= x0 <= 1
0 <= x1 <= 1
Generals
End
Problem solved OPTIMAL
# Objective 11.9887
x0_0 0
x0_1 0
x1_0 1
x1_1 0
xd1 1
xd0 0
x0 0
x1 1
~~~~~~~~~~~~~~~~
Be aware that only Capacity Scaling algorithm supports real value costs, while all other algorithms only support integer costs. 
All capacity and flow need to be integers. 

## Dual Min-Cost Flow Solvers {#Solvers_Examples_MCF}

The analysis and background of using dual min-cost flow to solve linear programming problem can be found 
in the detailed description of class @ref limbo::solvers::DualMinCostFlow and @ref limbo::solvers::lpmcf::LpDualMcf. 

See documented version: [test/solvers/test_DualMinCostFlow.cpp](@ref test_DualMinCostFlow.cpp)
\include test/solvers/test_DualMinCostFlow.cpp

Compiling and running commands (assuming LIMBO_DIR, BOOST_DIR and LEMON_DIR are well defined). 
@ref Parsers_LpParser is required for @ref limbo::solvers::DualMinCostFlow to read input files in .lp format. 
~~~~~~~~~~~~~~~~
g++ -o test_DualMinCostFlow test_DualMinCostFlow.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include -I $LEMON_DIR/include -L $LEMON_DIR/lib -lemon -L $LIMBO_DIR/lib -llpparser
# test dual min-cost flow for linear programming problem 
./test_DualMinCostFlow lpmcf/benchmarks/problem.lp
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
# debug.lgf 
@nodes
label	supply	name	potential	
0	-4	R1	-2	
1	-5	L1	-5	
2	-5	R2	-2	
3	-1	L2	-2	
4	3	x2	-3	
5	6	x1	-7	
6	3	x3	-4	
7	3	additional	-7	
@arcs
		label	capacity_upper	cost	flow	
5	4	0	3	4	1	
5	1	1	3	2	2	
5	0	2	3	2	3	
4	1	3	3	1	0	
4	0	4	3	1	1	
4	3	5	3	1	1	
4	2	6	3	1	2	
6	3	7	3	2	0	
6	2	8	3	2	3	
0	7	9	3	10	0	
7	1	10	3	2	0	
7	1	11	3	2	3	
2	7	12	3	10	0	
3	7	13	3	10	0	
4	7	14	3	10	0	
5	7	15	3	10	0	
6	7	16	3	10	0	
~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~
Minimize
2 R1 + -2 L1 + 1 R2 + -1 L2


Subject To
C0: 1 x2 + -1 x1 >= 4
C1: -1 L1 + 1 x1 >= -2
C2: 1 R1 + -1 x1 >= 2
C3: -1 L1 + 1 x2 >= -1
C4: 1 R1 + -1 x2 >= 1
C5: -1 L2 + 1 x2 >= -1
C6: 1 R2 + -1 x2 >= 1
C7: -1 L2 + 1 x3 >= -2
C8: 1 R2 + -1 x3 >= 2
Bounds
R1 >= -10
2 <= L1 <= 2
R2 >= -10
L2 >= -10
x2 >= -10
x1 >= -10
x3 >= -10
Generals
R1
L1
R2
L2
x2
x1
x3
End

Problem solved OPTIMAL
# Objective 6
R1 5
L1 2
R2 5
L2 5
x2 4
x1 0
x3 3
~~~~~~~~~~~~~~~~

## Csdp Solver {#Solvers_Csdp}

See documented version: [limbo/algorithms/coloring/SDPColoringCsdp.h](@ref SDPColoringCsdp.h)
\include limbo/algorithms/coloring/SDPColoringCsdp.h
See documented version: [test/algorithms/test_SDPColoring.cpp](@ref test_SDPColoring.cpp)
\include test/algorithms/test_SDPColoring.cpp

Compiling and running commands (assuming LIMBO_DIR, BOOST_DIR and LEMON_DIR are well defined). 
@ref Parsers_LpParser is required for @ref limbo::solvers::lpmcf::LpDualMcf to read input files in .lp format. 
~~~~~~~~~~~~~~~~
g++ -o test_lpmcf compare.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include -I $LEMON_DIR/include -L $LEMON_DIR/lib -lemon -L $LIMBO_DIR/lib -llpparser
# test: min-cost flow for network graph 
./test_lpmcf benchmarks/graph.lgf 
~~~~~~~~~~~~~~~~

## Gurobi API {#Solvers_Examples_GurobiApi}

A generalized API to solve problem described by @ref limbo::solvers::LinearModel with [Gurobi](https://www.gurobi.com "Gurobi") solver. 

See documented version: [test/solvers/test_GurobiApi.cpp](@ref test_GurobiApi.cpp)
\include test/solvers/test_GurobiApi.cpp

Compiling and running commands (assuming LIMBO_DIR, and GUROBI_HOME are well defined). 
~~~~~~~~~~~~~~~~
g++ -o test_GurobiApi test_GurobiApi.cpp -I $LIMBO_DIR/include -I $GUROBI_HOME/include -L $GUROBI_HOME/lib -lgurobi_c++ -lgurobi60 
# test Gurobi API for linear programming problem 
./test_GurobiApi
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
Optimize a model with 3 rows, 4 columns and 6 nonzeros
Coefficient statistics:
  Matrix range    [1e+00, 1e+00]
  Objective range [1e+00, 1e+00]
  Bounds range    [1e+00, 1e+00]
  RHS range       [1e-01, 5e-01]
Presolve removed 3 rows and 4 columns
Presolve time: 0.00s
Presolve: All rows and columns removed
Iteration    Objective       Primal Inf.    Dual Inf.      Time
       0    1.0000000e+00   0.000000e+00   0.000000e+00      0s

Solved in 0 iterations and 0.00 seconds
Optimal objective  1.000000000e+00
optStatus = 5
~~~~~~~~~~~~~~~~

## All Examples {#Solvers_Examples_All}

Possible dependencies: 
[Boost](http://www.boost.org), 
[Lemon](https://lemon.cs.elte.hu).  

- [test/solvers/lpmcf/test_lpmcf.cpp](@ref test_lpmcf.cpp)
- [test/solvers/test_solvers.cpp](@ref test_solvers.cpp)
- [test/solvers/test_MultiKnapsackLagRelax.cpp](@ref test_MultiKnapsackLagRelax.cpp)
- [test/solvers/test_MinCostFlow.cpp](@ref test_MinCostFlow.cpp)
- [test/solvers/test_DualMinCostFlow.cpp](@ref test_DualMinCostFlow.cpp)
- [test/solvers/test_GurobiApi.cpp](@ref test_GurobiApi.cpp)

# References {#Solvers_References}

- [limbo/solvers/lpmcf/Lgf.h](@ref Lgf.h)
- [limbo/solvers/lpmcf/LpDualMcf.h](@ref LpDualMcf.h)
- [limbo/solvers/api/CsdpEasySdpApi.h](@ref CsdpEasySdpApi.h)
- [limbo/solvers/api/GurobiApi.h](@ref GurobiApi.h)
- [limbo/solvers/MultiKnapsackLagRelax.h](@ref MultiKnapsackLagRelax.h)
- [limbo/solvers/MinCostFlow.h](@ref MinCostFlow.h)
- [limbo/solvers/DualMinCostFlow.h](@ref DualMinCostFlow.h)
