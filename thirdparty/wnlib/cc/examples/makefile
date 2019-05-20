#!gmake

CFLAGS = -g -I../matrix -I../low $(ADDCFLAGS)
OBJS = test_anl2.o 
#OBJS = test_anl.o 

compile:	examples

examples: $(OBJS) $(ARCHIVE)
	cc -o examples $(OBJS) $(ARCHIVE) ../matrix/text.a ../low/text.a -lm

clean:
	set nonomatch; rm -f *.o
	rm -f $(ARCHIVE)
	rm -f examples
	rm -f core errors.txt tmp

all: clean compile

