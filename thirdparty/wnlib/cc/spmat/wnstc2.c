/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/**********************************************************************

wn_truncate_tributary_nodes(mat,significant_nodes)

**********************************************************************/

#include "wnlib.h"
#include "wnbtr.h"
#include "wnary.h"
#include "wnsmat.h"


typedef struct node_data_struct *node_data;
typedef struct edge_data_struct *edge_data;

struct node_data_struct
{
  int index;
  wn_set edges;

  int stack_level;
  bool node_visited,node_on_stack,delete_this_node;
  edge_data stack_forward_edge;
};

struct edge_data_struct
{
  node_data node1,node2;
  bool forward,backward,
       accum_forward,accum_backward,
       delete_this_edge;
  wn_set bound_edges;
  wn_bhandle handle;
};

local int len;
local wn_btree edges_by_level;

local wn_set edge_set;
local node_data *node_array;


/*
  delete nodes and edges that can not possibly be on a path 
  between significant nodes.

  resources:

    time = e*log(e)
    stack_memory = n
    dynamic_memory = e

*/
wn_truncate_tributary_nodes(mat,significant_nodes)

wn_sparse_matrix mat;
wn_set significant_nodes;

{
  if(wn_setempty(significant_nodes))
  {
    return;
  }

  wn_gpmake("no_free");

  initialize(mat);

  find_nodes_to_delete(significant_nodes);

  /*
  {
    edge_data edge;

    for(wn_loopinit();wn_setloop(edge_set,&edge);)
    {
      printf("[%d,%d],accum_forward=%d,accum_backward=%d,del_edge=%d\n",
	     edge->node1->index,edge->node2->index,
	     edge->accum_forward,edge->accum_backward,
	     edge->delete_this_edge);
    }
  }
  {
  int i;
  for(i=0;i<len;++i)
  {
    printf("node_array[%d]->delete_this_node = %d\n",
	   i,node_array[i]->delete_this_node);
  }
  }
  */

  free_nodes(mat);

  wn_gpfree();
}



local initialize(mat)

wn_sparse_matrix mat;

{
  int i,j;
  node_data node;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  wn_assertmsg(mat->len_i == mat->len_j,
	       "wn_truncate_tributary_nodes: matrix must be square.");
  
  len = mat->len_i;

  node_array = (node_data *)wn_zalloc(len*wn_sizeof(node_data));

  for(i=0;i<len;++i)
  {
    make_node(&node);

    node->index = i;

    node_array[i] = node;
  }

  wn_mkset(&edge_set);

  for(i=0;i<len;++i)
  {
    for(wn_loopinit(),el=(mat->i_lists)[i];wn_sllloop(&el,&entry);)
    {
      j = entry->j;

      if(i < j)
      {
	make_edge_for_entry(entry);
      }
    }
  }
}



local make_edge_for_entry(entry)

wn_sparse_matrix_entry entry;

{
  edge_data edge;
  node_data nodei,nodej;
  int i,j;

  edge = wn_znew(edge_data);
  wn_setins(edge_set,edge);

  i = entry->i;
  j = entry->j;

  nodei = node_array[i];
  nodej = node_array[j];

  wn_setins(nodei->edges,edge);
  wn_setins(nodej->edges,edge);

  edge->node1 = nodei;
  edge->node2 = nodej;
}



local make_node(pnode)

node_data *pnode;

{
  *pnode = wn_znew(node_data);

  wn_mkset(&((*pnode)->edges));
}



local find_nodes_to_delete(significant_nodes)

wn_set significant_nodes;

{
  int node_index;
  node_data node;
  bool success;

  for(wn_loopinit();wn_setloop(significant_nodes,&node_index);)
  {
    node = node_array[node_index];

    get_edge_directions_from_node(node);
  }

  delete_single_direction_edges();

  success = wn_setgetany(&node_index,significant_nodes);
  wn_assert(success);
  node = node_array[node_index];
  find_unreachable_nodes(node);
}



local get_edge_directions_from_node(node)

node_data node;

{
  wn_gpmake("no_free");

  initialize_get_directions();
  expand_get_directions((edge_data)NULL,node,0);

  wn_gpfree();
}



local initialize_get_directions()
{
  edge_data edge;
  int i;
  node_data node;

  wn_mkintbtree(&edges_by_level);

  for(wn_loopinit();wn_setloop(edge_set,&edge);)
  {
    edge->forward = edge->backward = FALSE;
    wn_mkset(&(edge->bound_edges));
  }

  for(i=0;i<len;++i)
  {
    node = node_array[i];

    node->node_visited = FALSE;
    node->node_on_stack = FALSE;
    node->stack_forward_edge = NULL;
  }
}



local expand_get_directions(from_edge,node,stack_level)

edge_data from_edge;
node_data node;
int stack_level;

{
  if(node->node_visited)  /* back onto earlier part of stack */
  {
    edge_data stack_forward_edge;

    if(
	not(node->node_on_stack)
	  ||
	((stack_level-2) == node->stack_level)
      )
    {
      return;
    }

    totalfill_edge_stack(node->stack_level+2,stack_level-1);

    stack_forward_edge = node->stack_forward_edge;
    wn_assert(stack_forward_edge != NULL);

    if(edge_is_full(stack_forward_edge))
    {
      force_edge_full(from_edge);
    }
    else
    {
      node_data from_node;

      get_other_node(&from_node,from_edge,node);  /* give edge reverse 
						     direction */
      note_edge_direction(from_edge,from_node);

      wn_setins(stack_forward_edge->bound_edges,from_edge);
    }
  }
  else  /* new, virgin node */
  {
    wn_set edges;
    edge_data edge;
    node_data other_node;
    int next_stack_level;

    if(from_edge != NULL)
    {
      note_edge_direction(from_edge,node);
      push_edge_onto_edge_stack(from_edge,stack_level);
    }

    node->node_on_stack = TRUE;
    node->node_visited = TRUE;
    node->stack_level = stack_level;
    next_stack_level = stack_level+1;

    wn_setcpy(&edges,node->edges);
    for(wn_loopinit();wn_setloop(edges,&edge);)
    {
      get_other_node(&other_node,edge,node);

      node->stack_forward_edge = edge;
      expand_get_directions(edge,other_node,next_stack_level);
      node->stack_forward_edge = NULL;
    }

    node->node_on_stack = FALSE;

    if(from_edge != NULL)
    {
      pop_edge_from_edge_stack(from_edge);
    }
  }
}



local wn_set selected_edges;

local totalfill_edge_stack(start_stack_level,fin_stack_level)

int start_stack_level,fin_stack_level;

{
  void insert_edge_into_selected_edges();
  edge_data edge;

  wn_assert(start_stack_level <= fin_stack_level);

  wn_mkset(&selected_edges);
  wn_bact(edges_by_level,
	  (insert_edge_into_selected_edges),
	  start_stack_level,WN_B_GE,fin_stack_level,WN_B_LE);

  for(wn_loopinit();wn_setloop(selected_edges,&edge);)
  {
    delete_edge_from_edge_stack(edge);
    force_edge_and_bound_edges_full(edge);
  }
}



local void insert_edge_into_selected_edges(handle)

wn_bhandle handle;

{
  edge_data edge;

  edge = (edge_data)(handle->contents);

  wn_setins(selected_edges,edge);
}



local force_edge_and_bound_edges_full(edge)

edge_data edge;

{
  edge_data bound_edge;

  force_edge_full(edge);

  for(wn_loopinit();wn_setloop(edge->bound_edges,&bound_edge);)
  {
    force_edge_full(bound_edge);
  }
}



local bool edge_is_full(edge)

edge_data edge;

{
  return(edge->forward && edge->backward);
}



local force_edge_full(edge)

edge_data edge;

{
  edge->forward = edge->backward = TRUE;
  edge->accum_forward = edge->accum_backward = TRUE;
}



local note_edge_direction(edge,to_node)

edge_data edge;
node_data to_node;

{
  if(edge->node2 == to_node)
  {
    wn_assert(edge->node1 != to_node);

    edge->forward = TRUE;
    edge->accum_forward = TRUE;
  }
  else
  {
    wn_assert(edge->node1 == to_node);
    wn_assert(edge->node2 != to_node);

    edge->backward = TRUE;
    edge->accum_backward = TRUE;
  }
}



local push_edge_onto_edge_stack(edge,stack_level)

edge_data edge;
int stack_level;

{
  wn_bhandle handle;

  wn_bins(&handle,edges_by_level,stack_level);
  handle->contents = (ptr)edge;
  wn_assert(edge->handle == NULL);
  edge->handle = handle;
}



local pop_edge_from_edge_stack(edge)

edge_data edge;

{
  delete_edge_from_edge_stack(edge);
}



local delete_edge_from_edge_stack(edge)

edge_data edge;

{
  if(edge->handle != NULL)
  {
    wn_bdel(edge->handle,edges_by_level);
    edge->handle = NULL;
  }
}



local delete_single_direction_edges()
{
  edge_data edge;

  for(wn_loopinit();wn_setloop(edge_set,&edge);)
  {
    edge->delete_this_edge = 
       (not(edge->accum_forward) || not(edge->accum_backward));
  }
}



local find_unreachable_nodes(node)

node_data node;

{
  wn_gpmake("no_free");

  initialize_delete_this_node();

  expand_to_reachable_nodes(node);

  wn_gpfree();
}



local initialize_delete_this_node()
{
  int i;

  for(i=0;i<len;++i)
  {
    node_array[i]->delete_this_node = TRUE;
  }
}



local expand_to_reachable_nodes(node)

node_data node;

{
  wn_set edges;
  edge_data edge;
  node_data other_node;

  if(node->delete_this_node)
  {
    node->delete_this_node = FALSE;

    wn_setcpy(&edges,node->edges);
    for(wn_loopinit();wn_setloop(edges,&edge);)
    {
      if(not(edge->delete_this_edge))
      {
        get_other_node(&other_node,edge,node);

        expand_to_reachable_nodes(other_node);
      }
    }
  }
}



local get_other_node(pother_node,edge,node)

node_data *pother_node,node;
edge_data edge;

{
  if(edge->node1 == node)
  {
    *pother_node = edge->node2;
  }
  else
  {
    wn_assert(edge->node2 == node);

    *pother_node = edge->node1;
  }

  wn_assert(*pother_node != node);
}



local free_nodes(mat)

wn_sparse_matrix mat;

{
  int i,j;
  wn_sll *pel,el;
  wn_sparse_matrix_entry entry;

  wn_gppush(mat->group);

  for(j=0;j<len;++j)
  {
    if(node_array[j]->delete_this_node)
    {
      wn_freesll_list((mat->j_lists)[j]);

      (mat->j_lists)[j] = NULL;
    }
    else
    {
      pel = &((mat->j_lists)[j]);

      while(*pel != NULL)
      {
	entry = (wn_sparse_matrix_entry)((*pel)->contents);

	if(node_array[entry->i]->delete_this_node)
	{
	  wn_slledel(pel);
	}
	else
	{
	  pel = &((*pel)->next);
	}
      }
    }
  }

  for(i=0;i<len;++i)
  {
    if(node_array[i]->delete_this_node)
    {
      for(wn_loopinit(),el=(mat->i_lists)[i];wn_sllloop(&el,&entry);)
      {
	wn_free((ptr)entry);
      }

      wn_freesll_list((mat->i_lists)[i]);

      mat->i_lists[i] = NULL;
    }
    else
    {
      pel = &((mat->i_lists)[i]);

      while(*pel != NULL)
      {
	entry = (wn_sparse_matrix_entry)((*pel)->contents);

	if(node_array[entry->j]->delete_this_node)
	{
	  wn_free((ptr)entry);

	  wn_slledel(pel);
	}
	else
	{
	  pel = &((*pel)->next);
	}
      }
    }
  }

  wn_gppop();
}




