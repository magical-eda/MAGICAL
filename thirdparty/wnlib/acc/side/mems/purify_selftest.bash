#!/usr/bin/env bash

set -x
cp ../acc/side/mems/wnmems.c selftest
cd selftest
echo Ignore the following linker err, we just want wnmems.o
../../command/wnmake1 wnmems.c
rm -rf pure.tmp
mkdir -p pure.tmp
purify -cache-dir=pure.tmp gcc -g wnmems.o selftest_aux.o -o selftest_aux.pur ../../text.a -lm
