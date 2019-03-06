#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
#CC = gcc
#CC = acc
CC = cc
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
OBJS = wntrn.o wntrnf.o wntrni.o wntrns.o \
       wnspcon.o wnstrn.o wnspcpy.o wnspio.o wnspmk.o wnsprnd.o \
       wnspcnt.o wnlp.o \
       wncp.o wnsp.o wnmst.o wnflow.o
LIBS = ../vect/text.a ../sort/text.a ../list/text.a ../btr/text.a ../hash/text.a ../cpy/text.a ../cmp/text.a ../misc/text.a ../random/text.a ../mem/text.a ../low/text.a
AR = ar lrcu
RANLIB = ranlib

compile:	$(ARCHIVE) examples selftest

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
#	ar lrcu $(ARCHIVE) $(OBJS)
#	ranlib $(ARCHIVE)
	$(AR) $(ARCHIVE) $(OBJS)
	$(RANLIB) $(ARCHIVE)

examples: examples.o $(ARCHIVE)
	$(CC) -o examples examples.o $(ARCHIVE) $(LIBS) -lm

selftest: selftest.o $(ARCHIVE)
	$(CC) -o selftest selftest.o $(ARCHIVE) $(LIBS) -lm

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

