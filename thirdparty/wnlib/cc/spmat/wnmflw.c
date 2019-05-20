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
#include "wninr.h"
#include "wnsmat.h"



typedef struct node_data_struct
{
  double len;
  wn_sparse_matrix_entry from_edge;
  int node_index,
      incoming_edges_remaining;
} *node_data;

local node_data *node_array;

local wn_set full_nodes;
local int len_i;
local wn_memgp top_group;



wn_critical_path(pcode,plen,presult,length_mat,start_node,fin_node)

int *pcode;
double *plen;
wn_sll *presult;          /* list of edges */
wn_sparse_matrix length_mat;
int start_node,fin_node;

{
  wn_assertmsg(length_mat->len_i == length_mat->len_j,
               "wn_critical_path: matrix not square.");
  len_i = length_mat->len_i;
  wn_assertmsg(wn_inrange(0,<=,start_node,<,len_i),
               "wn_critical_path: start_node out of range.");
  wn_assertmsg(wn_inrange(0,<=,fin_node,<,len_i),
               "wn_critical_path: fin_node out of range.");

  top_group = wn_curgp();
  wn_gpmake("no_free");

  main_loop(pcode,length_mat,start_node,fin_node);

  if(*pcode == WN_SUCCESS)
  {
    grab_results(plen,presult,fin_node);
  }

  wn_gpfree();
}



local main_loop(pcode,length_mat,start_node,fin_node)

int *pcode;
wn_sparse_matrix length_mat;
int start_node,fin_node;

{
  int node_index_to_expand;
  bool success;

  initialize(length_mat,start_node);

  node_array[start_node]->len = 0.0;
  wn_setins(full_nodes,start_node);

  while(TRUE)
  {
    success = wn_setgetany(&node_index_to_expand,full_nodes);

    if(not(success))
    {
      get_error_code(pcode);

      break;
    }

    if(node_index_to_expand == fin_node)
    {
      *pcode = WN_SUCCESS;

      break;
    }

    expand_node(length_mat,node_index_to_expand);

    success = wn_setdel(full_nodes,node_index_to_expand);
    wn_assert(success);
  }
}



local initialize(length_mat,start_node)

wn_sparse_matrix length_mat;
int start_node;

{
  int i;

  wn_mkset(&full_nodes);

  node_array = (node_data *)wn_zalloc(len_i*wn_sizeof(node_data));

  for(i=0;i<len_i;++i)
  {
    make_node(&(node_array[i]),(length_mat->j_lists)[i],i);
  }

  wn_assertmsg(node_array[start_node]->incoming_edges_remaining == 0,
               "wn_critical_path: start_node must not have incoming edges.");

  uncount_orphans_edges(length_mat,start_node);
}



local make_node(pnode,j_list,i)

node_data *pnode;
wn_sll j_list;
int i;

{
  *pnode = wn_znew(node_data);

  (*pnode)->len = -WN_FHUGE;
  (*pnode)->node_index = i;
  (*pnode)->incoming_edges_remaining = wn_sllcount(j_list);
}



local uncount_orphans_edges(length_mat,start_node)

wn_sparse_matrix length_mat;
int start_node;

{
  int i;
  node_data node;

  for(i=0;i<len_i;++i)
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



local uncount_orphan_edges(length_mat,node_index)

wn_sparse_matrix length_mat;
int node_index;

{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  node_data node;

  for(wn_loopinit(),el=(length_mat->i_lists)[node_index];
      wn_sllloop(&el,&entry);)
  {
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



local get_error_code(pcode)

int *pcode;

{
  *pcode = WN_INFEASIBLE;
}



local expand_node(length_mat,node_index_to_expand)

wn_sparse_matrix length_mat;
int node_index_to_expand;

{
  wn_sll list,el;
  double len,new_len;
  int to_node_index;
  node_data node_to_expand,to_node;
  wn_sparse_matrix_entry entry;

  node_to_expand = node_array[node_index_to_expand];
  list = (length_mat->i_lists)[node_index_to_expand];
  len = node_to_expand->len;

  for(wn_loopinit(),el=list;wn_sllloop(&el,&entry);)
  {
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
      wn_setins(full_nodes,to_node_index);
    }
  }
}



local grab_results(plen,presult,fin_node_index)

double *plen;
wn_sll *presult;
int fin_node_index;

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


