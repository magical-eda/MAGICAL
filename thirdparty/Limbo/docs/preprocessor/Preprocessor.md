Limbo.Preprocessor {#Preprocessor}
---------

[TOC]

# Introduction {#Preprocessor_Introduction}

Some macros such as assertion with message. 

# Examples {#Preprocessor_Examples}

test_msg.cpp
~~~~~~~~~~~~~~~~{.cpp}
#include <limbo/preprocessor/Msg.h>

int main()
{
    int a = 1; 
    int b = 2; 

    limboPrint(limbo::kDEBUG, "a = %d, b = %d\n", a, b); 
    limboAssertMsg(a+b < 3, "a+b fails to be less than 3: a+b = %d", a+b); 

    return 0; 
}
~~~~~~~~~~~~~~~~

Compiling and running commands (assuming LIMBO_DIR is exported as the environment variable to the path where limbo library is installed)
~~~~~~~~~~~~~~~~
g++ -o test_msg test_msg.cpp -I $LIMBO_DIR/include
./test_msg
~~~~~~~~~~~~~~~~

Output 
~~~~~~~~~~~~~~~~
(D) a = 1, b = 2
(A) test_msg.cpp:14: int main(): Assertion `a+b < 3' failed: a+b fails to be less than 3: a+b = 3
Aborted
~~~~~~~~~~~~~~~~


# References {#Preprocessor_References}

- [limbo/preprocessor/Msg.h](@ref Msg.h)
- [limbo/preprocessor/AssertMsg.h](@ref AssertMsg.h)
- [limbo/preprocessor/PrintMsg.h](@ref PrintMsg.h)
