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
#ifndef wnaryH
#define wnaryH

#include "wnlib.h"


void wn_grow_array(ptr *parray,int *pmemsize,int blocksize);
void wn_grow_2array(ptr *parray1,ptr *parray2,int *pmemsize,
                    int blocksize1,int blocksize2);


/*
wn_arrayins(array,usedsize,memsize,e,type)
type array[];
int usedsize;
int memsize;
type e;
*/
#define wn_arrayins(_array,_usedsize,_memsize,_e,_type) \
{ \
  if((_usedsize) == (_memsize)) \
  { \
    wn_grow_array((ptr *)(&(_array)),&(_memsize),sizeof(_type)); \
  } \
\
  (_array)[_usedsize] = (_e); \
  ++(_usedsize); \
}


/*
wn_arrayins_into_group(array,usedsize,memsize,e,group,type)
type array[];
int usedsize;
int memsize;
type e;
wn_memgp group;
*/
#define wn_arrayins_into_group(_array,_usedsize,_memsize,_e,_group,_type) \
{ \
  if((_usedsize) == (_memsize)) \
  { \
    wn_gppush(_group); \
    wn_grow_array((ptr *)(&(_array)),&(_memsize),sizeof(_type)); \
    wn_gppop(); \
  } \
\
  (_array)[_usedsize] = (_e); \
  ++(_usedsize); \
}


/*
wn_arraydelindex(array,usedsize,index)
type array[];
int usedsize;
int index;
*/
#define wn_arraydelindex(_array,_usedsize,_index) \
{ \
  --(_usedsize); \
  if((_index) < (_usedsize)) \
  { \
    (_array)[_index] = (_array)[_usedsize]; \
  } \
}


/*
wn_for_all_array_entries(array,usedsize,e,type)
type array[];
int usedsize;
type e;
{
} 
wn_end_for_all_array_entries
*/
#define wn_for_all_array_entries(_array,_usedsize,_e,_type) \
{ \
  _type *_pe,*_plimit; \
\
  _plimit = (_array) + (_usedsize); \
\
  for(_pe = (_array);_pe != _plimit;++_pe) \
  { \
    (_e) = *_pe; 


/*
wn_for_all_array_pentries(array,usedsize,pe,type)
type array[];
int usedsize;
type *pe;
{
} 
wn_end_for_all_array_entries
*/
#define wn_for_all_array_pentries(_array,_usedsize,_passed_pe,_type) \
{ \
  _type *_pe,*_plimit; \
\
  _plimit = (_array) + (_usedsize); \
\
  for(_pe = (_array);_pe != _plimit;++_pe) \
  { \
    (_passed_pe) = _pe; 


#define wn_end_for_all_array_entries }}


/*
wn_2arrayins(array1,array2,usedsize,memsize,e1,e2,type1,type2)
type1 array1[];
type2 array2[];
int usedsize;
int memsize;
type1 e1;
type2 e2;
*/
#define wn_2arrayins(_array1,_array2,_usedsize,_memsize,_e1,_e2,_type1,_type2) \
{ \
  if((_usedsize) == (_memsize)) \
  { \
    wn_grow_2array((ptr *)(&(_array1)),(ptr *)(&(_array2)), \
                   &(_memsize), \
                   sizeof(_type1),sizeof(_type2)); \
  } \
\
  (_array1)[_usedsize] = (_e1); \
  (_array2)[_usedsize] = (_e2); \
  ++(_usedsize); \
}


/*
wn_2arrayins_into_group(array1,array2,usedsize,memsize,e1,e2,
                        group,type1,type2)
type1 array1[];
type2 array2[];
int usedsize;
int memsize;
type1 e1;
type2 e2;
wn_memgp group;
*/
#define wn_2arrayins_into_group(_array1,_array2,_usedsize,_memsize,_e1,_e2, \
                                _group,_type1,_type2) \
{ \
  if((_usedsize) == (_memsize)) \
  { \
    wn_gppush(_group); \
    wn_grow_2array((ptr *)(&(_array1)),(ptr *)(&(_array2)), \
                   &(_memsize), \
                   sizeof(_type1),sizeof(_type2)); \
    wn_gppop(); \
  } \
\
  (_array1)[_usedsize] = (_e1); \
  (_array2)[_usedsize] = (_e2); \
  ++(_usedsize); \
}


/*
wn_2arraydelindex(array1,array2,usedsize,index)
type1 array1[];
type2 array2[];
int usedsize;
int index;
*/
#define wn_2arraydelindex(_array1,_array2,_usedsize,_index) \
{ \
  --(_usedsize); \
  if((_index) < (_usedsize)) \
  { \
    (_array1)[_index] = (_array1)[_usedsize]; \
    (_array2)[_index] = (_array2)[_usedsize]; \
  } \
}


#endif

