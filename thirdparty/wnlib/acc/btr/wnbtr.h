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


#include "wnlib.h"
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

  int (*pcompare_keys_func)(ptr key1,ptr key2);
  void (*palloc_copy_key_func)(ptr *pkey,ptr key);
  void (*pfree_key_func)(ptr key);

  wn_memgp group;
};

struct wn_bhandle_struct
{
  ptr contents,key;

  int level,count;

  wn_bhandle left_child,right_child,parent;
};
  

EXTERN void wn_mkbtree
(
  wn_btree *ptree,
  int (*pcompare_keys_func)(ptr key1,ptr key2),
  void (*palloc_copy_key_func)(ptr *pkey,ptr key),
  void (*pfree_key_func)(ptr key)
);
EXTERN void wn_freebtree(wn_btree tree);

EXTERN void wn_bget(wn_bhandle *phandle,wn_btree tree,ptr key,int compare);
EXTERN void wn_bins(wn_bhandle *phandle,wn_btree tree,ptr key);
EXTERN void wn_bdel(wn_bhandle handle,wn_btree tree);
EXTERN void wn_bmove(wn_bhandle handle,wn_btree tree,ptr new_key);
EXTERN void wn_bget_index_of_handle(int *pindex,wn_btree tree,wn_bhandle handle);
EXTERN void wn_bget_handle_of_index(wn_bhandle *phandle,wn_btree tree,int index);

EXTERN int wn_bcount(wn_btree tree);
EXTERN void wn_bact
(
  wn_btree tree,
  void (*paction)(wn_bhandle handle),
  ptr low_key,
  int low_compare,
  ptr high_key,
  int high_compare
);

EXTERN void wn_bverify(wn_btree tree);

/* ---------------------------------------------------------------- */
/* tree traversal, forward */

extern wn_bhandle _wn_for_btree_get_first(wn_btree   tree);
extern void       _wn_for_btree_get_next( wn_bhandle *phandle);

#define WN_FOR_BTREE(_vtype, _val, _tree, _ktype, _key) {   		\
    _vtype _val;							\
    _ktype _key;							\
    wn_bhandle _wn_for_btree_handle;					\
									\
    wn_fast_assert(sizeof(_val) == sizeof(ptr));			\
    wn_fast_assert(sizeof(_key) == sizeof(ptr));			\
									\
    for (_wn_for_btree_handle = _wn_for_btree_get_first(_tree);		\
    /**/		_wn_for_btree_handle;				\
    /**/		_wn_for_btree_get_next(&_wn_for_btree_handle)) {\
      _val = (_vtype) _wn_for_btree_handle->contents;			\
      _key = (_ktype) _wn_for_btree_handle->key;			\
      {

#define WN_END_FOR_BTREE()						\
      }									\
    }									\
									\
    /* just checking we're matched with a WN_FOR_BTREE */		\
    wn_fast_assert(TRUE ||						\
    /**/	(wn_bhandle) NULL == _wn_for_btree_handle);		\
  }

/* ---------------------------------------------------------------- */
/* tree traversal, backward */

extern wn_bhandle _wn_for_btree_get_last( wn_btree   tree);
extern void       _wn_for_btree_get_prev( wn_bhandle *phandle);

#define WN_FOR_BTREE_REVERSE(_vtype, _val, _tree, _ktype, _key) {	\
    _vtype _val;							\
    _ktype _key;							\
    wn_bhandle _wn_for_btree_handle_r;					\
									\
    wn_fast_assert(sizeof(_val) == sizeof(ptr));			\
    wn_fast_assert(sizeof(_key) == sizeof(ptr));			\
									\
    for (_wn_for_btree_handle_r = _wn_for_btree_get_last(_tree);	\
    /**/		_wn_for_btree_handle_r;				\
    /**/		_wn_for_btree_get_prev(&_wn_for_btree_handle_r)) {\
      _val = (_vtype) _wn_for_btree_handle_r->contents;			\
      _key = (_ktype) _wn_for_btree_handle_r->key;			\
      {

#define WN_END_FOR_BTREE_REVERSE()					\
      }									\
    }									\
									\
    /* just checking we're matched with a WN_FOR_BTREE_REVERSE */	\
    wn_fast_assert(TRUE ||						\
    /**/		(wn_bhandle) NULL == _wn_for_btree_handle_r);	\
  }

#endif
