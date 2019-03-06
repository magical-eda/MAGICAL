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
#ifndef wnchtbH
#define wnchtbH


#include "wnlib.h"
#include "wnmem.h"
#include "wnbvec.h"


typedef struct wn_chtab_struct *wn_chtab;
typedef struct wn_chtab_entry_struct *wn_chtab_entry;


struct wn_chtab_struct
{
  int begin_magic;

  int (*phash_func)(ptr key);
  bool (*pkeys_eq_func)(ptr key1,ptr key2);
  void (*palloc_copy_func)(ptr *pkey,ptr key);
  void (*pfree_func)(ptr key);

  int length;           /* always a power of 2 */
  int length_mask;      /* just (1 << table_length_shift)-1 */

  float fraction_full_before_grow;      /* 0 < x < 1 */
  int max_entries_before_grow;          /* 1 < x <= table_length-1.  If
  ** valid_entries+deleted_entries == this amount and we add one more,
  ** we grow the table.  The table is always to have at least one empty
  ** entry. */

  int valid_entries;
  int deleted_entries;

  wn_memgp group;

  wn_chtab_entry *entry_ptrs;   /* array of ptrs to entry records */
  wn_bitvect deleted_flags;     /*   bit vector of flags indicating item
  **                            ** is deleted */

  int end_magic;
};


struct wn_chtab_entry_struct
{
  int hashed_key;
  ptr data, key;
};


void wn_mkchtab
(
  wn_chtab *ptable,
  int (*phash_func)(ptr key),
  bool (*pkeys_eq_func)(ptr key1,ptr key2),     /* returns 1 for eq, else 0 */
  void (*palloc_copy_func)(ptr *pkey,ptr key),  /* alloc & copy key */
  void (*pfree_func)(ptr key),                  /* free key */
  int start_table_length,               /* how long should the table start as?
  **                                    **  Rounded up to 2^n. */
  float fraction_full_before_grow       /* how full should the table get before
  **                                    ** we grow it?  0 < x < 1 */
);
EXTERN void wn_freechtab(wn_chtab table);

EXTERN bool wn_chins(ptr data, wn_chtab table, ptr key);
EXTERN bool wn_chfins(ptr data, wn_chtab table, ptr key);
EXTERN bool wn_chget(ptr *pdata, wn_chtab table, ptr key);
EXTERN bool wn_chdel(wn_chtab table, ptr key);

EXTERN int wn_chcount(wn_chtab table);

/* note the thunk passed in MUST be of type extern "C" */
#ifdef __cplusplus
extern "C" {
#endif
  typedef void (*wn_chact_thunk)(ptr data, ptr key);
#ifdef __cplusplus
}
#endif
EXTERN void wn_chact(wn_chtab table, wn_chact_thunk paction);

EXTERN void wn_chverify(wn_chtab table);
EXTERN bool wn_chtab_checks_for_duplicates;     /* tells wn_chverify to check
**              for duplicates.  O(n^2), very slow.  Defaults to off. */

EXTERN void wn_chanalyze(wn_chtab table);

EXTERN void wn_purge_and_resize_table(wn_chtab old_table, int new_length);

EXTERN unsigned wn_32_bit_hash_collides(wn_chtab table);

EXTERN long unsigned wn_memory_taken_by_chash(wn_chtab table);

#endif /* wnchtbH */
