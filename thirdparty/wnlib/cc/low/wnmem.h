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
#ifndef wnmemH
#define wnmemH


#include "wntype.h"



typedef struct wn_mem_block_struct *wn_mem_block;
typedef struct wn_memgp_struct *wn_memgp;

struct wn_memgp_struct
{
  int begin_magic;

  wn_memgp next,*plast;
  wn_memgp children,sub_next,*psub_last;

  wn_mem_block current_block;
  ptr block_ptr;
  int block_mem_left;
  int block_size;
  int mem_used;

  ptr pad_list,free_list;

  void (*pverify_group)(/*group*/),
       (*pfree_piece)(/*p,group*/);
  ptr  (*palloc_piece)(/*size,group*/);

  char *label;

  int end_magic;
};

struct wn_mem_block_struct
{
  wn_mem_block next;
  int size,leftover;
  int memory;   /* memory begins after here */
};



extern wn_memgp wn_curgp(),wn_defaultgp();
extern ptr wn_zalloc(),wn_alloc();

#define wn_znew(_type) \
(\
  (_type)wn_zalloc(wn_sizeof(wn_structof(_type))) \
)
  
#define wn_new(_type) \
(\
  (_type)wn_alloc(wn_sizeof(wn_structof(_type))) \
)


#define GROUP_STACK_SIZE    100

typedef struct wn_gpstack_struct *wn_gpstack;
struct wn_gpstack_struct      /* kludge for now */
{
  int stack_ptr;
  wn_memgp *group_stack; 
  wn_memgp current_group;
};
  

#endif

