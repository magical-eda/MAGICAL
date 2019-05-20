/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Bill Chapman

****************************************************************************/
#ifndef wnsklistH
#define wnsklistH


#include "wnlib.h"
#include "wnmem.h"


#define  WN_SK_MIN    0
#define  WN_SK_MAX    1
#define  WN_SK_LT     2
#define  WN_SK_GT     3
#define  WN_SK_LE     4
#define  WN_SK_GE     5
#define  WN_SK_EQ     6


typedef struct wn_skhandle_struct *wn_skhandle;
typedef struct wn_sklist_struct *wn_sklist;

struct wn_skhandle_struct
{
  ptr contents;
  ptr key;
  wn_skhandle nexts;	/* really the beginning of an array */
};

/* macro to provide ptr to array of next ptrs following skip handle */
#define WN_SKIP_NEXTS(handle)  (&(handle)->nexts)

#define WN_SKHANDLE_SIZE(ptr_count) \
	(sizeof(struct wn_skhandle_struct) + \
				sizeof(wn_skhandle)*((ptr_count)-1))


struct wn_sklist_struct
{
  int threshold;	/*   probability, in 64th's, that we'll go up a
  **			** level of ptrs */
  int max_levels;	/* 1000 for now */

  int (*pcompare_keys_func)(ptr key1,ptr key2);
  void (*palloc_copy_key_func)(ptr *pkey,ptr key);
  void (*pfree_key_func)(ptr key);

  wn_skhandle handle;	/* headnode */
  int ptr_count;	/* ptrs in headnode */

  wn_memgp group;
};


EXTERN void wn_mksklist
(
    wn_sklist *psklist,
    double threshold,	/* this will be rounded to an appropriate fraction
    **			** of 64ths */
    int (*pcompare_keys_func)(ptr key1,ptr key2),
    void (*palloc_copy_key_func)(ptr *pkey,ptr key),
    void (*pfree_key_func)(ptr key)
);

EXTERN void wn_freesklist(wn_sklist sklist);

EXTERN void wn_skget(wn_skhandle *phandle, wn_sklist sklist, ptr key, int compare);
EXTERN void wn_skins(wn_skhandle *phandle, wn_sklist sklist, ptr key);
EXTERN void wn_skdel(wn_skhandle handle, wn_sklist sklist);
EXTERN void wn_skmove(wn_skhandle handle, wn_sklist sklist, ptr new_key);
#if 0  /* not included because not possible to implement efficiently */
EXTERN void wn_skget_index_of_handle(int *pindex, wn_sklist sklist,
/**/							wn_skhandle handle);
EXTERN void wn_skget_handle_of_index(wn_skhandle *phandle, wn_sklist sklist,
/**/								int index);
#endif

int wn_skcount(wn_sklist sklist);

EXTERN void wn_skact
(
  wn_sklist sklist,
  void (*paction)(wn_skhandle handle),
  ptr low_key,
  int low_compare,
  ptr high_key,
  int high_compare
);

EXTERN void wn_skverify(wn_sklist sklist);

#define WN_FOR_SKL(_vtype, _val, _list, _ktype, _key) {			\
    _vtype _val;							\
    _ktype _key;							\
    wn_skhandle _wn_for_skl_handle;					\
									\
    /* just checking types */						\
    wn_fast_assert(TRUE  ||  (wn_sklist) NULL == _list);		\
    wn_fast_assert(sizeof(_val) == sizeof(ptr));			\
    wn_fast_assert(sizeof(_key) == sizeof(ptr));			\
									\
    for (_wn_for_skl_handle = WN_SKIP_NEXTS((_list)->handle)[0];	\
    /**/					_wn_for_skl_handle;	\
    /**/	_wn_for_skl_handle = WN_SKIP_NEXTS(_wn_for_skl_handle)[0]) {\
      _val = (_vtype) _wn_for_skl_handle->contents;			\
      _key = (_ktype) _wn_for_skl_handle->key;				\
      {

#define WN_END_FOR_SKL()						\
      }									\
    }									\
									\
    /* just checking we're matched with a WN_FOR_SKL */			\
    wn_fast_assert(TRUE ||						\
    /**/		(wn_skhandle) NULL == _wn_for_skl_handle);	\
  }

#endif
