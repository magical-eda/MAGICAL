## @file   FindGurobi.mk
## @brief  find Gurobi library given GUROBI_HOME, set GUROBI_VERSION and GUROBI_LINK_LIB
## 
## It checks the version of Gurobi and generate linking flag. 
## Use following command in the Makefile to view the results 
## 
##     $(info Gurobi: GUROBI_VERSION = $(GUROBI_VERSION), GUROBI_LINK_LIB = $(GUROBI_LINK_LIB))
##
## @author Yibo Lin
## @date   Jan 2017 11:18:47 AM CST 
## 
## @param  GUROBI_VERSION version of Gurobi
## @param  GUROBI_LINK_LIB link to Gurobi
## @param  GUROBI_CXX_LINK_LIB link to Gurobi

## @cond

GUROBI_VERSION = $(subst .so,,$(subst libgurobi,,$(notdir $(wildcard $(GUROBI_HOME)/lib/libgurobi??.so))))
GUROBI_LINK_LIB = -Wl,-rpath,$(GUROBI_HOME)/lib -L $(GUROBI_HOME)/lib -lgurobi$(GUROBI_VERSION)
GUROBI_CXX_LINK_LIB = -Wl,-rpath,$(GUROBI_HOME)/lib -L $(GUROBI_HOME)/lib -lgurobi_c++ -lgurobi$(GUROBI_VERSION)


## @endcond 
