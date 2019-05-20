Limbo.Parsers {#Parsers}
---------

This package provides parsers for various file formats. 
In general, a parser provides API for users to read and initialize data easily to their own database. 
Parsers should not be storage hungry since they only need to store temporary data before render to users' database. 
Therefore, all the parsers developed in the package follows the same philosophy. 
That is, users only need to provide a database object with required callback functions well defined, 
and a parser will read input files and invoke those callback functions with proper data. 
There is no need for users to know the process of parsing. 
The only thing they need to do is to initialize their own database in the callback functions with the data from the parser. 

# Components 
| Components                                              | Languages & Libraries           | Description                                              |
| ------------------------------------------------------- | ------------------------------- | -------------------------------------------------------- |
| [Bookshelf Parser](@ref Parsers_BookshelfParser)        | Flex/Bison                      | Placement file format from UCSD                          |
| [DEF Parser](@ref Parsers_DefParser)                    | Flex/Bison                      |                                                          |
|                                                         | Boost.Spirit                    | Deprecated due to efficiency                             |
| [Ebeam Parser](@ref Parsers_EbeamParser)                | Flex/Bison                      |                                                          |
|                                                         | Boost.Spirit                    | Deprecated due to efficiency                             |
| [LEF Parser](@ref Parsers_LefParser)                    | Flex/Bison                      |                                                          |
|                                                         | Boost.Spirit                    | Deprecated due to efficiency                             |
| [GDF Parser](@ref Parsers_GdfParser)                    | Flex/Bison                      | Routing file format from Jason Cong's group at UCLA      |
| [GDSII Parser](@ref Parsers_GdsiiParser)                | Stream parser: C/C++            | Support .gds and .gds.gz when Boost and Zlib available   |
|                                                         | ASCII parser: Boost.Spirit      | Deprecated due to memory efficiency                      |
|                                                         | GdsDB wrapper: C/C++, Boost     | High level wrapper for full layout import/export/flatten |
| [Tf Parser](@ref Parsers_TfParser)                      | Boost.Spirit                    | Read technology file for physical design                 |
| [Verilog Netlist Parser](@ref Parsers_VerilogParser)    | Flex/Bison                      | Read verilog netlist to initialize nets during placement |
| [LP Parser](@ref Parsers_LpParser)                      | Flex/Bison                      | Read linear programming problem, compatible with Gurobi  |
