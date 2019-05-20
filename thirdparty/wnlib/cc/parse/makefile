#!gmake

TARGET_ARCH =

#CFLAGS = -g -I../low $(ADDCFLAGS)
CFLAGS = -I../low $(ADDCFLAGS)
ARCHIVE = text.a
OBJS = wnptok.o \
       wnwtok.o wnwtokp.o wncstr.o wncstl.o 


compile: $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	ar lrcu $(ARCHIVE) $(OBJS)
	ranlib $(ARCHIVE)

examples: examples.o $(ARCHIVE) 
	cc -o examples examples.o $(ARCHIVE) -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f core 'e'rrors.txt tmp

man:
	./manscript.csh
	
hfiles:
	./hscript.csh
	
all: clean compile hfiles man

