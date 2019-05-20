Limbo.Parsers.LefParser {#Parsers_LefParser}
---------

[TOC]

# Introduction {#Parsers_LefParser_Introduction}

[LEF](https://en.wikipedia.org/wiki/Library_Exchange_Format) together with [DEF](https://en.wikipedia.org/wiki/Design_Exchange_Format) is a common file format in VLSI physical design. 
Generally LEF/DEF formats are quit complicated due to the complex cases in chip design. 
This parser is adjusted from the open source LEF parser released by [Cadence Design Systems](https://www.cadence.com) with C++ wrappers. 
The original parsers lie in the [Thirdparty package](@ref ThirdParty).
Users have to follow the [LICENSE](@ref Parsers_LefParser_License) agreement from the original release. 
It is tested under various academic benchmarks for VLSI placement. 

# Examples {#Parsers_LefParser_Examples}

## Flex/Bison Parser {#Parsers_LefParser_Flex_Bison}

See documented version: [test/parsers/lef/test_adapt.cpp](@ref lef/test_adapt.cpp)
\include test/parsers/lef/test_adapt.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_adapt test_adapt.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -llefparseradapt
./test_adapt benchmarks/cells.lef
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_LefParser_Examples_All}

- [test/parsers/lef/test_adapt.cpp](@ref lef/test_adapt.cpp)


# References {#Parsers_LefParser_References}

- [limbo/parsers/lef/adapt/LefDataBase.h](@ref LefDataBase.h)
- [limbo/parsers/lef/adapt/LefDriver.h](@ref LefDriver.h)

# LICENSE {#Parsers_LefParser_License}

\include limbo/thirdparty/lefdef/5.8/lef/LICENSE.TXT
