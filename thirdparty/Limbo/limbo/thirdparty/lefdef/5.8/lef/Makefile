#  $Source: /usr1/mfg/3.4C/solaris_bld/group/util/makefiles/RCS/dir.mk,v $
#
#  $Author: wanda $
#  $Revision: #3 $
#  $Date: 2004/09/29 $
#  $State: Exp $
#

.PHONY: all
all: install release

#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../../../../..)
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = mkdir -p $(LIBDIR)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

BUILD_ORDER	= \
			lef \
			clef \
			clefzlib \
			lefzlib \
			lefrw \
			lefwrite \
			lefdiff

## HP-UX 9.0.X
OS_TYPE := $(shell uname)
ifeq ($(OS_TYPE),HP-UX)
OPTIMIZE_FLAG = +O2
else
OS_VER := $(shell uname -r)
ifeq ($(findstring 4.1,$(OS_VER)),4.1)
OPTIMIZE_FLAG = -O
else
OPTIMIZE_FLAG = -O
endif
endif

OPTIMIZE_FLAG += -Wno-reorder

install:
	@$(MAKE) $(MFLAGS) installhdrs installlib installbin

release:
	@$(LIBMKDIR)
	@$(MAKE) "DEBUG=$(OPTIMIZE_FLAG)" install
	mv lib/libclef.a $(LIMBO_ROOT_DIR)/lib
	mv lib/libclefzlib.a $(LIMBO_ROOT_DIR)/lib
	mv lib/liblef.a $(LIMBO_ROOT_DIR)/lib
	mv lib/liblefzlib.a $(LIMBO_ROOT_DIR)/lib

myinstall: 
	@$(LIBMKDIR)
	mkdir -p $(PREFIX)/include/limbo/thirdparty/lefdef/5.8/lef/include
	cp $(wildcard include/*.h) $(PREFIX)/include/limbo/thirdparty/lefdef/5.8/lef/include
	cp $(wildcard include/*.hpp) $(PREFIX)/include/limbo/thirdparty/lefdef/5.8/lef/include
	cmp -s $(PREFIX)/lib/libclef.a $(LIBDIR)/libclef.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		cp $(LIMBO_ROOT_DIR)/lib/libclef.a $(PREFIX)/lib; \
		cp $(LIMBO_ROOT_DIR)/lib/libclefzlib.a $(PREFIX)/lib; \
		cp $(LIMBO_ROOT_DIR)/lib/liblef.a $(PREFIX)/lib; \
		cp $(LIMBO_ROOT_DIR)/lib/liblefzlib.a $(PREFIX)/lib; \
	fi

test:
	@$(MAKE) "BUILD_ORDER=TEST" dotest

.PHONY: clean
clean:
	@$(MAKE) "BUILD_ORDER += TEST" doclean;
	echo $(BUILD_ORDER);
	@$(MAKE) doclean;

.DEFAULT:
	@for i in $(BUILD_ORDER) ;do \
		echo $(MAKE) $@ in $$i ; \
		cd $$i ; \
		$(MAKE) $(MFLAGS) $@ || exit ; \
		cd .. ; \
	done

.DELETE_ON_ERROR:
