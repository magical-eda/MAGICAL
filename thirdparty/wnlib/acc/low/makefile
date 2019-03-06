#!gmake

TARGET_ARCH =
ADDCFLAGS = -g
CC = cc
CFLAGS = $(ADDCFLAGS) 
ARCHIVE = text.a
OBJS = wnasrt.o wnint.o wnnop.o

compile: $(ARCHIVE) examples selftest xlateasrt

$(ARCHIVE): $(OBJS)
	rm -f $(ARCHIVE)
	ar slrcu $(ARCHIVE) $(OBJS)
#	$(AR) $(ARCHIVE) $(OBJS)
#	$(RANLIB) $(ARCHIVE) 

selftest: selftest.o $(ARCHIVE) 
	$(CC) -o selftest selftest.o $(ARCHIVE) -lm

examples: examples.o $(ARCHIVE) 
	$(CC) -o examples examples.o $(ARCHIVE) -lm

xlateasrt: xlateasrt.o $(ARCHIVE) 
	$(CC) -o xlateasrt xlateasrt.o $(ARCHIVE) -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples selftest xlateasrt
	rm -f core 'e'rrors.txt tmp

man:
	./manscript.csh
	
hfiles:
	./hscript.csh
	
all: clean compile hfiles man

