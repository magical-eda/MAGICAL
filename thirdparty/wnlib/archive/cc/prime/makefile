#!gmake

TARGET_ARCH =
WNLIB = ../../..
CFLAGS = -g  -I$(WNLIB)/acc/h
ARCHIVES = $(WNLIB)/acc/text.a
OBJS = prime.o
#CC = /depot/sunpro-4.0.1/acc
CC = cc

compile: prime

prime: $(OBJS)
	$(CC) -g -o prime $(OBJS) $(ARCHIVES) -lm 

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f prime

all: clean compile
