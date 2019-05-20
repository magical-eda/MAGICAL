## @file   FindClangxx.mk
## @brief  find clang++, determine CXX
## @author Yibo Lin
## @date   Jan 2017 
##
## @param  CXX set CXX as C++ compiler 

## @cond 

ifneq ($(shell which clang++),)
	CXX = clang++
else 
	CXX = g++
endif

## @endcond
