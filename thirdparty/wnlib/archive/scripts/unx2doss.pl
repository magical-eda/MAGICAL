#!/usr/bin/perl -w

use strict;	# just comment this line out if it's a problem

my ($p, $t, $file, $line);
$p = "unx2doss.pl";
$t = "unx2doss.tmp";


foreach $file (@ARGV) {
  open TMP,    ">$t";
  open INFILE, "<$file";

  while ($line = <INFILE>) {
    $line =~ s/\r//g;
    $line =~ s/\n/\r\n/;
    print TMP "$line";
  }

  close INFILE;
  close TMP;

  rename $t, $file;
}
