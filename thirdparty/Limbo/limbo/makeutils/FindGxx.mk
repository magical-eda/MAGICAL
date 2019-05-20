## @file   FindGxx.mk
## @brief  find g++, determine CXX
## @author Yibo Lin
## @date   Jan 2017
##
## @param  CXX set CXX as C++ compiler 

## @cond 

ifneq ($(shell which g++),)
	CXX = g++
else
ifneq ($(shell which g++48),)
	CXX = g++48
else
ifneq ($(shell which g++-4.8),)
	CXX = g++-4.8
else
ifneq ($(shell which g++47),)
	CXX = g++47
else
ifneq ($(shell which g++-4.7),)
	CXX = g++-4.7
else 
ifneq ($(shell which g++44),)
	CXX = g++44
else
ifneq ($(shell which g++-4.4),)
	CXX = g++-4.4
else 
	CXX = g++
endif
endif
endif
endif
endif
endif 
endif 

## @endcond
