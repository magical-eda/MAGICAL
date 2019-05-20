/**********************************************************************

  wn_initialize_group_for_no_free(group)

  local ptr alloc_piece(size,group)
  local ptr free_piece(p,group)

  local void verify_group(group)
  local int memory_used_in_group(group)

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

#include "wnlib.h"
#include "wnmax.h"
#include "wnasrt.h"

#include "wnmem.h"


#define PAD_BEGIN_MAGIC  (2111222111)
#define PAD_END_MAGIC    (1222111222)

typedef struct pad_type_struct *pad_type;
struct pad_type_struct
{
  unsigned begin_magic;
  pad_type next;
  unsigned end_magic;
};

/* the following type is the same as the above but its size is 8-aligned */
union pad_type_8aligned_union
{
  struct pad_type_struct p;
  double d;
};


extern bool wn_gp_fill_flag,wn_gp_pad_flag,wn_gp_trap_address_flag;
extern ptr wn_gp_trap_address_address;
extern ptr wn_system_alloc(int);


local void get_more_memory(int size,wn_memgp group)
{
  wn_mem_block block;
  int size_to_get,size_to_alloc;

  if(group->current_block != NULL)
  {
    group->current_block->leftover += group->block_mem_left;
  }

  size_to_get = wn_max(size, group->block_size);
  /* we add in 4 here in case we have to realign the block_ptr */
  size_to_alloc = size_to_get + 4 + sizeof(struct wn_mem_block_struct);
  block = (wn_mem_block)wn_system_alloc(size_to_alloc);
  wn_assert(sizeof(ptr) < 8 || !(((long unsigned) block) & 7));

  block->next = group->current_block;
  group->current_block = block;

  block->leftover = 0;
  block->size = size_to_alloc;

  group->block_ptr = WN_MEM_BLOCK_MEMORY(block);
  group->block_end_ptr = (ptr)((char*)block + size_to_alloc);
  group->block_mem_left = (char *) group->block_end_ptr -
  /**/						(char *) group->block_ptr;
  /* make sure block_ptr is 8 bit aligned.  (Note under valgrind on linux,
  ** malloc sometimes returns non 8-byte aligned segments.) */
  if ((unsigned long)group->block_ptr & 0x7)
  {
    group->block_ptr = (ptr) ((char *)group->block_ptr + 4);
    group->block_mem_left -= 4;
    block->leftover += 4;
  }
  wn_assert(!((unsigned long)group->block_ptr & 0x7));
  wn_assert(!((unsigned long)group->block_end_ptr & 0x3));
  wn_assert(group->block_mem_left >= size_to_get);
} /* get_more_memory */


local void fill_pad(pad_type pad,wn_memgp group)
{
  pad->begin_magic = PAD_BEGIN_MAGIC;
  pad->end_magic   = PAD_END_MAGIC;

  pad->next = (pad_type)(group->pad_list);
  group->pad_list = (ptr)pad;
}


local ptr alloc_piece(int size,wn_memgp group)
{
  int old_size;
  ptr ret;

  old_size = size;
  if(wn_gp_pad_flag)
  {
    size += sizeof(union pad_type_8aligned_union);
  }
  if(size >= group->block_mem_left)
  {
    get_more_memory(size,group);
  }
  if (size & 0x7) /* 4-aligned -- get from end */
  {
    group->block_end_ptr = (ptr) ((char *)group->block_end_ptr - size);
    ret = group->block_end_ptr;
  }
  else /* 8-aligned -- get from start */
  {
    ret = group->block_ptr;
    group->block_ptr = (ptr) ((char *)group->block_ptr + size);
  }
  group->block_mem_left -= size;
  group->mem_used += old_size;
  if(wn_gp_pad_flag)
  {
    fill_pad((pad_type)((char *)ret + old_size), group);
  }
  if(wn_gp_trap_address_flag)
  {
    if(ret == wn_gp_trap_address_address)
    {
      fprintf(stderr,"address found.\n");
      wn_assert_notreached();
    }
  }

  wn_assert(!(((long unsigned)  ret) & 3));
  wn_assert( (((long unsigned) size) & 7) || !(((long unsigned) ret) & 7));

  return(ret);
} /* alloc_piece */


/*ARGSUSED*/ local void free_piece(ptr p,wn_memgp group)
{
}


local void verify_pad(pad_type pad)
{
  wn_assert(pad->begin_magic == PAD_BEGIN_MAGIC);
  wn_assert(pad->end_magic   == PAD_END_MAGIC);
}


local void verify_pad_list(pad_type pad_list)
{
  pad_type pad;

  for(pad=pad_list;pad != NULL;pad=pad->next)
  {
    verify_pad(pad);
  }
}


local void verify_group(wn_memgp group)
{
  if (group->current_block)
  {
    wn_assert(group->block_mem_left >= 0);
    wn_assert(group->block_mem_left == (char *) group->block_end_ptr -
    /**/			       (char *) group->block_ptr);
  }

  wn_assert(*group->plast == group);
  wn_assert(group->mem_used >= 0);

  if(wn_gp_pad_flag)
  {
    verify_pad_list((pad_type)(group->pad_list));
  }
}


void wn_initialize_group_for_no_free(wn_memgp group)
{
  group->pverify_group = (verify_group);
  group->pfree_piece = (free_piece);
  group->palloc_piece = (alloc_piece);
}
