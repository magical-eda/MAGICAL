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
#ifndef wnddtrH
#define wnddtrH


#include "wnlib.h"
#include "wnmem.h"


WN_EXTERN_BEGIN

#define WN_DD_UNBOUNDED  1
#define WN_DD_INCLUSIVE  2
#define WN_DD_EXCLUSIVE  3


typedef struct wn_ddtree_struct *wn_ddtree;
typedef struct wn_ddhandle_struct *wn_ddhandle;

typedef int (*compare_keys_func_type)(ptr key1,ptr key2);
typedef void (*alloc_copy_key_func_type)(ptr *pkey,ptr key);
typedef void (*free_key_func_type)(ptr key);

struct wn_ddtree_struct
{
  wn_ddhandle handle_tree;

  int num_dimensions;

  compare_keys_func_type *compare_keys_func;
  alloc_copy_key_func_type *alloc_copy_key_func;
  free_key_func_type *free_key_func;

  wn_memgp group;
};

struct wn_ddhandle_struct
{
  ptr contents,*key;

  int split_dimension;

  wn_ddhandle left_child,right_child,parent;
};
  


EXTERN void wn_mkddtree
(
  wn_ddtree *ptree,
  int num_dimensions,
  compare_keys_func_type *compare_keys_func,
  alloc_copy_key_func_type *alloc_copy_key_func,
  free_key_func_type *free_key_func
);
EXTERN void wn_freeddtree(wn_ddtree tree);

EXTERN void wn_ddins
(
  wn_ddhandle *phandle,
  wn_ddtree tree,
  ptr key[]
);
EXTERN void wn_ddact
(
  wn_ddtree tree,
  void (*paction)(wn_ddhandle handle),
  ptr low_key[],
  int low_compare[],
  ptr high_key[],
  int high_compare[]
);
EXTERN int wn_ddcount(wn_ddtree tree);

WN_EXTERN_END

#endif
