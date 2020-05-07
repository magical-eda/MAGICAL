#makefile 

PROJECT_ROOT_DIR = $(realpath ../)
include $(PROJECT_ROOT_DIR)/Include.mk
VERBOSE_FLAG = # empty or VERBOSE=1
CONSTRAINT_DIR = $(PREFIX)/constraint_generation

.PHONY: signal_flow_analysis
all: signal_flow_analysis

signal_flow_analysis:
	@echo ">> make signal_flow_analysis"
	mkdir -p signal_flow_analysis/build; cd signal_flow_analysis/build; cmake ..; make -j $(VERBOSE_FLAG); cd ../.. 

.PHONY: clean 
clean: 
	@echo ">> clean signal_flow_analysis"
	rm -rf signal_flow_analysis/build

.PHONY: install
install:
	mkdir -p $(CONSTRAINT_DIR)/signal_flow_analysis/bin; 
	mkdir -p $(CONSTRAINT_DIR)/python; 
	cp -rf python $(CONSTRAINT_DIR); 
	cp -rf test $(CONSTRAINT_DIR); 
	cp -rf signal_flow_analysis/bin $(CONSTRAINT_DIR)/signal_flow_analysis; 
    

