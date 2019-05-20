Limbo.Parsers.EbeamParser {#Parsers_EbeamParser}
---------

[TOC]

# Introduction {#Parsers_EbeamParser_Introduction}

A file format to describe electron-ebeam lithography configurations used in {\cite DFMP_ASPDAC2016_Lin}. 
Here is a sample file. 
\include test/parsers/ebeam/benchmarks/simple.ebeam

# Examples {#Parsers_EbeamParser_Examples}

## Flex/Bison Parser {#Parsers_EbeamParser_Flex_Bison}

See documented version: [test/parsers/ebeam/test_bison.cpp](@ref ebeam/test_bison.cpp)
\include test/parsers/ebeam/test_bison.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_bison test_bison.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lebeamparser
./test_bison benchmarks/NanGate_15nm_UTDA.ebeam
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_EbeamParser_Examples_All}

- [test/parsers/ebeam/test_bison.cpp](@ref ebeam/test_bison.cpp)

# References {#Parsers_EbeamParser_References}

- [limbo/parsers/ebeam/bison/EbeamDataBase.h](@ref EbeamDataBase.h)
- [limbo/parsers/ebeam/bison/EbeamDriver.h](@ref EbeamDriver.h)
