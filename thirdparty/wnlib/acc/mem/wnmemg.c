#define wnmemgC

/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor, Bill Chapman

****************************************************************************/

#include <stdio.h>

#include "wnlib.h"
#include "wnmax.h"
#include "wnasrt.h"
#include "wnmemb.h"

#include "wnmem.h"
#include "wnmbtr.h"


extern bool wn_gp_fill_flag,wn_gp_pad_flag,wn_gp_trap_address_flag;
extern ptr wn_gp_trap_address_address;
extern ptr wn_system_alloc(int);


#define PAD_BEGIN_MAGIC		1212121212
#define PAD_END_MAGIC		1313131313
#define PAD_DELETED_BEGIN_MAGIC	1414141414
#define PAD_DELETED_END_MAGIC	1515151515

typedef struct pad_type_struct *pad_type;
struct pad_type_struct
{
  unsigned begin_magic;
  pad_type *plast, next;
  unsigned end_magic;
};



#define SMALL        100
#define SMALL_SIZE   (SMALL<<2)


typedef struct free_list_type_struct *free_list_type;
struct free_list_type_struct
{
  wn_free_block small_blocks[SMALL];		/* headers of linked lists
  **	of free_blocks, where small_blocks[i>>2] is the list of blocks of
  **	size i */
  wn_mbtree big_blocks_tree;			/* binary tree similar to
  **	wnbtr, key is size, free_block_list is list of free blocks, node
  **	itself is an additional free block */
};

/*   block of allocated memory.  "Size" is size of caller's portion of the
** block, which begins at &memory.  Thus total size of an allocated block is
** (->size + sizeof(int)). */

typedef struct allocated_block_type_struct *allocated_block_type;
struct allocated_block_type_struct
{
  int size;
  int memory;
};


local void insert_into_block_list
(
  wn_free_block *pblock_list,
  wn_free_block free_block
)
{
  WN_FREE_BLOCK_TO_MBHANDLE(free_block)->next_free_block = *pblock_list;
  *pblock_list = free_block;
}


/* debug count of handle chunks allocated through this routine */
/* local int lo_insert_chunks_allocated = 0; ** unused - bchapman 041111 */

local void lo_insert_free_block_into_big_free_list(wn_free_block free_block,
/**/								wn_memgp group)
{
  free_list_type free_list = (free_list_type) group->free_list;
  wn_mbtree tree = free_list->big_blocks_tree;
  wn_mbhandle handle, hdr_handle;
  /* int mbhandle_chunk_size;	 ** unused - bchapman 041111 */
  /* int total_size;		 ** unused - bchapman 041111 */
  /* wn_free_block free_block_2; ** unused - bchapman 041111 */

  wn_assert(free_block->free_block_size >= SMALL_SIZE);
  wn_assert((((long unsigned int) free_block) & 7) == 4);

  handle = WN_FREE_BLOCK_TO_MBHANDLE(free_block);

  wn_mbget(&hdr_handle, tree, (ptr) free_block->free_block_size, WN_MB_EQ);
  if (hdr_handle)
  {
    handle->next_free_block = hdr_handle->free_block_list;
    hdr_handle->free_block_list = free_block;
  }
  else
  {
    wn_mbins(handle, tree, (ptr) free_block->free_block_size);
    handle->next_free_block =
    handle->free_block_list = NULL;
    wn_assert(handle->key == (ptr) free_block->free_block_size);
  }
} /* lo_insert_free_block_into_big_free_list */


local void insert_free_block_into_free_list
(
  wn_free_block free_block,
  wn_memgp group
)
{
  free_list_type free_list = (free_list_type) group->free_list;
  int size;

  /* assert proper alignment */
  wn_assert((((long unsigned int)free_block)&7) == 4);

  size = free_block->free_block_size;

  wn_assert(size + sizeof(int) >= WN_SIZEOF_FREE_BLOCK_STRUCT);

  if (size < SMALL_SIZE)
  {
    insert_into_block_list(&((free_list->small_blocks)[size>>2]), free_block);
  }
  else
  {
    lo_insert_free_block_into_big_free_list(free_block, group);
  }
} /* insert_free_block_into_free_list */


local void remove_free_block_from_list(wn_free_block *pfree_block)
{
  *pfree_block = WN_FREE_BLOCK_TO_MBHANDLE(*pfree_block)->next_free_block;
}


local void get_memory_from_small_blocks
(
  allocated_block_type *pallocated_block,
  int size,
  free_list_type free_list
)
{
  wn_free_block *pfree_block;

  pfree_block = &((free_list->small_blocks)[size>>2]);

  if(*pfree_block != NULL)
  {
    *pallocated_block = (allocated_block_type)(*pfree_block);

    remove_free_block_from_list(pfree_block);
  }
  else
  {
    *pallocated_block = NULL;
  }
}


local void lo_get_free_memory_from_big_blocks
(
  allocated_block_type *pallocated_block,
  int size,
  wn_memgp group
)
{
  free_list_type free_list = (free_list_type) group->free_list;
  wn_mbtree tree = free_list->big_blocks_tree;
  wn_mbhandle handle;

  wn_mbget(&handle, tree, (ptr) size, WN_MB_GE);

  if (handle  &&  (long int) handle->key < 2*size)
  {
    if (handle->free_block_list)
    {
      *pallocated_block = (allocated_block_type) handle->free_block_list;
      remove_free_block_from_list(
      /**/		(wn_free_block *) &handle->free_block_list);
    }
    else
    {
      wn_mbdel(handle, tree);
      *pallocated_block =
      /**/	(allocated_block_type) WN_MBHANDLE_TO_FREE_BLOCK(handle);
    }
  }
  else
  {
    *pallocated_block = NULL;
  }
} /* lo_get_free_memory_from_big_blocks */


local void get_memory_from_free_list
(
  allocated_block_type *pallocated_block,
  int size,
  wn_memgp group
)
{
  free_list_type free_list = (free_list_type) group->free_list;

  if(size < SMALL_SIZE)
  {
    get_memory_from_small_blocks(pallocated_block,size,free_list);
  }
  else
  {
    lo_get_free_memory_from_big_blocks(pallocated_block, size, group);
  }
} /* get_memory_from_free_list */


local void get_more_memory(int size,wn_memgp group)
{
  wn_free_block free_block;
  wn_mem_block block;
  int size_to_get,size_to_alloc;

  free_block = NULL;

  if(group->block_ptr != NULL)
  {
    wn_assert((((long unsigned int) group->block_ptr) & 7) == 4);
    wn_assert(group->current_block->size -
    /**/	((char *) group->block_ptr - (char *) group->current_block) ==
    /**/						group->block_mem_left);
    wn_assert(-4 <= group->block_mem_left);

    free_block = (wn_free_block)(group->block_ptr);

    if(!wn_gp_pad_flag  &&
    /**/	group->block_mem_left >= WN_SIZEOF_FREE_BLOCK_STRUCT  ||
    /**/  group->block_mem_left  >=
    /**/      WN_SIZEOF_FREE_BLOCK_STRUCT + (int) sizeof(struct pad_type_struct))
    {
      free_block->free_block_size = group->block_mem_left - sizeof(int);

      wn_assert(free_block->free_block_size >= 4);
    }
    else
    {
      /*   There's not enough memory in the block to be worth saving, throw it
      ** away. */

      free_block = NULL;
      group->current_block->leftover += group->block_mem_left;
    }
  }

  size_to_get = wn_max(size, group->block_size);
  size_to_alloc = size_to_get + sizeof(struct wn_mem_block_struct) + 4;
  block = (wn_mem_block) wn_system_alloc(size_to_alloc);
  wn_assert(sizeof(ptr) < 8 || !(((long unsigned) block) & 7));

  block->next = group->current_block;
  block->size = size_to_alloc;
  block->leftover = 0;

  group->current_block = block;

  group->block_mem_left = size_to_get + 4;
  group->block_ptr = WN_MEM_BLOCK_MEMORY(block);
  if ((((long unsigned int) group->block_ptr) & 7) == 0)
  {
    group->block_ptr = (ptr) (((char *) group->block_ptr) + 4);
    group->current_block->leftover += 4;
    group->block_mem_left -= 4;
  }

  wn_assert(((long unsigned int) group->block_ptr & 7) == 4);

  if (free_block && group->free_list)
  {
    insert_free_block_into_free_list(free_block, group);
    /* otherwise just throw it away, this can only happen before the
    ** group is fully initialized, should in fact never happen. */
  }

  /* make sure the insert free block didn't take any of the memory
  ** from our new block. */
  wn_assert(group->block_mem_left >= size_to_get);
  wn_assert(group->block_mem_left <= size_to_get + 4);
} /* get_more_memory */


/* allocate memory without looking at the free list, and without using
** the group stack. */

local void lo_simple_alloc(ptr *pmem,int size,wn_memgp group)
{
  if(size >= group->block_mem_left)
  {
    get_more_memory(size,group);
  }
  wn_assert(size <= group->block_mem_left);
  wn_assert((((long unsigned int) group->block_ptr) & 7) == 4);

  *pmem = group->block_ptr;
  group->block_ptr = (ptr) ((char *)(group->block_ptr) + size);
  group->block_mem_left -= size;
  wn_assert(group->block_mem_left >= 0);
  if ((((long unsigned int) group->block_ptr) & 7) == 0)
  {
    group->block_ptr = (ptr) (((char *) group->block_ptr) + 4);
    group->current_block->leftover += 4;
    group->block_mem_left -= 4;
  }

  wn_assert((((long unsigned int) group->block_ptr) & 7) == 4);
} /* lo_simple_alloc */


local void get_new_memory
(
  ptr *pallocated_block,
  int size,
  wn_memgp group
)
{
  int total_segment_size;

  total_segment_size = size + sizeof(int);
  total_segment_size = wn_max(total_segment_size, WN_SIZEOF_FREE_BLOCK_STRUCT);

  lo_simple_alloc(pallocated_block, total_segment_size, group);

  wn_assert((((long unsigned int)(*pallocated_block)) & 7) == 4);

  ((allocated_block_type) *pallocated_block)->size = total_segment_size -
  /**/								sizeof(int);
}


local void allocate_block
(
  allocated_block_type *pallocated_block,
  int size,
  wn_memgp group
)
{
  wn_fast_assert(group->free_list != NULL);

  get_memory_from_free_list(pallocated_block, size, group);

  if(*pallocated_block == NULL)
  {
    get_new_memory((ptr *) pallocated_block,size,group);
  }
}


local void make_pad(allocated_block_type allocated_block, wn_memgp group)
{
  pad_type pad;
  pad_type next;
  int pad_offset;

  if(!wn_gp_pad_flag)
  {
    return;
  }

  pad_offset = sizeof(int) + allocated_block->size -
  /**/						sizeof(struct pad_type_struct);
  pad = (pad_type) ((char *) allocated_block + pad_offset);

  pad->begin_magic = PAD_BEGIN_MAGIC;
  pad->end_magic   = PAD_END_MAGIC;

  next = (pad_type)(group->pad_list);
  if(next != NULL)
  {
    next->plast = &(pad->next);
  }
  pad->next = next;
  group->pad_list = (ptr)pad;
  pad->plast = (pad_type *)(&(group->pad_list));
}


local void delete_pad(allocated_block_type allocated_block)
{
  pad_type pad;
  int pad_offset;
  pad_type next;

  if(!wn_gp_pad_flag)
  {
    return;
  }

  pad_offset = sizeof(int) + allocated_block->size -
  /**/						sizeof(struct pad_type_struct);
  pad = (pad_type) ((char *) allocated_block + pad_offset);

  wn_assert(PAD_BEGIN_MAGIC == pad->begin_magic);
  wn_assert(PAD_END_MAGIC   == pad->end_magic);

  next = pad->next;
  if(next != NULL)
  {
    next->plast = pad->plast;
  }
  *(pad->plast) = next;

  pad->begin_magic = PAD_DELETED_BEGIN_MAGIC;
  pad->end_magic   = PAD_DELETED_END_MAGIC;
} /* delete_pad */


ptr alloc_piece(int size, wn_memgp group)
{
  int old_size = 0 /* shut up gcc uninit complaint */;
  allocated_block_type allocated_block;
  ptr ret;

  if(wn_gp_pad_flag)
  {
    old_size = size;
    size += sizeof(struct pad_type_struct);
  }

  allocate_block(&allocated_block,size,group);

  ret = (ptr)(&(allocated_block->memory));

  if(wn_gp_pad_flag)
  {
    make_pad(allocated_block, group);

    group->mem_used += allocated_block->size - sizeof(struct pad_type_struct);
  }
  else
  {
    group->mem_used += allocated_block->size;
  }
  if(wn_gp_fill_flag)
  {
    if(wn_gp_pad_flag)
    {
      wn_memset(ret,'a',old_size);
    }
    else
    {
      wn_memset(ret,'a',size);
    }
  }
  if(wn_gp_trap_address_flag)
  {
    if(ret == wn_gp_trap_address_address)
    {
      fprintf(stderr,"address found.\n");
      wn_assert_notreached();
    }
  }

  wn_assert((((long unsigned int) ret) & 7) == 0);

  return(ret);
} /* alloc_piece */


/*ARGSUSED*/ void free_piece(ptr p,wn_memgp group)
{
  allocated_block_type allocated_block;

  allocated_block = (allocated_block_type)(((char *)p)-sizeof(int));

  if(wn_gp_pad_flag)
  {
    delete_pad(allocated_block);

    group->mem_used -=
        (allocated_block->size - sizeof(struct pad_type_struct));
  }
  else
  {
    group->mem_used -= allocated_block->size;
  }

  if(wn_gp_fill_flag)
  {
    wn_memset(p,'f',allocated_block->size);
  }

  insert_free_block_into_free_list((wn_free_block)allocated_block, group);
} /* free_piece */


local void verify_pad(pad_type pad)
{
  wn_assert(pad->begin_magic == PAD_BEGIN_MAGIC);
  wn_assert(pad->end_magic == PAD_END_MAGIC);
  wn_assert(*(pad->plast) == pad);
}


local void verify_pad_list(pad_type pad_list)
{
  pad_type pad;

  for(pad=pad_list;pad != NULL;pad=pad->next)
  {
    verify_pad(pad);
  }
}


local void verify_free_block
(
  wn_free_block free_block,
  int size,
  wn_memgp group
)
{
  wn_assert(free_block->free_block_size == size);
  wn_assert(WN_FREE_BLOCK_TO_MBHANDLE(free_block)->next_free_block !=
  /**/								free_block);
  wn_assert(wn_mem_in_group(free_block, group));
}


local void verify_free_blocks_of_one_size
(
  wn_free_block free_block_list,
  long int size,
  wn_memgp group
)
{
  wn_free_block free_block;

  for(free_block = free_block_list; free_block;
  /**/	free_block = WN_FREE_BLOCK_TO_MBHANDLE(free_block)->next_free_block)
  {
    verify_free_block(free_block, size, group);
  }
} /* verify_free_blocks_of_one_size */


wn_memgp lo_free_list_verify_act_group;

void wn_free_list_verify_act(wn_mbhandle handle)
{
  wn_assert((long int) handle->key >= SMALL_SIZE);

  verify_free_blocks_of_one_size(handle->free_block_list,
  /**/	(int) (long int) handle->key, lo_free_list_verify_act_group);
  verify_free_blocks_of_one_size(WN_MBHANDLE_TO_FREE_BLOCK(handle),
  /**/	(int) (long int) handle->key, lo_free_list_verify_act_group);
} /* lo_free_list_verify_act */


local void verify_free_list(wn_memgp group)
{
  free_list_type free_list;
  int size;
  /* wn_mbhandle handle; ** unused - bchapman 041111 */
  void (*routine_ptr)(wn_mbhandle);
  /* long int i; ** unused - bchapman 041111 */

  free_list = (free_list_type)(group->free_list);

  for(size = 4;size < SMALL_SIZE;size += 4)
  {
    verify_free_blocks_of_one_size((free_list->small_blocks)[size>>2], size,
    /**/								group);
  }

  lo_free_list_verify_act_group = group;

  routine_ptr = wn_free_list_verify_act;
  /* couldn't make call to wn_mbact work on 64 bit */

  /* note tree does compares as pointers, as unsigned ints */
  wn_mbact(free_list->big_blocks_tree, routine_ptr,
  /**/				(ptr) 0, WN_MB_GE, (ptr) -1, WN_MB_LE);
} /* verify_free_list */


local void verify_group(wn_memgp group)
{
  if(wn_gp_pad_flag)
  {
    verify_pad_list((pad_type)(group->pad_list));
  }

  verify_free_list(group);
} /* verify_group */


local void make_free_list(free_list_type *pfree_list,wn_memgp group)
{
  wn_mbtree tree;

  /*   note lo_simple_alloc always returns a result not 8-byte aligned,
  ** we have to align it ourselves. */
  lo_simple_alloc((ptr *) pfree_list,
  /**/		sizeof(struct free_list_type_struct) + sizeof(int), group);
  *pfree_list = (free_list_type) ((char *) *pfree_list + sizeof(int));
  wn_memzero(*pfree_list, sizeof(struct free_list_type_struct));

  lo_simple_alloc((ptr *) &tree, sizeof(*tree) + sizeof(int), group);
  tree = (wn_mbtree) ((char *) tree + sizeof(int));
  wn_memzero(tree, sizeof(*tree));
  (*pfree_list)->big_blocks_tree = tree;
  wn_mmkptrbtree(tree);
} /* make_free_list */


void wn_initialize_group_for_general_free(wn_memgp group)
{
  group->pverify_group = (verify_group);
  group->pfree_piece = (free_piece);
  group->palloc_piece = (alloc_piece);

  make_free_list((free_list_type *)(&(group->free_list)), group);
} /* wn_initialize_group_for_general_free */


long int lo_amount_of_free_memory_total;
local void lo_amount_of_free_memory_act(wn_mbhandle handle)
{
  wn_free_block free_block;

  wn_assert((long int) handle->key >= SMALL_SIZE);

  for (free_block = handle->free_block_list;  free_block;
  /**/ free_block = WN_FREE_BLOCK_TO_MBHANDLE(free_block)->next_free_block)
  {
    lo_amount_of_free_memory_total += (long int) handle->key;
  }

  lo_amount_of_free_memory_total += (long int) handle->key;	/* headnode */
} /* lo_amount_of_free_memory_act */


/*     if group is a freeable group, return the amount of memory on the
** free list, -1 otherwise */

long int wn_amount_of_free_memory_in_group(wn_memgp group)
{
  free_list_type free_list;
  wn_free_block free_block;
  /* wn_mbhandle handle; ** unused - bchapman 041111 */
  long int result = 0;
  long int i;

  if ((alloc_piece) != group->palloc_piece)
  {
    return -1;
  }

  free_list = (free_list_type) group->free_list;
  for (i = 0;  i < SMALL;  ++i)
  {
    for (free_block = free_list->small_blocks[i];  free_block;
    /**/ free_block = WN_FREE_BLOCK_TO_MBHANDLE(free_block)->next_free_block)
    {
      wn_assert(i*sizeof(int) == (size_t) free_block->free_block_size);

      result += free_block->free_block_size;
    }
  }

  lo_amount_of_free_memory_total = 0;
  wn_mbact(free_list->big_blocks_tree, lo_amount_of_free_memory_act,
  /**/				(ptr) 0, WN_MB_GE, (ptr) -1, WN_MB_LE);
  result += lo_amount_of_free_memory_total;

  return result;
} /* wn_amount_of_free_memory_in_group */


void wn_print_composition_of_big_blocks_free_list(wn_memgp group)
{
  free_list_type free_list = (free_list_type) group->free_list;
  wn_free_block free_block;
  /* int size, next_size; ** unused - bchapman 041111 */
  int count_of_this_size;
  wn_mbhandle handle;
  long int i;

  (void) printf("%-6s: %s\n", "size", "count");

  /* this may develop speed problems, if so, replace with a call to wn_mbact */
  for (i = 0;  wn_mbget(&handle, free_list->big_blocks_tree, (ptr) i,
  /**/							WN_MB_GT), handle;  )
  {
    i = (long int) handle->key;

    count_of_this_size = 1;	/* headnode */
    for (free_block = handle->free_block_list;  free_block;
    /**/ free_block = WN_FREE_BLOCK_TO_MBHANDLE(free_block)->next_free_block)
    {
      wn_assert(free_block->free_block_size == i);

      ++ count_of_this_size;
    }

    (void) printf("%6d: %d\n", (int) i, count_of_this_size);
  }
} /* wn_print_composition_of_big_blocks_free_list */
