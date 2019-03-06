#!gmake

TARGET_ARCH =
# STD = -pedantic -Dlinux -std=c89
# STD = -pedantic -Dlinux -std=iso9899:199409
# STD = -pedantic -Dlinux -std=c99
# STD = -pedantic
STD =
ADDCFLAGS = -g
# ADDCFLAGS = -O
CC = g++
#CC = gcc
#CC = acc
AR = ar lrcu
RANLIB = ranlib
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../mem/text.a ../low/text.a
OBJS = wnrary.o wnrnd.o wnrmod.o wnrtab.o wntwp.o \
       wnrib.o wnrdb.o wnrflt.o wnrnrm.o \
       wnrexp.o wnpois.o wnrchy.o wnrndt.o

compile: $(ARCHIVE) examples selftest

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	$(AR) $(ARCHIVE) $(OBJS)
	$(RANLIB) $(ARCHIVE) 

selftest: selftest.o $(ARCHIVE) 
	$(CC) -o selftest selftest.o $(ARCHIVE) $(LIBS) -lm

examples: examples.o $(ARCHIVE) 
	$(CC) -o examples examples.o $(ARCHIVE) $(LIBS) -lm

primes: primes.o 
	$(CC) -o primes primes.o $(ARCHIVE) ../list/text.a $(LIBS) -lm

gen:    gen.o 
	$(CC) -o gen gen.o $(ARCHIVE) $(LIBS) -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples selftest
	rm -f core 'e'rrors.txt tmp

man:
	./manscript.csh
	
hfiles:
	./hscript.csh
	
all: clean compile hfiles man

