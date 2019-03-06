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




main()
{
  int c,col;

  while(TRUE)
  {
    for(col=0;col<10;++col)
    {
      c = getchar();

      if(c == EOF)
      {
        putchar('\n');

        return;
      }

      print_char(c);
    }

    putchar('\n');
  }
}



local print_char(c)

int c;

{
  printf("%2.2x ",c);
}


