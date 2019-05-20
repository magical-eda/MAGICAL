#==========================================================================
#                         Directories and names 
# ==========================================================================

PARSER_PREFIX = Lef
LIB_PREFIX = lef
DEBUG_PREFIX = $(shell echo $(PARSER_PREFIX) | tr a-z A-Z)# lower case to upper case 
LIMBO_ROOT_DIR = $(realpath ../../../..)
OBJDIR = $(LIMBO_ROOT_DIR)/obj/parsers/lef/adapt
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
	CXXFLAGS = $(CXXFLAGS_DEBUG) -DDEBUG_$(DEBUG_PREFIX)PARSER
else
	CXXFLAGS = $(CXXFLAGS_RELEASE)
endif

#==========================================================================
#                         Special Library
# ==========================================================================

INCLUDE = -I $(realpath .) \
		  -I $(LIMBO_ROOT_DIR) \
		  -I $(LEX_INCLUDE_DIR)

# ==========================================================================
#                         Standard Setting
# ==========================================================================

# parser.cc and scanner.cc are generated 
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:%.cc=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

all: lib$(LIB_PREFIX)parseradapt

# Compile dependency 

$(OBJDIR)/%.d: %.cc
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)

-include $(DEPS)

# Implicit rule to compile c++ files

$(OBJDIR)/%.o: %.cc
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) 

# Link executable

lib$(LIB_PREFIX)parseradapt: $(OBJS) $(LIBDIR)/liblef.a
	@$(LIBMKDIR)
	mkdir merge; \
	cd merge; \
	$(AR) -x $(LIBDIR)/lib$(LIB_PREFIX).a; \
	$(AR) $(ARFLAGS) $(LIBDIR)/lib$(LIB_PREFIX)parseradapt.a *.o $(OBJS); \
	cd ..; \
	rm -rf merge 

.PHONY: install
install: 
	cmp -s $(PREFIX)/lib/lib$(LIB_PREFIX)parseradapt.a $(LIBDIR)/lib$(LIB_PREFIX)parseradapt.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/lib$(LIB_PREFIX)parseradapt.* $(PREFIX)/lib; \
	fi
	mkdir -p $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)/adapt
	cp $(PARSER_PREFIX)DataBase.h $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)/adapt
	cp $(PARSER_PREFIX)Driver.h $(PREFIX)/include/limbo/parsers/$(LIB_PREFIX)/adapt
	rm -f $(OBJS)

.PHONY: clean
clean: cleandep
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS)

.PHONY: extraclean
extraclean: clean
	rm -f $(LIBDIR)/lib$(LIB_PREFIX)parseradapt.a
