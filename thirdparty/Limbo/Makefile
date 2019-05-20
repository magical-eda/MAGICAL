# top makefile 
#
#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath .)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

# ==========================================================================
#                         Standard Setting
# ==========================================================================

all: limbo

.PHONY: limbo
limbo:
	$(MAKE) -C limbo

.PHONY: test
test: 
	$(MAKE) -C test

.PHONY: doc
doc:
	$(MAKE) -C docs

.PHONY: install
install:
	$(MAKE) install -C limbo
	$(MAKE) install -C docs

.PHONY: clean
clean:
	$(MAKE) clean -C limbo
	$(MAKE) clean -C test

.PHONY: extraclean
extraclean:
	$(MAKE) extraclean -C limbo
	$(MAKE) extraclean -C test
	$(MAKE) extraclean -C docs
