/**********************************************************************

    This module is to provide an exact copy of the interface provided by
wnmem.h.

FUNCTIONS PROVIDED:

  wn_gpmake(parms)
  wn_gpfree()
  wn_gppush(group)
  wn_gppop()
  
  ptr wn_alloc(size)
  ptr wn_zalloc(size)
  wn_free(p)

  wn_realloc(&p,old_size,new_size)
  wn_zrealloc(&p,old_size,new_size)

  wn_memgp wn_curgp();
  wn_memgp wn_defaultgp();
  
  wn_gperrfpush(pfunc)
  wn_gperrfpop()  * not implemented *

  wn_make_gpstack(&stack)
  wn_get_current_gpstack(stack)
  wn_set_current_gpstack(stack)
  wn_swap_current_gpstack(stack)
  
DEBUGGING:

  wn_gp_fill();
  wn_gp_pad();
  wn_stack_fill();
  wn_gp_args_check();  * not implemented *
  wn_gp_trap_address(address);
  
  wn_allmem_verify();
  wn_group_verify(group);

  int wn_mem_used()
  int wn_group_mem_used(group)

  wn_print_mem_used()
  wn_print_all_groups_mem_used()
  wn_print_group_mem_used(group)

  bool wn_mem_in_group(p,group)

  wn_print_gp_stack()

  wn_gplabel(label)

**********************************************************************/
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
#include <malloc.h>
#include <string.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnmemb.h"

#include "wnmem.h"
#include "wnchtb.h"
#include "wnchtl.h"


/*     increment and decrement pointers to foil purify keeping track of our
** memory references.  This enables purify and the like to check for leaks. */
local long lo_tmp;
#define LO_INCP(p) (lo_tmp = (long) p, ++lo_tmp, (void *) lo_tmp)
#define LO_DECP(p) (lo_tmp = (long) p, --lo_tmp, (void *) lo_tmp)

typedef struct lo_smemgp_struct *lo_smemgp;

struct lo_smemgp_struct
{
  int begin_magic;

  lo_smemgp next, *pprev;		/* global list of memory groups */
  lo_smemgp child;			/* first child of this group */
  lo_smemgp next_child, *pprev_child;	/* next & prev child of parent group */

  wn_chtab chtab;	/*   keys = ptrs to segments, data contains segment
			** size */

  char *label;

  int end_magic;
};


#define LO_SGROUP_STACK_SIZE    100

typedef struct lo_sgpstack_struct *lo_sgpstack;
struct lo_sgpstack_struct {
  int stack_index;	/* points to currently valid top of stack, grows +ve */
  lo_smemgp *groups;
};

#define LO_SMEMGP_BEGIN_MAGIC	1001001001
#define LO_SMEMGP_END_MAGIC	2002002002

local wn_memgp lo_default_group = NULL;

/* group_stack */

lo_smemgp lo_default_group_stack_groups[LO_SGROUP_STACK_SIZE];

struct lo_sgpstack_struct lo_default_group_stack = {
  -1,
  lo_default_group_stack_groups };

local lo_sgpstack lo_current_sgpstack = &lo_default_group_stack;

lo_smemgp lo_global_group_list = NULL;

extern bool	wn_gp_fill_flag;

local bool lo_gp_trap_address_flag = FALSE;
local ptr  lo_gp_trap_address_address;

local void lo_default_error_func(int size);
typedef void (*voidfunc)(int size);
local voidfunc perror_func = (lo_default_error_func);

local void lo_default_error_func(int size)
{
  ;
} /* lo_default_error_func */


#if 1
  /* wnmcmp.c stuff */
#if 0
# if !defined(linux)
    extern int memcmp(void *,void *,unsigned int);
#  endif
#endif

  int wn_memcmp(ptr m1,ptr m2,int len)
  {
    return(memcmp(m1,m2,len));
  }
#endif /* wnmcmp.c stuff */


#if 1
  /* wnmcpy.c stuff */

  void wn_memcpy(ptr out,ptr in,int size)
  {
    memcpy(out,in,size);
  }
#endif /* wnmcpy.c stuff */


#if 1
  /* wnmemb.c stuff */

  void wn_memset(ptr out,char c,int n)
  {
    (void)memset(out,(int)c,n);
  }


/* return true iff all bytes in the block of memory are set to c */
bool wn_memtest(ptr mem, char c, int n)
{
  int i;
  char *pmem = (char *) mem;

  for (i = 0;  i < n;  ++i)
  {
    if (pmem[i] != c)
    {
      return FALSE;
    }
  }

  return TRUE;
} /* wn_memtest */

  void wn_memzero(ptr out,int n)
  {
    wn_memset(out,'\0',n);
  }
#endif /* wnmemb.c stuff */


#if 1
  /* wnmemw.c stuff */

  bool wn_memwrap_use_malloc = TRUE;
#endif


#if 1
  /* wnmeq.c stuff */

  bool wn_memeq(ptr m1,ptr m2,int len)
  {
    return(memcmp(m1,m2,len) == 0);
  }
#endif /* wnmeq.c stuff */


#if 1
  /* wnmlc.c stuff */

  /*   need to do extern's explicitly here instead of including wnmlc.h
  ** since that would #define malloc() */
  EXTERN ptr wn_f_malloc(unsigned int size);
  EXTERN ptr wn_f_calloc(unsigned int nelem,unsigned int elsize);
  EXTERN void wn_f_free(ptr p);
  EXTERN void wn_f_cfree(ptr p);

  local wn_memgp lo_f_malloc_group = NULL;

  local void initialize_lo_f_malloc_group(void)
  {
    if(!(lo_f_malloc_group))
    {
      wn_gppush(wn_defaultgp());
      wn_gpmake("general_free");
      wn_gplabel("wn_f_malloc_group");
      lo_f_malloc_group = wn_curgp();
      wn_gppop();
      wn_gppop();
    }
  }


  ptr wn_f_malloc(unsigned int size)
  {
    ptr ret;

    initialize_lo_f_malloc_group();

    wn_gppush(lo_f_malloc_group);

    ret = wn_alloc(size);

    wn_gppop();

    return(ret);
  }


  ptr wn_f_calloc(unsigned int nelem,unsigned int elsize)
  {
    ptr ret;
    unsigned int size;

    size = nelem*elsize;

    ret = wn_f_malloc(size);

    wn_memzero(ret,size);

    return(ret);
  }


  void wn_f_free(ptr p)
  {
    wn_gppush(lo_f_malloc_group);

    wn_free(p);

    wn_gppop();
  }


  void wn_f_cfree(ptr p)
  {
    wn_f_free(p);
  }
#endif /* wnmlc.c stuff */


bool lo_in_ensure_init_flag = FALSE;

void lo_ensure_init()
{
  lo_in_ensure_init_flag = TRUE;

  if (!lo_default_group)
  {
    wn_gpmake("general_free");
    wn_assert(lo_current_sgpstack->stack_index == 0);
    lo_default_group = (wn_memgp)
    /**/	lo_current_sgpstack->groups[lo_current_sgpstack->stack_index];
    wn_gplabel("default");
  }

  lo_in_ensure_init_flag = FALSE;
}


wn_memgp wn_defaultgp(void)
{
  lo_ensure_init();

  return lo_default_group;
} /* wn_defaultgp */


wn_memgp wn_curgp(void)
{
  lo_ensure_init();
  wn_assert(lo_current_sgpstack->stack_index >= 0);

  return
    (wn_memgp) lo_current_sgpstack->groups[lo_current_sgpstack->stack_index];
} /* wn_curgp */


void wn_gppush(wn_memgp group)
{
  if (!lo_in_ensure_init_flag)
  {
    wn_assert(lo_default_group);
  }
  wn_assert(LO_SGROUP_STACK_SIZE-1 > lo_current_sgpstack->stack_index);

  lo_current_sgpstack->groups[++ lo_current_sgpstack->stack_index] =
  /**/							(lo_smemgp) group;
} /* wn_gppush */


void wn_gppop()
{
  wn_assert(0 < lo_current_sgpstack->stack_index);

  -- lo_current_sgpstack->stack_index;
} /* wn_gppop */


void wn_gpmake(const char parms[] /* ignored */)
{
  lo_smemgp ret, parent;

  if (!lo_in_ensure_init_flag)
  {
    lo_ensure_init();	/* makes and pushes default group */
  }

  ret = (lo_smemgp) malloc(sizeof(*ret));

  ret->begin_magic = LO_SMEMGP_BEGIN_MAGIC;
  ret->next = lo_global_group_list;
  ret->pprev = &lo_global_group_list;
  lo_global_group_list = ret;

  ret->child = NULL;

  /* is there a parent group? */
  if (0 <= lo_current_sgpstack->stack_index)
  {
    parent = (lo_smemgp) wn_curgp();
    wn_assert(parent != ret);
    ret->next_child = parent->child;
    ret->pprev_child = &parent->child;
    parent->child = ret;
    if (ret->next_child)
    {
      ret->next_child->pprev_child = &ret->next_child;
    }
  }
  else
  {
    ret->next_child = NULL;
    ret->pprev_child = NULL;
  }

  wn_mkptrchtab(&ret->chtab, 8, 0.7);

  ret->label = NULL;

  ret->end_magic = LO_SMEMGP_END_MAGIC;

  wn_gppush((wn_memgp) ret);
} /* wn_gpmake */


local void lo_free_action(ptr data, ptr key)
{
  if (key)
  {
    key = LO_INCP(key);

    /* trash it before freeing */
    wn_memset(key, (char) 0xb2, (int) (long) data);

    free(key);
  }
} /* lo_free_action */


void wn_gpfree(void)
{
  lo_smemgp group, child, condemned_child;

  wn_assert(0 < lo_current_sgpstack->stack_index);

  group = (lo_smemgp) wn_curgp();

  /* free the individual segments */
  wn_chact(group->chtab, lo_free_action);

  /* free the hash table */
  wn_freechtab(group->chtab);

  /* free all child groups */
  for (child = group->child;  child;  )
  {
    condemned_child = child;
    child = child->next_child;

    wn_gppush((wn_memgp) condemned_child);
    wn_gpfree();
  }

  if (group->label)
  {
    /* trash before freeing */
    wn_memset((ptr) group->label, (char) 0xc3, strlen(group->label)+1);

    free((ptr) group->label);
  }

  /* delete the group from linked lists */
  wn_assert(group->pprev);
  *group->pprev = group->next;
  if (group->pprev_child)
  {
    *group->pprev_child = group->next_child;
  }
  if (group->next_child) {
    group->next_child->pprev_child = group->pprev_child;
  }

  wn_memset((ptr) group, (char) 0xd8, sizeof(*group));
  free((void *) group);

  -- lo_current_sgpstack->stack_index;
} /* wn_gpfree */


ptr wn_alloc(int size)
{
  ptr ret;
  bool sts;

  lo_ensure_init();

  ret = (ptr) malloc(size);
  sts = wn_chins((ptr) size, ((lo_smemgp) wn_curgp())->chtab, LO_DECP(ret));
  wn_assert(sts);

  if (lo_gp_trap_address_flag  &&  ret == lo_gp_trap_address_address)
  {
    fprintf(stderr, "Address found, trapping.\n");
    wn_assert_notreached();
  }

  return ret;
} /* wn_alloc */


ptr wn_zalloc(int size)
{
  ptr ret;

  ret = wn_alloc(size);

  wn_memset(ret, 0, size);

  return ret;
} /* wn_zalloc */


void wn_free(ptr p)
{
  wn_chtab chtab;
  long size, sts;

  wn_assert(lo_default_group);
  wn_assert(lo_current_sgpstack->stack_index >= 0);

  if (!p)
  {
    return;
  }

  chtab = ((lo_smemgp) wn_curgp())->chtab;

  /* trash memory before freeing it */
  sts = wn_chget((ptr*) &size, chtab, LO_DECP(p));
  wn_assert(sts);
  wn_memset(p, (char) 0xb1, size);

  free(p);

  wn_chdel(chtab, LO_DECP(p));
} /* wn_free */


void wn_realloc(ptr *pp,int old_size,int new_size)
{
  ptr new_p;

  new_p = wn_alloc(new_size);
  wn_memcpy(new_p,*pp,wn_min(old_size,new_size));

  wn_memset(*pp, (char) 0xa4, old_size);
  wn_free(*pp);

  *pp = new_p;
} /* wn_realloc */


void wn_zrealloc(ptr *pp,int old_size,int new_size)
{
  ptr new_p;

  new_p = wn_zalloc(new_size);
  wn_memcpy(new_p,*pp,wn_min(old_size,new_size));
  wn_free(*pp);

  *pp = new_p;
} /* wn_zrealloc */


void wn_gp_fill(void)
{
  /* stub for now, don't want to fill on purify */
  ;
}


void wn_gp_pad(void)
{
  /* stub for now, don't want to pad on purify */
  ;
}


local void lo_smemgp_verify(lo_smemgp group)
{
  lo_smemgp g;
  
  wn_assert(LO_SMEMGP_BEGIN_MAGIC == group->begin_magic);
  wn_assert(LO_SMEMGP_END_MAGIC   == group->end_magic);

  wn_chverify(group->chtab);

  for (g = group->child;  g;  g = g->next_child)
  {
    lo_smemgp_verify(g);
  }
} /* lo_smemgp_verify */


void wn_allmem_verify(void)
{
  lo_smemgp g;

  lo_ensure_init();

  for (g = lo_global_group_list;  g;  g = g->next)
  {
    lo_smemgp_verify(g);
  }

  wn_assert(0 <= lo_current_sgpstack->stack_index);
} /* wn_allmem_verify */


local long lo_total_mem_used;

EXTERN void lo_mem_used_action(ptr data, ptr key)
{
  lo_total_mem_used += (long) data;
} /* lo_mem_used_action */


long int wn_group_mem_used(wn_memgp group)
{
  wn_assert(0 <= lo_current_sgpstack->stack_index);

  lo_total_mem_used = 0;
  wn_chact(((lo_smemgp) group)->chtab, lo_mem_used_action);

  /* don't count label memory since it's not in the ht, wnmem.c doesn't count it */

  return lo_total_mem_used;
} /* wn_group_mem_used */


long unsigned wn_total_memory_in_group(wn_memgp group)
{
  return wn_group_mem_used(group) +
  /**/			wn_memory_taken_by_chash(((lo_smemgp) group)->chtab);
}


long int wn_mem_used(void)
{
  lo_smemgp g;
  int ret;

  lo_ensure_init();

  wn_assert(0 <= lo_current_sgpstack->stack_index);

  ret = 0;
  for (g = lo_global_group_list;  g;  g = g->next)
  {
    ret += wn_group_mem_used((wn_memgp) g);
  }

  return ret;
} /* wn_mem_used */


long unsigned wn_total_wn_memory(void)
{
  lo_smemgp g;
  long unsigned ret;

  lo_ensure_init();

  wn_assert(0 <= lo_current_sgpstack->stack_index);

  ret = 0;
  for (g = lo_global_group_list;  g;  g = g->next)
  {
    ret += wn_total_memory_in_group((wn_memgp) g);
  }

  return ret;
} /* wn_mem_used */


void wn_gplabel(const char *label)
{
  char *label_copy;

  wn_assert(0 <= lo_current_sgpstack->stack_index);

  label_copy = (char *) malloc(strlen(label)+1);
  strcpy(label_copy,label);

  wn_assert(((lo_smemgp) wn_curgp())->label == NULL);

  ((lo_smemgp) wn_curgp())->label = label_copy;
} /* wn_gplabel */


void wn_print_gp_stack(void)
{
  int i;

  printf("************** memory group stack *************\n");
  for (i = lo_current_sgpstack->stack_index;  i >= 0;  --i)
  {
    printf("%7s %20s  0x%x\n", i == lo_current_sgpstack->stack_index ?
    /**/	"top -> " : "", lo_current_sgpstack->groups[i]->label,
    /**/			(long) lo_current_sgpstack->groups[i]);
  }
}


void wn_get_current_gpstack(wn_gpstack *pstack)
{
  fprintf(stderr, "wn_get_current_gpstack not supported in wnmems.c\n");

  wn_assert(0);
}


void wn_set_current_gpstack(wn_gpstack stack)
{
  fprintf(stderr, "wn_set_current_gpstack not supported in wnmems.c\n");

  wn_assert(0);
}


void wn_gperrfpush(void (*pfunc)(int size))
{
  perror_func = pfunc;
}


local ptr lo_mem_in_group_ptr;
local bool lo_mem_in_group_found;

local void lo_mem_in_group_act(ptr data, ptr key)
{
  long unsigned u = (long unsigned) key;
  long unsigned size = (long unsigned) data;

  if ((long unsigned) lo_mem_in_group_ptr >= u  &&
  /**/(long unsigned) lo_mem_in_group_ptr <  u + size)
  {
    lo_mem_in_group_found = TRUE;
  }
} /* lo_mem_in_group_act */


bool wn_mem_in_group(ptr p, wn_memgp group)
{
  lo_mem_in_group_ptr = p;
  lo_mem_in_group_found = FALSE;

  wn_chact(((lo_smemgp) group)->chtab, lo_mem_in_group_act);

  return lo_mem_in_group_found;
} /* wn_mem_in_group */


void wn_gp_trap_address(ptr address)
{
  lo_gp_trap_address_flag = TRUE;
  lo_gp_trap_address_address = address;
}


local void lo_stack_fill(int size)
{
  int filler[1000/sizeof(int)];

  if(size < 0)
  {
    return;
  }

  wn_memset(filler,'s',1000);

  lo_stack_fill(size-1000);
}


void wn_stack_fill(void)
{
  lo_stack_fill(100000);
}


void wn_print_mem_used(void)
{
  char string[150];

  (void)sprintf(string,"total memory used = %ld\n",wn_mem_used());
  fputs(string,stdout);
  fflush(stdout);
}


void wn_print_group_mem_used(wn_memgp group)
{
  char string[150];

  (void)sprintf(string,"total memory used for group = %ld\n",
                       wn_group_mem_used(group));
  fputs(string,stdout);
  fflush(stdout);
}


void wn_print_all_groups_mem_used(void)
{
  int total,used,num_groups;
  lo_smemgp g;

  total = 0;
  num_groups = 0;

  printf("=======================================\n"); 
  printf("                       group       used\n"); 
  printf("=======================================\n"); 

  for (g = lo_global_group_list;  g;  g = g->next)
  {
    used = wn_group_mem_used((wn_memgp) g);

    printf("%28s  %9d\n", g->label, used);

    total += used;
    num_groups++;
  }

  printf("---------------------------------------\n"); 
  printf("   total (%3d groups)  %9d memory.\n", num_groups, total);
  printf("=======================================\n"); 
} /* wn_print_all_groups_mem_used */


/**
 * AJS - below here, added to bring up to date with current wnlib/mem API
 **/

void wn_set_system_memory_alloc_func(wn_system_memory_alloc_func_type func)
{
  /* empty */
}

void wn_set_system_memory_free_func( wn_system_memory_free_func_type  func)
{
  /* empty */
}

EXTERN void wn_make_gpstack(wn_gpstack *pstack);

void wn_make_gpstack(wn_gpstack *pstack)
{
  *pstack = (wn_gpstack)wn_zalloc(sizeof(struct wn_gpstack_struct));

  (*pstack)->stack_ptr = -1;
  (*pstack)->group_stack = (wn_memgp *)
                               wn_zalloc(GROUP_STACK_SIZE*sizeof(wn_memgp));
  (*pstack)->current_group = NULL;
}


EXTERN void wn_group_verify(wn_memgp group);
void wn_group_verify(wn_memgp group)
{
  /* skip */
}

EXTERN void wn_memory_fragmentation_report(void);

void wn_memory_fragmentation_report(void)
{
  /* empty*/
}

long int wn_amount_of_free_memory_in_group(wn_memgp group)
{
  return 0;
}

ptr wn_system_alloc(int size)
{
  return malloc(size);
}

void wn_system_free(ptr mem,int size)
{
  wn_memset(mem, (char) 0x93, size);
  free(mem);
}

void wn_print_composition_of_big_blocks_free_list(wn_memgp group)
{
  /* empty */
}

typedef struct wn_mbhandle_struct *wn_mbhandle;

EXTERN void wn_free_list_verify_act(wn_mbhandle handle);

void wn_free_list_verify_act(wn_mbhandle handle)
{
  /* empty */
}

void wn_gpmakef(int flags, int block_size)
{
  /* we always make freeable groups with no block size, size ignore parms */

  wn_gpmake("ignored");
}


void wn_abandon_memory_leaks(bool abandon_default_group)
{
  lo_smemgp g, condemned_g;

  for (g = lo_global_group_list;  g;  )
  {
    condemned_g = g;
    g = g->next;

    if (abandon_default_group  ||  condemned_g != (lo_smemgp) lo_default_group)
    {
      /* free the hash table */
      wn_freechtab(condemned_g->chtab);

      *condemned_g->pprev = condemned_g->next;
      if (condemned_g->next)
      {
	condemned_g->next->pprev = condemned_g->pprev;
      }

      if (condemned_g->label)
      {
	free(condemned_g->label);
      }
      free(condemned_g);
    }
  }
} /* wn_abandon_memory_leaks */
