#!/bin/csh -f

set nonomatch

foreach manpage (*.man)
  rm -f ../../doc/man/$manpage
  ln -s ../../cc/matrix/$manpage ../../doc/man/
end

