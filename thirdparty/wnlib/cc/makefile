#!gmake

TARGET_ARCH =
CC = cc
#ADDCFLAGS = -cckr
ADDCFLAGS = 

# compile everything
compile:
	cd low; make compile CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..
	cd matrix; make compile CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..
	cd spmat; make compile CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..
	cd parse; make compile CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..

# h file links
hfiles:
	set nonomatch; rm -f h/*
	cd low; make hfiles; cd ..
	cd matrix; make hfiles; cd ..
	cd spmat; make hfiles; cd ..
	cd parse; make hfiles; cd ..

# remove objects 
clean:
	rm -f h/*
	cd low; make clean; cd ..
	cd matrix; make clean; cd ..
	cd spmat; make clean; cd ..
	cd parse; make clean; cd ..

# make man pages
man:
	set nonomatch; rm -f ../doc/man/*
	cd low; make man; cd ..
	cd matrix; make man; cd ..
	cd spmat; make man; cd ..
	cd parse; make man; cd ..

# make everything
all: 
	set nonomatch; rm -f ../doc/man/*
	cd low; make all CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..
	cd matrix; make all CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..
	cd spmat; make all CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..
	cd parse; make all CC=$(CC) ADDCFLAGS=$(ADDCFLAGS); cd ..

