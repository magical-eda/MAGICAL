#makefile 

PROJECT_ROOT_DIR = $(realpath ../)

# default WELL_GENERATION_INSTALL_DIR path
WELL_GENERATION_INSTALL_DIR = $(PROJECT_ROOT_DIR)/well_generation/install

.PHONY: well_gen legalize
#all: well_gen legalize
all:
	@echo ">> make well_generation directory"
	mkdir -p build; cd build; cmake .. -DWNLIB_DIR=$(WNLIB_DIR) -DLIMBO_DIR=$(LIMBO_DIR) -DLPSOLVE_DIR=$(LPSOLVE_DIR) -DLEMON_DIR=$(LEMON_DIR); make -j $(VERBOSE_FLAG); cd ..

well_gen:
	@echo ">> make well_gen"
	mkdir -p build; cd build; cmake .. -DWNLIB_DIR=$(WNLIB_DIR) -DLIMBO_DIR=$(LIMBO_DIR) -DLPSOLVE_DIR=$(LPSOLVE_DIR) -DLEMON_DIR=$(LEMON_DIR); make -j $(VERBOSE_FLAG) well_gen; cd ..

legalize:
	@echo ">> make legalize"
	mkdir -p build; cd build; cmake .. -DWNLIB_DIR=$(WNLIB_DIR) -DLIMBO_DIR=$(LIMBO_DIR) -DLPSOLVE_DIR=$(LPSOLVE_DIR) -DLEMON_DIR=$(LEMON_DIR); make -j $(VERBOSE_FLAG) legalize; cd ..


.PHONY: install
install:
	@echo ">> make install well_generation at $(WELL_GENERATION_INSTALL_DIR)"
	mkdir -p $(WELL_GENERATION_INSTALL_DIR)/test
	cp test/run.sh $(WELL_GENERATION_INSTALL_DIR)/test/. 
	mkdir -p $(WELL_GENERATION_INSTALL_DIR)/bin
	cp bin/well_gen $(WELL_GENERATION_INSTALL_DIR)/bin/.
	cp bin/legalize $(WELL_GENERATION_INSTALL_DIR)/bin/.
	mkdir -p $(WELL_GENERATION_INSTALL_DIR)/python
	cp python/combine_results.py $(WELL_GENERATION_INSTALL_DIR)/python/.
	cp python/legalize.py $(WELL_GENERATION_INSTALL_DIR)/python/.
	cp python/parser_v2.py $(WELL_GENERATION_INSTALL_DIR)/python/.

.PHONY: clean 
clean: 
	@echo ">> clean well_generation"
	rm -rf build
	rm -rf bin
