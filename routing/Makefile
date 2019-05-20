#makefile 

PROJECT_ROOT_DIR = $(realpath ../)

include $(PROJECT_ROOT_DIR)/Include.mk


LIMBO_DIR = $(PROJECT_ROOT_DIR)/thirdparty/Limbo
SPARSEHASH_DIR = $(PROJECT_ROOT_DIR)/thirdparty/sparsehash_build
ROUTE_INSTALL_DIR = $(PREFIX)/routing
VERBOSE_FLAG = # empty or VERBOSE=1 

.PHONY: arouterex
all: arouterex

arouterex:
	@echo ">> make arouterex"
	mkdir -p arouterex/build; cd arouterex/build; cmake -DLIMBO_DIR=$(LIMBO_DIR) -DSPARSEHASH_DIR=$(SPARSEHASH_DIR) ..; make -j10 $(VERBOSE_FLAG); cd ../.. 


.PHONY: install
install:
	mkdir -p $(ROUTE_INSTALL_DIR); mkdir -p $(ROUTE_INSTALL_DIR)/arouterex/bin; cp arouterex/bin/ARouter $(ROUTE_INSTALL_DIR)/arouterex/bin/; mkdir -p $(ROUTE_INSTALL_DIR)/test; cp ./test/run.sh $(ROUTE_INSTALL_DIR)/test; mkdir -p $(ROUTE_INSTALL_DIR)/python; cp python/getPinLoc.py $(ROUTE_INSTALL_DIR)/python/.; cp python/addPinsToGR.py $(ROUTE_INSTALL_DIR)/python/.;


.PHONY: clean 
clean: 
	@echo ">> clean arouterex"
	rm -rf arouterex/build
	rm -rf arouterex/bin
