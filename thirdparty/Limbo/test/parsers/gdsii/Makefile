#==========================================================================
#                         Directories and names 
# ==========================================================================

PARSER_PREFIX = Gds
LIB_PREFIX = gds
DEBUG_PREFIX = $(shell echo $(PARSER_PREFIX) | tr a-z A-Z) # lower case to upper case 
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
	CXXFLAGS = $(CXXFLAGS_DEBUG) -DDEBUG_GDSREADER -DDEBUG_GDSWRITER
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(LIMBO_ROOT_DIR) 

ifdef BOOST_DIR
	LIB += $(BOOST_LINK_FLAG) -L $(BOOST_DIR)/lib -lboost_iostreams $(DYNAMIC_LINK_FLAG)
endif
ifdef ZLIB_DIR
	LIB += $(ZLIB_LINK_FLAG) -L $(ZLIB_DIR) -lz $(DYNAMIC_LINK_FLAG)
endif

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

# boost is required for gdsdb
ifdef BOOST_DIR
all: test_reader test_writer test_driver test_gdsdb
else
all: test_reader test_writer test_driver
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

test_reader: $(OBJDIR)/test_reader.o $(LIMBO_ROOT_DIR)/lib/lib$(LIB_PREFIX)parser.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_reader.o -L $(LIMBO_ROOT_DIR)/lib -l$(LIB_PREFIX)parser $(LIB)

test_writer: $(OBJDIR)/test_writer.o $(LIMBO_ROOT_DIR)/lib/lib$(LIB_PREFIX)parser.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_writer.o -L $(LIMBO_ROOT_DIR)/lib -l$(LIB_PREFIX)parser $(LIB)

test_driver: $(OBJDIR)/test_driver.o $(LIMBO_ROOT_DIR)/lib/lib$(LIB_PREFIX)parser.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_driver.o -L $(LIMBO_ROOT_DIR)/lib -l$(LIB_PREFIX)parser $(LIB)

test_gdsdb: $(OBJDIR)/test_gdsdb.o $(LIMBO_ROOT_DIR)/lib/lib$(LIB_PREFIX)parser.a $(LIMBO_ROOT_DIR)/lib/lib$(LIB_PREFIX)db.a
	$(CXX) $(CXXFLAGS) -o $@ $(OBJDIR)/test_gdsdb.o -L $(LIMBO_ROOT_DIR)/lib -l$(LIB_PREFIX)db -l$(LIB_PREFIX)parser $(LIB)

.PHONY: clean
clean: cleandep
	rm -f test_reader 
	rm -f test_writer 
	rm -f test_driver 
	rm -f test_gdsdb
	rm -f $(OBJS) 

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) 

.PHONY: extraclean
extraclean: clean
	rm -rf $(OBJDIR)
