/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include "wnlib.h"

#include "wncstr.h"



void main()
{
  FILE *write_tty;
  int char_number;
  int c,code;

  write_tty = fopen("/dev/tty","w");

  char_number = 0;

  do
  {
    c = getchar();
    code = putchar(c);
    if(code == EOF)
    {
      fprintf(write_tty,
	      "countcat: tape is full.  Write failed.  %d bytes written.\n",
	      char_number);
      exit(-1);
    }

    ++char_number;
  } while(c != EOF);

  fprintf(write_tty,"countcat: Write successful!!!!!!\n");
  fprintf(write_tty,"countcat: %d bytes written.\n",
	  char_number);

  exit(0);
}

