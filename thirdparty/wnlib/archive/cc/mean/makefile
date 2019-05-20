#!gmake

TARGET_ARCH =
# uses constant in wnlib h file, but doesn't need to link with it

WNLIB = ../../..
CFLAGS = -g  -I$(WNLIB)/acc/h
OBJS = mean.o
#CC = /depot/sunpro-4.0.1/acc
#CC = acc
CC = cc

compile: mean

mean: $(OBJS)
	$(CC) -g -o mean $(OBJS) -lm 

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f mean

all: clean compile
