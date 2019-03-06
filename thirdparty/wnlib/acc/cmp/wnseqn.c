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
#include "wnlib.h"

#include "wneq.h"



bool wn_streqnc(register char s1[],register char s2[])
{
  register char c1,c2;

  for(;;)
  {
    c1 = *s1;
    c2 = *s2;
    
    if(islower(c1))      /* upcase c1 */
    {
      c1 = toupper(c1);
    }
    if(islower(c2))      /* upcase c2 */
    {
      c2 = toupper(c2);
    }

    if(c1 != c2)
    {
      return(FALSE);
    }
    
    if(c1 == '\0')
    {
      return(TRUE);
    }

    ++s1; ++s2;
  }
}


