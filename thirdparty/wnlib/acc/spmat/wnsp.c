/**********************************************************************

wn_shortest_path(&code,&len,&result,length_mat,start_node,fin_node)

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
#include "wnasrt.h"
#include "wnhtbl.h"
#include "wnbtr.h"
#include "wnbtrl.h"
#include "wnspmat.h"
#include "wnsp.h"



typedef struct node_data_struct
{
  double len;
  wn_sparse_matrix_entry from_edge;
  int node_index;
  wn_bhandle handle;
} *node_data;

local wn_htab node_table;
local wn_btree queue;

local int len_i;
local wn_memgp top_group;



local void insert_node_if_better
(
  int to_node_index,
  double len,
  wn_sparse_matrix_entry entry
)
{
  node_data node;
  wn_bhandle handle;

  if(wn_hget((ptr *)&node,node_table,(ptr)to_node_index))
  {
    if(len < node->len)   /* new len is shorter */
    {
      node->len = len;
      node->from_edge = entry;
      
      wn_bmove(node->handle,queue,&len);
    }
  }
  else
  {
    node = (node_data)wn_zalloc(sizeof(struct node_data_struct));

    node->len = len;
    node->from_edge = entry;
    node->node_index = to_node_index;

    wn_hins(node,node_table,(ptr)to_node_index);
    wn_bins(&handle,queue,&len);

    handle->contents = (ptr)node;
    node->handle = handle;
  }
}


local void expand_node(wn_sparse_matrix length_mat,node_data node)
{
  wn_sll list,el;
  double len,value;
  int to_node_index;
  wn_sparse_matrix_entry entry;

  list = (length_mat->i_lists)[node->node_index];
  len = node->len;

  for(el=list;el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    value = entry->value;
    to_node_index = entry->j;

    wn_assert(value >= 0.0);

    insert_node_if_better(to_node_index,len+value,entry);
  }
}


local void delete_node(node_data node)
{
  wn_bdel(node->handle,queue);
}


local void main_loop
(
  int *pcode,
  wn_sparse_matrix length_mat,
  int start_node,
  int fin_node
)
{
  node_data node;
  wn_bhandle node_handle;

  wn_mklonghtab(&node_table);
  wn_mkdoublebtree(&queue);

  insert_node_if_better(start_node,0.0,(wn_sparse_matrix_entry)NULL);

  for(;;)
  {
    wn_bget(&node_handle,queue,NULL,WN_B_MIN);

    if(node_handle == NULL)
    {
      *pcode = WN_INFEASIBLE;

      break;
    }

    node = (node_data)(node_handle->contents);

    if(node->node_index == fin_node)
    {
      *pcode = WN_SUCCESS;

      break;
    }

    expand_node(length_mat,node);

    delete_node(node);
  }
}


local void grab_results
(
  double *plen,
  wn_sll *presult,
  int fin_node_index
)
{
  node_data fin_node,node;
  bool success;

  wn_gppush(top_group);

  success = wn_hget((ptr *)&fin_node,node_table,(ptr)fin_node_index);
  wn_assert(success);

  *plen = fin_node->len;

  *presult = NULL;

  for(node = fin_node;node->from_edge != NULL;
      wn_hget((ptr *)&node,node_table,(ptr)(node->from_edge->i)))
  {
    wn_sllins(presult,node->from_edge);
  }

  wn_gppop();
}


void wn_shortest_path
(
  int *pcode,
  double *plen,
  wn_sll *presult,          /* list of edges */
  wn_sparse_matrix length_mat,
  int start_node,
  int fin_node
)
{
  wn_assert(length_mat->len_i == length_mat->len_j);
  len_i = length_mat->len_i;
  wn_assert((0<=start_node)&&(start_node<len_i));
  wn_assert((0<=fin_node)&&(fin_node<len_i));

  top_group = wn_curgp();
  wn_gpmake("no_free");

  main_loop(pcode,length_mat,start_node,fin_node);

  if(*pcode == WN_SUCCESS)
  {
    grab_results(plen,presult,fin_node);
  }

  wn_gpfree();
}



#if 0
print_queue()
{
  void print_handle();

  printf("queue=\n");
  wn_bact(queue,(print_handle),
	  NULL,WN_B_MIN,NULL,WN_B_MAX);
}



local void print_handle(handle)

wn_bhandle handle;

{
  node_data node;
  double *key;

  node = (node_data)(handle->contents);
  key = (double *)(handle->key);

  printf("%d: len = %f,key = %f\n",
	 node->node_index,(float)(node->len),(float)(*key));
}
#endif


