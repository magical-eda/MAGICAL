#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../..)
OBJDIR = obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

VPATH = .

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

INCLUDE = -I $(realpath .) \
		  -I $(LIMBO_ROOT_DIR) \
		  -I $(BLIB_DIR)/lib \
		  -I $(BOOST_DIR)/include

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = test_p2r.cpp test_boostpolygonapi.cpp 
# blib is optional 
ifdef BLIB_DIR
SRCS += test_blibapi.cpp
endif 
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

# blib is optional 
ifdef BLIB_DIR 
all: test_p2r test_blibapi test_boostpolygonapi 
else 
all: test_p2r test_boostpolygonapi 
endif 

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

test_p2r: $(OBJDIR)/test_p2r.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_p2r.o 

test_blibapi: $(OBJDIR)/test_blibapi.o
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_blibapi.o 

test_boostpolygonapi: $(OBJDIR)/test_boostpolygonapi.o $(LIMBO_ROOT_DIR)/lib/libGeoBoostPolygonApi.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_boostpolygonapi.o -L $(LIMBO_ROOT_DIR)/lib -lGeoBoostPolygonApi

.PHONY: clean
clean: cleandep
	rm -f test_p2r 
	rm -f test_blibapi 
	rm -f test_boostpolygonapi 
	rm -f $(OBJS) 

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) 

.PHONY: extraclean
extraclean: clean
	rm -rf $(OBJDIR)
