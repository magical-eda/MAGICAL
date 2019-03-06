#!/bin/csh -f

set nonomatch

set h = (*.h)
if ($h != "*.h") then
  echo '**** error **** hscript.csh needs to change, mems has .h file(s)'
  exit 1
endif
