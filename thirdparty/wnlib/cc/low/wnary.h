/*************************************************************************

  wn_arrayfill(array,len,fill,type)

  wn_arraycpy(out,in,len,type)

  wn_arrayscan(&index,array,len,cond,type)

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



#define wn_arrayfill(_out,_len,_fill,_type) \
{\
  register _type *_out_copy;\
  register _type *_fin;\
  \
  _out_copy = (_out);\
  _fin = _out_copy+(_len);\
  \
  for(;_out_copy != _fin;_out_copy++)\
  {\
    *_out_copy = (_fill);\
  }\
}



#define wn_arraycpy(_out,_in,_len,_type)\
{\
  register _type *_out_copy;\
  register _type *_in_copy;\
  register _type *_fin;\
  \
  _in_copy = (_in);\
  _out_copy = (_out);\
  \
  _fin = _in_copy+(_len);\
  \
  for(;_in_copy != _fin;_out_copy++,_in_copy++)\
  {\
    *_out_copy = *_in_copy;\
  }\
}



#define wn_element  (*_array_copy)

#define wn_arrayscan(_pindex,_array,_len,_cond,_type) \
{\
  register _type *_array_copy;\
  register _type *_fin;\
  _type *_start;\
  \
  _start = _array_copy = (_array);\
  _fin = _array_copy+(_len);\
  \
  *(_pindex) = -1;\
  \
  for(;_array_copy != _fin;_array_copy++)\
  {\
    if(_cond)\
    {\
      *(_pindex) = (_array_copy-_start);\
      \
      break;\
    }\
  }\
}




