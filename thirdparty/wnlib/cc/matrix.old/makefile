#!gmake

#CFLAGS = -g -I../low $(ADDCFLAGS)
CFLAGS = -I../low $(ADDCFLAGS)
ARCHIVE = text.a
OBJS = wnmadd.o wnmcpy.o wnmden.o wnmgen.o \
       wnminv.o wnmio.o wnmliv.o wnmmk.o wnmmlt.o wnmnrm.o wnmtrn.o \
       wnrspx.o wnsplx.o \
       wncplx.o wnsft.o wnfft.o

compile:	$(ARCHIVE) examples

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
	ar lrcu $(ARCHIVE) $(OBJS)
	ranlib $(ARCHIVE)

examples: examples.o rnd.o asg.o $(ARCHIVE)
	cc -o examples examples.o rnd.o asg.o $(ARCHIVE) ../low/text.a -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples
	rm -f core errors.txt tmp

man:
	./manscript.csh

hfiles:
	./hscript.csh

all: clean compile hfiles man

