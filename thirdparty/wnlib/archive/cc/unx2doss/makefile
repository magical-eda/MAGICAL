#!gmake

TARGET_ARCH =
CFLAGS = -g
OBJS = unx2doss.o
#CC = acc
CC = cc

compile: unx2doss

unx2doss: $(OBJS)
	$(CC) -g -o unx2doss $(OBJS)

clean:
	set nonomatch; rm -f *.o core 'e'rrors.txt *tmp* unx2doss

all: clean compile
