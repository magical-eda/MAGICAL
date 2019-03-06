#!/bin/csh -f

set nonomatch

foreach manpage (*.man)
  pushd ../../../doc/man >/dev/null
    ln -s ../../acc/side/mems/$manpage .
  popd                   >/dev/null
end
