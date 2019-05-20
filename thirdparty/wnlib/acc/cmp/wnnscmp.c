/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor

****************************************************************************/
#include <ctype.h>
#include "wncmp.h"



int wn_numstrcmp(register char *s1,register char *s2)
{
  register char c1,c2;
  register char *ss1,*ss2;
  register int diff;

  while(TRUE)
  {
    c1 = *s1;  c2 = *s2;

    if(isdigit(c1)&&isdigit(c2))  /* compare numeric strings */
    {
      ss1 = s1;  ss2 = s2;

      for(++s1;isdigit(*s1);++s1)
        ;
      for(++s2;isdigit(*s2);++s2)
        ;
       
      diff = (s1 - ss1)-(s2 - ss2);  /* which numeric string longer */

      if(diff != 0)
      {
        return(diff);
      }

      while(TRUE)
      {
        if(ss1 == s1)
        {
          break;
        }

        diff = (*ss1) - (*ss2);

        if(diff != 0)
        {
          return(diff);
        }

        ++ss1;  ++ss2;
      }
    }
    else
    {
      if(c1 == '\0')
      {
        if(c2 == '\0')
        {
          return(0);
        }
        else  /* (c1 == '\0')&&(c2 != '\0') */
        {
          return(-1);
        }
      }
      else /* c1 != '\0' */
      {
        if(c2 == '\0')
        {
          return(1);
        }
      }

      diff = c1 - c2;

      if(diff != 0)
      {
        return(diff);
      }

      ++s1;  ++s2;
    }
  }
}

