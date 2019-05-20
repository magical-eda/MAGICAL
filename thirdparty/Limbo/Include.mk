# ==========================================================================
# general file for inclusion in sub-makefiles 
# make sure required variables are already set before including this file 

# define PREFIX for installation destination
# default PREFIX is current folder 
# it must be absolute path 
PREFIX = $(realpath $(LIMBO_ROOT_DIR))

# ==========================================================================
#                         Compilation Platform
# ==========================================================================

# detect platform
# determine linking flags 
UNAME_S = $(shell uname -s)
ifeq ($(UNAME_S), Linux)
	LINK_FLAG = # overall linking flag, maybe changed later 
	STATIC_LINK_FLAG = -Wl,-Bstatic # ld under Linux has fine control 
	DYNAMIC_LINK_FLAG = -Wl,-Bdynamic # ld under Linux has fine control 
endif 
ifeq ($(UNAME_S), Darwin)
	LINK_FLAG = # overall linking flag, set to empty
	STATIC_LINK_FLAG = # dummy flags for linker
	DYNAMIC_LINK_FLAG = # dummy flags for linker
endif 

# ==========================================================================
#                                Compilers
# ==========================================================================

MAKE = make
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindCompiler.mk

# ==========================================================================
#                                Compilation Flags
# ==========================================================================

ifneq ($(findstring clang, $(CXX)), ) # CXX contains clang 
	include $(LIMBO_ROOT_DIR)/Makefile.clang 
else 
	include $(LIMBO_ROOT_DIR)/Makefile.gcc
endif 

# dependency to Boost and get BOOST_LINK_FLAG
ifdef BOOST_DIR
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindBoost.mk
endif
# dependency to Zlib and get ZLIB_LINK_FLAG
ifdef ZLIB_DIR
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindZlib.mk
endif
# dependency to GUROBI and get GUROBI_LINK_LIB and GUROBI_CXX_LINK_LIB
ifdef GUROBI_HOME
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindGurobi.mk
endif
# dependency to Lemon and get LEMON_LINK_FLAG
ifdef LEMON_DIR
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindLemon.mk
endif

# ==========================================================================
#                                 Lex/Yacc
# ==========================================================================

LEX = flex
YACC = bison

# detect flex version for FlexLexer.h
# a backup for version 2.5.37 is available in the library to be compatible with previous releases of limbo 
# for other versions, user need to specify FLEX_DIR environment variable 
ifneq ($(findstring 2.5.37,$(shell $(LEX) --version)),)
	LEX_INCLUDE_DIR = $(LIMBO_ROOT_DIR)/limbo/thirdparty/flex/2.5.37
else 
# LEX_INCLUDE_DIR is not set as environment variable, so that user has more flexibility to set either FLEX_DIR or LEX_INCLUDE_DIR 
ifeq ($(LEX_INCLUDE_DIR),) 
	LEX_INCLUDE_DIR = $(FLEX_DIR)/include # the path to FlexLexer.h 
endif 
endif 

# ==========================================================================
#                                 Libtool
# ==========================================================================

LIBTOOL = libtool

# ==========================================================================
#                                 Doxygen
# ==========================================================================

DOXYGEN = doxygen
