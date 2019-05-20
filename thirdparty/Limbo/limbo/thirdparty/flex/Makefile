#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../../..)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

# ==========================================================================
#                         Standard Setting
# ==========================================================================

.PHONY: install
install:
	mkdir -p $(PREFIX)/include/limbo/thirdparty/flex/2.5.37
	cp $(wildcard 2.5.37/*.h) $(PREFIX)/include/limbo/thirdparty/flex/2.5.37

.PHONY: clean
clean:
