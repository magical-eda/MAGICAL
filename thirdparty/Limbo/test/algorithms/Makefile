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
# default GUROBI is on 
GUROBI = 1

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

ifeq ($(DBG), 1)
	CXXFLAGS = $(CXXFLAGS_DEBUG) -DDEBUG_LPCOLORING -DASSERT_LPCOLORING -DDEBUG_GRAPHSIMPLIFICATION -DDEBUG_ILPCOLORING -DDEBUG_SDPCOLORING -DDEBUG_MISCOLORING -DDEBUG_FM
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

# moved to SDPColoringCsdp.h
#CXXFLAGS += -DNOSHORTS
CXXFLAGS += -DGUROBI=$(GUROBI)

#==========================================================================
#                         Special Library
# ==========================================================================

# internal dependency is lp parser 
# external dependency is boost
INCLUDE = -I $(LIMBO_ROOT_DIR) \
		  -I $(BOOST_DIR)/include 
LIBS = $(BOOST_LINK_FLAG) -L $(BOOST_DIR)/lib -lboost_graph -lboost_regex -lboost_unit_test_framework $(DYNAMIC_LINK_FLAG)

# ==== boost chrono links to to librt.a under Linux
ifeq ($(UNAME_S), Linux)
LIBS += -lrt 
endif 

# ==== GUROBI 
INCLUDE += -I $(GUROBI_HOME)/include
LIBS += $(GUROBI_CXX_LINK_LIB) \
		-lpthread 
# ==== Csdp api (OpenBLAS and Fortran library are necessary)
# it looks like pthead must be linked dynamically when GUROBI is also on 
ifeq ($(OPENBLAS), 1)
LIBS += -L $(LIMBO_ROOT_DIR)/lib -lsdp -lopenblas-st -lm 
# ==== Csdp links to Fortran under Linux 
ifeq ($(UNAME_S), Linux)
LIBS += -lgfortran 
endif
endif

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

TARGETS = test_ChromaticNumber test_GraphSimplification test_FM
ifdef GUROBI_HOME
	TARGETS += test_LPColoring test_ILPColoring test_MISColoring
endif 
ifeq ($(OPENBLAS), 1)
	TARGETS += test_SDPColoring
endif 

all: $(TARGETS)
	@echo ">> building with GUROBI_HOME = $(GUROBI_HOME), OPENBLAS = $(OPENBLAS)"

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

test_ChromaticNumber: $(OBJDIR)/test_ChromaticNumber.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_ChromaticNumber.o $(LIBS) 

test_LPColoring: $(OBJDIR)/test_LPColoring.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_LPColoring.o $(CXXFLAGS) $(LIBS)

test_GraphSimplification: $(OBJDIR)/test_GraphSimplification.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_GraphSimplification.o $(CXXFLAGS) $(LIBS)

test_ILPColoring: $(OBJDIR)/test_ILPColoring.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_ILPColoring.o $(CXXFLAGS) $(LIBS)

test_SDPColoring: $(OBJDIR)/test_SDPColoring.o $(LIMBO_ROOT_DIR)/lib/libsdp.a $(LIMBO_ROOT_DIR)/lib/libopenblas-st.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_SDPColoring.o $(CXXFLAGS) $(LIBS)

test_MISColoring: $(OBJDIR)/test_MISColoring.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_MISColoring.o $(CXXFLAGS) $(LIBS)

test_FM: $(OBJDIR)/test_FM.o 
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_FM.o $(LIBS) 

.PHONY: clean
clean: cleandep
	rm -f test_ChromaticNumber 
	rm -f test_LPColoring 
	rm -f test_GraphSimplification 
	rm -f test_ILPColoring 
	rm -f test_MISColoring 
	rm -f test_SDPColoring 
	rm -f test_FM
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	rm -rf $(OBJDIR)
