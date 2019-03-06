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
#include <stdlib.h>
#include <stdio.h>

#include "wnlib.h"

#include <string.h>
#if !defined(linux) && !defined(__linux__) && !defined(WN_WINDOWS)
#  include <strings.h>
#endif

#if !defined(WN_WINDOWS)
# include <sys/times.h>
#else
# define clock_t long
#endif

#include "wnargp.h"
#include "wnasrt.h"

#include "wneq.h"
#include "wncpy.h"
#include "wnnop.h"
#include "wnrnd.h"
#include "wnrndd.h"
#include "wnhash.h"
#include "wnhtbl.h"

#include "wnchtb.h"
#include "wnchtl.h"
#include "wnsll.h"


/* total memory in blocks of this group and its children. */

local int lo_total_block_group_memory_size(wn_memgp group)
{
  wn_mem_block block;
  wn_memgp child;
  int ret;

  ret = 0;
  for (block = group->current_block;  block;  block = block->next)
  {
    ret += block->size;
  }

  for (child = group->children;  child;  child->sub_next)
  {
    ret += lo_total_block_group_memory_size(child);
  }

  return ret;
} /* lo_total_block_group_memory_size */


char *lo_strdup(char *s)
{
  char *result;

  result = (char *) wn_alloc(strlen(s)+1);
  strcpy(result, s);
  return result;
}


/* for radix sort */
local char string_index(char string[],int index)
{
  return(string[index]);
}


/* dump a list of the english dictionary */

wn_sll lo_dictionary_list;

local void lo_dictionary_list_action(ptr data, ptr key)
{
  wn_assert(key);
  wn_sllins(&lo_dictionary_list, key);
}


#ifndef WN_WINDOWS
void lo_new_spellcheck_constitution(void)
{
  FILE *fp;
  char buffer[1000];
  wn_chtab  new_english_dictionary;
  char *pc;
  wn_sll constitution_list, error_list, el, el2, dictionary_list, *pel;
  wn_sll condemned;
  ptr dummy_data;
  struct tms time_buf;
  clock_t start_time, end_time;
  const char *filename;
  int sts, num, pre_valid_count;
  int i;

  wn_gpmake("no_free");

    wn_mkstrchtab(&new_english_dictionary, 16, 0.5);

#   if defined(linux) || defined(__linux__)
      filename = "/usr/share/dict/words";
#   else
      filename = "/usr/dict/words";
#   endif 

    fp = fopen(filename, "r");
    if(fp == NULL)
    {
      printf("warning:  cannot find file <%s>\n",filename);
      goto finish;
    }

    dictionary_list = NULL;
    while (TRUE)
    {
      sts = fscanf(fp, "%s", buffer);
      if (1 != sts)
      {
	break;
      }

      /* convert all to lower case */
      pc = buffer;
      for ( ; *pc; ++pc)
      {
	if ('A' <= *pc  &&  'Z' >= *pc)
	{
	  *pc += 'a' - 'A';
	}
      }

      wn_sllins(&dictionary_list, (ptr) lo_strdup(buffer));
    }

    fclose(fp);

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    start_time = time_buf.tms_utime;

    for (el = dictionary_list;  el;  el = el->next)
    {    
      sts = wn_chins((ptr)0, new_english_dictionary, el->contents);
      /* ignore status - some dictionaries have duplicate entries 
      wn_assert(sts);
      */
    }

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    end_time = time_buf.tms_utime;

    printf("%g seconds to load new_english dictionary from ram\n",
    /**/				(end_time - start_time) / 100.0);

    fp = fopen("constitution.txt", "r");
    wn_assert(fp);

    constitution_list = NULL;
    while (TRUE)
    {
      sts = fscanf(fp, "%s", buffer);
      if (1 != sts)
      {
	break;
      }

      pc = buffer;
      for ( ; *pc; ++pc)
      {
	if ('A' <= *pc  &&  'Z' >= *pc)
	{
	  *pc += 'a' - 'A';
	}

	if ('a' > *pc  ||  'z' < *pc)
	{
	  /* toss out non-alpha */
	  *pc = 0;
	  break;
	}
      } /* while chars in word */

      if (buffer[0])
      {
	wn_sllins(&constitution_list, (ptr) lo_strdup(buffer));
      }
    } /* while words in constition */
    
    sts = times(&time_buf);
    wn_assert(-1 != sts);
    start_time = time_buf.tms_utime;

    /* spell check the constitution, all in ram, 10000 times */
    for (i = 0;  i < 1000;  ++i)
    {
      error_list = NULL;
      for (el = constitution_list;  el;  el = el->next)
      {
	sts = wn_chget(&dummy_data, new_english_dictionary, el->contents);
	if (!sts)
	{
#	  if 0
	    wn_sllins(&error_list, el->contents);
#	  endif
	  ;
	}
      }
    }

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    end_time = time_buf.tms_utime;

    printf("1000 new spellchecks in ram took %.2g seconds\n",
    /**/			((double) end_time - start_time) / 100.0);

    printf("total ram taken for new spellchecks: %d\n",
    /**/		lo_total_block_group_memory_size(wn_curgp()));

finish:
  wn_gpfree();
} /* lo_new_spellcheck_constitution */
#endif /* !WN_WINDOWS */


#ifndef WN_WINDOWS
void lo_old_spellcheck_constitution(void)
{
  FILE *fp;
  char buffer[1000];
  wn_htab  old_english_dictionary;
  char *pc;
  wn_sll constitution_list, error_list, el, el2, dictionary_list, *pel;
  wn_sll condemned;
  ptr dummy_data;
  struct tms time_buf;
  clock_t start_time, end_time;
  char *filename;
  int sts, num, pre_valid_count;
  int i;

  wn_gpmake("no_free");

    wn_mkstrhtab(&old_english_dictionary);

#   if defined(linux) || defined(__linux__)
      filename = "/usr/share/dict/words";
#   else
      filename = "/usr/dict/words";
#   endif 

    fp = fopen(filename, "r");
    if(fp == NULL)
    {
      printf("warning:  cannot find file <%s>\n",filename);
      goto finish;
    }

    dictionary_list = NULL;
    while (TRUE)
    {
      sts = fscanf(fp, "%s", buffer);
      if (1 != sts)
      {
	break;
      }

      /* convert all to lower case */
      pc = buffer;
      for ( ; *pc; ++pc)
      {
	if ('A' <= *pc  &&  'Z' >= *pc)
	{
	  *pc += 'a' - 'A';
	}
      }

      wn_sllins(&dictionary_list, (ptr) lo_strdup(buffer));
    }

    fclose(fp);

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    start_time = time_buf.tms_utime;

    for (el = dictionary_list;  el;  el = el->next)
    {    
      sts = wn_hins((ptr)0, old_english_dictionary, el->contents);
      /* ignore status - some dictionaries have duplicate entries 
      wn_assert(sts);
      */
    }

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    end_time = time_buf.tms_utime;

    printf("%g seconds to load old_english dictionary from ram\n",
    /**/				(end_time - start_time) / 100.0);

    fp = fopen("constitution.txt", "r");
    wn_assert(fp);

    constitution_list = NULL;
    while (TRUE)
    {
      sts = fscanf(fp, "%s", buffer);
      if (1 != sts)
      {
	break;
      }

      pc = buffer;
      for ( ; *pc; ++pc)
      {
	if ('A' <= *pc  &&  'Z' >= *pc)
	{
	  *pc += 'a' - 'A';
	}

	if ('a' > *pc  ||  'z' < *pc)
	{
	  /* toss out non-alpha */
	  *pc = 0;
	  break;
	}
      } /* while chars in word */

      if (buffer[0])
      {
	wn_sllins(&constitution_list, (ptr) lo_strdup(buffer));
      }
    } /* while words in constition */
    
    sts = times(&time_buf);
    wn_assert(-1 != sts);
    start_time = time_buf.tms_utime;

    /* spell check the constitution, all in ram, 10000 times */
    for (i = 0;  i < 1000;  ++i)
    {
      error_list = NULL;
      for (el = constitution_list;  el;  el = el->next)
      {
	sts = wn_hget(&dummy_data, old_english_dictionary, el->contents);
	if (!sts)
	{
#	  if 0
	    wn_sllins(&error_list, el->contents);
#	  endif
	  ;
	}
      }
    }

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    end_time = time_buf.tms_utime;

    printf("1000 old spellchecks in ram took %.2g seconds\n",
    /**/			((double) end_time - start_time) / 100.0);

    printf("total ram taken for old spellchecks: %d\n",
    /**/		lo_total_block_group_memory_size(wn_curgp()));
    
finish:
  wn_gpfree();
} /* lo_old_spellcheck_constitution */
#endif /* !WN_WINDOWS */


local clock_t lo_get_time(void)
{
# ifndef WN_WINDOWS
    struct tms time_buf;
    int sts;

    sts = times(&time_buf);
    wn_assert(-1 != sts);
    return time_buf.tms_utime;
# else
    static bool first_time = TRUE;

    if (first_time) {
      first_time = FALSE;
      fprintf(stderr, "Time function not working on windows - clock\n");
      fprintf(stderr,				"    is just returning 0\n");
    }

    return 0;
# endif
} /* lo_get_time */


int lo_int_problem_size = 100000;

local void lo_rand_int_lookup(void)
{
  wn_chtab new_int_table;
  wn_htab  old_int_table;
  clock_t  start_time, end_time;
  int	   *random_ints, *random_ints_for_tables;
  ptr	   dummy_data;
  wn_memgp old_hash_table_group, new_hash_table_group;
  int	   i, sts;

  wn_gpmake("no_free");

    random_ints =            (int*)wn_alloc(sizeof(int)*lo_int_problem_size);
    random_ints_for_tables = (int*)wn_alloc(sizeof(int)*lo_int_problem_size);

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      random_ints_for_tables[i] = wn_random_int_between(0,lo_int_problem_size);
      random_ints[i]            = wn_random_int_between(0,lo_int_problem_size);
    }

    start_time = lo_get_time();

    wn_gpmake("no_free");
      old_hash_table_group = wn_curgp();

      wn_mkinthtab( &old_int_table);
    wn_gppop();

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_hins((ptr) 0, old_int_table, (ptr) random_ints_for_tables[i]);
    }

    end_time = lo_get_time();

    printf("Starting large old int table: %.2g seconds\n",
    /**/				(end_time - start_time) / 100.0);

    start_time = lo_get_time();

    wn_gpmake("no_free");
      new_hash_table_group = wn_curgp();

      wn_mkintchtab(&new_int_table, lo_int_problem_size, 0.7);
    wn_gppop();

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_chins((ptr) 0, new_int_table, (ptr) random_ints_for_tables[i]);
    }

    end_time = lo_get_time();

    printf("Starting large new int table: %.2g seconds\n",
    /**/				(end_time - start_time) / 100.0);

    printf("Old ht group memory == %d\n", lo_total_block_group_memory_size(
    /**/						old_hash_table_group));
    printf("New ht group memory == %d\n", lo_total_block_group_memory_size(
    /**/						new_hash_table_group));

    start_time = lo_get_time();

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_hget(&dummy_data, old_int_table, (ptr) random_ints[i]);
    }

    end_time = lo_get_time();

    printf("%d old int gets: %.2g seconds\n", lo_int_problem_size,
    /**/				(end_time - start_time) / 100.0);

    start_time = lo_get_time();

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_chget(&dummy_data, new_int_table, (ptr) random_ints[i]);
    }

    end_time = lo_get_time();

    printf("%d new int gets: %.2g seconds\n", lo_int_problem_size,
    /**/				(end_time - start_time) / 100.0);

    wn_chanalyze(new_int_table);

    wn_gppush(old_hash_table_group);
    wn_gpfree();
    wn_gppush(new_hash_table_group);
    wn_gpfree();

  wn_gpfree();
} /* lo_rand_int_lookup */


int lo_identity_hash(ptr p)
{
  return (int) (long) p;
}

/* like lo_rand_int_lookup, only using identity hash function */

local void lo_rand_int_lookup_identity(void)
{
  wn_chtab new_int_table;
  wn_htab  old_int_table;
  clock_t  start_time, end_time;
  int      *random_ints, *random_ints_for_tables;
  ptr	   dummy_data;
  int	   i, sts;

  wn_gpmake("no_free");

    random_ints =            (int*)wn_alloc(sizeof(int)*lo_int_problem_size);
    random_ints_for_tables = (int*)wn_alloc(sizeof(int)*lo_int_problem_size);

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      random_ints_for_tables[i] = wn_random_int_between(0,
      /**/					lo_int_problem_size);
      random_ints[i]            = wn_random_int_between(0,
      /**/					lo_int_problem_size);
    }

    start_time = lo_get_time();

    wn_mkhtab(&old_int_table,
    /**/	    lo_identity_hash, (bool (*)(ptr,ptr))(wn_longeq),
    /**/	    (void (*)(ptr *,ptr))(wn_longcpy),
    /**/	    (void (*)(ptr))(wn_do_nothing));
    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_hins((ptr) 0, old_int_table, (ptr) random_ints_for_tables[i]);
    }

    end_time = lo_get_time();

    printf("Ihash: Starting large old int table: %.2g seconds\n",
    /**/				(end_time - start_time) / 100.0);

    start_time = lo_get_time();

    wn_mkchtab(&new_int_table,
    /**/	    lo_identity_hash, (bool (*)(ptr,ptr))(wn_longeq),
    /**/	    (void (*)(ptr *,ptr))(wn_longcpy),
    /**/	    (void (*)(ptr))(wn_do_nothing),
    /**/	    2, 0.5);
    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_chins((ptr) 0, new_int_table, (ptr) random_ints_for_tables[i]);
    }

    end_time = lo_get_time();

    printf("Ihash: Starting large new int table: %.2g seconds\n",
    /**/				(end_time - start_time) / 100.0);

    start_time = lo_get_time();

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_hget(&dummy_data, old_int_table, (ptr) random_ints[i]);
    }

    end_time = lo_get_time();

    printf("Ihash: %d old int gets: %.2g seconds\n", lo_int_problem_size,
    /**/				(end_time - start_time) / 100.0);

    start_time = lo_get_time();

    for (i = 0;  i < lo_int_problem_size;  ++i)
    {
      (void) wn_chget(&dummy_data, new_int_table, (ptr) random_ints[i]);
    }

    end_time = lo_get_time();

    printf("Ihash: %d new int gets: %.2g seconds\n", lo_int_problem_size,
    /**/				(end_time - start_time) / 100.0);

    wn_chanalyze(new_int_table);

  wn_gpfree();
} /* lo_rand_int_lookup_identity */


int main(int argc,char *argv[])
{
  static char *int_problem_size_string = NULL;
  static bool old_spellcheck_only, new_spellcheck_only;
  static bool irand_only;
  static wn_arg_format arg_format_array[] =
  {
    WN_KEY_BOOL(&wn_chtab_checks_for_duplicates, "d"),	/* checking for
    **			duplicates is O(n^2), takes a LONG time. */
#   if defined(WN_WINDOWS)
      WN_KEY_BOOL(&new_spellcheck_only, "n"),
      WN_KEY_BOOL(&old_spellcheck_only, "o"),
#   endif
    WN_KEY_BOOL(&irand_only, "i"),
    WN_KEY_STRING(&int_problem_size_string, "size", "test problem size",
    /**/							WN_OPTIONAL),
    WN_ARG_TERMINATE
  };
  int sts;

  wn_parse_args(argc,argv,arg_format_array);

  if (int_problem_size_string)
  {
    sts = sscanf(int_problem_size_string, "%u", &lo_int_problem_size);
    wn_assert(1 == sts);
  }

  printf("testing chash... set -d for duplicate checking (v slow)\n");

# if !defined(WN_WINDOWS)
    if (new_spellcheck_only)
    {
      lo_new_spellcheck_constitution();
      return 0;
    }

    if (old_spellcheck_only)
    {
      lo_old_spellcheck_constitution();
      return 0;
    }
# endif

  if (irand_only)
  {
    lo_rand_int_lookup_identity();
    return 0;
  }

# if !defined(WN_WINDOWS)
    printf("\nSpell checking constitution\n");
    lo_old_spellcheck_constitution();
    lo_new_spellcheck_constitution();
# else
    fprintf(stderr, "No spellcheck constitution on Windows\n");
# endif

  printf("\nRand Lookup\n");
  lo_rand_int_lookup();

  printf("\nIrand Lookup\n");
  lo_rand_int_lookup_identity();

  printf("  ok!!!!!!\n");

  return 0;
} /* main */
