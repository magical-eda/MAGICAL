Limbo.Parsers.TfParser {#Parsers_TfParser}
---------

[TOC]

# Introduction {#Parsers_TfParser_Introduction}

Technology file (tf) is used as a configuration file in EDA tools for different technology nodes. 
It defines various properties like the layer mapping.
Currently the parser reads mapping of layer name, layer id and layer abbreviation from the technology file. 

# Examples {#Parsers_TfParser_Examples}

## Boost.Spirit Parser {#Parsers_TfParser_Spirit}

See documented version: [test/parsers/tf/test_spirit.cpp](@ref tf/test_spirit.cpp)
\include test/parsers/tf/test_spirit.cpp

Compiling and running commands (assuming LIMBO_DIR and BOOST_DIR are exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_spirit test_spirit.cpp -I $LIMBO_DIR/include -I $BOOST_DIR/include 
./test_spirit benchmarks/technology.tf
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_TfParser_Examples_All}

- [test/parsers/tf/test_spirit.cpp](@ref tf/test_spirit.cpp)

# References {#Parsers_TfParser_References}

- [limbo/parsers/tf/spirit/TfParser.h](@ref TfParser.h)
- [limbo/parsers/tf/spirit/ErrorHandler.h](@ref ErrorHandler.h)
