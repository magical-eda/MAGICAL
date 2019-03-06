#!/usr/bin/env bash

set -x
cp ../acc/side/mems/wnmems.c selftest
cd selftest
echo Ignore the following linker err, we just want wnmems.o
../../command/wnmake1 wnmems.c
rm -rf pure.tmp
mkdir -p pure.tmp
purify -cache-dir=pure.tmp /depot/qsc/QSCV/bin/cc -xarch=v9 -xildoff -g wnmems.o $(excl wnst_*.o - *pure*) selftest_main.o -o selftest_main.pur ../../acc/text.a -lm
