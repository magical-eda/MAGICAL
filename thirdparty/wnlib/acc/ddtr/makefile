#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
CC = cc
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../random/text.a ../cpy/text.a ../cmp/text.a ../arg/text.a ../list/text.a ../mem/text.a ../low/text.a
OBJS = wnddtr.o wnddtl.o

compile: $(ARCHIVE) examples selftest

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
	ar slrcu $(ARCHIVE) $(OBJS)

selftest: selftest.o $(ARCHIVE) 
	$(CC) -o selftest selftest.o $(ARCHIVE) $(LIBS) -lm

examples: examples.o $(ARCHIVE) 
	$(CC) -o examples examples.o $(ARCHIVE) $(LIBS) -lm

clean:
	rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples selftest
	rm -f core 'e'rrors.txt tmp

man:
	./manscript.csh
	
hfiles:
	./hscript.csh
	
all: clean compile hfiles man

