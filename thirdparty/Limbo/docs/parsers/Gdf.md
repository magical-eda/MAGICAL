Limbo.Parsers.GdfParser {#Parsers_GdfParser}
---------

[TOC]

# Introduction {#Parsers_GdfParser_Introduction}

[GDF](http://cadlab.cs.ucla.edu/~pubbench/routing/.index.html) format is an academic format for VLSI routing from Prof. Jason Cong's group at UCLA. 
Here is a sample file. 
\include test/parsers/gdf/benchmarks/simple.gdf

# Examples {#Parsers_GdfParser_Examples}

## Flex/Bison Parser {#Parsers_GdfParser_Flex_Bison}

See documented version: [test/parsers/gdf/test_bison.cpp](@ref gdf/test_bison.cpp)
\include test/parsers/gdf/test_bison.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_bison test_bison.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lgdfparser
./test_bison benchmarks/simple.gdf
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_GdfParser_Examples_All}

- [test/parsers/gdf/test_bison.cpp](@ref gdf/test_bison.cpp)

# References {#Parsers_GdfParser_References}

- [limbo/parsers/gdf/bison/GdfDataBase.h](@ref GdfDataBase.h)
- [limbo/parsers/gdf/bison/GdfDriver.h](@ref GdfDriver.h)
