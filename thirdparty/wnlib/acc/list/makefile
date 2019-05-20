#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
CC = cc
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../random/text.a ../cmp/text.a ../mem/text.a ../low/text.a
OBJS = wnsmk.o wnsdel.o wnsrev.o wnscpy.o wnscat.o wnsend.o wnsind.o wnscnt.o \
       wnspmk.o wnspdel.o wnsprev.o wnspcat.o wnspend.o wnspind.o wnspcnt.o \
       wnrndl.o wnbll.o \
       wnrndlp.o wnbllp.o
AR = ar lrcu
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

