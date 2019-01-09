# ==========================================================================
# general file for inclusion in sub-makefiles 
# make sure required variables are already set before including this file 

# define PREFIX for installation destination
# default PREFIX is current folder 
# it must be absolute path 
PREFIX = $(realpath $(PROJECT_ROOT_DIR))/install
