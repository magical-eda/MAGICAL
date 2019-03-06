#!gmake

TARGET_ARCH =
WNLIB = ../../..
CFLAGS = -g  -I$(WNLIB)/acc/h
ARCHIVES = $(WNLIB)/acc/text.a
OBJS = gendoc.o
#CC = /depot/sunpro-4.0.1/acc
CC = cc

compile: gendoc

gendoc: $(OBJS)
	$(CC) -g -o gendoc $(OBJS) $(ARCHIVES) -lm 

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f gendoc

all: clean compile
