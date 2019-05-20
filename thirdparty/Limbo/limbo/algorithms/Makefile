#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../..)

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
	mkdir -p $(PREFIX)/include/limbo/algorithms
	cp $(wildcard *.h) $(PREFIX)/include/limbo/algorithms
	mkdir -p $(PREFIX)/include/limbo/algorithms/coloring
	cp $(wildcard coloring/*.h) $(PREFIX)/include/limbo/algorithms/coloring
	mkdir -p $(PREFIX)/include/limbo/algorithms/partition
	cp $(wildcard partition/*.h) $(PREFIX)/include/limbo/algorithms/partition
	mkdir -p $(PREFIX)/include/limbo/algorithms/placement
	cp $(wildcard placement/*.h) $(PREFIX)/include/limbo/algorithms/placement
