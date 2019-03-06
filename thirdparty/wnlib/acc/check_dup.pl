#!/usr/bin/perl -w

use strict;
use File::Basename;

sub excl {
  my ($past_dash, $a, %exclude, @result);

  @result = ();

  $past_dash = 0;
  for $a (@_) {
    if ('-' eq $a) {
      $past_dash = 1;
    } elsif ($past_dash) {
      $exclude{$a} = 1;
    }
  }

  for $a (@_) {
    if ('-' eq $a) {
      last;
    }

    if (! $exclude{$a}) {
      push @result, $a;
    }
  }

  @result;
} # excl

my (@dups, $a, $b, @c, @out, %hits);

@dups = ();
for $a (&excl(<*/*.[hc]>, "-", <h/*.[hc]>, <*/examples.c>, <*/selftest.c>)) {
  $b = basename($a);
  if (! $hits{$b}) {
    $hits{$b} = 0;
  } 
  if (++ $hits{$b} == 2) {
    push @dups, $b;
  }
}
@dups = sort @dups;

if ($#dups+1 > 0) {
  print "Duplicate";
  if ($#dups+1 > 1) { print "s"; }
  print " found:\n";

  for (@dups) {
    @out = &excl(<*/$_>, "-", <h/$_>);
    print "  @out\n";
  }
} else {
  print "No duplicates found\n";
}
