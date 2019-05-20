#!/bin/csh -f

foreach file (*.man)
  head -2 < $file | tail +2
end
