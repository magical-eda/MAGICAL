# makefile 
 
PROJECT_ROOT_DIR = $(realpath ../)

.PHONY: lemon limbo wnlib sparsehash
all: lemon limbo wnlib sparsehash

lemon:
	@echo ">> make lemon"
	mkdir -p $(PROJECT_ROOT_DIR)/thirdparty/lemon
	mkdir -p lemon-1.3.1/build; cd lemon-1.3.1/build; cmake -DCMAKE_INSTALL_PREFIX=$(PROJECT_ROOT_DIR)/thirdparty/lemon ..; make; make install; cd ../..

limbo: 
	@echo ">> make limbo"
	cd Limbo; make CXXSTD="-std=c++11"; make install; cd ..

wnlib: 
	@echo ">> make wnlib"
	cd wnlib; make all; cd ..

sparsehash:
	@echo ">> make sparsehash"
	cd sparsehash; ./configure --prefix=$(PROJECT_ROOT_DIR)/thirdparty/sparsehash_build; make; make install; git checkout .; cd ..

.PHONY: clean 
clean: 
	@echo ">> clean lemon"
	rm -rf $(PROJECT_ROOT_DIR)/thirdparty/lemon
	rm -rf $(PROJECT_ROOT_DIR)/thirdparty/lemon-1.3.1/build
	@echo ">> clean limbo"
	cd Limbo; make clean; cd ..
	@echo ">> clean wnlib"
	cd wnlib; make clean; cd ..
	@echo ">> clean sparsehash"
	cd sparsehash; make clean; cd ..
	rm -rf $(PROJECT_ROOT_DIR)/thirdparty/sparsehash_build
