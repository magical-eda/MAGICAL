## @file   FindCompiler.mk
## @brief  find compiler under various platform, determine CXX, CC, FC, AR 
##
## Use following command in the Makefile to view the results 
## 
##     $(info Compilers: CC = $(CC), CXX = $(CXX), FC = $(FC), AR = $(AR))
##
## @author Yibo Lin
## @date   Jan 2017
##
## @param  LIMBO_DIR, LIMBO_ROOT_DIR given LIMBO_DIR or LIMBO_ROOT_DIR 
## @param  CC, CXX, FC, AR set CC as c compiler, CXX as c++ compiler, FC as fortran compiler, AR as archiver 

## @cond 

# detect platform
UNAME_S = $(shell uname -s)

# consider two cases, called when building Limbo library or called from other programs 
ifndef LIMBO_ROOT_DIR
ifdef LIMBO_DIR
	LIMBO_ROOT_DIR = $(LIMBO_DIR)/include
else
	$(error LIMBO_ROOT_DIR = $(LIMBO_ROOT_DIR), LIMBO_DIR = $(LIMBO_DIR))
endif
endif

# ==== search compilers under Linux
ifeq ($(UNAME_S), Linux)
#FOUNDCC = $(shell find ${PATH//:/ } -maxdepth 1 -executable -name gcc* -exec basename {} \;)

include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindGcc.mk
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindGxx.mk
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindGfortran.mk
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindAR.mk

endif 

# ==== search for compilers under Darwin
ifeq ($(UNAME_S), Darwin)

include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindClang.mk
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindClangxx.mk
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindGfortran.mk
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindAR.mk

endif 

## @endcond 
