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
#include "wnasrt.h"

#include "wnhash.h"



#if 0
/* 
  This hashing method was invented by Jay Adams in March 1997.

  jay 1-long xor-shift hash 
*/
int wn_memhash(ptr key,int len)
{
  register unsigned int ret;
  register char *ckey;

  ckey = (char *)key;

  ret = ckey[0]^1892366639;

small:
  switch(len)
  {
    case(8):   
		ret = ckey[7]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(7):   
		ret = ckey[6]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(6):   
		ret = ckey[5]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(5):   
		ret = ckey[4]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(4):   
		ret = ckey[3]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(3):   
		ret = ckey[2]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(2):   
		ret = ckey[1]^(ret<<9)^(ret>>7)^(ret&0x6e);
    case(1):   
		return(ret);
    case(0):   
                return(0);
    default:
    {
      wn_assert(len > 8);

      for(;len > 8;len -= 8)
      {
	ret = ckey[8]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[7]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[6]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[5]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[4]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[3]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[2]^(ret<<9)^(ret>>7)^(ret&0x6e);
	ret = ckey[1]^(ret<<9)^(ret>>7)^(ret&0x6e);

	ckey += 8;
      }

      goto small;
    }
  }
}
#endif


#if 0
int wn_memhash(ptr key,int len)
{
  extern int wn_random_int_table[];
  register int ret;
  register int *random_int_table;
  register char *ckey;

  ckey = (char *)key;

  random_int_table = wn_random_int_table + 260;

  ret = random_int_table[*(char *)key];

small:
  switch(len)
  {
    case(8):   
		ret ^= (ret >> 5);
                ret ^= random_int_table[*(++ckey)];
    case(7):   
		ret ^= (ret << 17);
                ret ^= random_int_table[*(++ckey)];
    case(6):   
		ret ^= (ret >> 19);
                ret ^= random_int_table[*(++ckey)];
    case(5):   
		ret ^= (ret << 11);
                ret ^= random_int_table[*(++ckey)];
    case(4):   
		ret ^= (ret >> 13);
                ret ^= random_int_table[*(++ckey)];
    case(3):   
		ret ^= (ret << 3);
                ret ^= random_int_table[*(++ckey)];
    case(2):   
		ret ^= (ret >> 7);
                ret ^= random_int_table[*(++ckey)];
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
        ret ^= random_int_table[*(++ckey)];

	ret ^= (ret << 17);
        ret ^= random_int_table[*(++ckey)];

	ret ^= (ret >> 19);
        ret ^= random_int_table[*(++ckey)];

	ret ^= (ret << 11);
        ret ^= random_int_table[*(++ckey)];

	ret ^= (ret >> 13);
        ret ^= random_int_table[*(++ckey)];

        ret ^= (ret << 3);
        ret ^= random_int_table[*(++ckey)];

        ret ^= (ret >> 2);
        ret ^= random_int_table[*(++ckey)];

	ret ^= (ret << 7);
        ret ^= random_int_table[*(++ckey)];
      }

      goto small;
    }
  }
}
#endif


#if 1
int wn_memhash(register ptr key,int len)
{
  switch(len)
  {
    case(4):   
		return((
			 (wn_random_int_table+128      )[((char *)key)[0]]
			   ^
		         (wn_random_int_table+128+1*256)[((char *)key)[1]]
		       )
		  	 +
		       (
		         (wn_random_int_table+128+2*256)[((char *)key)[2]]
			   ^
		         (wn_random_int_table+128+3*256)[((char *)key)[3]]
		       )
		      );
    case(3):   
		return((wn_random_int_table+128      )[((char *)key)[0]]^
		       (wn_random_int_table+128+1*256)[((char *)key)[1]]^
		       (wn_random_int_table+128+2*256)[((char *)key)[2]]
		      );
    case(2):   
		return((wn_random_int_table+128      )[((char *)key)[0]]^
		       (wn_random_int_table+128+1*256)[((char *)key)[1]]
		      );
    case(1):   
		return((wn_random_int_table+128)[((char *)key)[0]]);
    case(0):
		return(0);
    default:
    {
      register unsigned int ret;

      wn_assert(len > 4);

      ret = 
		       (
			 (wn_random_int_table+128      )[((char *)key)[0]]
			   ^
		         (wn_random_int_table+128+1*256)[((char *)key)[1]]
		       )
		  	 +
		       (
		         (wn_random_int_table+128+2*256)[((char *)key)[2]]
			   ^
		         (wn_random_int_table+128+3*256)[((char *)key)[3]]
		       );
      ret = (ret << 1)^(ret >> 1)^(ret&9);
      len -= 4;
      key = ((char *)key)+4;

      for(;len > 4;len -= 4)
      {
        ret ^= 
		       (
			 (wn_random_int_table+128      )[((char *)key)[0]]
			   ^
		         (wn_random_int_table+128+1*256)[((char *)key)[1]]
		       )
		  	 +
		       (
		         (wn_random_int_table+128+2*256)[((char *)key)[2]]
			   ^
		         (wn_random_int_table+128+3*256)[((char *)key)[3]]
		       );
        ret = (ret << 1)^(ret >> 1)^(ret&9);
        key = ((char *)key)+4;
      }

      return(ret^wn_memhash(key,len));
    }
  }
}
#endif


#if 0
int wn_memhash(ptr key,int len)
{
  register unsigned int ret;
  register char *ckey;

  ckey = (char *)key;

  ret = ckey[0]^1892366639;

small:
  switch(len)
  {
    case(8):   
		ret ^= ckey[7]^(ret<<16)^(ret>>14);
    case(7):   
		ret ^= ckey[6]^(ret<<15)^(ret>>11);
    case(6):   
		ret ^= ckey[5]^(ret<<10)^((ret&(-1756261384))>>8);
    case(5):   
		ret ^= ckey[4]^(ret<<12)^(ret>>11);
    /*
    case(4):   
		ret ^= ckey[3]^(ret<<16)^(ret>>14);
    case(3):   
		ret ^= ckey[2]^(ret<<15)^(ret>>11);
    case(2):   
		ret ^= ckey[1]^(ret<<10)^((ret&(-1756261384))>>8);
    */
    /*
    case(4):   
		ret ^= ckey[3]^(ret<< 8)^(ret>>10);
    case(3):   
		ret ^= ckey[2]^(ret<<12)^(ret>>11);
    case(2):   
		ret ^= ckey[1]^(ret<<10)^(ret>>14);
    */
    case(4):   
		ret ^= ckey[3]^(ret<< 8)^(ret>>10);
    case(3):   
		ret ^= ckey[2]^(ret<<12)^(ret>>11);
    case(2):   
		ret ^= ckey[1]^(ret<<10)^(ret>>14);
    case(1):   
		return(ret);
    case(0):   
                return(0);
    default:
    {
      wn_assert(len > 8);

      for(;len > 8;len -= 8)
      {
	ret ^= ckey[8]^(ret<<16)^(ret>>14);
	ret ^= ckey[7]^(ret<<15)^(ret>>11);
	ret ^= ckey[6]^(ret<<10)^((ret&(-1756261384))>>8);
	ret ^= ckey[5]^(ret<<12)^(ret>>11);
	ret ^= ckey[4]^(ret<<16)^(ret>>14);
	ret ^= ckey[3]^(ret<<15)^(ret>>11);
	ret ^= ckey[2]^(ret<<10)^((ret&(-1756261384))>>8);
	ret ^= ckey[1]^(ret<<12)^(ret>>11);

	ckey += 8;
      }

      goto small;
    }
  }
}
#endif


#if 0
int wn_memhash(ptr key,int len)
{
  register unsigned int ret;
  register char *ckey;

  ckey = (char *)key;

  ret = ckey[0]^1892366639;

small:
  switch(len)
  {
    case(8):   
		ret ^= ckey[7]^(ret<< 9)^(ret>>15);
    case(7):   
		ret ^= ckey[6]^(ret<<12)^(ret>> 9);
    case(6):   
		ret ^= ckey[5]^(ret<<14)^(ret>>16);
    case(5):   
		ret ^= ckey[4]^(ret<<14)^(ret>> 9);
    case(4):   
		ret ^= ckey[3]^(ret<< 8)^(ret>>10);
    case(3):   
		ret ^= ckey[2]^(ret<<12)^(ret>>11);
    case(2):   
		ret ^= ckey[1]^(ret<<10)^(ret>>14);
    case(1):   
		return(ret);
    case(0):   
                return(0);
    default:
    {
      wn_assert(len > 8);

      for(;len > 8;len -= 8)
      {
	ret ^= ckey[8]^(ret<< 9)^(ret>>15);
	ret ^= ckey[7]^(ret<<12)^(ret>> 9);
	ret ^= ckey[6]^(ret<<14)^(ret>>16);
	ret ^= ckey[5]^(ret<<14)^(ret>> 9);
	ret ^= ckey[4]^(ret<< 8)^(ret>>10);
	ret ^= ckey[3]^(ret<<12)^(ret>>11);
	ret ^= ckey[2]^(ret<<10)^(ret>>14);
	ret ^= ckey[1]^(ret<<14)^((ret&1797530587)>>12);

	ckey += 8;
      }

      goto small;
    }
  }
}
#endif




