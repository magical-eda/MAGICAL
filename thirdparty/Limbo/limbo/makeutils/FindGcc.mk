## @file   FindGcc.mk
## @brief  find gcc, determine CC
## @author Yibo Lin
## @date   Jan 2017
##
## @param  CC set CC as C compiler 

## @cond 

ifneq ($(shell which gcc),)
	CC = gcc
else
ifneq ($(shell which gcc48),)
	CC = gcc48
else
ifneq ($(shell which gcc-4.8),)
	CC = gcc-4.8
else
ifneq ($(shell which gcc47),)
	CC = gcc47
else
ifneq ($(shell which gcc-4.7),)
	CC = gcc-4.7
else 
ifneq ($(shell which gcc44),)
	CC = gcc44
else
ifneq ($(shell which gcc-4.4),)
	CC = gcc-4.4
else 
	CC = gcc
endif
endif
endif
endif
endif
endif 
endif 

## @endcond
