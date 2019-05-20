/**************************************************************************

wn_hctab table;

wn_mkhctab(&table,phash_func,pkeys_eq_func,palloc_copy_func,pfree_func)
wn_freehctab(table)

bool wn_chget(&data,table,key)
bool wn_chins(data,table,key)
bool wn_chfins(data,table,key)
bool wn_chdel(table,key)

int wn_chcount(table)
wn_chact(table,paction)      void (*paction)(data,key);

wn_chverify(table)
wn_chanalyze(table)

**************************************************************************/
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

#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnmem.h"
#include "wnmemb.h"
#include "wnmemw.h"
#include "wnbvec.h"

#include "wnchtb.h"


/* ---------------------------------------------------------------- */
/* debugging variables. */

#define LO_MEASURE_COLLISIONS	1

/* make these into consts instead of #defines so you can see them in gdb */
const int wn_chtab_begin_magic = 123454321;
const int wn_chtab_end_magic   = 123212321;

int wn_count_collisions;
int wn_max_collisions_depth;
int lo_32_bit_hash_collides;

void wn_mkchtab
(
  wn_chtab *ptable,
  int (*phash_func)(ptr key),
  bool (*pkeys_eq_func)(ptr key1,ptr key2),
  void (*palloc_copy_func)(ptr *pkey,ptr key),
  void (*pfree_func)(ptr key),
  int start_table_length,
  float fraction_full_before_grow
)
{
  int i;

  *ptable = (wn_chtab) wn_memwrap_alloc(sizeof(struct wn_chtab_struct));

  (*ptable)->begin_magic = wn_chtab_begin_magic;

  (*ptable)->phash_func = phash_func;
  (*ptable)->pkeys_eq_func = pkeys_eq_func;
  (*ptable)->palloc_copy_func = palloc_copy_func;
  (*ptable)->pfree_func = pfree_func;

  /* determine size of table, minimum 16 */
  wn_assert(start_table_length > 0);
  wn_assert(start_table_length < (1 << 30)-1);
  for (i = 2;  (1 << i) < start_table_length;  ++i) ;

  (*ptable)->length = 1 << i;
  (*ptable)->length_mask = (*ptable)->length - 1;

  wn_assert((*ptable)->length >= start_table_length);

  wn_assert(0 < fraction_full_before_grow  &&  fraction_full_before_grow < 1);
  (*ptable)->fraction_full_before_grow = fraction_full_before_grow;
  (*ptable)->max_entries_before_grow = (int) wn_min(
  /**/			fraction_full_before_grow * (*ptable)->length,
  /**/			(*ptable)->length-1);
  (*ptable)->max_entries_before_grow = wn_max(
  /**/			(*ptable)->max_entries_before_grow, 1);

  (*ptable)->valid_entries = 0;
  (*ptable)->deleted_entries = 0;

  (*ptable)->group = wn_memwrap_curgp();

  (*ptable)->entry_ptrs = (wn_chtab_entry *) wn_memwrap_alloc(
  /**/			sizeof(wn_chtab_entry) * (*ptable)->length);
  wn_memset((*ptable)->entry_ptrs, 0,
  /**/			sizeof(wn_chtab_entry) * (*ptable)->length);

  wn_bitvect_make(        &(*ptable)->deleted_flags, (*ptable)->length);
  wn_bitvect_set_all_false((*ptable)->deleted_flags, (*ptable)->length);

  (*ptable)->end_magic = wn_chtab_end_magic;

  /*
  wn_chverify(*ptable);
  */
} /* wn_mkchtab */


void wn_freechtab(wn_chtab table)
{
  int i;

  wn_assert(table->pfree_func);

  wn_gppush(table->group);

    for (i = 0;  i < table->length;  ++i)
    {
      if (table->entry_ptrs[i])
      {
	(*table->pfree_func)(table->entry_ptrs[i]->key);
	wn_memwrap_free(table->entry_ptrs[i]);
      }
    }

    wn_memwrap_free(table->entry_ptrs);
    wn_bitvect_free(table->deleted_flags);
    wn_memwrap_free(table);

  wn_gppop();
} /* wn_freechtab */


local int lo_find_entry(int hashed_key, wn_chtab table, ptr key)
{
  int result_index;
  register wn_chtab_entry entry;
# if LO_MEASURE_COLLISIONS
    int probe_depth = 0;
# endif

  result_index = hashed_key & table->length_mask;
  entry = table->entry_ptrs[result_index];

  if (!entry)	/* empty */
  {
    return result_index;
  }
  else if (entry->hashed_key == hashed_key)
  {
    if ((*table->pkeys_eq_func)(entry->key, key))
    {
      /* note this covers both valid and deleted cases */

      return result_index;
    }

#   if LO_MEASURE_COLLISIONS
      ++ lo_32_bit_hash_collides;
#   endif
  }

  /*   we have a collision. */

  while (TRUE)
  {
#   if LO_MEASURE_COLLISIONS
      ++ wn_count_collisions;
      ++ probe_depth;
      wn_max_collisions_depth = wn_max(wn_max_collisions_depth, probe_depth);
#   endif

    ++result_index;
    result_index &= table->length_mask;
    entry = table->entry_ptrs[result_index];

    /* there is always guaranteed to be one entry empty, and we probe by
    ** an odd amount so we'll hit it sooner or later.  This code is
    ** duplicated from above before the loop in an all-out attempt for
    ** speed */
    if (!entry)
    {
      return result_index;
    }
    else if (entry->hashed_key == hashed_key)
    {
      if ((*table->pkeys_eq_func)(entry->key, key))
      {
	/* note this covers both valid and deleted cases */

	return result_index;
      }

#     if LO_MEASURE_COLLISIONS
	++ lo_32_bit_hash_collides;
#     endif
    }
  }

# if defined(linux) || defined(__linux__)
    wn_assert_notreached();

    return 0;
# endif
} /* lo_find_entry */


/* ----------------------------------------------------------------
**   change table length, rehashing everything into it, getting rid
** of deleted entries.  Note the given table length is an ADVISORY
** only, the routine may choose a bigger size, but will not choose
** a smaller one. */

void wn_purge_and_resize_table(wn_chtab old_table, int new_length_arg)
{
  int new_length;
  wn_chtab new_table;	/* this is created, copied to old_table, then thrown
  **			** away */
  wn_chtab_entry entry;	/* ptrs to where a given entry goes
  **			** in new & old tables */
  wn_chtab_entry *pnew_entry;
  bool entry_deleted;
  int i;

  /* massage the length into something we can live with */
  new_length = wn_max(new_length_arg, old_table->valid_entries+1);
  new_length = wn_max(new_length,
  /**/	(int) ((old_table->valid_entries + 1) /
  /**/				old_table->fraction_full_before_grow + 1));
  for (i = 1;  i < 31  &&  (1 << i) < new_length;  ++i)
  {
    ;
  }
  wn_assert(i < 31);
  new_length = 1 << i;

  wn_assert(new_length >= new_length_arg);
  wn_assert(new_length * old_table->fraction_full_before_grow >=
  /**/						old_table->valid_entries);

  wn_gppush(old_table->group);

    wn_mkchtab(&new_table, old_table->phash_func, old_table->pkeys_eq_func,
    /**/    old_table->palloc_copy_func, old_table->pfree_func,
    /**/    new_length, old_table->fraction_full_before_grow);

    for (i = 0;  i < old_table->length;  ++i)
    {
      entry = old_table->entry_ptrs[i];
      if (entry) {
	wn_bitvect_get_bit(&entry_deleted, old_table->deleted_flags, i);
	if (!entry_deleted)
	{
	  pnew_entry = new_table->entry_ptrs + lo_find_entry(entry->hashed_key,
	  /**/					new_table, entry->key);
	  wn_assert(!*pnew_entry);
	  *pnew_entry = entry;

	  ++new_table->valid_entries;
	  wn_assert(new_table->valid_entries < new_table->length);
	}
	else
	{
	  (*old_table->pfree_func)(entry->key);

	  wn_memwrap_free((ptr) entry);

	  --old_table->deleted_entries;
	}
      } /* if entry */
    } /* for i */

    wn_assert(! old_table->deleted_entries);

    wn_memwrap_free((ptr) old_table->entry_ptrs);
    wn_bitvect_free(old_table->deleted_flags);

    wn_memcpy((ptr) old_table, (ptr) new_table, sizeof(*old_table));

    wn_memwrap_free((ptr) new_table);
  wn_gppop();
} /* wn_purge_and_resize_table */


bool wn_chins(ptr data, wn_chtab table, ptr key)
{
  int hashed_key;
  int entry_index;
  wn_chtab_entry entry;
  bool entry_deleted;

  hashed_key = (*table->phash_func)(key);
  entry_index = lo_find_entry(hashed_key, table, key);
  entry = table->entry_ptrs[entry_index];

  if (!entry)
  {
    /* an empty slot, we're going to add it */

    if (table->valid_entries + table->deleted_entries >=
    /**/				table->max_entries_before_grow)
    {
      wn_assert(table->valid_entries + table->deleted_entries ==
      /**/				table->max_entries_before_grow);

      wn_purge_and_resize_table(table, table->length<<1);

      /* grow moves entries */
      entry_index = lo_find_entry(hashed_key, table, key);
    }

    wn_gppush(table->group);

      entry = (wn_chtab_entry) wn_memwrap_alloc(sizeof(*entry));
      table->entry_ptrs[entry_index] = entry;

      entry->hashed_key = hashed_key;
      entry->data = data;
      (*table->palloc_copy_func)(&entry->key, key);

      ++table->valid_entries;
    wn_gppop();

    return TRUE;
  }

  wn_bitvect_get_bit(&entry_deleted, table->deleted_flags, entry_index);

  if (entry_deleted)
  {
    wn_assert(entry);

    /* hashed_key and key are already fine */
    entry->data = data;

    wn_bitvect_set_bit(FALSE, table->deleted_flags, entry_index);

    ++table->valid_entries;
    --table->deleted_entries;

    return TRUE;
  }
  else
  {
    /* value already there, this is not a forced insert */

    return FALSE;
  }
} /* wn_chins */


bool wn_chfins(ptr data, wn_chtab table, ptr key)
{
  int hashed_key;
  register wn_chtab_entry entry;
  bool entry_deleted;
  int entry_index;

  hashed_key = (*table->phash_func)(key);
  entry_index = lo_find_entry(hashed_key, table, key);
  entry = table->entry_ptrs[entry_index];

  if (!entry)
  {
    /* an empty slot, we're going to add it */

    if (table->valid_entries + table->deleted_entries >=
    /**/				table->max_entries_before_grow)
    {
      wn_assert(table->valid_entries + table->deleted_entries ==
      /**/				table->max_entries_before_grow);

      wn_purge_and_resize_table(table, table->length<<1);

      /* grow moves entries */
      entry_index = lo_find_entry(hashed_key, table, key);
    }

    wn_gppush(table->group);

      entry = (wn_chtab_entry) wn_memwrap_alloc(sizeof(*entry));
      table->entry_ptrs[entry_index] = entry;

      entry->hashed_key = hashed_key;
      entry->data = data;
      (*table->palloc_copy_func)(&entry->key, key);

      ++table->valid_entries;
    wn_gppop();

    return TRUE;
  }

  wn_assert(entry);

  /* entry is there, valid or deleted */

  /* hashed_key and key are already fine */
  entry->data = data;

  wn_bitvect_get_bit(&entry_deleted, table->deleted_flags, entry_index);
  if (entry_deleted)
  {
    ++table->valid_entries;
    --table->deleted_entries;
  }

  wn_bitvect_set_bit(FALSE, table->deleted_flags, entry_index);

  return TRUE;
} /* wn_chfins */


bool wn_chget(ptr *pdata, wn_chtab table, ptr key)
{
  wn_chtab_entry entry;
  int hashed_key;
  bool entry_deleted;
  int entry_index;

  hashed_key = (*table->phash_func)(key);

  entry_index = lo_find_entry(hashed_key, table, key);
  entry = table->entry_ptrs[entry_index];

  if (!entry)
  {
    *pdata = NULL;
    return FALSE;
  }

  wn_bitvect_get_bit(&entry_deleted, table->deleted_flags, entry_index);

  if (entry_deleted)
  {
    *pdata = NULL;
    return FALSE;
  }
  else
  {
    *pdata = entry->data;
    return TRUE;
  }
} /* wn_chget */


bool wn_chdel(wn_chtab table, ptr key)
{
  wn_chtab_entry entry;
  int hashed_key;
  bool entry_deleted;
  int entry_index;

  hashed_key = (*table->phash_func)(key);
  entry_index = lo_find_entry(hashed_key, table, key);
  entry = table->entry_ptrs[entry_index];

  if (!entry)
  {
    return FALSE;
  }

  wn_bitvect_get_bit(&entry_deleted, table->deleted_flags, entry_index);

  if (entry_deleted)
  {
    return FALSE;
  }
  else
  {
    wn_bitvect_set_bit(TRUE, table->deleted_flags, entry_index);

    ++table->deleted_entries;
    --table->valid_entries;

    return TRUE;
  }
} /* wn_chdel */


int wn_chcount(wn_chtab table)
{
  return table->valid_entries;
}

void wn_chact(wn_chtab table, wn_chact_thunk paction)
{
  register wn_chtab_entry *pentry;
  int call_count;	/* for sanity check */
  bool entry_deleted;
  int i;

  call_count = 0;
  for (i = 0, pentry = table->entry_ptrs;  i < table->length;  ++i, ++pentry)
  {
    if (*pentry)
    {
      wn_bitvect_get_bit(&entry_deleted, table->deleted_flags, i);

      if (!entry_deleted)
      {
	(*paction)((*pentry)->data, (*pentry)->key);
	++ call_count;
      }
    }
  }

  wn_assert(table->valid_entries == call_count);
} /* wn_chact */


bool wn_chtab_checks_for_duplicates = FALSE;

void wn_chverify(wn_chtab table)
{
  wn_chtab_entry *pentry, *pentry2;
  int valid_entries, deleted_entries;
  bool entry_deleted;
  int i, j;

  wn_assert(table->begin_magic == wn_chtab_begin_magic);

  wn_assert((ptr)(table->phash_func) != NULL);
  wn_assert((ptr)(table->pkeys_eq_func) != NULL);
  wn_assert((ptr)(table->palloc_copy_func) != NULL);
  wn_assert((ptr)(table->pfree_func) != NULL);

  wn_assert(table->length > table->max_entries_before_grow);
  wn_assert(table->max_entries_before_grow >=
  /**/			table->valid_entries + table->deleted_entries);

  valid_entries = 0;
  deleted_entries = 0;
  for (i = 0, pentry = table->entry_ptrs;  i < table->length;  ++i, ++pentry)
  {
    wn_bitvect_get_bit(&entry_deleted, table->deleted_flags, i);

    wn_assert(*pentry  ||  !entry_deleted);

    valid_entries += (*pentry  &&  !entry_deleted);
    deleted_entries += entry_deleted;
  }

  wn_assert(valid_entries   == table->valid_entries);
  wn_assert(deleted_entries == table->deleted_entries);

  /* no duplicates.  Neither valids nor deleteds should be duplicates */
  if (wn_chtab_checks_for_duplicates)
  {
    for (i = 0, pentry = table->entry_ptrs;  i < table->length; ++i, ++pentry)
    {
      if   (*pentry)
      {
	for (j = 0, pentry2 = table->entry_ptrs;  j < i;  ++j, ++pentry2)
	{
	  if   (*pentry2)
	  {
	    wn_assert(!(*table->pkeys_eq_func)((*pentry)->key,
	    /**/					(*pentry2)->key));
	  }
	}
      }
    }
  } /* I could've done "} } } } }", but Will would've puked */

  wn_assert(table->end_magic == wn_chtab_end_magic);
} /* wn_chverify */


local wn_chtab lo_count_collisions_act_table;

local void lo_count_collisions_act(ptr data, ptr key)
{
  ptr got_data;
  int sts;

  sts = wn_chget(&got_data, lo_count_collisions_act_table, key);
  wn_assert(sts);
  wn_assert(got_data == data);
} /* lo_count_collisions_act */  


void wn_chanalyze(wn_chtab table)
{
  printf("Analyze Closed Hash Table\n");
  printf("  Table length: %d, Valid entries: %d, Deleted entries: %d\n",
  /**/	table->length, table->valid_entries, table->deleted_entries);
  printf("  Fraction before grow: %.3f, Max entries before grow: %d\n",
  /**/	table->fraction_full_before_grow, table->max_entries_before_grow);
  printf("  Empty entries: %d\n", table->length - table->valid_entries -
  /**/						table->deleted_entries);

  wn_count_collisions = 0;
  wn_max_collisions_depth = 0;
  lo_count_collisions_act_table = table;
  lo_32_bit_hash_collides = 0;

  wn_chact(table, lo_count_collisions_act);

  printf("  Total collisions: %d == %g%%, max_collisions_depth = %d\n",
  /**/	wn_count_collisions, wn_count_collisions * 100.0 /
  /**/				(table->valid_entries+table->deleted_entries),
  /**/				wn_max_collisions_depth);
  printf("  Hashed key collides on different keys: %d\n",
  /**/				lo_32_bit_hash_collides);
} /* wn_chanalyze */


unsigned wn_32_bit_hash_collides(wn_chtab table)
{
  wn_count_collisions = 0;
  wn_max_collisions_depth = 0;
  lo_count_collisions_act_table = table;
  lo_32_bit_hash_collides = 0;

  wn_chact(table, lo_count_collisions_act);

  return lo_32_bit_hash_collides;
} /* wn_32_bit_hash_collides */


/*     This just includes the memory taken by the table.  It does not
** include memory taken by anything pointed at by the entries in the
** table.  Note also that if the table is in a non-freeable group, and has
** been growing, there will be discarded smaller copies of the array
** in the group and deleted entry nodes going to waste that are not
** counted by this routine which could double the actual amount of
** memory being taken. */

long unsigned wn_memory_taken_by_chash(wn_chtab table)
{
  long unsigned ret;

  ret = sizeof(*table);
  ret += sizeof(table->entry_ptrs[0]) * table->length;
  ret += sizeof(*table->entry_ptrs[0]) *
  /**/			(table->valid_entries  + table->deleted_entries);
  ret += table->length/8 + 8;	/* rough size of deleted_flags bitvect */

  return ret;
} /* wn_memory_taken_by_chash */
