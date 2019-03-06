#!pmake

TARGET_ARCH =
WNLIB = ../../../..
CFLAGS = -g  -I$(WNLIB)/cc/h
ARCHIVES = $(WNLIB)/cc/low/text.a
OBJS = fromhex.o

compile: fromhex

fromhex: $(OBJS)
	cc -g -o fromhex $(OBJS) $(ARCHIVES) -lm 

clean:
	rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f fromhex

all: clean compile
