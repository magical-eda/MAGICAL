/**********************************************************************

  wn_initialize_group_for_general_free(group)

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



extern bool wn_gp_fill_flag,wn_gp_pad_flag,wn_gp_trap_address_flag;
extern ptr wn_gp_trap_address_address;
extern ptr wn_system_alloc();



#define PAD_BEGIN_MAGIC  0x53562946
#define PAD_END_MAGIC    0xabcd5524

typedef struct pad_data_struct *pad_data;
struct pad_data_struct
{
  int begin_magic;
  pad_data *plast,next;
  int end_magic;
};



#define SMALL        100
#define SMALL_SIZE   (SMALL<<2)

typedef struct free_block_data_struct *free_block_data;
typedef struct free_list_data_struct *free_list_data;
struct free_list_data_struct
{
  free_block_data small_blocks[SMALL];
  free_block_data big_blocks;
};
  
struct free_block_data_struct
{
  int size;
  free_block_data next;    
};
  
typedef struct allocated_block_data_struct *allocated_block_data;
struct allocated_block_data_struct
{
  int size;
  int memory;     
};
  


wn_initialize_group_for_general_free(group)

wn_memgp group;

{
  void verify_group(/*group*/),
       free_piece(/*p,group*/);
  ptr alloc_piece(/*size,group*/);

  group->pverify_group = (verify_group);
  group->pfree_piece = (free_piece);
  group->palloc_piece = (alloc_piece);

  make_free_list((free_list_data *)(&(group->free_list)),group);
}



local make_free_list(pfree_list,group)

free_list_data *pfree_list;
wn_memgp group;

{
  simple_alloc((ptr *)pfree_list,
               wn_sizeof(struct free_list_data_struct),group);

  wn_blkzero((ptr)(*pfree_list),wn_sizeof(struct free_list_data_struct)); 
}



local ptr alloc_piece(size,group)

int size;
wn_memgp group;

{
  int old_size,pad_offset;
  allocated_block_data allocated_block;
  ptr ret;

  if(wn_gp_pad_flag)
  {
    old_size = size;
    size += wn_sizeof(struct pad_data_struct);
  }

  allocate_block(&allocated_block,size,group);

  ret = (ptr)(&(allocated_block->memory));

  if(wn_gp_pad_flag)
  {
    pad_offset = allocated_block->size - wn_sizeof(struct pad_data_struct);

    make_pad((pad_data)(ret + pad_offset),group);

    group->mem_used += pad_offset;
  }
  else
  {
    group->mem_used += allocated_block->size;
  }
  if(wn_gp_fill_flag)
  {
    if(wn_gp_pad_flag)
    {
      wn_blkfill(ret,old_size,'a');
    }
    else
    {
      wn_blkfill(ret,size,'a');
    }
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



local allocate_block(pallocated_block,size,group)

allocated_block_data *pallocated_block;
int size;
wn_memgp group;

{
  get_memory_from_free_list(pallocated_block,
                            size,(free_list_data)(group->free_list));

  if(*pallocated_block == NULL)
  {
    get_new_memory(pallocated_block,size,group);
  }
}



local get_memory_from_free_list(pallocated_block,size,free_list)

allocated_block_data *pallocated_block;
int size;
free_list_data free_list;

{
  if(size < SMALL_SIZE)
  {
    get_memory_from_small_blocks(pallocated_block,size,free_list);
  }
  else
  {
    get_memory_from_big_blocks(pallocated_block,size,free_list);
  }
}



local get_memory_from_small_blocks(pallocated_block,size,free_list)

allocated_block_data *pallocated_block;
int size;
free_list_data free_list;

{
  free_block_data *pfree_block;

  pfree_block = &((free_list->small_blocks)[size>>2]);

  if(*pfree_block != NULL)
  {
    *pallocated_block = (allocated_block_data)(*pfree_block); 

    remove_free_block_from_list(pfree_block);
  }
  else
  {
    *pallocated_block = NULL;
  }
}



local get_memory_from_big_blocks(pallocated_block,size,free_list)

allocated_block_data *pallocated_block;
int size;
free_list_data free_list;

{
  free_block_data *pfree_block,*pfound_free_block;

  pfound_free_block = NULL;

  for(pfree_block = &(free_list->big_blocks);
      *pfree_block != NULL;
      pfree_block = &((*pfree_block)->next))
  {
    if(((*pfree_block)->size >= size)&&((*pfree_block)->size < 2*size))
    {
      if(
          (pfound_free_block == NULL)
            ||
          ((*pfree_block)->size < (*pfound_free_block)->size)
        )
      {
        pfound_free_block = pfree_block;
      }
    }
  }

  if(pfound_free_block != NULL)
  {
    *pallocated_block = (allocated_block_data)(*pfound_free_block); 

    remove_free_block_from_list(pfound_free_block);
  }
  else
  {
    *pallocated_block = NULL;
  }
}



local remove_free_block_from_list(pfree_block)

free_block_data *pfree_block;

{
  *pfree_block = (*pfree_block)->next;
}



local get_new_memory(pallocated_block,size,group)

allocated_block_data *pallocated_block;
int size;
wn_memgp group;

{
  simple_alloc((ptr *)pallocated_block,
               size+wn_sizeof(int),group);

  (*pallocated_block)->size = size;
}


   
local simple_alloc(pmem,size,group)

ptr *pmem;
int size;
wn_memgp group;

{
  if(size >= group->block_mem_left)
  {
    get_more_memory(size,group);
  }

  *pmem = group->block_ptr;
  group->block_ptr += size;
  group->block_mem_left -= size;
}



local get_more_memory(size,group)

int size;
wn_memgp group;

{
  free_block_data free_block;
  wn_mem_block block;
  int size_to_get,size_to_alloc;

  if(group->block_mem_left > 4)
  {
    free_block = (free_block_data)(group->block_ptr);
    free_block->size = group->block_mem_left - wn_sizeof(int);

    insert_free_block_into_free_list((free_list_data)(group->free_list),
                                     free_block);
  }
  else if(group->block_mem_left > 0)
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
  allocated_block_data allocated_block;

  allocated_block = (allocated_block_data)(p-wn_sizeof(int));

  if(wn_gp_pad_flag)
  {
    delete_pad(
       (pad_data)(p + allocated_block->size - wn_sizeof(struct pad_data_struct))
              );
    group->mem_used -=
        (allocated_block->size - wn_sizeof(struct pad_data_struct));
  }
  else
  {
    group->mem_used -= allocated_block->size;
  }

  if(wn_gp_fill_flag)
  {
    wn_blkfill(p,allocated_block->size,'f');
  }

  insert_free_block_into_free_list((free_list_data)(group->free_list),
                                   (free_block_data)allocated_block);
}


 
local insert_free_block_into_free_list(free_list,free_block)

free_list_data free_list;
free_block_data free_block;

{
  int size;

  size = free_block->size;

  if(size < SMALL_SIZE)
  {
    insert_into_block_list(&((free_list->small_blocks)[size>>2]),free_block);  
  }
  else
  {
    insert_into_block_list(&(free_list->big_blocks),free_block);  
  }
}



local insert_into_block_list(pblock_list,free_block)

free_block_data *pblock_list,free_block;

{
  free_block->next = *pblock_list;
  *pblock_list = free_block;
}



local make_pad(pad,group)

pad_data pad;
wn_memgp group;

{
  pad_data next;

  pad->begin_magic = PAD_BEGIN_MAGIC;
  pad->end_magic = PAD_END_MAGIC;

  next = (pad_data)(group->pad_list);
  if(next != NULL)
  {
    next->plast = &(pad->next);
  }
  pad->next = next;
  group->pad_list = (ptr)pad;
  pad->plast = (pad_data *)(&(group->pad_list));
}



local delete_pad(pad)

pad_data pad;

{
  pad_data next;

  next = pad->next;
  if(next != NULL)
  {
    next->plast = pad->plast;
  }
  *(pad->plast) = next;
}



local void verify_group(group)

wn_memgp group;

{
  if(wn_gp_pad_flag)
  {
    verify_pad_list((pad_data)(group->pad_list));
  }

  verify_free_list(group);
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
  wn_assertmsg(*(pad->plast) == pad,
               "memory overflow detected: pad pointers corrupt.");
}



local verify_free_list(group)

wn_memgp group;

{
  free_list_data free_list;
  int size;

  free_list = (free_list_data)(group->free_list);

  for(size = 4;size < SMALL_SIZE;size += 4)
  {
    verify_small_free_block_list((free_list->small_blocks)[size>>2],size,
                                 group);
  }

  verify_big_free_block_list(free_list->big_blocks,group);
}



local verify_small_free_block_list(free_block_list,size,group)

free_block_data free_block_list;
int size;
wn_memgp group;

{
  free_block_data free_block;

  for(free_block = free_block_list;
      free_block != NULL;
      free_block = free_block->next)
  {
    verify_small_free_block(free_block,size,group);
  }
}



local verify_small_free_block(free_block,size,group)

free_block_data free_block;
int size;
wn_memgp group;

{
  wn_assertmsg(free_block->size == size,
               "verify_small_free_block: size wrong.");
  wn_assertmsg(free_block->next != free_block,
               "verify_small_free_block: infinite loop.");
  wn_assertmsg(wn_mem_in_group((ptr)free_block,group),
               "verify_small_free_block: free block in wrong group.");
}



local verify_big_free_block_list(free_block_list,group)

free_block_data free_block_list;
wn_memgp group;

{
  free_block_data free_block;

  for(free_block = free_block_list;
      free_block != NULL;
      free_block = free_block->next)
  {
    verify_big_free_block(free_block,group);
  }
}



local verify_big_free_block(free_block,group)

free_block_data free_block;
wn_memgp group;

{
  wn_assertmsg(free_block->size >= SMALL_SIZE,
               "verify_big_free_block: size wrong.");
  wn_assertmsg(free_block->next != free_block,
               "verify_big_free_block: infinite loop.");
  wn_assertmsg(wn_mem_in_group((ptr)free_block,group),
               "verify_big_free_block: free block in wrong group.");
}




