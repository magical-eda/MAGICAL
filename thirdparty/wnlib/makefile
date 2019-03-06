#!gmake

MAKE = make
TARGET_ARCH =

# compile everything
compile:
#	cd cc; $(MAKE) compile; cd ..
	cd acc; $(MAKE) compile; cd ..
#	cd archive/cc; $(MAKE) compile; cd ..
#	cd windows; $(MAKE) compile; cd ..

# make everything
all: clean man
#	cd cc; $(MAKE) all; cd ..
	cd acc; $(MAKE) all; cd ..
#	cd archive/cc; $(MAKE) all; cd ..
#	cd windows; $(MAKE) all; cd ..

WLM = doc/mantext/wnlibman
# remove objects 
clean:
	rm -rf `find . -name '*~'` `find . -name '*.tmp'`
	rm -f `find . -name core`
	rm -f doc/man/*.man doc/mantext/*.txt
	cp -p doc/mantext/main_man.sav doc/mantext/main_man.txt
	rm -f `find . -name '*.[oa]'` `find . -name '*.ln'`
	cd cc; $(MAKE) clean; cd ..
	cd acc; $(MAKE) clean; cd ..
	cd archive/cc; $(MAKE) clean; cd ..
	cd windows; $(MAKE) clean; cd ..

# make man pages
man:
#	cd cc; $(MAKE) man; cd ..
	cd acc; $(MAKE) man; cd ..
	cd archive/cc; $(MAKE) man; cd ..
	cd archive/scripts; $(MAKE) man; cd ..

# selftest
selftest: 
#	cd cc; $(MAKE) selftest; cd ..
	cd acc; $(MAKE) selftest; cd ..
#	cd archive/cc; $(MAKE) selftest; cd ..

MM = doc/mantext/main_man
release: clean
	cp -p $(MM).sav $(MM).txt
	command/html2text.bash $(MM).html *.html
	cp -p $(MM).txt $(MM).sav
	command/html2text.bash ADVERTIZE.html
	cd windows; $(MAKE) release; cd ..
	cp README README_WINDOWS.txt
	command/unx2doss.pl *_WINDOWS.txt doc/mantext/*.txt
