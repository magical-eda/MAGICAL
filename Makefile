# makefile 

PROJECT_ROOT_DIR = $(realpath .)

#==========================================================================
#                         Compilation Flags
# ==========================================================================

# include environmental configurations 
include $(PROJECT_ROOT_DIR)/Include.mk

.PHONY: thirdparty placement routing constraint_generation
all: thirdparty placement routing constraint_generation

thirdparty:
	make -C thirdparty

placement:
	make -C placement

routing: 
	make -C routing

constraint_generation:
	make -C constraint_generation

.PHONY: install
install:
	@echo $(PREFIX)
	mkdir -p $(PREFIX)
	cp test/run.sh $(PREFIX)
	cp test/color.sh $(PREFIX)
	make install -C routing
	make install -C constraint_generation

.PHONY: clean
clean: 
	make clean -C thirdparty
	make clean -C constraint_generation
