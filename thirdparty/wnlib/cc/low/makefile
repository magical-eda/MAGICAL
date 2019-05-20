#!gmake

TARGET_ARCH =

CFLAGS = -g $(ADDCFLAGS)
#CFLAGS = $(ADDCFLAGS)
ARCHIVE = text.a
OBJS = wnblk.o wnbsr.o wncase.o wncrsh.o wnstk.o \
       wnhtab.o wnhtbl.o wnhash.o \
       wnrtab.o \
       wnmem.o wnmemn.o wnmemg.o wnmlc.o \
       wnset.o wnseto.o wnsll.o wnbll.o wnstr.o wnmap.o wnio.o \
       wnrnd.o wnrndd.o wnrndp.o wnrndl.o \
       wnsort.o wnqsrt.o wnsrtl.o wnrsrt.o \
       wnnull.o wnnop.o wnloop.o wnint.o wnmod.o wnprm.o \
       wnbtr.o wnbtrl.o wncpy.o \
       wnddtr.o wnddtl.o \
       wnargv.o wnargp.o wni2.o wnanl.o wnexp.o \
       wnlisn.o

compile: $(ARCHIVE) examples

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	ar lrcu $(ARCHIVE) $(OBJS)
	ranlib $(ARCHIVE)

examples: examples.o $(ARCHIVE) 
	cc -o examples examples.o $(ARCHIVE) -lm

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

