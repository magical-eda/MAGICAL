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

****************************************************************************

    This module is intended to be called by modules wnmbtr.c and wnmemg.c
  for a specific purpose.  Using it for anything else could cause problems.
  Ideally, this .h file should not be published outside of the mem
  directory.

****************************************************************************/
#ifndef wnmbtrH
#define wnmbtrH


#if !defined(wnmbtrC) && !defined(wnmemgC) && !defined(wnmbtsC)
# error wnmbtr.h included by file other than wnmbtr.c, wnmemg.c, and wnmbts.c
#endif


#include "wnlib.h"
#include "wnmem.h"


#define WN_MB_MIN    (-3)
#define WN_MB_MAX    3
#define WN_MB_LT     (-2)
#define WN_MB_GT     2
#define WN_MB_LE     (-1)
#define WN_MB_GE     1
#define WN_MB_EQ     0


typedef struct wn_mbtree_struct *wn_mbtree;
typedef struct wn_mbhandle_struct *wn_mbhandle;
typedef struct wn_free_block_struct *wn_free_block;

struct wn_mbtree_struct
{
  wn_mbhandle handle_tree;

  int (*pcompare_keys_func)(ptr key1,ptr key2);
};

/* A free block consists of a size field, always aligned such that its
** address & 7 == 4, followed by a *wn_handle, which will naturally be
** 8-byte aligned.  We cannot declare one struct like this because on
** 64-bit machines, the compiler will assume the struct is 8 byte aligned
** and pad between the fields to align the pointers accordingly.
**
** while ideally, we would just consolidate the size field with the
** mbhandle field, but as has just been explained, C and C++ won't
** cooperate.  So what we do is declare it in two separate structs and
** provide macros to get between the two. */

struct wn_mbhandle_struct
{
  wn_free_block next_free_block;	/* it is imperative that this
  **	be the 1st field, small free blocks will use this field but
  **	not the rest of the struct */

  wn_free_block free_block_list;	/* This is a header of a list
  **	of all the free blocks of this exact size, not including this one */

  ptr key;	/* size of the segment.  Redundant with the free_block_size
  **	field in the *wn_free_block, easier to leave it in */

  int level, count;

  wn_mbhandle left_child, right_child, parent;
};

struct wn_free_block_struct
{
  int free_block_size;
};

#define WN_FREE_BLOCK_TO_MBHANDLE(free_block) \
	(TRUE ? (wn_mbhandle) ((free_block) + 1) : \
	/**/    (wn_mbhandle)  (free_block)->free_block_size)
	/* the unreachable case here is to provide type checking on
	** free_block, the optimizer will get rid of the code */

#define WN_MBHANDLE_TO_FREE_BLOCK(mbhandle) \
	(TRUE ? ((wn_free_block) mbhandle) - 1 : mbhandle->free_block_list)
	/* again, the unreachable case is just for type checking */

/* the minimum size of a small free_block struct */
#define WN_SIZEOF_FREE_BLOCK_STRUCT	(sizeof(ptr) == 8 ? 12 : \
					(sizeof(ptr) == 4 ?  8 : \
						(wn_assert_notreached(), 0)))

EXTERN void wn_mmkptrbtree(wn_mbtree tree);
EXTERN void wn_mbget(wn_mbhandle *phandle,wn_mbtree tree,ptr key,int compare);
EXTERN void wn_mbins(wn_mbhandle   handle,wn_mbtree tree, ptr key);
EXTERN void wn_mbdel(wn_mbhandle handle,wn_mbtree tree);
EXTERN int wn_mbcount(wn_mbtree tree);

/* don't EXTERN this one - it confuses some (but not all) sparc compilers */
#if !defined(sparc)
  EXTERN void wn_mbact(wn_mbtree tree, void (*paction)(wn_mbhandle handle),
		  ptr low_key,int low_compare,ptr high_key,int high_compare);
#else
	 void wn_mbact(wn_mbtree tree, void (*paction)(wn_mbhandle handle),
		  ptr low_key,int low_compare,ptr high_key,int high_compare);
#endif

EXTERN void wn_mbverify(wn_mbtree tree);

#endif /* wnmbtrH */
