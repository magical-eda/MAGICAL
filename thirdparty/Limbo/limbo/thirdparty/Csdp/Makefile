#
# This Makefile can be used to automatically build the entire package.  
#
# If you make changes in the "Makefile" under any subdirectory, you can
# rebuild the system with "make clean" followed by "make all".
#
#
# Generic. On most systems, this should handle everything.
#

#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../../..)
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

# ==========================================================================
#                         Standard Setting
# ==========================================================================

all:
	make -C lib CC=$(CC) libsdp.a
	@$(LIBMKDIR)
	mv lib/libsdp.a $(LIBDIR)
	make -C solver CC=$(CC) csdp
	make -C theta CC=$(CC) all 
	make -C example CC=$(CC) all 
	@mkdir -p $(LIMBO_ROOT_DIR)/bin
	mv solver/csdp $(LIMBO_ROOT_DIR)/bin

#
# Perform a unitTest
#

unitTest:
	make -C test CC=$(CC) all

#
# Install the executables in /usr/local/bin.
#

.PHONY: install
install:
	cmp -s $(PREFIX)/lib/libsdp.a $(LIBDIR)/libsdp.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/libsdp.* $(PREFIX)/lib; \
		mkdir -p $(PREFIX)/bin; \
		cp $(LIMBO_ROOT_DIR)/bin/csdp $(PREFIX)/bin; \
	fi
	mkdir -p $(PREFIX)/include/limbo/thirdparty/Csdp/include 
	cp $(wildcard include/*.h) $(PREFIX)/include/limbo/thirdparty/Csdp/include 
	#cp -f solver/csdp /usr/local/bin
	#cp -f theta/theta /usr/local/bin
	#cp -f theta/graphtoprob /usr/local/bin
	#cp -f theta/complement /usr/local/bin
	#cp -f theta/rand_graph /usr/local/bin

#
# Clean out all of the directories.
# 

clean:
	make -C lib clean 
	rm -f $(LIBDIR)/libsdp.a
	make -C solver clean 
	rm -f $(LIMBO_ROOT_DIR)/bin/csdp
	make -C theta clean 
	make -C test clean 
	make -C example clean

