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
	mkdir -p $(PREFIX)
	mkdir -p $(PREFIX)/test
	cp test/run.sh $(PREFIX)/test/.
	cp test/color.sh $(PREFIX)/test/.
	make install -C routing
	make install -C constraint_generation
	make install -C placement

.PHONY: clean
clean: 
	make clean -C thirdparty
	make clean -C constraint_generation
	rm -rf install
    
