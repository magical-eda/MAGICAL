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

#include "wnlib.h"
#include "wnary.h"
#include "wnio.h"



extern char *malloc();



#define BEGIN_MAGIC      (0x58b2c29d)
#define END_MAGIC        (0x81c102bf)

local bool initialized = FALSE;

local wn_gpstack current_gpstack;

local wn_memgp group_list = NULL;
local wn_memgp default_group = NULL;

bool wn_gp_fill_flag=FALSE,
     wn_gp_pad_flag=FALSE,
     wn_gp_trap_address_flag=FALSE;
ptr wn_gp_trap_address_address;

ptr wn_system_alloc(/*size*/);

local void default_error_func();
typedef void (*voidfunc)();
local voidfunc perror_func = (default_error_func);



wn_print_gp_stack()
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



local print_group(group)

wn_memgp group;

{
  char *label;

  if(group == NULL)
  {
    printf("%20s  %x","",(ptr)group);
  }
  else
  {
    label = group->label;

    if(label == NULL)
    {
      label = "(NULL)";
    }

    printf("%20s  %x",label,(ptr)group);
  }
}



wn_memgp wn_curgp()
{
  wn_assertmsg(current_gpstack->stack_ptr >= 0,"wn_curgp: no group.");

  return(current_gpstack->current_group);
}



wn_gppush(group)

wn_memgp group;

{
  ++(current_gpstack->stack_ptr);

  wn_assertmsg(current_gpstack->stack_ptr < GROUP_STACK_SIZE,
               "wn_gppush: stack overflow."); 

  (current_gpstack->group_stack)[current_gpstack->stack_ptr] =
                                      current_gpstack->current_group;
  current_gpstack->current_group = group;
}



wn_gppop()
{
  current_gpstack->current_group =
               (current_gpstack->group_stack)[current_gpstack->stack_ptr];

  --(current_gpstack->stack_ptr);
}



local initialize_memory_allocator()
{
  make_system_gpstack(&current_gpstack);   

  initialized = TRUE;
}



wn_memgp wn_defaultgp()
{
  if(not(initialized))
  {
    initialize_memory_allocator();
  }

  if(default_group == NULL)
  {
    make_memory_group(&default_group,"general_free",(wn_memgp)NULL);

    label_group("default_group",default_group);
  }

  return(default_group);
}



wn_gpmake(parms)

char parms[];

{
  wn_memgp ret;

  if(not(initialized))
  {
    initialize_memory_allocator();
  }

  make_memory_group(&ret,parms,current_gpstack->current_group);
  wn_gppush(ret);
}



local make_memory_group(pgroup,parms,parent_group)

wn_memgp *pgroup;
char parms[];
wn_memgp parent_group;

{
  int parmc;
  char **parmv;

  *pgroup = (wn_memgp)wn_system_alloc(wn_sizeof(struct wn_memgp_struct));

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

  (*pgroup)->block_size = 4000;
  (*pgroup)->block_ptr = NULL;
  (*pgroup)->block_mem_left = 0;
  (*pgroup)->mem_used = 0;

  (*pgroup)->pad_list = NULL;
  (*pgroup)->children = NULL;

  (*pgroup)->label = NULL;

  wn_parse_into_args(&parmc,&parmv,parms);

  if(wn_streq(parmv[0],"no_free"))
  {
    wn_initialize_group_for_no_free(*pgroup);
  }
  else if(wn_streq(parmv[0],"general_free"))
  {
    wn_initialize_group_for_general_free(*pgroup);
  }
  else
  {
    wn_crashmsg("make_memory_group: illegal type of memory group");
  }

  if(parmc > 1)
  {
    sscanf(parmv[1],"%d",&((*pgroup)->block_size));
  }
}



wn_gpfree()
{
  free_group(current_gpstack->current_group);
  wn_gppop();
}



local free_group(group)

wn_memgp group;

{
  free_groups_children(group);

  free_group_memory(group);

  remove_group_from_group_list(group);
  remove_group_from_parent_subgroup_list(group);

  wn_blkfill((ptr)group,wn_sizeof(struct wn_memgp_struct),'f'); 

  wn_system_free((ptr)group,wn_sizeof(struct wn_memgp_struct));
}



local free_groups_children(group)

wn_memgp group;

{
  wn_memgp child,next;

  next = group->children;

  while(TRUE)
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



local free_group_memory(group)

wn_memgp group;

{
  wn_mem_block block,next;

  next = group->current_block;
  while(TRUE)
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



local remove_group_from_group_list(group)

wn_memgp group;

{
  *(group->plast) = group->next;
  if(group->next != NULL)
  {
    group->next->plast = group->plast;
  }
}



local remove_group_from_parent_subgroup_list(group)

wn_memgp group;

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



#define align(_size) \
{\
  (_size) = ((_size)+3)&(~3);\
}



ptr wn_zalloc(size)

int size;

{
  ptr p;

  if(size == 0)
  {
    return(NULL);
  }

  align(size);

  p = (*(current_gpstack->current_group->palloc_piece))(
                            size,current_gpstack->current_group);

  wn_blkzero(p,size);

  return(p);
}



ptr wn_alloc(size)

int size;

{
  if(size == 0)
  {
    return(NULL);
  }

  align(size);

  return(
          (*(current_gpstack->current_group->palloc_piece))(
                            size,current_gpstack->current_group)
        );
}



wn_free(p)

ptr p;

{
  if(p == NULL)  /* zero size block from wn_alloc */
  {
    return;
  }

  (*(current_gpstack->current_group->pfree_piece))(
                                          p,current_gpstack->current_group);
}


 
wn_realloc(pp,old_size,new_size)

ptr *pp;
int old_size,new_size;

{
  ptr new_p;

  new_p = wn_alloc(new_size);
  wn_blkcpy(new_p,*pp,old_size);
  wn_free(*pp);

  *pp = new_p;
}



wn_zrealloc(pp,old_size,new_size)

ptr *pp;
int old_size,new_size;

{
  ptr new_p;

  new_p = wn_zalloc(new_size);
  wn_blkcpy(new_p,*pp,old_size);
  wn_free(*pp);

  *pp = new_p;
}



ptr wn_system_alloc(size)

int size;

{
  ptr ret;

  ret = (ptr)malloc((unsigned int)size);

  if(ret == NULL)  /* out of memory */
  {
    (*perror_func)(size);  /* print out of memory message and crash,
                              or free up necessary memory */

    ret = (ptr)malloc((unsigned int)size);   /* try again */

    if(ret == NULL)    
    {
      wn_crashmsg("out of memory");  /* no second chance */
    }
  }

  if(wn_gp_fill_flag)
  {
    wn_blkfill(ret,size,'a'); 
  }

  return(ret);
}



wn_system_free(mem,size)

ptr mem;
int size;

{
  if(wn_gp_fill_flag)
  {
    wn_blkfill(mem,size,'f'); 
  }

  cfree((char *)mem);
}



wn_gperrfpush(pfunc)

void (*pfunc)();

{
  perror_func = pfunc;
}



/*ARGSUSED*/ local void default_error_func(size)

int size;

{
  wn_crashmsg("out of memory");
}



wn_gp_fill()
{
  wn_gp_fill_flag = TRUE;
}



wn_gp_pad()
{
  wn_gp_pad_flag = TRUE;
}



wn_stack_fill()
{
  stack_fill(100000);  /* fill 64k of stack */
}



local stack_fill(size)

int size;

{
  int filler[1000/wn_sizeof(int)];

  if(size < 0)
  {
    return;
  }

  wn_blkfill((ptr)filler,1000,'s');

  stack_fill(size-1000);
}



wn_gp_trap_address(address)

ptr address;

{
  wn_gp_trap_address_flag = TRUE;
  wn_gp_trap_address_address = address;
}



wn_allmem_verify()
{
  verify_group_stack();
  verify_all_groups();
}



local verify_group_stack()
{
  int i;
  wn_memgp group;

  verify_group_stack_group(current_gpstack->current_group);

  for(i=current_gpstack->stack_ptr;i>0;i--) /* don't include 0, 0 always NULL */
  {
    group = (current_gpstack->group_stack)[i];

    verify_group_stack_group(group);
  }
}



local verify_group_stack_group(group)

wn_memgp group;

{
  bool group_is_in_group_list();

  wn_assertmsg(group_is_in_group_list(group),
               "bad group in group stack.");
}


    
local bool group_is_in_group_list(group)

wn_memgp group;

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



local verify_all_groups()
{
  wn_memgp group;

  for(group=group_list;group != NULL;group=group->next)
  {
    verify_group(group);
  }
}



wn_group_verify(group)

wn_memgp group;

{
  bool group_is_in_group_list();

  wn_assertmsg(group_is_in_group_list(group),
               "bad group passed.");

  verify_group(group);
}



local verify_group(group)

wn_memgp group;

{
  generic_verify_group(group);

  (*(group->pverify_group))(group);
}



local generic_verify_group(group)

wn_memgp group;

{
  wn_assertmsg(group->begin_magic == BEGIN_MAGIC,
               "memory group begin magic violation");
  wn_assertmsg(group->end_magic == END_MAGIC,
               "memory group end magic violation");
}



wn_print_mem_used()
{
  char string[150];

  (void)sprintf(string,"total memory used = %d\n",wn_mem_used());
  wn_fputs(string,wn_stdout);
  wn_fflush(wn_stdout);
}



int wn_mem_used()
{
  int total;
  wn_memgp group;

  total = 0;

  for(group=group_list;group != NULL;group=group->next)
  {
    total += memory_used_in_group(group);
  }

  return(total);
}



local int memory_used_in_group(group)

wn_memgp group;

{
  return(group->mem_used);
}



wn_print_group_mem_used(group)

wn_memgp group;

{
  char string[150];

  (void)sprintf(string,"total memory used for group = %d\n",
                       wn_group_mem_used(group));
  wn_fputs(string,wn_stdout);
  wn_fflush(wn_stdout);
}



wn_print_all_groups_mem_used()
{
  int total,used,num_groups;
  wn_memgp group;

  total = 0;
  num_groups = 0;

  printf("=======================================\n"); 
  printf("                       group       used\n"); 
  printf("=======================================\n"); 

  for(group=group_list;group != NULL;group=group->next)
  {
    used = memory_used_in_group(group);

    print_group(group);
    printf("  %9d\n",used); 

    total += used;
    num_groups++;
  }

  printf("---------------------------------------\n"); 
  printf("          total (%3d groups)  %9d\n",num_groups,total); 
  printf("=======================================\n"); 
}



int wn_group_mem_used(group)

wn_memgp group;

{
  int total;

  total = memory_used_in_group(group);

  return(total);
}



bool wn_mem_in_group(p,group)

ptr p;
wn_memgp group;

{
  bool memory_is_in_block();
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



local bool memory_is_in_block(p,block)

ptr p;
wn_mem_block block;

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



wn_gplabel(label)

char *label;

{
  char *label_copy;
  int save_mem_used;

  wn_assertmsg(current_gpstack->stack_ptr >= 0,"wn_gplabel: no group.");
   
  save_mem_used = current_gpstack->current_group->mem_used;

  wn_stracpy(&label_copy,label);
  label_group(label_copy,current_gpstack->current_group);

  current_gpstack->current_group->mem_used = save_mem_used;
}



local label_group(label,group)

char *label;
wn_memgp group;

{
  wn_assertmsg(group->label == NULL,"label_group: already labeled.");

  group->label = label;
}



local make_system_gpstack(pstack)    

wn_gpstack *pstack;

{
  *pstack = (wn_gpstack)wn_system_alloc(wn_sizeof(struct wn_gpstack_struct));

  (*pstack)->stack_ptr = -1;
  (*pstack)->group_stack = (wn_memgp *)
                        wn_system_alloc(GROUP_STACK_SIZE*wn_sizeof(wn_memgp));
  (*pstack)->current_group = NULL;
}



wn_make_gpstack(pstack)    

wn_gpstack *pstack;

{
  *pstack = wn_znew(wn_gpstack);

  (*pstack)->stack_ptr = -1;
  (*pstack)->group_stack = (wn_memgp *)
                               wn_zalloc(GROUP_STACK_SIZE*wn_sizeof(wn_memgp));
  (*pstack)->current_group = NULL;
}



wn_get_current_gpstack(pstack)

wn_gpstack *pstack;

{
  *pstack = current_gpstack;
}



wn_set_current_gpstack(stack)

wn_gpstack stack;

{
  current_gpstack = stack;
}


