## @file   FindLemon.mk
## @brief  find Lemon library given LEMON_DIR, set LEMON_LINK_FLAG to either STATIC_LINK_FLAG or DYNAMIC_LINK_FLAG
##
## It check whether Lemon library is installed as static library. If so, link to static library; otherwise, link to shared library. 
## Use following command in the Makefile to view the results 
## 
##     $(info Lemon: LEMON_LINK_FLAG = $(LEMON_LINK_FLAG))
##
## @author Yibo Lin
## @date   Jan 2017 11:18:47 AM CST
##
## @param  LEMON_DIR given path to Lemon library 
## @param  LEMON_LINK_FLAG set linking flag to Lemon 

## @cond

LEMON_LINK_FLAG :=

# use libemon.a to test whether lemon has static library
ifeq ($(wildcard $(LEMON_DIR)/lib/libemon.a),) 
	LEMON_LINK_FLAG = -Wl,-rpath,$(LEMON_DIR)/lib $(DYNAMIC_LINK_FLAG)
else 
	LEMON_LINK_FLAG = $(STATIC_LINK_FLAG)
endif 

## @endcond
