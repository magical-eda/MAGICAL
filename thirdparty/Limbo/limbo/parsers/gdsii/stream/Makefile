#==========================================================================
#                         Directories and names 
# ==========================================================================

PARSER_PREFIX = Gds
LIB_PREFIX = gds
DEBUG_PREFIX = $(shell echo $(PARSER_PREFIX) | tr a-z A-Z) # lower case to upper case 
LIMBO_ROOT_DIR = $(realpath ../../../..)
OBJDIR = $(LIMBO_ROOT_DIR)/obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

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

# if zlib and boost are available, support compression/decompression features 
ifdef ZLIB_DIR
ifdef BOOST_DIR
	CXXFLAGS += -DZLIB=1
endif
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(LIMBO_ROOT_DIR) 

ifdef ZLIB_DIR
ifdef BOOST_DIR
	INCLUDE += -I $(BOOST_DIR)/include $(ZLIB_INCLUDE_FLAG)
	LIB += -L $(BOOST_DIR)/lib -lboost_iostreams \
		   $(ZLIB_LINK_FLAG) -lz
endif
endif

# ==========================================================================
#                         Standard Setting
# ==========================================================================

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

all: lib$(LIB_PREFIX)parser

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

lib$(LIB_PREFIX)parser: $(OBJS)
	@$(LIBMKDIR)
	$(AR) $(ARFLAGS) $(LIBDIR)/lib$(LIB_PREFIX)parser.a $(OBJS)

.PHONY: install
install: 
	cmp -s $(PREFIX)/lib/lib$(LIB_PREFIX)parser.a $(LIBDIR)/lib$(LIB_PREFIX)parser.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/lib$(LIB_PREFIX)parser.* $(PREFIX)/lib; \
	fi
	mkdir -p $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)ii/stream
	cp $(wildcard *.h) $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)ii/stream
	rm -f $(OBJS)

.PHONY: clean
clean: cleandep
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	rm -f $(LIBDIR)/lib$(LIB_PREFIX)parser.a
