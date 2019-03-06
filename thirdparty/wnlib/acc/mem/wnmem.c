/**********************************************************************

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

  wn_total_wn_memory(void)
  wn_total_memory_in_group(wn_memgp group)

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

  Will Naylor

****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <malloc.h>
#include <string.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnmemb.h"

#include "wnmem.h"



#define BEGIN_MAGIC      1122112211
#define END_MAGIC        1221122112

local bool initialized = FALSE;

local wn_gpstack current_gpstack = NULL;

local wn_memgp group_list = NULL;
local wn_memgp default_group = NULL;

bool wn_gp_fill_flag=FALSE,
     wn_gp_pad_flag=FALSE,
     wn_gp_trap_address_flag=FALSE;
ptr wn_gp_trap_address_address;

ptr wn_system_alloc(/*size*/);

local void default_error_func(int size);
typedef void (*voidfunc)(int size);
local voidfunc perror_func = (default_error_func);

void wn_initialize_group_for_general_free(wn_memgp group);
void wn_initialize_group_for_no_free(wn_memgp group);


local wn_system_memory_alloc_func_type lo_system_memory_alloc_func = &malloc;
local wn_system_memory_free_func_type  lo_system_memory_free_func  = &free;


void wn_set_system_memory_alloc_func(wn_system_memory_alloc_func_type func)
{
  lo_system_memory_alloc_func = func;
}


void wn_set_system_memory_free_func( wn_system_memory_free_func_type  func)
{
  lo_system_memory_free_func  = func;
}


ptr wn_system_alloc(int size)
{
  ptr ret;

  ret = (ptr)(*lo_system_memory_alloc_func)((unsigned int) size);

  if(ret == NULL)  /* out of memory */
  {
    (*perror_func)(size);  /* print out of memory message and crash,
                              or free up necessary memory */

    /* try again */
    ret = (ptr)(*lo_system_memory_alloc_func)((unsigned int) size);

    if(ret == NULL)
    {
      fprintf(stderr,"out of memory.\n");
      wn_assert_notreached();  /* no second chance */
    }
  }

  if(wn_gp_fill_flag)
  {
    wn_memset(ret,'a',size);
  }

  return(ret);
}


void wn_system_free(ptr mem,int size)
{
  if(wn_gp_fill_flag)
  {
    wn_memset(mem,'f',size); 
  }

  (*lo_system_memory_free_func)(mem);
}


local void print_group(wn_memgp group)
{
  char *label;

  if(group == NULL)
  {
    printf("%20s  %lx","",(unsigned long)group);
  }
  else
  {
    label = group->label;

    if(label == NULL)
    {
      label = "(NULL)";
    }

    printf("%20s  %lx",label,(unsigned long)group);
  }
}


void wn_print_gp_stack(void)
{
  int i;
  wn_memgp group;

  printf("************** memory group stack *************\n");

  printf("top -> ");    /* top of stack */
  print_group(current_gpstack->current_group);
  printf("\n");

  for(i=current_gpstack->stack_ptr;i>=0;i--)
  {
    group = (current_gpstack->group_stack)[i];

    printf("       "); 
    print_group(group);
    printf("\n");
  }

  printf("***********************************************\n");
}


void wn_gppush(wn_memgp group)
{
  ++(current_gpstack->stack_ptr);

  if (current_gpstack->stack_ptr >= GROUP_STACK_SIZE)
  {
    fprintf(stderr, "wnlib: ERROR: memory group stack overflow\n");
    wn_assert_notreached();
  }

  (current_gpstack->group_stack)[current_gpstack->stack_ptr] =
                                      current_gpstack->current_group;
  current_gpstack->current_group = group;
}


void wn_gppop(void)
{
  current_gpstack->current_group =
               (current_gpstack->group_stack)[current_gpstack->stack_ptr];

  --(current_gpstack->stack_ptr);

  /* it's an error to pop the last group (the default group) off the stack */
  if (current_gpstack->stack_ptr < 0)
  {
    fprintf(stderr, "wnlib: ERROR: memory group stack underflow\n");
    wn_assert_notreached();
  }
}


local void make_system_gpstack(wn_gpstack *pstack)    
{
  *pstack = (wn_gpstack)wn_system_alloc(sizeof(struct wn_gpstack_struct));

  (*pstack)->stack_ptr = -1;
  (*pstack)->group_stack = (wn_memgp *)
                        wn_system_alloc(GROUP_STACK_SIZE*sizeof(wn_memgp));
  (*pstack)->current_group = NULL;
}


void wn_make_gpstack(wn_gpstack *pstack)    
{
  *pstack = (wn_gpstack)wn_zalloc(sizeof(struct wn_gpstack_struct));

  (*pstack)->stack_ptr = -1;
  (*pstack)->group_stack = (wn_memgp *)
                               wn_zalloc(GROUP_STACK_SIZE*sizeof(wn_memgp));
  (*pstack)->current_group = NULL;
}


void wn_get_current_gpstack(wn_gpstack *pstack)
{
  *pstack = current_gpstack;
}


void wn_set_current_gpstack(wn_gpstack stack)
{
  current_gpstack = stack;
}


local void initialize_memory_allocator(void)
{
  make_system_gpstack(&current_gpstack);   

  initialized = TRUE;

  /*     make and push default memory group onto the bottom of the group
  ** stack.  It will be an error to pop it off. */
  wn_gppush(wn_defaultgp());
}


wn_memgp wn_curgp(void)
{
  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  /*     these checks are really not needed: stack_ptr is checked on every
  ** push or pop as it is. */
  wn_fast_assert(current_gpstack->stack_ptr >= 0);
  wn_fast_assert(current_gpstack->stack_ptr < GROUP_STACK_SIZE);

  return(current_gpstack->current_group);
}


local void make_memory_group 
(
  wn_memgp *pgroup,
  int flags,
  int blocksize,
  wn_memgp parent_group
)
{
  *pgroup = (wn_memgp)wn_system_alloc(sizeof(struct wn_memgp_struct));

  (*pgroup)->begin_magic = BEGIN_MAGIC;
  (*pgroup)->end_magic = END_MAGIC;

  (*pgroup)->next = group_list;
  if(group_list != NULL)
  {
    group_list->plast = &((*pgroup)->next);
  }
  (*pgroup)->plast = &group_list;
  group_list = (*pgroup);

  if(parent_group != NULL)
  {
    (*pgroup)->sub_next = parent_group->children;
    if(parent_group->children != NULL)
    {
      parent_group->children->psub_last = &((*pgroup)->sub_next);
    }
    (*pgroup)->psub_last = &(parent_group->children);
    parent_group->children = (*pgroup);
  }
  else
  {
    (*pgroup)->sub_next = NULL;
    (*pgroup)->psub_last = NULL;
  }

  (*pgroup)->current_block = NULL;

# define DESIRED_SYSTEM_ALLOC_SIZE  8192
  (*pgroup)->block_size = DESIRED_SYSTEM_ALLOC_SIZE - 4 -
  /**/					sizeof(struct wn_mem_block_struct);
  (*pgroup)->block_ptr = NULL;
  (*pgroup)->block_mem_left = 0;
  (*pgroup)->mem_used = 0;

  (*pgroup)->pad_list = NULL;
  (*pgroup)->children = NULL;

  (*pgroup)->label = NULL;

  wn_assert(!(flags & WN_NO_FREE) != !(flags & WN_GENERAL_FREE));
  if (WN_NO_FREE & flags)
  {
    wn_initialize_group_for_no_free(*pgroup);
  }
  else if (WN_GENERAL_FREE & flags)
  {
    wn_initialize_group_for_general_free(*pgroup);
  }
  else
  {
    fprintf(stderr,"make_memory_group: illegal type of memory group\n");
    wn_assert_notreached();
  }

  if (blocksize)
  {
    (*pgroup)->block_size = blocksize;
  }
}


local void label_group(char *label,wn_memgp group)
{
  wn_assert(group->label == NULL);

  group->label = label;
}


void wn_gplabel(const char *label)
{
  char *label_copy;
  int save_mem_used;

  wn_assert(current_gpstack->stack_ptr >= 0);
   
  save_mem_used = current_gpstack->current_group->mem_used;

  label_copy = (char *)wn_alloc(strlen(label)+1);
  strcpy(label_copy,label);
  label_group(label_copy,current_gpstack->current_group);

  current_gpstack->current_group->mem_used = save_mem_used;
}


wn_memgp wn_defaultgp(void)
{
  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  if(default_group == NULL)
  {
    make_memory_group(&default_group, WN_GENERAL_FREE, 4000, (wn_memgp)NULL);

    label_group("default_group",default_group);
  }

  return(default_group);
}


void wn_gpmake(const char parms[])
{
  char group_type[50];
  int group_size;
  int args;
  int flags;
  wn_memgp ret;

  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  args = sscanf(parms, "%s %d", group_type, &group_size);
  wn_assert(args > 0  &&  args <= 2);

  if(strcmp(group_type,"no_free") == 0)
  {
    flags = WN_NO_FREE;
  }
  else if(strcmp(group_type,"general_free") == 0)
  {
    flags = WN_GENERAL_FREE;
  }
  else
  {
    fprintf(stderr,"wn_gpmake: illegal type of memory group\n");
    wn_assert_notreached();
  }

  if (2 > args)
  {
    group_size = 0;
  }

  make_memory_group(&ret, flags, group_size, current_gpstack->current_group);

  wn_gppush(ret);
}


void wn_gpmakef(int flags, int block_size)
{
  wn_memgp ret;

  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  wn_assert(!(flags & WN_NO_FREE) != !(flags & WN_GENERAL_FREE));

  make_memory_group(&ret, flags, block_size, current_gpstack->current_group);

  wn_gppush(ret);
} /* wn_gpmakef */


local void free_group(wn_memgp group);

local void free_groups_children(wn_memgp group)
{
  wn_memgp child,next;

  next = group->children;

  for(;;)
  {
    if(next == NULL)
    {
      break;
    }

    child = next;
    next = child->sub_next;

    free_group(child);
  }
}


local void free_group_memory(wn_memgp group)
{
  wn_mem_block block,next;

  next = group->current_block;
  for(;;)
  {
    if(next == NULL)
    {
      break;
    }

    block = next;
    next = block->next;

    wn_system_free((ptr)block,block->size);
  }
}


local void remove_group_from_group_list(wn_memgp group)
{
  *(group->plast) = group->next;
  if(group->next != NULL)
  {
    group->next->plast = group->plast;
  }
}


local void remove_group_from_parent_subgroup_list(wn_memgp group)
{
  if(group->psub_last != NULL)
  {
    *(group->psub_last) = group->sub_next;
    if(group->sub_next != NULL)
    {
      group->sub_next->psub_last = group->psub_last;
    }
  }
}


local FILE *lo_trace_group_fp = NULL;
local long unsigned lo_last_total_wn_memory = 0;
local int lo_traces_displayed = 0;

local void lo_trace_group_one_line(FILE *out, wn_memgp group)
{
  wn_mem_block block;
  long unsigned total_memory, leftover;
  long int free;
  double percent_wasted, percent_free, percent_leftover;
  char percent_free_buf[10];

  if (!lo_traces_displayed)
  {
    fprintf(out, "    %14s  %6s  %5s %9s %16s label\n", "total bytes",
    /**/			"%waste", "%free", "%leftover", "total wn mem");
    fprintf(out, "    %14s  %6s  %5s %9s %16s ------------------\n",
    /**/	"--------------", "------", "-----", "---------",
    /**/						"----------------");
  }

  if (!(lo_traces_displayed % 3))
  {
    fprintf(out, "\n");
  }

  total_memory = wn_total_memory_in_group(group);

  percent_wasted = 100.0 * (total_memory - wn_group_mem_used(group)) / 
  /**/							total_memory;

  free = wn_amount_of_free_memory_in_group(group);
  if (-1 == free)
  {
    strcpy(percent_free_buf, "no");
  }
  else
  {
    percent_free = 100.0 * free / total_memory;
    sprintf(percent_free_buf, "%6.2f", percent_free);
  }

  leftover = 0;
  for (block = group->current_block;  block;  block = block->next)
  {
    wn_assert(block->size > 0);
    leftover += block->leftover;
  }

  percent_leftover = 100.0 * leftover / total_memory;

  fprintf(out, "%3d %14lu %7.2f %6s %9.2f %16lu %s\n",
  /**/	lo_traces_displayed, total_memory, percent_wasted, percent_free_buf,
  /**/	percent_leftover, wn_total_wn_memory(), group->label ? group->label : "");
  fflush(out);

  ++ lo_traces_displayed;
} /* lo_trace_group_one_line */


local void lo_trace_group_free_start(wn_memgp group)
{
  static bool first_time = TRUE;
  char *pfn;
  long unsigned new_total_wn_memory;

  if (first_time)
  {
    first_time = FALSE;

    pfn = getenv("WNLIB_MEMGP_TRACE_FILE");
    if (pfn)
    {
      lo_trace_group_fp = fopen(pfn, "w");
      setbuf(lo_trace_group_fp, NULL);
    }
  }

  if (!lo_trace_group_fp)
  {
    return;
  }

  new_total_wn_memory = wn_total_wn_memory();
  if (new_total_wn_memory < lo_last_total_wn_memory)
  {
    fprintf(lo_trace_group_fp,
    /**/  "Drop at START of group free, by %13lu %14lu %s\n",
    /**/	lo_last_total_wn_memory - new_total_wn_memory, new_total_wn_memory,
    /**/	group->label ? group->label : "");
  }
  lo_last_total_wn_memory = new_total_wn_memory;

  lo_trace_group_one_line(lo_trace_group_fp, group);
} /* lo_trace_group_free_start */


local void lo_trace_group_free_end()
{
  long unsigned new_total_wn_memory;

  if (!lo_trace_group_fp)
  {
    return;
  }

  new_total_wn_memory = wn_total_wn_memory();
  if (new_total_wn_memory < lo_last_total_wn_memory)
  {
    fprintf(lo_trace_group_fp,
    /**/  "Drop at end   of group free, by %13lu %14lu\n",
    /**/	lo_last_total_wn_memory - new_total_wn_memory, new_total_wn_memory);
  }
  lo_last_total_wn_memory = new_total_wn_memory;
} /* lo_trace_group_free_end */


local void free_group(wn_memgp group)
{
  /* checks, among other things, if group has already been freed */
  wn_assert(BEGIN_MAGIC == group->begin_magic);
  wn_assert(  END_MAGIC == group->  end_magic);

  free_groups_children(group);

  lo_trace_group_free_start(group);

  free_group_memory(group);

  remove_group_from_group_list(group);
  remove_group_from_parent_subgroup_list(group);

  wn_memset(group,'f',sizeof(struct wn_memgp_struct)); 

  wn_system_free((ptr)group,sizeof(struct wn_memgp_struct));

  lo_trace_group_free_end();
}


void wn_gpfree(void)
{
  /* it's illegal to free the default group under any circumstances */
  wn_assert(default_group != current_gpstack->current_group);

  free_group(current_gpstack->current_group);
  wn_gppop();
}


#define align(_size) \
{\
  (_size) = ((_size)+3)&(~3);\
}


ptr wn_zalloc(int size)
{
  ptr p;

  if(size == 0)
  {
    return(NULL);
  }
  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  align(size);

  wn_assert(current_gpstack->stack_ptr >= 0);
  wn_assert(current_gpstack->stack_ptr < GROUP_STACK_SIZE);

  p = (*(current_gpstack->current_group->palloc_piece))(
                            size,current_gpstack->current_group);

  wn_memzero(p,size);

  return(p);
}


ptr wn_alloc(int size)
{
  if(size == 0)
  {
    return(NULL);
  }
  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  align(size);

  wn_assert(current_gpstack->stack_ptr >= 0);
  wn_assert(current_gpstack->stack_ptr < GROUP_STACK_SIZE);

  return(
          (*(current_gpstack->current_group->palloc_piece))(
                            size,current_gpstack->current_group)
        );
}


void wn_free(ptr p)
{
  if(p == NULL)  /* zero size block from wn_alloc */
  {
    return;
  }

  wn_assert(current_gpstack->stack_ptr >= 0);
  wn_assert(current_gpstack->stack_ptr < GROUP_STACK_SIZE);

  (*(current_gpstack->current_group->pfree_piece))(
                                          p,current_gpstack->current_group);
}


void wn_realloc(ptr *pp,int old_size,int new_size)
{
  ptr new_p;

  new_p = wn_alloc(new_size);
  wn_memcpy(new_p,*pp,wn_min(old_size,new_size));
  wn_free(*pp);

  *pp = new_p;
}


void wn_zrealloc(ptr *pp,int old_size,int new_size)
{
  ptr new_p;

  new_p = wn_zalloc(new_size);
  wn_memcpy(new_p,*pp,wn_min(old_size,new_size));
  wn_free(*pp);

  *pp = new_p;
}


void wn_gperrfpush(void (*pfunc)(int size))
{
  perror_func = pfunc;
}


/*ARGSUSED*/ local void default_error_func(int size)
{
  fprintf(stderr,"out of memory\n");
  wn_assert_notreached();
}


void wn_gp_fill(void)
{
  wn_gp_fill_flag = TRUE;
}


void wn_gp_pad(void)
{
  if (sizeof(ptr) > sizeof(int))
  {
    fprintf(stderr,
    /**/  "Warning: pads not implemented on 64-bit: wn_gp_pad() ignored\n");
  }
  else
  {
    wn_gp_pad_flag = TRUE;
  }
}


local void stack_fill(int size)
{
  int filler[1000/sizeof(int)];

  if(size < 0)
  {
    return;
  }

  wn_memset(filler,'s',1000);

  stack_fill(size-1000);
}


void wn_stack_fill(void)
{
  stack_fill(100000);  /* fill 64k of stack */
}


void wn_gp_trap_address(ptr address)
{
  wn_gp_trap_address_flag = TRUE;
  wn_gp_trap_address_address = address;
}


local bool group_is_in_group_list(wn_memgp group)
{
  wn_memgp g;

  for(g=group_list;g != NULL;g=g->next)
  {
    if(group == g)  /* group is known */
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


local void verify_group_stack_group(wn_memgp group)
{
  if(!(group_is_in_group_list(group)))
  {
    fprintf(stderr,"bad group in group stack.\n");
    wn_assert_notreached();
  }
}


local void generic_verify_group(wn_memgp group)
{
  wn_assert(group->begin_magic == (int) BEGIN_MAGIC);
  wn_assert(group->end_magic   == (int) END_MAGIC);
}


local void verify_group(wn_memgp group)
{
  generic_verify_group(group);

  (*(group->pverify_group))(group);
}


local void verify_group_stack(void)
{
  int i;
  wn_memgp group;

  if(current_gpstack == NULL)   /* no stack created yet */
  {
    return;
  }
  if(current_gpstack->stack_ptr == -1)  /* stack is empty */
  {
    return;
  }

  verify_group_stack_group(current_gpstack->current_group);

  for(i=current_gpstack->stack_ptr;i>0;i--) /* don't include 0, 0 always NULL */
  {
    group = (current_gpstack->group_stack)[i];

    verify_group_stack_group(group);
  }
}


local void verify_all_groups(void)
{
  wn_memgp group;

  for(group=group_list;group != NULL;group=group->next)
  {
    verify_group(group);
  }
}


void wn_group_verify(wn_memgp group)
{
  if(!(group_is_in_group_list(group)))
  {
    fprintf(stderr,"bad group passed.\n");
    wn_assert_notreached();
  }

  verify_group(group);
}


void wn_allmem_verify(void)
{
  verify_group_stack();
  verify_all_groups();
}


local long int memory_used_in_group(wn_memgp group)
{
  return(group->mem_used);
}


long int wn_mem_used(void)
{
  long int total;
  wn_memgp group;

  total = 0;

  for(group=group_list;group != NULL;group=group->next)
  {
    total += memory_used_in_group(group);
  }

  return(total);
}


void wn_print_mem_used(void)
{
  char string[150];

  (void)sprintf(string,"total memory used = %ld\n", wn_mem_used());
  fputs(string,stdout);
  fflush(stdout);
}


void wn_print_group_mem_used(wn_memgp group)
{
  char string[150];

  (void)sprintf(string,"total memory used for group = %ld\n",
  /**/						wn_group_mem_used(group));
  fputs(string,stdout);
  fflush(stdout);
}


void wn_print_all_groups_mem_used(void)
{
  long int total_used;
  wn_memgp group;

  lo_traces_displayed = 0;

  total_used = 0;
  for(group = group_list; group != NULL; group = group->next)
  {
    lo_trace_group_one_line(stdout, group);

    total_used += wn_total_memory_in_group(group);
  }

  printf("--------------------------------------------------------------\n");
  printf("Total memory used = %22ld\n", total_used);
} /* wn_print_all_groups_mem_used */


long int wn_group_mem_used(wn_memgp group)
{
  long int total;

  total = memory_used_in_group(group);

  return(total);
}


/* total memory in all the blocks in the group, including wasted space.
** Does not include children of group. */

long unsigned wn_total_memory_in_group(wn_memgp group)
{
  long unsigned total = 0;
  wn_mem_block block;

  for (block = group->current_block;  block;  block = block->next)
  {
    wn_assert(block->size > 0);
    total += block->size;
  }

  total += sizeof(*group);

#if 0 /* ignore memory in children */
  /* note wn_total_wn_memory() depends on child memory not being counted */
  for (child = group->children;  child;  child = child->sub_next)
  {
    total += wn_total_memory_in_group(child);
  }
#endif

  return total;
} /* wn_total_memory_in_group */


/* total memory allocated by wn.  Does not include malloc overhead. */

long unsigned wn_total_wn_memory(void) {
  wn_memgp group;
  long unsigned total;

  if(!(initialized))
  {
    initialize_memory_allocator();
  }

  total = 0;
  for (group = group_list;  group;  group = group->next) {
    total += wn_total_memory_in_group(group);
  }

  return total;
} /* wn_total_wn_memory */


#if 0
/*     wn_heapsize() depends on sbrk(), which is no longer a valid measure
** of heapsize since malloc now often gets large allocations from mmap(),
** not from sbrk().  Also, sbrk()'s not part of POSIX and not supported on
** windows, so we're killing all uses of it. */
  #if defined(__unix) && ! defined(lint)
  #ifdef	__cplusplus
  extern "C" {
  #endif
  extern char *sbrk(int);
  extern int _end;
  #ifdef	__cplusplus
  }
  #endif
  #endif


  long unsigned wn_heapsize(void)
  {
      long unsigned mem;

  #if defined(__unix) && ! defined(lint) && ! defined(__CYGWIN__)
      mem = (char *) sbrk(0) - (char *) &_end;
  #else
      mem = 0;
  #endif

      return mem;
  }
#endif


local bool memory_is_in_block(ptr p,wn_mem_block block)
{
  long unsigned int start,fin,p_address;

  p_address = (long unsigned int)p;

  start = (long unsigned int)block;
  fin = start + block->size;

  return(
          (start <= p_address)
            &&
          (p_address < fin)
        );
}


bool wn_mem_in_group(ptr p,wn_memgp group)
{
  wn_mem_block block;

  for(block = group->current_block;
      block != NULL;
      block = block->next)
  {
    if(memory_is_in_block(p,block))
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


#if 0 /* unused - bchapman 041111 */
local void
group_memory_fragmentation_report(wn_memgp group, int level)
{
  /* stub */
}
#endif /* 0 */


void
wn_memory_fragmentation_report()
{
  /* stub */
}


void wn_abandon_memory_leaks(bool abandon_default_group)
{
  /* stub */
}
