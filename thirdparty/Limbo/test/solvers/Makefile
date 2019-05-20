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
# default GUROBI is off 
GUROBI = 0
# default LPSolve is off
LPSOLVE = 0

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

ifeq ($(DBG), 1)
	CXXFLAGS = $(CXXFLAGS_DEBUG) -DDEBUG_GUROBIAPI=1 -DDEBUG_LPSOLVEAPI=1 -DDEBUG_DUALMINCOSTFLOW -DDEBUG_MINCOSTFLOW
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

# dependency to GUROBI and get GUROBI_LINK_LIB
ifdef GUROBI_HOME
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindGurobi.mk
endif

# dependency to LPSOLVE and get LPSOLVE_LINK_LIB
ifdef LPSOLVE_DIR
include $(LIMBO_ROOT_DIR)/limbo/makeutils/FindLPSolve.mk
endif

#==========================================================================
#                         Special Library
# ==========================================================================

# internal dependency is lp parser 
# external dependency is boost
INCLUDE = -I $(LIMBO_ROOT_DIR) \
		  -I $(LEMON_DIR)/include 
LIBS = -L $(LIMBO_ROOT_DIR)/lib -llpparser \
	   -L $(LEMON_DIR)/lib -lemon 

# GUROBI 
ifeq ($(GUROBI), 1)
INCLUDE += -I $(GUROBI_HOME)/include
LIBS += $(DYNAMIC_LINK_FLAG) $(GUROBI_LINK_LIB)
endif 

# LPSOLVE 
ifeq ($(LPSOLVE), 1)
INCLUDE += -I $(LPSOLVE_DIR)
LIBS += $(DYNAMIC_LINK_FLAG) $(LPSOLVE_LINK_LIB)
endif 

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

ifeq ($(GUROBI), 0)
all: lpmcf test_solvers test_MultiKnapsackLagRelax test_DualMinCostFlow test_MinCostFlow test_LPSolveApi 
else
all: lpmcf test_solvers test_MultiKnapsackLagRelax test_DualMinCostFlow test_MinCostFlow test_GurobiApi test_LPSolveApi 
endif

.PHONY: lpmcf

lpmcf:
	$(MAKE) -C lpmcf

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

test_solvers: $(OBJDIR)/test_solvers.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_solvers.o $(LIBS) 

test_MultiKnapsackLagRelax: $(OBJDIR)/test_MultiKnapsackLagRelax.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_MultiKnapsackLagRelax.o $(LIBS) 

test_DualMinCostFlow: $(OBJDIR)/test_DualMinCostFlow.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_DualMinCostFlow.o $(LIBS) 

test_MinCostFlow: $(OBJDIR)/test_MinCostFlow.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_MinCostFlow.o $(LIBS) 

test_GurobiApi: $(OBJDIR)/test_GurobiApi.o
ifeq ($(GUROBI), 1)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_GurobiApi.o $(LIBS) 
endif 

test_LPSolveApi: $(OBJDIR)/test_LPSolveApi.o
ifeq ($(LPSOLVE), 1)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_LPSolveApi.o $(LIBS) 
endif 

.PHONY: clean
clean: cleandep
	$(MAKE) clean -C lpmcf
	rm -f test_solvers
	rm -f test_MultiKnapsackLagRelax
	rm -f test_DualMinCostFlow
	rm -f test_MinCostFlow
	rm -f test_GurobiApi
	rm -f test_LPSolveApi
	rm -f $(OBJS) 

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	$(MAKE) extraclean -C lpmcf
	rm -rf $(OBJDIR)
