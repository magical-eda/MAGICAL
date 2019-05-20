#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../..)
OBJDIR = $(LIMBO_ROOT_DIR)/obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

VPATH = . ./api ./lpmcf

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# default DBG is off
DBG = 0

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

ifeq ($(DBG), 1)
	CXXFLAGS = $(CXXFLAGS_DEBUG) -DDEBUG_MULTIKNAPSACKLAGRELAX -DDEBUG_DUALMINCOSTFLOW
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(LIMBO_ROOT_DIR) \
		  -I $(LEMON_DIR)/include

# ==========================================================================
#                         Standard Setting
# ==========================================================================


SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

all: 
	#$(LIBDIR)/libsolvers.a

# Compile dependency 

$(OBJDIR)/%.d: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)

-include $(DEPS)

# Implicit rule to compile c++ files

$(OBJDIR)/%.o: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) 

# Link executable

#$(LIBDIR)/libsolvers.a: $(OBJS)
#	@$(LIBMKDIR)
#	$(AR) $(ARFLAGS) $(LIBDIR)/libsolvers.a $(OBJS)

.PHONY: install
install:
	mkdir -p $(PREFIX)/include/limbo/solvers/api
	cp $(wildcard api/*.h) $(PREFIX)/include/limbo/solvers/api
	mkdir -p $(PREFIX)/include/limbo/solvers/lpmcf
	cp $(wildcard lpmcf/*.h) $(PREFIX)/include/limbo/solvers/lpmcf
	cp $(wildcard *.h) $(PREFIX)/include/limbo/solvers

.PHONY: clean
clean: cleandep
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) 

.PHONY: extraclean
extraclean: clean
	rm -f $(LIBDIR)/libsolvers.a
