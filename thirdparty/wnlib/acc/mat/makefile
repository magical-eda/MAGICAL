#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
#ADDCFLAGS = -g -xO3
#ADDCFLAGS = -dalign -xO3
CC = cc
CFLAGS = $(ADDCFLAGS) -I../h
ARCHIVE = text.a
LIBS = ../vect/text.a ../misc/text.a ../random/text.a ../arg/text.a ../sort/text.a ../list/text.a ../random/text.a ../mem/text.a ../low/text.a
OBJS = wnmmk.o wnmio.o wnmcpy.o wnmgen.o wnmrnd.o wnmtrn.o \
       wngs.o wnminv.o wnmmlt.o wnrspx.o wnsplx.o wnmpin.o \
       wnsvd.o

compile: $(ARCHIVE) examples selftest

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
	ar slrcu $(ARCHIVE) $(OBJS)
#	$(AR) $(ARCHIVE) $(OBJS)
#	$(RANLIB) $(ARCHIVE) 

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

