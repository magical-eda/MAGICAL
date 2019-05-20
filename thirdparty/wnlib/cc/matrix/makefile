#!gmake

TARGET_ARCH =

CFLAGS = -g -I../low $(ADDCFLAGS)
#CFLAGS = -I../low $(ADDCFLAGS)
ARCHIVE = text.a
OBJS = wnfft.o wnsft.o wncplx.o wnmmk.o wnmio.o wnmgen.o wnmcpy.o wnmrnd.o \
       wnmtrn.o wnmadd.o wnmmlt.o wnminv.o wnmpin.o wnmnrm.o wnsplx.o wnrspx.o \
       wngs.o

compile:	$(ARCHIVE) examples

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	ar lrcu $(ARCHIVE) $(OBJS)
	ranlib $(ARCHIVE)

examples: examples.o $(ARCHIVE)
	cc -o examples examples.o $(ARCHIVE) ../low/text.a -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples
	rm -f core 'e'rrors.txt tmp

man:
	./manscript.csh

hfiles:
	./hscript.csh

all: clean compile hfiles man

