#!gmake

TARGET_ARCH =
MAKE = make

#settings for platforms :  SUN, OSF1
STD = -pedantic -Dlinux -std=c89
# STD = -pedantic -Dlinux -std=iso9899:199409
# STD = -pedantic -Dlinux -std=c99
# STD = -pedantic
# STD =
# many platforms
#CC = gcc 
# for testing ansi compiles:
#CC = gcc $(STD)
#CC = g++ 
#CC = /usr/bin/g++ 
#CC = acc 
#CC = /usr/bin/gcc 
CC = cc 
#CC = CC 
#CC = /depot/SUNWspro/bin/CC 
# 64 bit Solaris:
#CC = cc -xarch=v9 -xildoff
#CC = CC -xarch=v9 -xildoff
ADDCFLAGS = -g
#ADDCFLAGS = -O
#ADDCFLAGS = $(STD)
#ADDCFLAGS = -fast   
#ADDCFLAGS = -dalign
AR = ar lrcu
RANLIB = ranlib

#settings for platforms :  SGI/MIPS
#CC = cc
#ADDCFLAGS = -g -mips2 -O
#AR = ar slrcu
#RANLIB = echo > /dev/null

#settings for platforms :  IBM AIX
#CC = cc
#ADDCFLAGS = -g 
#AR = ar lrcu
#RANLIB = ranlib


DIRS = low mem random cmp cpy array list arg sort str misc hash chash pqueue btr skl ddtr parse complex vect mat conjdir anneal spmat side/mems

# compile everything
compile: hfiles
	cd low; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd mem; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd random; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd cmp; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd cpy; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd array; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd list; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd arg; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd sort; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd str; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd misc; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd hash; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd chash; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd pqueue; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd btr; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd skl; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd ddtr; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd parse; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd complex; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd vect; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd mat; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd conjdir; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd anneal; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd spmat; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd side/mems; $(MAKE) compile CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd ../..
	make archive

# h file links
hfiles:
	./hscript.csh

# remove objects 
clean:
	set nonomatch; rm -rf `find . -name '*.tmp'`
	set nonomatch; rm -f text.a core h/* ../doc/man/*
	set nonomatch; rm -f doc/mantext/*.txt
	cd low; $(MAKE) clean; cd .. 
	cd mem; $(MAKE) clean; cd .. 
	cd random; $(MAKE) clean; cd .. 
	cd cmp; $(MAKE) clean; cd .. 
	cd cpy; $(MAKE) clean; cd .. 
	cd array; $(MAKE) clean; cd .. 
	cd list; $(MAKE) clean; cd .. 
	cd arg; $(MAKE) clean; cd .. 
	cd sort; $(MAKE) clean; cd .. 
	cd str; $(MAKE) clean; cd .. 
	cd misc; $(MAKE) clean; cd .. 
	cd hash; $(MAKE) clean; cd .. 
	cd chash; $(MAKE) clean; cd .. 
	cd pqueue; $(MAKE) clean; cd .. 
	cd btr; $(MAKE) clean; cd .. 
	cd skl; $(MAKE) clean; cd .. 
	cd ddtr; $(MAKE) clean; cd .. 
	cd parse; $(MAKE) clean; cd .. 
	cd complex; $(MAKE) clean; cd .. 
	cd vect; $(MAKE) clean; cd .. 
	cd mat; $(MAKE) clean; cd .. 
	cd conjdir; $(MAKE) clean; cd .. 
	cd anneal; $(MAKE) clean; cd .. 
	cd spmat; $(MAKE) clean; cd .. 
	cd side/mems; $(MAKE) clean; cd ../..

# make man pages
man:
	./manscript.csh

# this won't work
manheader:
	for dir in $(DIRS) 
	do
	  cd $dir
	  echo ""
	  echo "****" $dir "*****"
	  for file in *.man
	  do
	    head -2 < $file | tail -1
	  done
	  cd ..
	done

# self test 
selftest:
	./selftest.bash

#     ./selftest.bash was created because the old selftest code wasn't
# complaining AT ALL if some of the selftests were not there.

selftest_old:
#	cd low; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd mem; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd random; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd misc; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd cmp; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd cpy; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd array; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd list; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd arg; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd sort; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd str; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd hash; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd chash; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd pqueue; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd btr; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd skl; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd ddtr; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd parse; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd complex; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd vect; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd mat; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd conjdir; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd anneal; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
#	cd spmat; make selftest CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd .. 
	cd low; selftest; cd .. 
	cd mem; selftest; cd .. 
	cd random; selftest; cd .. 
	cd cmp; selftest; cd .. 
	cd cpy; selftest; cd .. 
	cd array; selftest; cd .. 
	cd list; selftest; cd .. 
	cd arg; selftest; cd .. 
	cd sort; selftest; cd .. 
	cd str; selftest; cd .. 
	cd misc; selftest; cd .. 
	cd hash; selftest; cd .. 
	cd chash; selftest; cd .. 
	cd pqueue; selftest; cd .. 
	cd btr; selftest; cd .. 
	cd skl; selftest; cd .. 
	cd ddtr; selftest; cd .. 
	cd parse; selftest; cd .. 
#	cd complex; selftest; cd .. 
	cd vect; selftest; cd .. 
	cd mat; selftest; cd .. 
	cd conjdir; selftest; cd .. 
#	cd anneal; selftest; cd .. 
	cd spmat; selftest; cd .. 
	cd side/mems; selftest; cd ../.. 

archive:
	$(AR) text.a */wn*.o
	$(RANLIB) text.a

# make everything
all: clean
	set nonomatch; rm -f ../doc/man/*
	set nonomatch; rm -f h/*
	make hfiles
	cd low; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd mem; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd random; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd cmp; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd cpy; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd array; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd list; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd arg; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd sort; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd str; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd misc; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd hash; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd chash; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd pqueue; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd btr; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd skl; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd ddtr; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd parse; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd complex; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd vect; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd mat; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd conjdir; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd anneal; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd spmat; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd .. 
	cd side/mems; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)" AR="$(AR)" RANLIB="$(RANLIB)"; cd ../.. 
#	for dir in $(DIRS) 
#	do
#	  cd $dir; $(MAKE) all CC="$(CC)" ADDCFLAGS="$(ADDCFLAGS)"; cd ..
#	done
	make archive
#	check_dup.pl	# users might not have perl

dirs:
	@echo $(DIRS) >dirs.txt
