# makefile 

PROJECT_ROOT_DIR = $(realpath .)

.PHONY: thirdparty placement routing
all: thirdparty placement routing 

thirdparty:
	make -C thirdparty

placement:
	make -C placement

routing: 
	make -C routing

.PHONY: clean
clean: 
	make clean -C thirdparty
