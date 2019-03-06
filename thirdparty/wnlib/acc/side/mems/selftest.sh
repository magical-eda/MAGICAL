#!/bin/sh

if [ "$1" = "-valgrind" ]; then
  VALGRIND="valgrind"
  echo Running Valgrind:
else
  VALGRIND=""
fi

$VALGRIND selftest_aux
if [ $? != 0 ]; then
  exit 1
fi
$VALGRIND selftest_aux       -pad
if [ $? != 0 ]; then
  exit 1
fi
$VALGRIND selftest_aux -fill
if [ $? != 0 ]; then
  exit 1
fi
$VALGRIND selftest_aux -fill -pad
if [ $? != 0 ]; then
  exit 1
fi
$VALGRIND selftest_aux -fill -pad -substitute
if [ $? != 0 ]; then
  exit 1
fi
$VALGRIND selftest_aux -substitute
if [ $? != 0 ]; then
  exit 1
fi
