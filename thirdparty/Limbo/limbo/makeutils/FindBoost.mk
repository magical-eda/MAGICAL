## @file   FindBoost.mk
## @brief  find Boost library given BOOST_DIR, set BOOST_LINK_FLAG to either STATIC_LINK_FLAG or DYNAMIC_LINK_FLAG 
##  
## It check whether Boost library is installed as static library. If so, link to static library; otherwise, link to shared library. 
## Use following command in the Makefile to view the results 
##
##     $(info Boost: BOOST_LINK_FLAG = $(BOOST_LINK_FLAG))
## 
## @author Yibo Lin
## @date   Jan 2017
##
## @param  BOOST_DIR given path to Boost library 
## @param  BOOST_LINK_FLAG set linking flag to Boost

## @cond 

BOOST_LINK_FLAG :=

# use libboost_system.a to test whether boost has static library
ifeq ($(wildcard $(BOOST_DIR)/lib/libboost_system.a),) 
	BOOST_LINK_FLAG = -Wl,-rpath,$(BOOST_DIR)/lib $(DYNAMIC_LINK_FLAG)
else 
	BOOST_LINK_FLAG = $(STATIC_LINK_FLAG)
endif 

## @endcond
