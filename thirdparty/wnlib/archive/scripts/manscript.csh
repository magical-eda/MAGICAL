#!/bin/csh -f

set nonomatch

set filelist = (*.man)
set files = ($filelist)
set flatfiles = ()
foreach manfile ($filelist)
  if (-e $manfile) then
    set flatfiles = ($flatfiles `basename $manfile`)
  endif
end

pushd ../../doc/man >/dev/null

foreach manfile ($files)
  set target = `basename $manfile`
  echo "linking $target"
  rm -f $target
  ln -s ../../archive/scripts/$manfile $target
end

cd ../mantext

foreach manfile ($flatfiles)
  set target = `basename $manfile .man`.txt
  echo "linking $target"
  rm -f $target
  ln -s ../man/$manfile $target
end

popd >/dev/null
