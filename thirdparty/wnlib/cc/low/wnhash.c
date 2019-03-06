/**************************************************************************

int wn_strhash(s)
int wn_ptrhash(p)
int wn_inthash(i)

int wn_blkhash(b,len)

wn_cumhasho(&accum,i)
wn_cumhashu(&accum,i)

**************************************************************************/
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
#include "wnlib.h"
#include "wnrtab.h"



#if 0    /* wcn: old code 11/1/89 */
int wn_strhash(key)

register char *key;

{
  extern int wn_random_int_table[];
  register int sum,ret,mask,c;
  register int *random_int_table;

  sum = 0;
  ret = 0;
  mask = WN_POW2_RANDOM_INT_TABLE_SIZE-1;

  random_int_table = wn_random_int_table;

  while(TRUE)
  {
    c = *(key++);

    if(c == '\0')
    {
      break;
    }

    sum += c;
    sum &= mask;

    ret += random_int_table[sum];
  }

  return(ret);
}
#endif



#define SUM_MASK  (WN_POW2_RANDOM_INT_TABLE_SIZE-1)



int wn_strhash(key)

register char *key;

{
  extern int wn_random_int_table[];
  register int c,ret;
  register int *random_int_table;

  c = (int)(*key);

  if(c == (int)'\0')
  {
    return(0);   /* empty string */
  }
  else
  {
    random_int_table = wn_random_int_table + WN_RANDOM_INT_TABLE_SIZE;

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
        ret += random_int_table[c];

        c = (int)(*(++key));
      }
      while(c != (int)'\0');

      return(ret);
    }
  }
}



int wn_ptrhash(p)

ptr p;

{
  int ret;

  ret = (int)p;

  ret ^= (ret >> 2);

  return(ret);
}



int wn_inthash(i)

int i;

{
  return(i);
}



int wn_blkhash(key,len)

register ptr key;
int len;

{
  extern int wn_random_int_table[];
  register int ret;
  register int *random_int_table;

  random_int_table = wn_random_int_table + WN_RANDOM_INT_TABLE_SIZE;

  ret = random_int_table[*(char *)key];

small:
  switch(len)
  {
    case(8):   
		ret ^= (ret >> 5);
                ret ^= random_int_table[*(char *)(++key)];
    case(7):   
		ret ^= (ret << 17);
                ret ^= random_int_table[*(char *)(++key)];
    case(6):   
		ret ^= (ret >> 19);
                ret ^= random_int_table[*(char *)(++key)];
    case(5):   
		ret ^= (ret << 11);
                ret ^= random_int_table[*(char *)(++key)];
    case(4):   
		ret ^= (ret >> 13);
                ret ^= random_int_table[*(char *)(++key)];
    case(3):   
		ret ^= (ret << 3);
                ret ^= random_int_table[*(char *)(++key)];
    case(2):   
		ret ^= (ret >> 7);
                ret ^= random_int_table[*(char *)(++key)];
    case(1):   
		return(ret);
    case(0):   
                return(0);
    default:
    {
      wn_assert(len > 8);

      for(;len > 8;len -= 8)
      {
	ret ^= (ret >> 5);
        ret ^= random_int_table[*(char *)(++key)];

	ret ^= (ret << 17);
        ret ^= random_int_table[*(char *)(++key)];

	ret ^= (ret >> 19);
        ret ^= random_int_table[*(char *)(++key)];

	ret ^= (ret << 11);
        ret ^= random_int_table[*(char *)(++key)];

	ret ^= (ret >> 13);
        ret ^= random_int_table[*(char *)(++key)];

        ret ^= (ret << 3);
        ret ^= random_int_table[*(char *)(++key)];

        ret ^= (ret >> 2);
        ret ^= random_int_table[*(char *)(++key)];

	ret ^= (ret << 7);
        ret ^= random_int_table[*(char *)(++key)];
      }

      goto small;
    }
  }
}



#if 0    /* wcn: old code 11/1/89 */
int wn_blkhash(key,len)

register ptr key;
int len;

{
  extern int wn_random_int_table[];
  register int sum,ret,mask;
  register int *random_int_table;

  sum = 0;
  ret = 0;
  mask = WN_POW2_RANDOM_INT_TABLE_SIZE-1;

  random_int_table = wn_random_int_table;

small:
  switch(len)
  {
    case(8):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(7):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(6):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(5):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(4):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(3):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(2):   
                sum += *(char *)(key++);
                sum &= mask;
                ret ^= random_int_table[sum];
    case(1):   
                sum += *(char *)key;
                sum &= mask;
                ret ^= random_int_table[sum];
    case(0):   
                return(ret);
    default:
    {
      wn_assert(len > 8);

      for(;len > 8;len -= 8)
      {
        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];

        sum += *(char *)(key++);
        sum &= mask;
        ret ^= random_int_table[sum];
      }

      goto small;
    }
  }
}
#endif



#if 0
int wn_blkhash(key,len)

register ptr key;
int len;

{
  extern int wn_random_int_table[];
  register ptr fin;
  register int ret,sum;

  ret = 0;
  sum = 0;

  fin = key+len;

  for(;key != fin;++key)
  {
    /*
    sum += *(char *)key;

    sum &= SUM_MASK;
    */
    ++sum;

    ret += wn_random_int_table[SUM_MASK & (sum + *(char *)key)];
  }

  return(ret);
}
#endif



#if 0
int wn_blkhash(key,len)

register ptr key;
int len;

{
  extern int wn_random_int_table[];
  register ptr fin;
  register int ret;

  ret = wn_random_int_table[0];

  fin = key+len;

  for(;key != fin;++key)
  {
    /*
    ret ^= wn_random_int_table[SUM_MASK & *(char *)key];
    */
    ret ^= (SUM_MASK & *(char *)key);

    ret ^= (ret << 2);
    ret ^= (ret >> 3);
  }

  return(ret);
}
#endif



wn_cumhasho(paccum,i)

int *paccum,i;

{
  struct
  {
    int i1,i2;
  } joint;

  joint.i1 = *paccum;
  joint.i2 = i;

  *paccum = wn_blkhash((ptr)&joint,wn_sizeof(joint));
}



#define MASK (0x3fffffff)

wn_cumhashu(paccum,i)

int *paccum,i;

{
  int tmp;

  tmp = ((*paccum)&MASK) + (i&MASK);
  *paccum = ((tmp)&MASK) + 0x2fc2ee1d;
}



