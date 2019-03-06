/*************************************************************************

  wn_load_map_array(map_array,from,to)

  wn_strmap(s,map_array)
  wn_memmap(mem,len,map_array)

  wn_strcharmap(s,from_c,to_c)

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
#include "wnasrt.h"

#include "wnmap.h"



#define MASK  (255)    /* make char positive */



void wn_strcharmap(register char *s,register char from_c,register char to_c)
{
  register int c;

  for(;;)
  {
    c = *s;

    if(c == '\0')
    {
      break;
    }
    else if(c == from_c)
    {
      *s = to_c;
    }

    ++s;
  }
}


void wn_strmap(register char *s,register char map_array[256])
{
  register char c;

  for(;;)
  {
    c = *s;

    if(c == '\0')
    {
      break;
    }

    *s = map_array[c&MASK]; 

    ++s;
  }
}


void wn_memmap(register char *string,int len,register char map_array[256])
{
  register char *fin;

  fin = string+len;

  for(;string != fin;++string)
  {
    *string = map_array[(*string)&MASK];
  }
}


local void identity_map_array(register char map_array[256])
{
  register int c;

  for(c=0;c<256;++c)
  {
    map_array[c] = (char)c;
  }
}


void wn_load_map_array(register char map_array[256],
		       register char *from,register char *to)
{
  identity_map_array(map_array);

  for(;;)
  {
    if(*from == '\0')
    {
      wn_assert(*to == '\0');

      break;
    }

    map_array[(*from)&MASK] = *to;

    ++from;  ++to;
  }
}





