#!/bin/csh -f

set nonomatch

mkdir -p h
foreach hfile (*/*.h)
  if ( ! -e h/`basename $hfile` ) then
    echo "linking $hfile"
    rm -f h/`basename $hfile`
    ln -s ../$hfile h
  endif
end

