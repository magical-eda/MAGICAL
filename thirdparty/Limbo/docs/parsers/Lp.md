Limbo.Parsers.LpParser {#Parsers_LpParser}
---------

[TOC]

# Introduction {#Parsers_LpParser_Introduction}

[LP format](https://www.gurobi.com/documentation/6.5/refman/lp_format.html) is among the various file format to describe optimization problems. 
The parser can read a special case of linear programming problem in the LP format compatible to [Gurobi](https://www.gurobi.com), which contains only differential constaints. 
The special linear programming problem can be solved by dual min-cost flow algorithm in the @ref Solvers package. 
Here is a sample file.
\include test/parsers/lp/benchmarks/problem.lp

# Examples {#Parsers_LpParser_Examples}

## Flex/Bison Parser {#Parsers_LpParser_Flex_Bison}

See documented version: [test/parsers/lp/test_bison.cpp](@ref lp/test_bison.cpp)
\include test/parsers/lp/test_bison.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_bison test_bison.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -llpparser
./test_bison benchmarks/problem.lp
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_LpParser_Examples_All}

- [test/parsers/lp/test_bison.cpp](@ref lp/test_bison.cpp)

# References {#Parsers_LpParser_References}

- [limbo/parsers/lp/bison/LpDataBase.h](@ref LpDataBase.h)
- [limbo/parsers/lp/bison/LpDriver.h](@ref LpDriver.h)
