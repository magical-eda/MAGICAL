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

#include "wnlib.h"
#include "wnmax.h"


#define PAD_BEGIN_MAGIC  (0x53562946)
#define PAD_END_MAGIC    (0xabcd5524)

typedef struct pad_data_struct *pad_data;
struct pad_data_struct
{
  int begin_magic;
  pad_data next;
  int end_magic;
};


extern bool wn_gp_fill_flag,wn_gp_pad_flag,wn_gp_trap_address_flag;
extern ptr wn_gp_trap_address_address;
extern ptr wn_system_alloc();



wn_initialize_group_for_no_free(group)

wn_memgp group;

{
  void verify_group(/*group*/),
       free_piece(/*p,group*/);
  ptr alloc_piece(/*size,group*/);

  group->pverify_group = (verify_group);
  group->pfree_piece = (free_piece);
  group->palloc_piece = (alloc_piece);
}



local ptr alloc_piece(size,group)

int size;
wn_memgp group;

{
  int old_size;
  ptr ret;

  if(wn_gp_pad_flag)
  {
    old_size = size;
    size += wn_sizeof(struct pad_data_struct);
  }

  if(size >= group->block_mem_left)
  {
    get_more_memory(size,group);
  }

  ret = group->block_ptr;
  group->block_ptr += size;
  group->block_mem_left -= size;
  group->mem_used += size;

  if(wn_gp_pad_flag)
  {
    fill_pad((pad_data)(ret+old_size),group);
  }
  if(wn_gp_trap_address_flag)
  {
    if(ret == wn_gp_trap_address_address)
    {
      wn_crashmsg("address found.");
    }
  }

  return(ret);
}



local get_more_memory(size,group)

int size;
wn_memgp group;

{
  wn_mem_block block;
  int size_to_get,size_to_alloc;

  if(group->current_block != NULL)
  {
    group->current_block->leftover += group->block_mem_left;
  }

  size_to_get = wn_max(size,group->block_size);
  size_to_alloc = size_to_get + wn_sizeof(struct wn_mem_block_struct) 
                  - wn_sizeof(int); 
  block = (wn_mem_block)wn_system_alloc(size_to_alloc);

  block->next = group->current_block;
  group->current_block = block;

  block->leftover = 0;
  block->size = size_to_alloc;

  group->block_mem_left = size_to_get;
  group->block_ptr = (ptr)(&(block->memory));
}



/*ARGSUSED*/ local void free_piece(p,group)

ptr p;
wn_memgp group;

{
}


 
local fill_pad(pad,group)

pad_data pad;
wn_memgp group;

{
  pad->begin_magic = PAD_BEGIN_MAGIC;
  pad->end_magic = PAD_END_MAGIC;

  pad->next = (pad_data)(group->pad_list);
  group->pad_list = (ptr)pad;
}



local void verify_group(group)

wn_memgp group;

{
  if(wn_gp_pad_flag)
  {
    verify_pad_list((pad_data)(group->pad_list));
  }
}



local verify_pad_list(pad_list)

pad_data pad_list;

{
  pad_data pad;

  for(pad=pad_list;pad != NULL;pad=pad->next)
  {
    verify_pad(pad);
  }
}



local verify_pad(pad)

pad_data pad;

{
  wn_assertmsg(pad->begin_magic == PAD_BEGIN_MAGIC,
               "memory overflow detected: pad begin corrupt.");
  wn_assertmsg(pad->end_magic == PAD_END_MAGIC,
               "memory overflow detected: pad end corrupt.");
}



