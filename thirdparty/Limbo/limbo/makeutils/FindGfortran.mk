## @file   FindGfortran.mk
## @brief  find gfortran, determine FC
## @author Yibo Lin
## @date   Jan 2017
##
## @param  FC set FC as fortran compiler 

## @cond

ifneq ($(shell which gfortran48),)
	FC = gfortran48
else
ifneq ($(shell which gfortran-4.8),)
	FC = gfortran-4.8
else
ifneq ($(shell which gfortran47),)
	FC = gfortran47
else
ifneq ($(shell which gfortran-4.7),)
	FC = gfortran-4.7
else 
ifneq ($(shell which gfortran44),)
	FC = gfortran44
else
ifneq ($(shell which gfortran-4.4),)
	FC = gfortran-4.4
else 
	FC = gfortran
endif
endif
endif
endif
endif
endif 

## @endcond
