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
#ifndef wnhtabH
#define wnhtabH


#include "wnset.h"
#include "wnmem.h"


typedef struct wn_htab_struct *wn_htab;
typedef struct wn_binary_tree_data_struct *wn_binary_tree_data;

struct wn_htab_struct
{
  wn_binary_tree_data binary_tree;

  int (*phash_func)(/* key */);
  bool (*pkeys_eq_func)(/* key1,key2 */);
  void (*palloc_copy_func)(/* pkey,key */);
  void (*pfree_func)(/* key */);

  wn_memgp group;
};

struct wn_binary_tree_data_struct
{
  wn_binary_tree_data children[2];
  int hashed_key;
  ptr data,key;
};
  


extern bool wn_hget_routine(),
            wn_hins_routine(),wn_hfins_routine(),
            wn_hdel_routine();

#define wn_hget(_pdata,_table,_key) \
               wn_hget_routine((ptr *)(_pdata),(_table),(ptr)(_key))
#define wn_hins(_data,_table,_key) \
               wn_hins_routine((ptr)(_data),(_table),(ptr)(_key))
#define wn_hfins(_data,_table,_key) \
               wn_hfins_routine((ptr)(_data),(_table),(ptr)(_key))
#define wn_hdel(_table,_key) \
               wn_hdel_routine((_table),(ptr)(_key))



#endif


