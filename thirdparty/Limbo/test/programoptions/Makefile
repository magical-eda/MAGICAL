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
		  -I $(LIMBO_ROOT_DIR) 

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard *.cpp) 
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

all: test_ProgramOptions test_ProgramOptions_simple

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

test_ProgramOptions: $(OBJDIR)/test_ProgramOptions.o $(LIMBO_ROOT_DIR)/lib/libprogramoptions.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_ProgramOptions.o -L $(LIMBO_ROOT_DIR)/lib -lprogramoptions

test_ProgramOptions_simple: $(OBJDIR)/test_ProgramOptions_simple.o $(LIMBO_ROOT_DIR)/lib/libprogramoptions.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_ProgramOptions_simple.o -L $(LIMBO_ROOT_DIR)/lib -lprogramoptions

install: libprogramoptions
	rm -f $(OBJS)

.PHONY: clean
clean: cleandep
	rm -f test_ProgramOptions
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	rm -rf $(OBJDIR)
