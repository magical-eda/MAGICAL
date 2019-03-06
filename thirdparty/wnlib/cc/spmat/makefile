#!gmake

TARGET_ARCH =

#CFLAGS = -g -I../low $(ADDCFLAGS)
CFLAGS = -I../low $(ADDCFLAGS)
ARCHIVE = text.a
#OBJS = wnscon.o wnsio.o wnsmk.o wnsrnd.o wnmst.o wntrn.o wntrna.o wnsp.o
OBJS = wnscon.o wnsio.o wnsmk.o wnsrnd.o wnscpy.o wnstrn.o wnscnt.o \
       wnmst.o wnsp.o wncp.o wnlp.o \
       wntrns.o wntrn.o wntrnf.o wntrni.o \
       wnyd.o wnydl.o wnres.o wnstc2.o

compile:	$(ARCHIVE) examples

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	ar lrcu $(ARCHIVE) $(OBJS)
	ranlib $(ARCHIVE)

examples: examples.o $(ARCHIVE)
	cc -o examples examples.o $(ARCHIVE) ../matrix/text.a ../low/text.a -lm

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

