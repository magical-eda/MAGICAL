Limbo.Parsers.VerilogParser {#Parsers_VerilogParser}
---------

[TOC]

# Introduction {#Parsers_VerilogParser_Introduction}

[Verilog](https://en.wikipedia.org/wiki/Verilog) is a hardware programming language. 
In VLSI design, after logic synthesis, the circuit is converted from behavior level description to gate level netlist, which will be used in physical design. 
The parser supports reading the gate level netlists to help users initialize their databases. 

# Examples {#Parsers_VerilogParser_Examples}

## Flex/Bison Parser {#Parsers_VerilogParser_Flex_Bison}

See documented version: [test/parsers/verilog/test_bison.cpp](@ref verilog/test_bison.cpp)
\include test/parsers/verilog/test_bison.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_bison test_bison.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lverilogparser
./test_bison benchmarks/simple.v
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_VerilogParser_Examples_All}

- [test/parsers/verilog/test_bison.cpp](@ref verilog/test_bison.cpp)

# References {#Parsers_VerilogParser_References}

- [limbo/parsers/verilog/bison/VerilogDataBase.h](@ref VerilogDataBase.h)
- [limbo/parsers/verilog/bison/VerilogDriver.h](@ref VerilogDriver.h)
