#!/usr/bin/perl -w

#     <<<<NOTE!!!!>>>>  The windows user doesn't have to understand or
# run this file.  It is just to be run by the developer to make the
# copysrc.bat file before zipping wnlib for release.

#     This file is part of wnlib and as such is in the public domain and may
# be used by anybody for any purpose.  It comes as is, with no warrantee or
# guarantee whatsoever.
#
# Author
#     Bill Chapman


#			Windows copysrc system

#     This file is run under unix to generate a .bat file in this directory
# to be run from this directory under windows.  The .bat file will copy
# the wn*.h files, the wn*.c files, and the examples files into the windows
# area.

#     Note that selftest files require some special processing and are being
# handled separately by gen_selftest.pl.

#     The idea of the windows directory is that we want to create a separate
# area for windows so that the machinery for windows building won't interfere
# with unix building.  Ideally, we would just create symbolic links to the
# files under ../acc, but symbolic links aren't supported on windows.

use strict;	# just comment this line out if it's a problem
use File::Basename;

my ($p, $o, $u, $pwd, $dirs_str, @dirs, $d, $st_main, $st_in, $st_out, $line);
my (@dirs2);
$p = "gen_copysrc.pl";
$o = "copysrc.bat";
$u = ".\\unx2doss";

$pwd = `pwd`;	chomp($pwd);
(basename($pwd) eq "windows") || die "$p: should be run from windows dir\n";

unlink $o;
open BATOUT, ">$o" or die "$p couldn't open output\n";
print BATOUT "\n";	# blank first line, no echo off

system 'make -s -f ../acc/makefile dirs';
$dirs_str = `cat dirs.txt`;
chomp $dirs_str;
@dirs = split / /, $dirs_str;

# filter out "side/mems" directory
@dirs2 = ();
for $d (@dirs) {
  push @dirs2, $d if ("side/mems" ne $d);
}
@dirs = @dirs2;

# includes ------------------------------------------------

foreach $d (@dirs) {
  print BATOUT "copy ..\\acc\\$d\\wn*.h h\n";
}
print BATOUT "del h\\wnrndt.h\n";
print BATOUT "$u h\\*.h\n";
print BATOUT "\n";

# c --------------------------------------------------------

foreach $d (@dirs) {
  print BATOUT "copy ..\\acc\\$d\\wn*.c src\n";
}
print BATOUT "del src\\wnrndt.c\n";
print BATOUT "del src\\wnqgfit.c\n";
print BATOUT "del src\\wncnjfg_bogdan.c\n";
print BATOUT "del src\\wnmadd.c\n";
print BATOUT "del src\\wncnjng.c\n";
print BATOUT "del src\\wncnjdn.c\n";
print BATOUT "del src\\wncnjdn2.c\n";
print BATOUT "$u src\\*.c\n";
print BATOUT "\n";

# examples ------------------------------------------------

foreach $d (@dirs) {
  if (-f "../acc/$d/examples.c") {
    print BATOUT "copy ..\\acc\\$d\\examples.c examples\\wnex_$d.c\n";
  }
}
print BATOUT "$u examples\\wnex_*.c\n";
print BATOUT "\n";

# mantext ------------------------------------------------

# we are copying *.man to *.txt here, so we have to copy
# them 1 by 1
my ($f, $b, $suffix);
foreach $f (<../acc/*/*.man>, <../archive/cc/*/*.man>,
				<../archive/scripts/*.man>) {
  ($b, $d, $suffix) = fileparse($f);
  $f =~ s#/#\\#g;
  $b = basename($b, ".man");
  print BATOUT "copy $f ..\\doc\\mantext\\$b.txt\n";
}
print BATOUT "$u ..\\doc\\mantext\\*\n";
print BATOUT "\n";

# selftest ------------------------------------------------

print BATOUT "$u selftest\\*\n";
