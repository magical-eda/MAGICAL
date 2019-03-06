#!/bin/csh -f

set nonomatch

foreach hfile (*.h)
  if ( "$hfile" != wnmbtr.h ) then
    if ( ! -e ../h/$hfile ) then
      ln -s ../mem/$hfile ../h
    endif
  endif
end
