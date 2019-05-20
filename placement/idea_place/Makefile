#makefile 

PROJECT_ROOT_DIR = $(realpath ../)

# default IDEA_PLACE_INSTALL_DIR path
IDEA_PLACE_INSTALL_DIR = $(PROJECT_ROOT_DIR)/idea_place/install

.PHONY: idea_place
all: idea_place

idea_place:
	@echo ">> make idea_place"
	mkdir -p build; cd build; cmake .. -DWNLIB_DIR=$(WNLIB_DIR) -DLIMBO_DIR=$(LIMBO_DIR) -DLPSOLVE_DIR=$(LPSOLVE_DIR) -DLEMON_DIR=$(LEMON_DIR); make -j $(VERBOSE_FLAG); cd ..

.PHONY: install
install:
	@echo ">> make install idea_place at $(IDEA_PLACE_INSTALL_DIR)"
	mkdir -p $(IDEA_PLACE_INSTALL_DIR)/test
	mkdir -p $(IDEA_PLACE_INSTALL_DIR)/bin
	cp test/run.sh $(IDEA_PLACE_INSTALL_DIR)/test/. 
	cp bin/idea_place $(IDEA_PLACE_INSTALL_DIR)/bin/.

.PHONY: clean 
clean: 
	@echo ">> clean idea_place"
	rm -rf build
	rm -rf bin
