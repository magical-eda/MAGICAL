/*************************************************************************

  wn_blkzero(blk,len)
  wn_blkfill(blk,len,c)

  wn_blkcpy(out,in,len)
  wn_blkacpy(&out,in,len)

  wn_blkcat(out,b1,l1,b2,l2)
  wn_blkacat(&out,b1,l1,b2,l2)

  int wn_blkcmp(b1,l1,b2,l2)
  bool wn_blkeq(b1,l1,b2,l2)

  wn_blkshift(out,in,len)

  bool wn_char_in_block(c,b,l)

*************************************************************************/
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
#include "wnmax.h"
#include "wnary.h"



wn_blkzero(blk,len)

ptr blk;
int len;

{
  wn_blkfill(blk,len,'\0');
}



#if 0   /* slow -- replaced 9/8/88 -- do not delete */
wn_blkfill(blk,len,c)

ptr blk;
int len;
register char c;

{
  wn_arrayfill((char *)blk,len,c,char);
}
#endif



#if 1
#define blkifill4(blki)\
  *(blki++) = ci;
  
#define blkifill8(blki)\
  blkifill4(blki);\
  blkifill4(blki);
  
#define blkifill16(blki)\
  blkifill8(blki);\
  blkifill8(blki);
  
#define blkifill32(blki)\
  blkifill16(blki);\
  blkifill16(blki);
  
#define blkifill64(blki)\
  blkifill32(blki);\
  blkifill32(blki);
  
#define blkfill1(blk)\
  *(char *)(blk++) = c;
  
#define blkfill2(blk)\
  blkfill1(blk)\
  blkfill1(blk)

#define blkfill4(blk)\
  blkfill2(blk)\
  blkfill2(blk)

#define blkfill8(blk)\
  blkfill4(blk)\
  blkfill4(blk)

#define blkfill16(blk)\
  blkfill8(blk)\
  blkfill8(blk)

/*
  super-fast block fill.  Assumes int is 4 bytes.
*/
wn_blkfill(blk,len,c)

register ptr blk;
register int len;
register char c;

{
small:
  switch(len)
  {
    case(11):      *(char *)(blk++) = c;
    case(10):      *(char *)(blk++) = c;
    case(9):       *(char *)(blk++) = c;
    case(8):       *(char *)(blk++) = c;
    case(7):       *(char *)(blk++) = c;
    case(6):       *(char *)(blk++) = c;
    case(5):       *(char *)(blk++) = c;
    case(4):       *(char *)(blk++) = c;
    case(3):       *(char *)(blk++) = c;
    case(2):       *(char *)(blk++) = c;
    case(1):       *(char *)blk = c;
    case(0):       return;
    default:
    {
      wn_assert(len >= 12);

      if((((int)blk)&(4-1)) != 0)  /* not int aligned */
      {
        for(;len >= 24;len -= 24)
        {
          blkfill16(blk);
          blkfill8(blk);
        }
        if(len >= 12)
        {
          blkfill8(blk);
          blkfill4(blk);
  
          len -= 12;
        }
  
        goto small;
      } 
      else                 /* int aligned */
      {
        register int ci;
        register int *blki;

        blkfill4(blk);
    
        blki = (int *)blk;
        ci = *(blki-1);

smalli:
        switch(len)
        {
          case(51):
          case(50):
          case(49):
          case(48):    *(blki++) = ci;
          case(47):
          case(46):
          case(45):
          case(44):    *(blki++) = ci;
          case(43):
          case(42):
          case(41):
          case(40):    *(blki++) = ci;
          case(39):
          case(38):
          case(37):
          case(36):    *(blki++) = ci;
          case(35):
          case(34):
          case(33):
          case(32):    *(blki++) = ci;
          case(31):
          case(30):
          case(29):
          case(28):    *(blki++) = ci;
          case(27):
          case(26):
          case(25):
          case(24):    *(blki++) = ci;
          case(23):
          case(22):
          case(21):
          case(20):    *(blki++) = ci;
          case(19):
          case(18):
          case(17):
          case(16):    *(blki++) = ci;
          case(15):
          case(14):
          case(13):
          case(12):    *(blki++) = ci;
          case(11):
          case(10):
          case(9):
          case(8):     *(blki++) = ci;
          case(7):
          case(6):
          case(5):
          case(4):     /* the beginning blkfill4 does this one */

          /* should be none <= 4 */
                       blk = (ptr)blki;
                       len &= (4-1);     
                       goto small;
          default:
          {
            for(;len >= (96+4);len -= 96)
            {
              blkifill64(blki);
              blkifill32(blki);
            }
            if(len >= (48+4))
            {
              blkifill32(blki);
              blkifill16(blki);
      
              len -= 48;
            }
      
            goto smalli;
          }
        }
      }
    }
  }
}
#endif
  
  

wn_blkacpy(pout,in,len)

ptr *pout,in;
int len;

{
  *pout = wn_alloc(len);
  wn_blkcpy(*pout,in,len);
}



wn_blkcpy(out,in,len)

register ptr out,in;
register int len;

{
small:
  switch(len)
  {
    case(15):  *(char *)(out++) = *(char *)(in++);
    case(14):  *(char *)(out++) = *(char *)(in++);
    case(13):  *(char *)(out++) = *(char *)(in++);
    case(12):  *(char *)(out++) = *(char *)(in++);
    case(11):  *(char *)(out++) = *(char *)(in++);
    case(10):  *(char *)(out++) = *(char *)(in++);
    case(9):   *(char *)(out++) = *(char *)(in++);
    case(8):   *(char *)(out++) = *(char *)(in++);
    case(7):   *(char *)(out++) = *(char *)(in++);
    case(6):   *(char *)(out++) = *(char *)(in++);
    case(5):   *(char *)(out++) = *(char *)(in++);
    case(4):   *(char *)(out++) = *(char *)(in++);
    case(3):   *(char *)(out++) = *(char *)(in++);
    case(2):   *(char *)(out++) = *(char *)(in++);
    case(1):   *(char *)out = *(char *)in;
    case(0):   return;
    default:
    {
      wn_assert(len > 0);

      for(;len >= 16;len -= 16)
      {
        /* 16 repeats */

        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);

        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);

        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);

        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
        *(char *)(out++) = *(char *)(in++);
      }

      goto small;
    }
  }
}



wn_blkacat(pout,s1,l1,s2,l2)

ptr *pout,s1,s2;
int l1,l2;

{
  *pout = wn_alloc(l1+l2);
  wn_blkcat(*pout,s1,l1,s2,l2);
}



wn_blkcat(out,s1,l1,s2,l2)

ptr out,s1,s2;
int l1,l2;

{
  wn_blkcpy(out,s1,l1);
  wn_blkcpy(out+l1,s2,l2);
}



int wn_blkcmp(b1,l1,b2,l2)

ptr b1,b2;
int l1,l2;

{
  int i,min_len,ret;

  min_len = wn_min(l1,l2);

  for(i=0;i<min_len;++i)
  {
    ret = ((unsigned)((char *)b1)[i]) - ((unsigned)((char *)b2)[i]);

    if(ret != 0)
    {
      return(ret);
    }
  }

  return(l1 - l2);
}



#if 0  /* deleted 10/12/88 */
bool wn_blkeq(b1,l1,b2,l2)

ptr b1,b2;
int l1,l2;

{
  return(wn_blkcmp(b1,l1,b2,l2) == 0);
}
#endif



bool wn_blkeq(b1,l1,b2,l2)

register ptr b1,b2;
int l1,l2;

{
  register ptr fin;

  if(l1 != l2)
  {
    return(FALSE);
  }

  fin = b1+l1;

  while(b1 != fin)
  {
    if((*(char *)b1) != (*(char *)b2))
    {
      return(FALSE);
    }

    ++b1; ++b2;
  }

  return(TRUE);
}



wn_blkshift(out,in,len)

ptr out,in;
int len;

{
  int amount;
  register char *to,*from,*fin;

  wn_assert(len >= 0);
  amount = out-in;

  if(amount > 0)
  {
    from = (char *)(in+len);
    to = (char *)(out+len);
    fin = (char *)in;

    while(from != fin)
    {
      *(--to) = *(--from);
    }
  }
  else if(amount == 0)
  {
    ;     /* do nothing, no shift requested */
  }
  else /* amount < 0 */
  {
    from = (char *)in;
    to = (char *)out;
    fin = from+len;

    while(from != fin)
    {
      *(to++) = *(from++);
    }
  }
}



bool wn_char_in_block(c,b,l)

register char c;
register ptr b;
int l;

{
  register ptr fin;

  fin = b+l;

  for(;b != fin;++b)
  {
    if(c == *(char *)b)
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


