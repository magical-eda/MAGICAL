# $Id$
# plain simple Makefile to build test

#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../../..)
# the real test directory is not used due to original setting of the package 
TEST_DIR = $(realpath ./example)
OBJDIR = $(LIMBO_ROOT_DIR)/obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

VPATH = ./include ./src $(TEST_DIR)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# default DBG is off
DBG = 0

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

ifeq ($(DBG), 1)
	CXXFLAGS = $(CXXFLAGS_DEBUG)
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(LIMBO_ROOT_DIR) \
		  -I $(BOOST_DIR)/include

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard src/*.cpp) 
OBJS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

SRCS_TEST = $(wildcard $(TEST_DIR)/dlx/*.cpp)
OBJS_TEST = $(SRCS_TEST:%.cpp=%.o)
DEPS_TEST = $(OBJS_TEST:%.o=%.d)

all: libdlx $(TEST_DIR)/dlx/dlx

# Compile dependency 

$(OBJDIR)/%.d: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)
$(TEST_DIR)/dlx/%.d: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)

-include $(DEPS)
-include $(DEPS_TEST)

# Implicit rule to compile c++ files

$(OBJDIR)/%.o: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)
$(TEST_DIR)/%.o: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE)

# Link executable

$(TEST_DIR)/dlx/dlx: libdlx $(OBJS_TEST)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS_TEST) $(INCLUDE) $(LIBDIR)/libdlx.a

libdlx: $(OBJS)
	@$(LIBMKDIR)
	$(AR) $(ARFLAGS) $(LIBDIR)/libdlx.a $(OBJS)

.PHONY: install 
install: 
	cmp -s $(PREFIX)/lib/libdlx.a $(LIBDIR)/libdlx.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/libdlx.* $(PREFIX)/lib; \
	fi
	mkdir -p $(PREFIX)/include/limbo/thirdparty/dlx/include/dlx 
	cp $(wildcard include/dlx/*.hpp) $(PREFIX)/include/limbo/thirdparty/dlx/include/dlx 
	rm -f $(OBJS) $(OBJS_TEST) 

.PHONY: clean
clean: cleandep
	rm -f $(TEST_DIR)/dlx/dlx $(OBJS) $(OBJS_TEST)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) $(DEPS_TEST)

.PHONY: extraclean
extraclean: clean
	rm -f $(LIBDIR)/libdlx.a

# ==========================================================================
# following packages are not available 
# they are still in C++11 
# ==========================================================================
# sudoku
# langford 
# nqueens 
# npieces 
# polyomino
