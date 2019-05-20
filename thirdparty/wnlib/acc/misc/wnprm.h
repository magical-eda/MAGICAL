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
  wn_assert((_result) != (_array));\
  wn_assert((int *)(_result) != (_permutation));\
  \
  for(_i=0;_i<_size_copy;_i++)\
  {\
    (_result)[_i] = (_array)[(_permutation)[_i]];\
  }\
}


EXTERN void wn_random_permutation(int permutation[],int size);
EXTERN void wn_identity_permutation(int permutation[],int size);
EXTERN void wn_permute_permutation(int result[],int perm1[],int perm2[],int size);
EXTERN void wn_invert_permutation(int inverse[],int permutation[],int size);
EXTERN bool wn_is_valid_permutation(int permutation[],int size);


#endif

