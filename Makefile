# makefile 

PROJECT_ROOT_DIR = $(realpath .)

.PHONY: thirdparty placement routing
all: thirdparty placement routing 

thirdparty:
	make -C $(PROJECT_ROOT_DIR)/thirdparty

placement:
	make -C $(PROJECT_ROOT_DIR)/placement

routing: 
	make -C $(PROJECT_ROOT_DIR)/routing

.PHONY: clean
clean: 
	make clean -C $(PROJECT_ROOT_DIR)/thirdparty
