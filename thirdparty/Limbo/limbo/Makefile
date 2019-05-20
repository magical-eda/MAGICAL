#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ..)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

# ==========================================================================
#                         Standard Setting
# ==========================================================================

all: geometry parsers programoptions thirdparty
.PHONY: geometry parsers programoptions thirdparty

geometry:
	$(MAKE) -C geometry

parsers: thirdparty
	$(MAKE) -C parsers

programoptions:
	$(MAKE) -C programoptions

thirdparty:
	$(MAKE) -C thirdparty

.PHONY: install
install:
	$(MAKE) install -C algorithms
	$(MAKE) install -C bibtex
	$(MAKE) install -C containers
	$(MAKE) install -C geometry
	$(MAKE) install -C makeutils
	$(MAKE) install -C math
	$(MAKE) install -C parsers
	$(MAKE) install -C preprocessor
	$(MAKE) install -C programoptions
	$(MAKE) install -C solvers
	$(MAKE) install -C string
	$(MAKE) install -C thirdparty
	
.PHONY: clean
clean:
	$(MAKE) clean -C geometry
	$(MAKE) clean -C parsers
	$(MAKE) clean -C programoptions
	$(MAKE) clean -C thirdparty

.PHONY: extraclean
extraclean:
	$(MAKE) extraclean -C geometry
	$(MAKE) extraclean -C parsers
	$(MAKE) extraclean -C programoptions
	$(MAKE) extraclean -C thirdparty
