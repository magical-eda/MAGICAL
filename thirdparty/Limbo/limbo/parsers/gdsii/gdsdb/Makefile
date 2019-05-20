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

# boost is still needed 
ifdef BOOST_DIR
	INCLUDE += -I $(BOOST_DIR)/include
endif

# boost iostreams is needed for compression
ifdef ZLIB_DIR
ifdef BOOST_DIR
	INCLUDE += $(ZLIB_INCLUDE_FLAG)
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

all: lib$(LIB_PREFIX)db

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

lib$(LIB_PREFIX)db: $(OBJS)
	@$(LIBMKDIR)
	$(AR) $(ARFLAGS) $(LIBDIR)/lib$(LIB_PREFIX)db.a $(OBJS)

.PHONY: install
install: 
	cmp -s $(PREFIX)/lib/lib$(LIB_PREFIX)db.a $(LIBDIR)/lib$(LIB_PREFIX)db.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/lib$(LIB_PREFIX)db.* $(PREFIX)/lib; \
	fi
	mkdir -p $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)ii/gdsdb
	cp $(wildcard *.h) $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)ii/gdsdb
	rm -f $(OBJS)

.PHONY: clean
clean: cleandep
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	rm -f $(LIBDIR)/lib$(LIB_PREFIX)db.a
