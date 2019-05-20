#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
#ADDCFLAGS = -fast
CC = cc
#CC=/depot/sunpro-4.0.1/acc
#CC=gcc
AR = ar lrcu
RANLIB = ranlib
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../sort/text.a ../hash/text.a ../arg/text.a ../cmp/text.a ../cpy/text.a ../list/text.a ../str/text.a ../misc/text.a ../random/text.a ../mem/text.a ../low/text.a
OBJS = wnchtb.o wnchtl.o wnchss.o

compile: $(ARCHIVE) selftest examples

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	$(AR) $(ARCHIVE) $(OBJS)
	$(RANLIB) $(ARCHIVE) 

#wnhtab.o: wnhtab.c
#	$(CC) -S $(CFLAGS) wnhtab.c

selftest: selftest.o $(ARCHIVE) 
	$(CC) -o selftest selftest.o $(ARCHIVE) $(LIBS) -lm

examples: examples.o $(ARCHIVE) 
	$(CC) -o examples examples.o $(ARCHIVE) $(LIBS) -lm

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
