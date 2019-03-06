#!gmake

TARGET_ARCH =
#ADDCFLAGS = -g
ADDCFLAGS = -O
#CC = gcc
#CC = acc
CC = cc
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../mat/text.a ../vect/text.a ../misc/text.a ../random/text.a ../hash/text.a \
	../sort/text.a ../list/text.a ../cmp/text.a ../cpy/text.a ../mem/text.a ../low/text.a
OBJS = wnbar.o wnbar2.o wnpen.o wnnlp.o wnmqf.o wnffln.o wnconjd.o wnconjg.o\
       wncnjfg.o wnroot.o wnqfit.o \
	wnlbfgs.o wn1dmin.o wnclamp.o wnnmgd.o wnparvect.o wntol.o
AR = ar lcru
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

