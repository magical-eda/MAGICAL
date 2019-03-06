#!/bin/csh -f

set nonomatch

foreach hfile (*.h)
  if ( ! -e ../h/$hfile ) then
    ln -s ../list/$hfile ../h
  endif
end

