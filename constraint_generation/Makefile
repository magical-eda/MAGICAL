#makefile 

PROJECT_ROOT_DIR = $(realpath ../)

.PHONY: signal_flow_analysis
all: signal_flow_analysis

signal_flow_analysis:
	@echo ">> make signal_flow_analysis"
	mkdir -p signal_flow_analysis/build; cd signal_flow_analysis/build; cmake ..; make -j; cd ../.. 

.PHONY: clean 
clean: 
	@echo ">> clean signal_flow_analysis"
	rm -rf signal_flow_analysis/build
