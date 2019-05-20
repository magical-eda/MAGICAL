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
#ifndef wnprmH
#define wnprmH


#define wn_permute_array(_result,_permutation,_array,_size)\
{\
  int _i,_size_copy;\
  \
  _size_copy = (_size);\
  \
  wn_assertmsg((_result) != (_array),\
               "wn_permute_array: result == array");\
  wn_assertmsg((int *)(_result) != (_permutation),\
               "wn_permute_array: result == permutation");\
  \
  for(_i=0;_i<_size_copy;_i++)\
  {\
    (_result)[_i] = (_array)[(_permutation)[_i]];\
  }\
}


#endif

