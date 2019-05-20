#!gmake

TARGET_ARCH =
WNLIB = ../../..
CFLAGS = -g  -I$(WNLIB)/cc/h
ARCHIVES = $(WNLIB)/cc/low/text.a

compile: countcat shatter unshatter

shatter: shatter.o
	cc -g -o shatter shatter.o $(ARCHIVES) -lm 

unshatter: unshatter.o
	cc -g -o unshatter unshatter.o $(ARCHIVES) -lm 

countcat: countcat.o
	cc -g -o countcat countcat.o $(ARCHIVES) -lm 

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f shatter countcat unshatter

all: clean compile
