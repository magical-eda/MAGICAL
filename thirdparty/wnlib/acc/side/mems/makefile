#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
#CC = gcc
CC = cc
#CC = CC
AR = ar slrcu
RANLIB = echo > /dev/null
CFLAGS = $(ADDCFLAGS) -I../../h
ARCHIVE = text.a
LIBS = ../../chash/text.a ../../hash/text.a ../../cmp/text.a ../../cpy/text.a ../../misc/text.a ../../random/text.a ../../low/text.a
OBJS = wnmems.o

compile: $(ARCHIVE) examples selftest selftest_aux

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
#	ar slrcu $(ARCHIVE) $(OBJS)
	$(AR) $(ARCHIVE) $(OBJS)
	$(RANLIB) $(ARCHIVE) 

selftest: selftest.sh selftest_aux
	cp selftest.sh selftest

selftest_aux: selftest_aux.o $(ARCHIVE) 
	$(CC) -o selftest_aux selftest_aux.o $(ARCHIVE) $(LIBS) -lm

test: test.o $(ARCHIVE) 
	$(CC) -o test test.o $(ARCHIVE) $(LIBS) -lm

examples: examples.o $(ARCHIVE) 
	$(CC) -o examples examples.o $(ARCHIVE) $(LIBS) -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples selftest selftest_aux
	rm -f core 'e'rrors.txt tmp
	rm -rf pure.tmp pure selftest_aux selftest_aux.pur

man:
	./manscript.csh
	
hfiles:
	./hscript.csh
	
all: clean compile hfiles man
