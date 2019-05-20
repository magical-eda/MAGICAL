Limbo.Parsers.BookshelfParser {#Parsers_BookshelfParser}
---------

[TOC]

# Introduction {#Parsers_BookshelfParser_Introduction}

[Bookshelf format](http://vlsicad.ucsd.edu/GSRC/bookshelf/formats) is widely used in VLSI physical design such as placement and routing. 
It is originally developed by UCSD. 
The Bookshelf parser can read .aux file and extract all other files. 
Then it parses the rest files and invokes user-defined callback functions. 

# Examples {#Parsers_BookshelfParser_Examples}

## Flex/Bison Parser {#Parsers_BookshelfParser_Flex_Bison}

See documented version: [test/parsers/bookshelf/test_bison.cpp](@ref bookshelf/test_bison.cpp)
\include test/parsers/bookshelf/test_bison.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_bison test_bison.cpp -I $LIMBO_DIR/include -L $LIMBO_DIR/lib -lbookshelfparser
./test_bison benchmarks/simple/acc64.aux
~~~~~~~~~~~~~~~~

## All Examples {#Parsers_BookshelfParser_Examples_All}

- [test/parsers/bookshelf/test_bison.cpp](@ref bookshelf/test_bison.cpp)

# References {#Parsers_BookshelfParser_References}

- [limbo/parsers/bookshelf/bison/BookshelfDataBase.h](@ref BookshelfDataBase.h)
- [limbo/parsers/bookshelf/bison/BookshelfDriver.h](@ref BookshelfDriver.h)
