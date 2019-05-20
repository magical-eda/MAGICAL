Limbo.String {#String}
---------

[TOC]

# Introduction {#String_Introduction}

Some useful utilities for string processing, such as check whether a string is integer, floating numbers, etc. 
It also provides functions to convert numbers to string. 

# Examples {#String_Examples}

## Compare two strings case-insensitive {#String_Compare}

See documented version: [test/string/test_compare.cpp](@ref test_compare.cpp)
\include test/string/test_compare.cpp

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_compare compare.cpp -I $LIMBO_DIR/include
./test_compare
~~~~~~~~~~~~~~~~
Output 
~~~~~~~~~~~~~~~~
string limbo2343slimbo and LiMbo2343SliMbo is not equal case-sensitive
string limbo2343slimbo and LiMbo2343SliMbo is equal case-insensitive
~~~~~~~~~~~~~~~~

## All Examples {#String_Examples_All}

- [test/string/test_compare.cpp](@ref test_compare.cpp)
- [test/string/test_string.cpp](@ref test_string.cpp)

# References {#String_References}

- [limbo/string/String.h](@ref String.h)
- [limbo/string/ToString.h](@ref ToString.h)
