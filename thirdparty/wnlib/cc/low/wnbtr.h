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
#ifndef wnbtrH
#define wnbtrH


#include "wnmem.h"


#define  WN_B_MIN    (-3)
#define  WN_B_MAX    3
#define  WN_B_LT     (-2)
#define  WN_B_GT     2
#define  WN_B_LE     (-1)
#define  WN_B_GE     1
#define  WN_B_EQ     0


typedef struct wn_btree_struct *wn_btree;
typedef struct wn_bhandle_struct *wn_bhandle;

struct wn_btree_struct
{
  wn_bhandle handle_tree;

  int (*pcompare_keys_func)(/* key1,key2 */);
  void (*palloc_copy_key_func)(/* &key,key */);
  void (*pfree_key_func)(/* key */);

  wn_memgp group;
};

struct wn_bhandle_struct
{
  ptr contents,key;

  int level,count;

  wn_bhandle left_child,right_child,parent;
};
  


#endif


