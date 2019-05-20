## @file   FindLPSolve.mk
## @brief  find lpsolve library given LPSOLVE_DIR, set LPSOLVE_LINK_LIB
## 
## It checks the version of lpsolve and generates linking flag. 
## Use following command in the Makefile to view the results 
## 
##     $(info LPSolve: LPSOLVE_VERSION = $(LPSOLVE_VERSION), LPSOLVE_LINK_LIB = $(LPSOLVE_LINK_LIB))
##
## @author Yibo Lin
## @date   Dec 2018 11:18:47 AM CST 
## 
## @param  LPSOLVE_VERSION version of lpsolve
## @param  LPSOLVE_LINK_LIB link to lpsolve

## @cond

LPSOLVE_VERSION = $(subst .so,,$(subst liblpsolve,,$(notdir $(wildcard $(LPSOLVE_DIR)/liblpsolve*.so))))
LPSOLVE_LINK_LIB = -Wl,-rpath,$(LPSOLVE_DIR) -L $(LPSOLVE_DIR) -llpsolve$(LPSOLVE_VERSION)


## @endcond 
