TARGET_ARCH =
ADDCFLAGS = -O
CC = cc
CFLAGS = $(ADDCFLAGS) -I../../acc/h
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
LIBS = ../../acc/text.a -lm

compile: selftest_main

clean:
	rm *~ *.o selftest_main

all: clean compile

selftest_main: $(OBJECTS) $(LIBS)
	$(CC) $^ -o $@
