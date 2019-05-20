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
#ifndef wnarypH
#define wnarypH

#include "wnlib.h"



EXTERN void wn_grow_arrayp
(
  ptr *parray,
  ptr *pnextptr,
  ptr *pfinptr,
  int blocksize
);


/*
     wn_arraypins(array,nextptr,finptr,e,type)
     type array[];
     type *nextptr;
     type *finptr;
     type e;
*/
#define wn_arraypins(_array,_nextptr,_finptr,_e,_type) \
{ \
  if((_nextptr) == (_finptr)) \
  { \
    wn_grow_arrayp((ptr *)(&(_array)),(ptr *)&(_nextptr),(ptr *)&(_finptr), \
		   sizeof(_type)); \
  } \
\
  *(_nextptr) = (_e); \
  ++(_nextptr); \
}


/*
     wn_arraypins_into_group(array,nextptr,finptr,e,group,type)
     type array[];
     type *nextptr;
     type *finptr;
     type e;
     wn_memgp group;
*/
#define wn_arraypins_into_group(_array,_nextptr,_finptr,_e,_group,_type) \
{ \
  if((_nextptr) == (_finptr)) \
  { \
    wn_gppush(_group); \
    wn_grow_arrayp((ptr *)(&(_array)),(ptr *)&(_nextptr),(ptr *)&(_finptr), \
		   sizeof(_type)); \
    wn_gppop(); \
  } \
\
  *(_nextptr) = (_e); \
  ++(_nextptr); \
}


#if 0
     wn_arraypdeloffset(array,nextptr,entry_offset,type)
     type array[];
     type *nextptr;
     int entry_offset;  /* offset of entry in bytes from array start */
#endif
#define wn_arraypdeloffset(_array,_nextptr,_entry_offset,_type) \
{ \
  --(_nextptr); \
  if(((_type *)(((char *)(_array)) + (_entry_offset))) != (_nextptr)) \
  { \
    *((_type *)(((char *)(_array)) + (_entry_offset))) = *(_nextptr); \
  } \
}


/*
     int wn_arrayp_entry_offset(array,nextptr)
     type array[];
     type *nextptr;
*/
#define wn_arrayp_entry_offset(_array,_nextptr) \
(\
  ((char *)(_nextptr)) - ((char *)(_array)) \
)


#if 0
     wn_arraypdelindex(array,nextptr,entry_index,type)
     type array[];
     type *nextptr;
     int entry_index;  /* index of entry from array start */
#endif
#define wn_arraypdelindex(_array,_nextptr,_entry_index,_type) \
{ \
  --(_nextptr); \
  if(((_array) + (_entry_index)) != (_nextptr)) \
  { \
    *((_array) + (_entry_index)) = *(_nextptr); \
  } \
}


#endif

