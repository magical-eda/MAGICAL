#!/usr/bin/env bash

make dirs
for d in `cat dirs.txt` ; do
  if [ ! -x $d/selftest ] ; then
    echo "****error**** -- $d/selftest doesn't exist"
  else
    cd $d; selftest; cd ..
  fi
done
