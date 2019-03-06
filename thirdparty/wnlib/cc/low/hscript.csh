#!/bin/csh -f

set nonomatch

foreach hfile (*.h)
  rm -f ../h/$hfile
  ln -s ../low/$hfile ../h
end

