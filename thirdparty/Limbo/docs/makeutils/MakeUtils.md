Limbo.MakeUtils {#MakeUtils}
---------

[TOC]

# Introduction {#MakeUtils_Introduction}

It offers various small makefile utilities to find compilers and external packages with suitable linking flags. 

# Examples {#MakeUtils_Examples}

Example of usage in Makefile (assuming BOOST_DIR and LIMBO_DIR are exported as the environment variable to the path where Boost and limbo library are installed)
~~~~~~~~~~~~~~~~{.py}
# ==========================================================================
#                         Compilation Platform
# ==========================================================================

# detect platform
# determine linking flags 
UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	STATIC_LINK_FLAG = -Wl,-Bstatic # ld under Linux has fine control 
	DYNAMIC_LINK_FLAG = -Wl,-Bdynamic # ld under Linux has fine control 
endif 
ifeq ($(UNAME_S), Darwin)
	STATIC_LINK_FLAG = # dummy flags for linker
	DYNAMIC_LINK_FLAG = # dummy flags for linker
endif 

# ==========================================================================
#                         Compilation Flags
# ==========================================================================

# set CC, CXX, FC, AR
include $(LIMBO_DIR)/include/limbo/makeutils/FindCompiler.mk
# dependency to Boost and get BOOST_LINK_FLAG
include $(LIMBO_DIR)/include/limbo/makeutils/FindBoost.mk

# view compiler settings and BOOST_LINK_FLAG
$(info Compilers: CC = $(CC), CXX = $(CXX), FC = $(FC), AR = $(AR))
$(info Boost: BOOST_LINK_FLAG = $(BOOST_LINK_FLAG))

all: test 

# the appending DYNAMIC_LINK_FLAG is used to recover the linking flag 
test: test.cpp
    $(CXX) -o $@ $< -I $(BOOST_DIR) $(BOOST_LINK_FLAG) -L $(BOOST_DIR)/lib -lboost_iostreams $(DYNAMIC_LINK_FLAG)
~~~~~~~~~~~~~~~~

# References {#MakeUtils_References}

- [limbo/makeutils/FindAR.mk](@ref FindAR.mk)
- [limbo/makeutils/FindBoost.mk](@ref FindBoost.mk)
- [limbo/makeutils/FindGurobi.mk](@ref FindGurobi.mk)
- [limbo/makeutils/FindLemon.mk](@ref FindLemon.mk)
- [limbo/makeutils/FindZlib.mk](@ref FindZlib.mk)
- [limbo/makeutils/FindCompiler.mk](@ref FindCompiler.mk)
- [limbo/makeutils/FindClang.mk](@ref FindClang.mk)
- [limbo/makeutils/FindClangxx.mk](@ref FindClangxx.mk)
- [limbo/makeutils/FindGcc.mk](@ref FindGcc.mk)
- [limbo/makeutils/FindGxx.mk](@ref FindGxx.mk)
- [limbo/makeutils/FindGfortran.mk](@ref FindGfortran.mk)
- [limbo/makeutils/GCCVersion.mk](@ref GCCVersion.mk)
