/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/


#include "wnlib.h"
#include "wnmat.h"



local void test_inverse_for_size(size)

int size;

{
}


local void test_inverse()
{
  int i;

  for(i=1;i<200;i*=3)
  {
    test_inverse_for_size(i);
  }
}


void test_matrix()
{
  test_inverse();
}


