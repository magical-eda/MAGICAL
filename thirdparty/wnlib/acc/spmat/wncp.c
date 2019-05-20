/**********************************************************************

wn_critical_path(&code,&len,&result,length_mat,start_node,fin_node)

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
#include "wnsll.h"
#include "wnspmat.h"
#include "wncp.h"



typedef struct node_data_struct
{
  double len;
  wn_sparse_matrix_entry from_edge;
  int node_index,
      incoming_edges_remaining;
  bool is_orphaned;
} *node_data;

local node_data *node_array;

local wn_sll nodes_to_expand;
local int len;
local wn_memgp top_group;



local void make_node
(
  node_data *pnode,
  wn_sll j_list,
  int i
)
{
  *pnode = (node_data)wn_zalloc(sizeof(struct node_data_struct));

  (*pnode)->len = -WN_FHUGE;
  (*pnode)->node_index = i;
  (*pnode)->incoming_edges_remaining = wn_sllcount(j_list);
  (*pnode)->is_orphaned = FALSE;
}


#if 0
local void uncount_orphan_edges
(
  wn_sparse_matrix length_mat,
  int node_index
)
{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  node_data node;
 
  for(el=(length_mat->i_lists)[node_index];el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    node = node_array[entry->j];

    --(node->incoming_edges_remaining);

    if(
	(node->incoming_edges_remaining == 0)
	  &&
	(node->node_index < node_index)
      )
    {
      uncount_orphan_edges(length_mat,node->node_index);
    }
  }
}


local void uncount_orphans_edges
(
  wn_sparse_matrix length_mat,
  int start_node
)
{
  int i;
  node_data node;

  for(i=0;i<len;++i)
  {
    if(i != start_node)
    {
      node = node_array[i];

      if(node->incoming_edges_remaining == 0)
      {
        uncount_orphan_edges(length_mat,i);
      }
    }
  }
}
#endif


local void uncount_new_orphan_edges
(
  wn_sparse_matrix length_mat,
  int node_index,
  int start_node_index
)
{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  node_data node,connected_node;
 
  if(node_index == start_node_index)  /* don't delete start node */
  {
    return;  
  }

  node = node_array[node_index];

  if(node->is_orphaned)  /* node already orphaned, don't do it again */
  {
    return;
  }

  node->is_orphaned = TRUE;

  for(el=(length_mat->i_lists)[node_index];el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    connected_node = node_array[entry->j];

    --(connected_node->incoming_edges_remaining);

    if(connected_node->incoming_edges_remaining == 0)
    {
      uncount_new_orphan_edges(length_mat,
			       connected_node->node_index,start_node_index);
    }
  }
}


local void uncount_orphans_edges
(
  wn_sparse_matrix length_mat,
  int start_node_index
)
{
  int i;
  node_data node;

  for(i=0;i<len;++i)
  {
    if(i != start_node_index)
    {
      node = node_array[i];

      if(node->incoming_edges_remaining == 0)
      {
        uncount_new_orphan_edges(length_mat,i,start_node_index);
      }
    }
  }
}


local void initialize
(
  wn_sparse_matrix length_mat,
  int start_node
)
{
  int i;

  nodes_to_expand = NULL;

  node_array = (node_data *)wn_zalloc(len*sizeof(node_data));

  for(i=0;i<len;++i)
  {
    make_node(&(node_array[i]),(length_mat->j_lists)[i],i);
  }

  uncount_orphans_edges(length_mat,start_node);
}


local void expand_node
(
  wn_sparse_matrix length_mat,
  int node_index_to_expand
)
{
  wn_sll list,el;
  double len,new_len;
  int to_node_index;
  node_data node_to_expand,to_node;
  wn_sparse_matrix_entry entry;

  node_to_expand = node_array[node_index_to_expand];
  list = (length_mat->i_lists)[node_index_to_expand];
  len = node_to_expand->len;

  for(el=list;el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    to_node_index = entry->j;
    to_node = node_array[to_node_index];
    new_len = entry->value + len;

    if(new_len > to_node->len)
    {
      to_node->len = new_len;
      to_node->from_edge = entry;
    }

    --(to_node->incoming_edges_remaining);

    if(to_node->incoming_edges_remaining == 0)
    {
      wn_sllins(&nodes_to_expand,(ptr)to_node_index);
    }
  }
}


local void main_loop
(
  int *pcode,
  wn_sparse_matrix length_mat,
  int start_node,
  int fin_node
)
{
  int node_index_to_expand;

  initialize(length_mat,start_node);

  if(node_array[start_node]->incoming_edges_remaining != 0)
  {
    *pcode = WN_INFEASIBLE;
    return;
  }

  node_array[start_node]->len = 0.0;
  wn_sllins(&nodes_to_expand,(ptr)start_node);

  for(;;)
  {
    if(nodes_to_expand == NULL)
    {
      *pcode = WN_INFEASIBLE;
      break;
    }

    node_index_to_expand = (int) (long) (nodes_to_expand->contents);
    wn_slledel(&nodes_to_expand);

    if(node_index_to_expand == fin_node)
    {
      *pcode = WN_SUCCESS;

      break;
    }

    expand_node(length_mat,node_index_to_expand);
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

  wn_gppush(top_group);

  fin_node = node_array[fin_node_index];

  *plen = fin_node->len;

  *presult = NULL;

  for(node = fin_node;node->from_edge != NULL;
      node = node_array[node->from_edge->i])
  {
    wn_sllins(presult,node->from_edge);
  }

  wn_gppop();
}


void wn_critical_path
(
  int *pcode,
  double *plen,
  wn_sll *presult,         /* list of edges */
  wn_sparse_matrix length_mat,
  int start_node,
  int fin_node
)
{
  wn_assert(length_mat->len_i == length_mat->len_j);
  len = length_mat->len_i;

  wn_assert((0<=start_node)&&(start_node<len));
  wn_assert((0<=fin_node)&&(fin_node<len));

  top_group = wn_curgp();
  wn_gpmake("no_free");

  main_loop(pcode,length_mat,start_node,fin_node);

  if(*pcode == WN_SUCCESS)
  {
    grab_results(plen,presult,fin_node);
  }

  wn_gpfree();
}
