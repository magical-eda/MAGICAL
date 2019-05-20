#!gmake

TARGET_ARCH =
MAKE = make

# compile everything
compile:
	cd unx2doss; $(MAKE) compile; cd ..
	cd wnsum; $(MAKE) compile; cd ..
	cd gendoc; $(MAKE) compile; cd ..
	cd mean; $(MAKE) compile; cd ..
#	cd wncrypt; $(MAKE) compile; cd ..
	cd hex/fromhex; $(MAKE) compile; cd ..
	cd hex/tohex; $(MAKE) compile; cd ..
	cd wnlint; $(MAKE) compile; cd ..
	cd shatter; $(MAKE) compile; cd ..

# remove objects 
clean:
	cd unx2doss; $(MAKE) clean; cd ..
	cd wnsum; $(MAKE) clean; cd ..
	cd gendoc; $(MAKE) clean; cd ..
	cd mean; $(MAKE) clean; cd ..
#	cd wncrypt; $(MAKE) clean; cd ..
	cd hex/fromhex; $(MAKE) clean; cd ..
	cd hex/tohex; $(MAKE) clean; cd ..
	cd wnlint; $(MAKE) clean; cd ..
	cd shatter; $(MAKE) clean; cd ..

man:
	./manscript.csh

# make everything
all:
	cd unx2doss; $(MAKE) all; cd ..
	cd wnsum; $(MAKE) all; cd ..
	cd gendoc; $(MAKE) all; cd ..
	cd mean; $(MAKE) all; cd ..
#	cd wncrypt; $(MAKE) all; cd ..
	cd hex/fromhex; $(MAKE) all; cd ..
	cd hex/tohex; $(MAKE) all; cd ..
	cd wnlint; $(MAKE) all; cd ..
	cd shatter; $(MAKE) all; cd ..
