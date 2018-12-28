# makefile 

PROJECT_ROOT_DIR = $(realpath .)

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

.PHONY: clean
clean: 
	make clean -C thirdparty
	make clean -C constraint_generation
