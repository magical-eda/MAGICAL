#     This file is NOT to be run on windows.  It is just to be run on unix,
# getting the wnlib/windows directory packaged for windows.  Windows users
# can ignore everything that follows.

SHELL = /bin/sh
TARGET_ARCH =

release: clean
	rm -rf selftest/*.c copysrc.bat
	make -f ../acc/makefile dirs
	./gen_copysrc.pl		# makes copysrc.bat
	./gen_selftest.pl		# make selftest directory
	../command/html2text.bash *.html
	../command/unx2doss.pl *.html *.txt *.bat

compile:
	cd selftest; make compile; cd ..

#     note "make clean" is not to remove selftest/*.c or copysrc.bat
# or dirs.txt
clean:
	rm -rf `find . -name '*.tmp'`
	rm -f `find . -name 'core*'` `find . -name '.pure*'`
	rm -f h/* src/* examples/*
	rm -rf selftest/*.o selftest/selftest_main selftest/*tmp*
	rm -rf selftest/selftest_main.pur

all: clean release compile
