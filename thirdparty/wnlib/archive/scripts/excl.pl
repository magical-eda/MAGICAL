#!/usr/bin/perl -w

use strict;

my ($past_dash, %exclude);

$past_dash = 0;
for (@ARGV) {
  if ('-' eq $_) {
    $past_dash = 1;
  } elsif ($past_dash) {
    $exclude{$_} = 1;
  }
}

for (@ARGV) {
  if ('-' eq $_) {
    last;
  }

  if (! $exclude{$_}) {
    print "$_\n";
  }
}
