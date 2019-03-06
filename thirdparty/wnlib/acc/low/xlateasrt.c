/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <string.h>

#include "wnlib.h"

#include "wnswap.h"
#include "wnasrt.h"


int main(int argc, char *argv[])
{
  int i1,i2;
  char override_buffer[1000];
  int override_line_num;
  int sts;

  i1 = 1;
  i2 = 2;

  if (4 == argc  &&  !strcmp("-override_code", argv[1]))
  {
    sts = sscanf(argv[3], "%d", &override_line_num);
    if (1 != sts)
    {
      fprintf(stderr,
      /**/	"Usage: %s -override_code <filename> <line#>\n", argv[0]);

      return 0;
    }
    wn_get_assert_override_string(override_buffer, argv[2], override_line_num);
    printf("Override string == \"%s\"\n", override_buffer);

    return 0;
  }

  printf("i1=%d,i2=%d\n",i1,i2);
  printf("swapping\n");
  wn_swap(i1,i2,int);
  printf("i1=%d,i2=%d\n",i1,i2);

  printf("first, warn assert....\n");
  wn_assert_warn(1 == 2);
  printf("next, warn notreached....\n");
  wn_assert_warn_notreached();
  if (2 == argc  &&  !strcmp("-notreached", argv[1]))
  {
    printf("now, crash notreached....\n");
    wn_assert_notreached();
  }
  else
  {
    printf("now, crash assert....\n");
    wn_assert(1 == 2);
  }

  return(0);
} /* main */
