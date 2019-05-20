## @file   GCCVersion.mk
## @brief  compute version of CXX, set GCCVERSION, assume CXX is gcc
##
## calculate gcc version without . and save to GCCVERSION
## Use following command in the Makefile to view the results 
## 
##     $(info GCCVERSION = $(GCCVERSION))
##
## @author Yibo Lin
## @date   Aug 2015
##
## @param CXX given CXX and assume it is gcc 
## @param GCCVERSION set version of gcc 

## @cond

empty:=
space:= $(empty) $(empty)
GCCVERSIONSTRING := $(shell expr `$(CXX) -dumpversion`)
#Create version number without "."
GCCVERSION := $(shell expr `echo $(GCCVERSIONSTRING)` | cut -f1 -d.)
GCCVERSION += $(shell expr `echo $(GCCVERSIONSTRING)` | cut -f2 -d.)
GCCVERSION += $(shell expr `echo $(GCCVERSIONSTRING)` | cut -f3 -d.)
# Make sure the version number has at least 3 decimals
GCCVERSION += 00
# Remove spaces from the version number
GCCVERSION := $(subst $(space),$(empty),$(GCCVERSION))
# Crop the version number to 3 decimals.
GCCVERSION := $(shell expr `echo $(GCCVERSION)` | cut -b1-3)

## @endcond
