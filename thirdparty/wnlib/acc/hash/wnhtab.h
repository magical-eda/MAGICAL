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


#include "wnlib.h"
#include "wnmem.h"


typedef struct wn_htab_struct *wn_htab;
typedef struct wn_binary_tree_data_struct *wn_binary_tree_data;

struct wn_htab_struct
{
  wn_binary_tree_data binary_tree;

  int (*phash_func)(ptr key);
  bool (*pkeys_eq_func)(ptr key1,ptr key2);
  void (*palloc_copy_func)(ptr *pkey,ptr key);
  void (*pfree_func)(ptr key);

  wn_memgp group;
};

struct wn_binary_tree_data_struct
{
  wn_binary_tree_data children[2];
  int hashed_key;
  ptr data,key;
};
  

EXTERN void wn_mkhtab
(
  wn_htab *ptable,
  int (*phash_func)(ptr key),
  bool (*pkeys_eq_func)(ptr key1,ptr key2),
  void (*palloc_copy_func)(ptr *pkey,ptr key),
  void (*pfree_func)(ptr key)
);
EXTERN void wn_freehtab(wn_htab table);

EXTERN bool wn_hins(ptr data,wn_htab table,ptr key);
EXTERN bool wn_hfins(ptr data,wn_htab table,ptr key);
EXTERN bool wn_hget(ptr *pdata,wn_htab table,ptr key);
EXTERN bool wn_hdel(wn_htab table,ptr key);

EXTERN int wn_hcount(wn_htab table);
EXTERN void wn_hact(wn_htab table,void (*paction)(ptr data,ptr key));

EXTERN void wn_hverify(wn_htab table);
EXTERN void wn_hanalyze(wn_htab table);

#endif



