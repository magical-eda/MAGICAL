#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
#ADDCFLAGS = -fast
#CC = cc
#CC=/depot/sunpro-4.0.1/acc
CC=acc
AR = ar lrcu
RANLIB = ranlib
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../cmp/text.a ../cpy/text.a ../list/text.a ../str/text.a ../random/text.a ../mem/text.a ../low/text.a
OBJS = wnhtab.o wnshsl.o wnihsl.o wnlhsl.o wnphsl.o wnhhsl.o \
       wnshsh.o wnihsh.o wnphsh.o wnmhsh.o wnchsh.o wnmodh.o

compile: $(ARCHIVE) examples selftest

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

