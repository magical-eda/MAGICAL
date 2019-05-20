Limbo.Parsers.DefParser {#Parsers_DefParser}
---------

[TOC]

# Introduction {#Parsers_DefParser_Introduction}

[DEF](https://en.wikipedia.org/wiki/Design_Exchange_Format) together with [LEF](https://en.wikipedia.org/wiki/Library_Exchange_Format) is a common file format in VLSI physical design. 
Generally LEF/DEF formats are quit complicated due to the complex cases in chip design. 
This parser is adjusted from the open source LEF parser released by [Cadence Design Systems](https://www.cadence.com) with C++ wrappers. 
The original parsers lie in the [Thirdparty package](@ref Thirdparty).
Users have to follow the [LICENSE](@ref Parsers_DefParser_License) agreement from the original release. 
The parser does not contain full API of the original LEF/DEF parsers, but it is tested under various academic benchmarks for VLSI placement. 

# Examples {#Parsers_DefParser_Examples}

## Flex/Bison Parser {#Parsers_DefParser_Flex_Bison}

See documented version: [test/parsers/def/test_adapt.cpp](@ref def/test_adapt.cpp)
\include test/parsers/def/test_adapt.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_adapt test_adapt.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -ldefparseradapt
./test_adapt benchmarks/simple.def
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_DefParser_Examples_All}

- [test/parsers/def/test_adapt.cpp](@ref def/test_adapt.cpp)

# References {#Parsers_DefParser_References}

- [limbo/parsers/def/adapt/DefDataBase.h](@ref DefDataBase.h)
- [limbo/parsers/def/adapt/DefDriver.h](@ref DefDriver.h)

# LICENSE {#Parsers_DefParser_License}

\include limbo/thirdparty/lefdef/5.8/def/LICENSE.TXT
