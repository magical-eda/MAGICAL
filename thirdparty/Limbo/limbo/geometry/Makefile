#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ../..)
OBJDIR = $(LIMBO_ROOT_DIR)/obj
MKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi
LIBDIR = $(LIMBO_ROOT_DIR)/lib
LIBMKDIR = if [ ! -d $(@D) ]; then mkdir -p $(@D); fi

VPATH = . ./api

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

SRCS1 = $(wildcard *.cpp)
SRCS2 = $(wildcard api/*.cpp)
SRCS = $(SRCS1) $(SRCS2)
OBJS = $(SRCS1:%.cpp=$(OBJDIR)/%.o) $(SRCS2:api/%.cpp=$(OBJDIR)/%.o)
DEPS = $(OBJS:%.o=%.d) 	# one dependency file for each source

all: $(LIBDIR)/libGeoBoostPolygonApi.a

# Compile dependency 

$(OBJDIR)/%.d: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) $< -MM -MT $(@:%.d=%.o) >$@ $(INCLUDE)

-include $(DEPS)
-include $(DEPS_TEST)

# Implicit rule to compile c++ files

$(OBJDIR)/%.o: %.cpp
	@$(MKDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCLUDE) 

# Link executable

$(LIBDIR)/libGeoBoostPolygonApi.a: $(OBJDIR)/GeoBoostPolygonApi.o
	@$(LIBMKDIR)
	$(AR) $(ARFLAGS) $(LIBDIR)/libGeoBoostPolygonApi.a $(OBJDIR)/GeoBoostPolygonApi.o

.PHONY: install
install:
	cmp -s $(PREFIX)/lib/libGeoBoostPolygonApi.a $(LIBDIR)/libGeoBoostPolygonApi.a; \
	RETVAL=$$?; \
	if [ $$RETVAL -ne 0 ]; then \
		mkdir -p $(PREFIX)/lib; \
		cp $(LIBDIR)/libGeoBoostPolygonApi.* $(PREFIX)/lib; \
	fi
	mkdir -p $(PREFIX)/include/limbo/geometry
	cp $(wildcard *.h) $(PREFIX)/include/limbo/geometry
	mkdir -p $(PREFIX)/include/limbo/geometry/api
	cp $(wildcard api/*.h) $(PREFIX)/include/limbo/geometry/api

.PHONY: clean
clean: cleandep
	rm -f $(OBJS)

.PHONY: cleandep
cleandep:
	rm -f $(DEPS) 

.PHONY: extraclean
extraclean: clean
	rm -f $(LIBDIR)/libGeoBoostPolygonApi.a
