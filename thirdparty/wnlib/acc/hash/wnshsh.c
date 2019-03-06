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
#include <string.h>

#include "wnlib.h"
#include "wnrtab.h"

#include "wnhash.h"



/* 
  This hashing method was invented by Jay Adams in March 1997.

  jay 1-long xor-shift hash 
*/
#if 0
int wn_strhash(register char *key)
{
  register int c,ret;

  c = (int)(*key);

  if(c == (int)'\0')
  {
    return(0);   /* empty string */
  }
  else
  {
    ret = c;

    c = (int)(*(++key));

    if(c == (int)'\0')   /* single char string */
    {
      return(ret);   
    }
    else
    {
      do
      {
	ret = c^(ret<<9)^(ret>>7)^(ret&0x6e);
	/*
	ret = c^(ret<<9)^(ret>>7)^(ret&0xe6);
	*/
        c = (int)(*(++key));
      }
      while(c != (int)'\0');

      return(ret);
    }
  }
}
#endif


#if 0
int wn_strhash(register char *key)
{
  register int c,ret;
  register int *random_int_table;

  c = (int)(*key);

  if(c == (int)'\0')
  {
    return(0);   /* empty string */
  }
  else
  {
    random_int_table = wn_random_int_table + 260;

    ret = random_int_table[c];

    c = (int)(*(++key));

    if(c == (int)'\0')   /* single char string */
    {
      return(ret);   
    }
    else
    {
      do
      {
	/*
	ret = c^(ret<<1)^(ret>>1)^(ret&9);
        ret ^= random_int_table[c];
	*/
        ret += random_int_table[c];

        c = (int)(*(++key));
      }
      while(c != (int)'\0');

      return(ret);
    }
  }
}
#endif


#if 0
/* new naylor random-table-lookup hash */
int wn_strhash(register char *key)
{
  register unsigned int c;
  register unsigned int ret;
  register int *random_int_table;

  c = (int)(*key);
  if(c == (int)'\0')   return(0);   /* empty string */

  random_int_table = wn_random_int_table+128;
  ret = random_int_table[c];

  c = (int)(*(++key));
  if(c == (int)'\0')  return(ret);  /* single char string */

  do
  {
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table += 64;
    ret ^= random_int_table[c];

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    random_int_table -= 7*64;
    ret = (ret<<1)^(ret>>1)^(ret&9);
    ret ^= random_int_table[c];

    c = (int)(*(++key));
  }
  while(c != (int)'\0');

  return(ret);
}
#endif


#if 0
/* new jay 8-long xor hash */
int wn_strhash(register char *key)
{
  register unsigned int c;
  register unsigned int ret;

  c = (int)(*key);
  if(c == (int)'\0')   return(0);   /* empty string */

  ret = c;

  c = (int)(*(++key));
  if(c == (int)'\0')  return(ret);  /* single char string */

  do
  {
    ret ^= c^(ret<<13)^(ret>>15);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<13)^(ret>>16);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<10)^(ret>>11);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<14)^(ret>>16);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<16)^(ret>>15);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<15)^(ret>>13);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<11)^(ret>>14);

    c = (int)(*(++key));
    if(c == (int)'\0') return(ret);
    ret ^= c^(ret<<13)^((ret&1531793578)>>12);

    c = (int)(*(++key));
  }
  while(c != (int)'\0');

  return(ret);
}
#endif


#if 1
int wn_strhash(char *key)
{
  return(wn_memhash(key,strlen(key)));
}
#endif
