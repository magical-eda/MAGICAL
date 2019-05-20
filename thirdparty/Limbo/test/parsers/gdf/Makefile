#==========================================================================
#                         Directories and names 
# ==========================================================================

PARSER_PREFIX = Gdf
LIB_PREFIX = gdf
DEBUG_PREFIX = $(shell echo $(PARSER_PREFIX) | tr a-z A-Z)# lower case to upper case 
LIMBO_ROOT_DIR = $(realpath ../../..)
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
	CXXFLAGS = $(CXXFLAGS_DEBUG) -DDEBUG_$(DEBUG_PREFIX)PARSER
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(realpath .) \
		  -I $(LIMBO_ROOT_DIR) \
		  -I $(LEX_INCLUDE_DIR) \
		  -I $(BOOST_DIR)/include

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

all: test_bison

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

test_bison: $(OBJDIR)/test_bison.o $(LIMBO_ROOT_DIR)/lib/lib$(LIB_PREFIX)parser.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_bison.o -L $(LIMBO_ROOT_DIR)/lib -l$(LIB_PREFIX)parser

.PHONY: clean
clean: cleandep
	rm -f test_bison 
	rm -f test_spirit
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	rm -rf $(OBJDIR)
