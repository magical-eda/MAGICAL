#!pmake

TARGET_ARCH =
WNLIB = ../../../..
CFLAGS = -g  -I$(WNLIB)/cc/h
ARCHIVES = $(WNLIB)/acc/text.a
OBJS = tohex.o

compile: tohex

tohex: $(OBJS)
	cc -g -o tohex $(OBJS) $(ARCHIVES) -lm 

clean:
	rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f tohex

all: clean compile
