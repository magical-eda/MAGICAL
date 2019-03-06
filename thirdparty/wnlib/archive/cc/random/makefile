#!gmake

TARGET_ARCH =
WNLIB = ../../..
#CFLAGS = -g -I$(WNLIB)/acc/h
CFLAGS = -I$(WNLIB)/acc/h
ARCHIVES = $(WNLIB)/acc/text.a
OBJS = random.o
CC = gcc
#CC = acc

compile: random

random: $(OBJS)
	$(CC) -g -o random $(OBJS) $(ARCHIVES) -lm 

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f wncrypt

all: clean compile
