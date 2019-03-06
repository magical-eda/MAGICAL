#makefile 

PROJECT_ROOT_DIR = $(realpath ../)

include $(PROJECT_ROOT_DIR)/Include.mk

PLACEMENT_INSTALL_DIR = $(PREFIX)/placement

WNLIB_DIR = $(PROJECT_ROOT_DIR)/thirdparty/wnlib
LIMBO_DIR = $(PROJECT_ROOT_DIR)/thirdparty/Limbo
LPSOLVE_DIR = $(PROJECT_ROOT_DIR)/thirdparty/lpsolve
LEMON_DIR = $(PROJECT_ROOT_DIR)/thirdparty/lemon
VERBOSE_FLAG = # empty or VERBOSE=1 

.PHONY: idea_place well_generation
all: idea_place well_generation

idea_place:
	make -C idea_place WNLIB_DIR=$(WNLIB_DIR) LIMBO_DIR=$(LIMBO_DIR) LPSOLVE_DIR=$(LPSOLVE_DIR) LEMON_DIR=$(LEMON_DIR) VERBOSE_FLAG=$(VERBOSE_FLAG)

well_generation:
	make -C well_generation WNLIB_DIR=$(WNLIB_DIR) LIMBO_DIR=$(LIMBO_DIR) LPSOLVE_DIR=$(LPSOLVE_DIR) LEMON_DIR=$(LEMON_DIR) VERBOSE_FLAG=$(VERBOSE_FLAG)

.PHONY: install 
install:
	mkdir -p $(PLACEMENT_INSTALL_DIR)
	mkdir -p $(PLACEMENT_INSTALL_DIR)/idea_place
	make install -C idea_place IDEA_PLACE_INSTALL_DIR=$(PLACEMENT_INSTALL_DIR)/idea_place
	mkdir -p $(PLACEMENT_INSTALL_DIR)/well_generation
	make install -C well_generation WELL_GENERATION_INSTALL_DIR=$(PLACEMENT_INSTALL_DIR)/well_generation

.PHONY: clean 
clean: 
	make clean -C idea_place
	make clean -C well_generation
