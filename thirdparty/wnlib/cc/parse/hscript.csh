#!/bin/csh -f

set nonomatch

foreach hfile (*.h)
  rm -f ../h/$hfile
  ln -s ../parse/$hfile ../h
end

