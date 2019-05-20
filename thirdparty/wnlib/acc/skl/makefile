#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
#CC = gcc
CC = CC
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../btr/text.a ../cmp/text.a ../cpy/text.a ../str/text.a ../arg/text.a ../list/text.a ../random/text.a ../mem/text.a ../low/text.a
OBJS = wnskl.o wnskll.o
AR = ar slrcu
RANLIB = ranlib

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

