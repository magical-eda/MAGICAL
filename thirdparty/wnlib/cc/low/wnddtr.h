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


#include "wnmem.h"


#define WN_DD_UNBOUNDED  1
#define WN_DD_INCLUSIVE  2
#define WN_DD_EXCLUSIVE  3


typedef struct wn_ddtree_struct *wn_ddtree;
typedef struct wn_ddhandle_struct *wn_ddhandle;

struct wn_ddtree_struct
{
  wn_ddhandle handle_tree;

  int num_dimensions;

  int (**compare_keys_func)(/* key1,key2 */);
  void (**alloc_copy_key_func)(/* &key,key */);
  void (**free_key_func)(/* key */);

  wn_memgp group;
};

struct wn_ddhandle_struct
{
  ptr contents,*key;

  int split_dimension;

  wn_ddhandle left_child,right_child,parent;
};
  


#endif


