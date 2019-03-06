#!gmake

TARGET_ARCH =
#CFLAGS = -g
CFLAGS = -I../../../acc/h
ARCHIVE = ../../../acc/text.a

compile: wnlint

wnlint: wnlint.o
	cc -o wnlint wnlint.o $(ARCHIVE)

clean:
	set nonomatch; rm -f *.o
	rm -f core 'e'rrors.txt tmp
	rm -f wnlint

all: clean compile

