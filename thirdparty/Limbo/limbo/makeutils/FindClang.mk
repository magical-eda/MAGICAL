## @file   FindClang.mk
## @brief  find clang, determine CC
## @author Yibo Lin
## @date   Jan 2017
##
## @param  CC set CC as C compiler 

## @cond

ifneq ($(shell which clang),)
	CC = clang
else 
	CC = gcc
endif

## @endcond
