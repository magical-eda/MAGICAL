/**********************************************************************

int wn_true_random_bit()

**********************************************************************/
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

#include <stdio.h>
#include <stdlib.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnrndt.h"



local FILE *f1,*f2;
/* local FILE *f3; ** unused - bchapman 041111 */
local bool initialized = FALSE;


local void initialize(void)
{
  int i;
# ifndef __cplusplus
    extern FILE *popen (const char *__command, const char *__modes);
    /*					Sometimes redundant, sometimes not */
# endif

  initialized = TRUE;

  /*
  f = popen("echo \"find / -exec cat {} \\; |& crypt sillypassword | compress\" | csh -f","r");
  f = popen("echo \"find /usr/spool/news -exec cat {} \\; |& crypt sillypassword | compress\" | csh -f","r");
  */

  /* bc - 8/7/2004 */
  /* compress did not exist on Linux, changed to gzip */
  /* ignored 1st mbyte of disk output.  Don't skip much audio, the mic's slow */

  f1 = popen("echo \"tar cf - / |& gzip\" | csh -f","r");  /* system disk */
  for(i=0;i < 1<<20;++i)  
    (void)getc(f1);

  f2 = popen("echo \"gzip < /dev/audio\" | csh -f","r");   /* microphone */
  for(i=0;i<100;++i)  
    (void)getc(f2);

#if 0
  f3 = popen("echo \"tar cf - /usr/spool/news |& compress\" | csh -f","r");  /* news */
  for(i=0;i<100;++i)  
    (void)getc(f3);
#endif
}


/*   note this will return 8 bytes from f1, then 7 bytes from f2 ..., assuming
** that was what is wanted - bc 2004.8.7 */

local int harvest_random_byte(void)
{
  static int count=0;
  int c;

  if(count <= 7)
  {
    c = getc(f1);
  }
  else
  {
    c = getc(f2);
  }

  wn_assert(EOF != c);

  ++count;
  if (count > 14)
  /**/		  count = 0;

  return c;
} /* harvest_random_byte */


int wn_true_random_bit(void)
{
  int ret;
  int c;
  int i;

  if(!initialized)
  {
    initialize();
  }

  ret = 0;

  for(i=0;i<97;++i)
  {
    c = harvest_random_byte();
    /*
    printf("%c",c);
    */

    if(c ==  EOF)
    {
      fprintf(stderr,"\n\nwn_true_random_bit: no more input text.  exiting.\n");
      exit(-1);
    }

    ret ^= c;
  }

  ret &= ((1<<8)-1);

  ret ^= (ret >> 4);
  ret ^= (ret >> 2);
  ret ^= (ret >> 1);

  ret &= 1;

  return(ret);
}


int wn_true_random_int(void)
{
  int ret;
  int i;

  ret = 0;

  for(i=0;i<32;++i)
  {
    ret <<= 1;
    ret ^= wn_true_random_bit();
  }

  return(ret);
}


