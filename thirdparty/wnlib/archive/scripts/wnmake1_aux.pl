#!/usr/bin/perl -w

use strict;

# this script assumes it is called by a shell script that has already
# sourced wnmake_flags

my ($makefile, $inc, $libs, $prog, $wnlib, $f, $base);
my ($wnmake_cc, $wnmake_addcflags, $wnmake_make);

$makefile = "/tmp/wnmakeq.$$.makefile";

open MAKEFILE, ">$makefile";

$wnlib            = $ENV{WNLIB};
$wnmake_cc        = $ENV{WNMAKE_CC};
$wnmake_addcflags = $ENV{WNMAKE_ADDCFLAGS};
$wnmake_make	  = $ENV{WNMAKE_MAKE};

$inc  = "-I$wnlib/acc/h";
$libs = "$wnlib/acc/text.a -lm";
$prog = $ARGV[0];
$prog =~ s/[.][^.]*$//;

if ($prog eq $ARGV[0]) {
  print "error: program '$prog' == $prog ==" . ' $1' . "\n";
  exit 1;
}

print MAKEFILE "\n.c.o:\n";
print MAKEFILE "\t$wnmake_cc $wnmake_addcflags $inc -c \$<\n\n";

print MAKEFILE "$prog: ";
for $f (@ARGV) {
  $base = $f;
  $base =~ s/[.][^.]*$//;

  print MAKEFILE "$base.o ";
}
print MAKEFILE "\n";
print MAKEFILE "\t$wnmake_cc \$^ $libs -o $prog\n";

close MAKEFILE;
system "$wnmake_make -f $makefile";

unlink $makefile;
