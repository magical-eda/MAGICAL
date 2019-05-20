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
#include "wninr.h"




main()
{
  int in_c,out_c;
  bool blank;

  blank = TRUE;
  out_c = 0;

  while(TRUE)
  {
    in_c = getchar();

    if(in_c == EOF)
    {
      if(blank)
      {
	return;
      }
      else
      {
	wn_crash();
      }
    }

    if(wn_char_in_string(in_c,"0123456789abcdef"))
    {
      out_c <<= 4;
      out_c |= convert_in_c(in_c);

      if(blank)
      {
	blank = FALSE;
      }
      else
      {
	wn_assert(wn_inrange(0,<=,out_c,<,256));
	putchar(out_c);

        blank = TRUE;
        out_c = 0;
      }
    }
    else
    {
      if(not(blank))
      {
	wn_crash();
      }
    }
  }
}



local int convert_in_c(in_c)

int in_c;

{
  switch(in_c)
  {
    case('0'):  return(0);
    case('1'):  return(1);
    case('2'):  return(2);
    case('3'):  return(3);
    case('4'):  return(4);
    case('5'):  return(5);
    case('6'):  return(6);
    case('7'):  return(7);
    case('8'):  return(8);
    case('9'):  return(9);
    case('a'):  return(10);
    case('b'):  return(11);
    case('c'):  return(12);
    case('d'):  return(13);
    case('e'):  return(14);
    case('f'):  return(15);
    default:
      wn_crash();
      return(-1);
  }
}



