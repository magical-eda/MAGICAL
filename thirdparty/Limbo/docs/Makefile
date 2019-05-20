#==========================================================================
#                         Directories and names 
# ==========================================================================

LIMBO_ROOT_DIR = $(realpath ..)
DOCDIR = $(LIMBO_ROOT_DIR)/docs
SRCS = $(DOCDIR)/Readme.md \
		  $(wildcard $(DOCDIR)/algorithms/*) \
		  $(wildcard $(DOCDIR)/bibtex/*) \
		  $(wildcard $(DOCDIR)/containers/*) \
		  $(wildcard $(DOCDIR)/geometry/*) \
		  $(wildcard $(DOCDIR)/makeutils/*) \
		  $(wildcard $(DOCDIR)/math/*) \
		  $(wildcard $(DOCDIR)/parsers/*) \
		  $(wildcard $(DOCDIR)/preprocessor/*) \
		  $(wildcard $(DOCDIR)/programoptions/*) \
		  $(wildcard $(DOCDIR)/solvers/*) \
		  $(wildcard $(DOCDIR)/string/*) \
		  $(wildcard $(DOCDIR)/thirdparty/*) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/algorithms/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/algorithms/coloring/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/algorithms/partition/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/algorithms/placement/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/algorithms/*.cpp) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/bibtex/*.py) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/containers/*.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/makeutils/*.mk) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/math/*.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/bookshelf/test_bison.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/bookshelf/bison/BookshelfDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/bookshelf/bison/BookshelfDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/def/test_adapt.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/def/adapt/DefDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/def/adapt/DefDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/ebeam/test_bison.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/ebeam/bison/EbeamDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/ebeam/bison/EbeamDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/gdf/test_bison.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/gdf/bison/GdfDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/gdf/bison/GdfDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/gdsii/*.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/gdsii/stream/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/gdsii/gdsdb/*.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/lef/test_adapt.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/lef/adapt/LefDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/lef/adapt/LefDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/lp/test_bison.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/lp/bison/LpDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/lp/bison/LpDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/tf/test_spirit.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/tf/spirit/TfParser.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/tf/spirit/ErrorHandler.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/parsers/verilog/test_bison.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/verilog/bison/VerilogDataBase.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/parsers/verilog/bison/VerilogDriver.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/geometry/*.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/geometry/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/geometry/api/*.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/preprocessor/*.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/programoptions/*.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/programoptions/*.h) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/solvers/api/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/solvers/lpmcf/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/solvers/*.h) \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/solvers/lpmcf/*.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/solvers/*.cpp) \
		  \
		  $(wildcard $(LIMBO_ROOT_DIR)/test/string/*.cpp) \
		  $(wildcard $(LIMBO_ROOT_DIR)/limbo/string/*.h) 


#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(LIMBO_ROOT_DIR)/Include.mk

# ==========================================================================
#                         Standard Setting
# ==========================================================================

all: doc

Doxyfile: $(DOCDIR)/Doxyfile.in $(DOCDIR)/Makefile $(SRCS)
	echo INCLUDE_PATH           = $(LIMBO_ROOT_DIR) > $(DOCDIR)/Doxyfile
	echo EXAMPLE_PATH           = $(LIMBO_ROOT_DIR) >> $(DOCDIR)/Doxyfile
	echo IMAGE_PATH             = $(LIMBO_ROOT_DIR)/docs/geometry >> $(DOCDIR)/Doxyfile
	echo INPUT                  = $(SRCS) >> $(DOCDIR)/Doxyfile
	echo OUTPUT_DIRECTORY       = $(DOCDIR) >> $(DOCDIR)/Doxyfile
	echo FILE_PATTERNS          = *.c *.cc *.cpp *.md *.py *.mk >> $(DOCDIR)/Doxyfile
	echo INCLUDE_FILE_PATTERNS  = *.h *.hpp >> $(DOCDIR)/Doxyfile
	echo ALIASES               += nowarn=\"\\{\" endnowarn=\"\\}\" >> $(DOCDIR)/Doxyfile
	echo EXTENSION_MAPPING      = mk=C++ >> $(DOCDIR)/Doxyfile 
	echo FILTER_PATTERNS        = *.mk=$(LIMBO_ROOT_DIR)/docs/MKFilter.sh >> $(DOCDIR)/Doxyfile
	echo CITE_BIB_FILES         = $(LIMBO_ROOT_DIR)/docs/ref/Top.bib $(wildcard $(LIMBO_ROOT_DIR)/docs/ref/*.bib) >> $(DOCDIR)/Doxyfile 

doc: Doxyfile $(SRCS)
	$(DOXYGEN) $(DOCDIR)/Doxyfile.in

.PHONY: install
install: 
ifneq ($(realpath $(LIMBO_ROOT_DIR)),$(realpath $(PREFIX)))
	mkdir -p $(PREFIX)/docs
	if [ -a $(DOCDIR)/html/index.html ]; then \
		cp -r $(DOCDIR)/html $(PREFIX)/docs; \
	fi 
endif

.PHONY: clean
clean: 
	rm -rf $(DOCDIR)/html
	rm -f $(DOCDIR)/Doxyfile

.PHONY: extraclean
extraclean: clean
