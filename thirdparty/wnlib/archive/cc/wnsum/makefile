#!gmake

TARGET_ARCH =
WNLIB = ../../..
CFLAGS = -g  -cckr -I$(WNLIB)/cc/h
ARCHIVES = $(WNLIB)/acc/text.a
OBJS = wnsum.o

compile: wnsum

wnsum: $(OBJS)
	cc -g -o wnsum $(OBJS) $(ARCHIVES) -lm 

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f wnsum

all: clean compile
