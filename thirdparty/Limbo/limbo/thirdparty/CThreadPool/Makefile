# $Id$
# plain simple Makefile to build test

#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../../..)
TEST_DIR = $(realpath .)
OBJDIR = $(LIMBO_ROOT_DIR)/obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

VPATH = . $(TEST_DIR)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# default DBG is off
DBG = 0

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

ifeq ($(DBG), 1)
	CFLAGS = $(CFLAGS_DEBUG) -DTHPOOL_DEBUG 
else
	CFLAGS = $(CFLAGS_RELEASE)
endif

CFLAGS += -D_POSIX_C_SOURCE=199506 -D_XOPEN_SOURCE=500 -Wno-unused-parameter

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(LIMBO_ROOT_DIR) -pthread 
LIBS = -lpthread

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = thpool.c
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

SRCS_TEST = example.c
OBJS_TEST = example.o
DEPS_TEST = $(OBJS_TEST:%.o=%.d)

all: 
	@$(MKDIR)
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/thpool.o thpool.c $(INCLUDE)
	$(CC) $(CFLAGS) -c -o $(TEST_DIR)/example.o $(TEST_DIR)/example.c $(INCLUDE)
	$(AR) $(ARFLAGS) $(LIBDIR)/libthpool.a $(OBJDIR)/thpool.o
	$(CC) $(CFLAGS) -o example $(TEST_DIR)/example.o $(LIBDIR)/libthpool.a $(LIBS) 

.PHONY: install
install:
	cmp -s $(PREFIX)/lib/libthpool.a $(LIBDIR)/libthpool.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/libthpool.* $(PREFIX)/lib; \
	fi
	mkdir -p $(PREFIX)/include/limbo/thirdparty/CThreadPool
	cp $(wildcard *.h) $(PREFIX)/include/limbo/thirdparty/CThreadPool

.PHONY: clean
clean: cleandep
	rm -f $(TEST_DIR)/example $(OBJS) $(OBJS_TEST)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) $(DEPS_TEST)

