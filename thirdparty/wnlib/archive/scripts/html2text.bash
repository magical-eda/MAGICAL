#!/bin/sh

t=/tmp/html2text.$$.tmp

for f in $@ ; do
  if [ -f $f ] ; then
    b=`echo $f | sed -e '/[.][^.]*$/s///'`
    if [ $b.htm != $f -a $b.html != $f ] ; then
      echo $0: error: $f is not an html file 1>&2
      exit 1
    fi
    html2text -nobs <$f >$t
    if [ ! -s $t ] ; then
      echo $0: html2text command had trouble, might not be
      echo '    supported on this platform.'
      if [ -f $b.txt ] ; then
	echo "     Don't worry, we just kept the old $b.txt"
      else
	echo "     $b.txt not created"
      fi
      rm -f $t
    else
      mv $t $b.txt
    fi
  fi
done
