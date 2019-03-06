#!/bin/csh -f

set nonomatch

foreach hfile (*.h)
  if ( ! -e ../h/$hfile ) then
    ln -s ../chash/$hfile ../h
  endif
end

