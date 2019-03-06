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

**********************************************************************/

#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnbtr.h"
#include "wnbtrl.h"
#include "wnprm.h"
#include "wnrnd.h"
#include "wnspmat.h"

#include "wnflow.h"


#define PRINT TRUE

#define TOL   (0.0000001)


typedef struct edge_data_type_struct *edge_data_type;
typedef struct node_data_type_struct *node_data_type;

struct edge_data_type_struct
{
  double flow,forward_residual_flow;
  double cost;

  wn_bhandle i_handle,j_handle;
};

struct node_data_type_struct
{
  int traversal_id;
  wn_sll outgoing_edges,incoming_edges;

  bool next_is_outgoing,start_next_is_outgoing;
  wn_sll next_edge_el,start_next_edge_el;

  bool in_edge_is_incoming;
  wn_sparse_matrix_entry in_entry;
  double augment_in_flow;

  wn_btree unexpanded_edges_by_cost;
  wn_sparse_matrix_entry best_unexpanded_edge;
  double best_unexpanded_edge_cost;
  int best_unexpanded_edge_index;
  double best_sum_cost;

  wn_bhandle handle;
  int path_length,reset_loop_check_path_length;
  int loop_node_index;

  bool node_touched;  /* node touched during minimize cycle */
};


local wn_sparse_matrix capacity_mat;
local wn_sparse_matrix cost_mat;
local int source_i_index;
local int dest_j_index;

local int len,num_edges;

local int pending_id,done_id;

local double total_flow,total_cost;

local int *random_nodes_to_expand;
local int *random_node_to_expand_indexes;
local int num_random_nodes_to_expand;

local node_data_type *node_data_array;


local wn_btree node_indexes_by_sum_cost;
local int *node_permutation_array;
local int node_permutation_pointer;
local double time_count,max_time_count;

local bool optimality_flag;



local void reset_traversal_ids(void)
{
  pending_id += 2;
  done_id = pending_id+1;
}


local void setup_node_data_array(void)
{
  int i;
  node_data_type node_data;

  node_data_array = (node_data_type *)wn_zalloc(len*sizeof(node_data_type));

  for(i=0;i<len;++i)
  {
    node_data = (node_data_type)
		 wn_zalloc(sizeof(struct node_data_type_struct));

    wn_sllcpy(&(node_data->outgoing_edges),(capacity_mat->i_lists)[i]);
    wn_sllcpy(&(node_data->incoming_edges),(capacity_mat->j_lists)[i]);

    wn_randomize_sll(&(node_data->outgoing_edges));
    wn_randomize_sll(&(node_data->incoming_edges));

    node_data->next_is_outgoing = TRUE;
    node_data->next_edge_el = node_data->outgoing_edges;

    node_data_array[i] = node_data;
  }
}


local void set_edge_flow(wn_sparse_matrix_entry entry,double flow)
{
  edge_data_type edge_data;
  double capacity;

  wn_assert(flow >= 0.0);

  edge_data = (edge_data_type)(entry->baggage);
  capacity = entry->value;

  wn_assert(flow <= capacity);

  edge_data->flow = flow;
  edge_data->forward_residual_flow = capacity-flow;
}


local void increment_edge_flow(wn_sparse_matrix_entry entry,double flow)
{
  edge_data_type edge_data;
  double capacity;
  double new_flow;

  wn_assert(flow >= 0.0);

  edge_data = (edge_data_type)(entry->baggage);
  capacity = entry->value;

  wn_assert(flow <= edge_data->forward_residual_flow);

  if(flow == edge_data->forward_residual_flow)
  {
    /* naylor 2005 June 23: if no numerical error, then 
       edge_data->flow + flow == capacity.
       But because of numerical error, difference is possible.  The
       algorithm is trying to fully capacitate this edge, so we set
       flow == capacity
    */
    new_flow = capacity;
  }
  else
  {
    new_flow = edge_data->flow + flow;

    if(new_flow > capacity)
    {
      new_flow = capacity;
    }
  }

  set_edge_flow(entry,new_flow);
}


local void decrement_edge_flow(wn_sparse_matrix_entry entry,double flow)
{
  edge_data_type edge_data;
  /*
  double capacity;
  */
  double new_flow;

  wn_assert(flow >= 0.0);

  edge_data = (edge_data_type)(entry->baggage);
  /*
  capacity = entry->value;
  */

  wn_assert(flow <= edge_data->flow);

  new_flow = edge_data->flow - flow;

  wn_assert(new_flow >= 0.0);

  set_edge_flow(entry,new_flow);
}


local void setup_edge_data(void)
{
  int i;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;

  for(i=0;i<len;++i)
  {
    for(el=(capacity_mat->i_lists)[i];el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      edge_data = (edge_data_type)
		   wn_zalloc(sizeof(struct edge_data_type_struct));

      entry->baggage = edge_data;

      set_edge_flow(entry,0.0);

      edge_data->cost = 0.0;
    }
  }
}


local void clear_random_nodes_to_expand(void)
{
  int i;

  num_random_nodes_to_expand = 0;

  for(i=0;i<len;++i)
  {
    random_nodes_to_expand[i] = -1;
    random_node_to_expand_indexes[i] = -1;
  }
}


local void make_random_nodes_to_expand(void)
{
  random_nodes_to_expand = (int *)wn_zalloc(len*sizeof(int));
  random_node_to_expand_indexes = (int *)wn_zalloc(len*sizeof(int));

  clear_random_nodes_to_expand();
}


local void insert_random_node_to_expand(int node_index)
{
  wn_assert(random_node_to_expand_indexes[node_index] == -1);

  random_node_to_expand_indexes[node_index] = num_random_nodes_to_expand;
  random_nodes_to_expand[num_random_nodes_to_expand] = node_index;

  ++num_random_nodes_to_expand;
  wn_assert(num_random_nodes_to_expand < len);
}


local void delete_random_node_to_expand(int node_index)
{
  int index;
  int move_node_index;

  --num_random_nodes_to_expand;

  index = random_node_to_expand_indexes[node_index];
  wn_assert(index != -1);

  if(index < num_random_nodes_to_expand)
  {
    move_node_index = random_nodes_to_expand[num_random_nodes_to_expand];
    random_nodes_to_expand[index] = move_node_index;
    random_node_to_expand_indexes[move_node_index] = index;
  }

  random_nodes_to_expand[num_random_nodes_to_expand] = -1;
  random_node_to_expand_indexes[node_index] = -1;
}


local void get_random_node_to_expand(int *prandom_node_index)
{
  int index;

  wn_assert(num_random_nodes_to_expand >= 0);

  if(num_random_nodes_to_expand == 0)
  {
    *prandom_node_index = -1;  /* fail */
  }
  else
  {
    index = wn_random_mod_int(num_random_nodes_to_expand);

    *prandom_node_index = random_nodes_to_expand[index];
  }
}


local void initialize(void)
{
  len = capacity_mat->len_i;
  wn_assert(capacity_mat->len_j == len);
  if(cost_mat != NULL)
  {
    wn_assert(cost_mat->len_i == len);
    wn_assert(cost_mat->len_j == len);
  }
  wn_assert(source_i_index >= 0);
  wn_assert(source_i_index < len);
  wn_assert(dest_j_index >= 0);
  wn_assert(dest_j_index < len);

  wn_sort_sparse_matrix(capacity_mat);
  wn_sort_sparse_matrix(cost_mat);

  num_edges = wn_sparse_matrix_entry_count(capacity_mat);

  pending_id = done_id = 0;

  make_random_nodes_to_expand();

  setup_node_data_array();
  setup_edge_data();

  total_cost = 0.0;
}


local void newstart_node_expand_edge(node_data_type node_data)
{
  node_data->start_next_is_outgoing = node_data->next_is_outgoing;
  node_data->start_next_edge_el = node_data->next_edge_el;
}


local bool node_expand_edge_is_start(node_data_type node_data)
{
  return(node_data->next_edge_el == node_data->start_next_edge_el);
}


local void increment_node_expand_edge(node_data_type node_data)
{
  if((node_data->outgoing_edges == NULL)&&(node_data->incoming_edges == NULL))
  {
    node_data->next_edge_el = NULL;
    return;
  }

  if(node_data->next_edge_el == NULL)
  {
    node_data->next_is_outgoing = !node_data->next_is_outgoing;

    if(node_data->next_is_outgoing)
    {
      node_data->next_edge_el = node_data->outgoing_edges;
    }
    else
    {
      node_data->next_edge_el = node_data->incoming_edges;
    }
  }
  else
  {
    node_data->next_edge_el = node_data->next_edge_el->next;

    if(node_data->next_edge_el == NULL)
    {
      node_data->next_is_outgoing = !node_data->next_is_outgoing;

      if(node_data->next_is_outgoing)
      {
        node_data->next_edge_el = node_data->outgoing_edges;
      }
      else
      {
        node_data->next_edge_el = node_data->incoming_edges;
      }
    }
  }

  /*
  if(node_data->next_edge_el != NULL)
  {
    node_data->next_edge_el = node_data->next_edge_el->next;
  }
  else
    node_data->next_edge_el = node_data->next_edge_el->next;
  {
    while(node_data->next_edge_el == NULL)
    {
      node_data->next_is_outgoing = !node_data->next_is_outgoing;

      if(node_data->next_is_outgoing)
      {
        node_data->next_edge_el = node_data->outgoing_edges;
      }
      else
      {
        node_data->next_edge_el = node_data->incoming_edges;
      }
    }
  }
  */
}


local void expand_node(int *pnew_node_index,int node_to_expand_index)
{
  node_data_type node_to_expand,expand_into_node;
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;

  node_to_expand = node_data_array[node_to_expand_index];
  
  if(node_to_expand->next_edge_el == NULL)
  {
    *pnew_node_index = -1;
    return;
  }

  entry = (wn_sparse_matrix_entry)(node_to_expand->next_edge_el->contents);
  edge_data = (edge_data_type)(entry->baggage);

  if(node_to_expand->next_is_outgoing)
  {
    *pnew_node_index = entry->j;
  }
  else
  {
    *pnew_node_index = entry->i;
  }

  expand_into_node = node_data_array[*pnew_node_index];
  if(expand_into_node->traversal_id >= pending_id)
  {
    *pnew_node_index = -1;
    return;
  }

  expand_into_node->in_entry = entry;					     
  expand_into_node->in_edge_is_incoming = node_to_expand->next_is_outgoing;
  if(expand_into_node->in_edge_is_incoming)
  {
    if(edge_data->forward_residual_flow <= 0.0)  /* no flow left */
    {
      wn_assert(edge_data->forward_residual_flow == 0.0);

      *pnew_node_index = -1;
      return;
    }

    expand_into_node->augment_in_flow = 
      wn_min(node_to_expand->augment_in_flow,edge_data->forward_residual_flow);
  }
  else
  {
    if(edge_data->flow <= 0.0)  /* no flow left */
    {
      wn_assert(edge_data->flow == 0.0);

      *pnew_node_index = -1;
      return;
    }

    expand_into_node->augment_in_flow = 
      wn_min(node_to_expand->augment_in_flow,edge_data->flow);
  }
}


local void augment_flow_of_path_edges(void)
{
  double flow;
# if PRINT
  int path_len;
# endif
  int node_index,next_node_index;
  wn_sparse_matrix_entry in_entry;
  edge_data_type edge_data;
  node_data_type node;

# if PRINT
  path_len = 0;
# endif

  flow = node_data_array[dest_j_index]->augment_in_flow;

  for(node_index=dest_j_index;node_index != source_i_index;
      node_index=next_node_index)
  {
    node = node_data_array[node_index];
    in_entry = node->in_entry;
    edge_data = (edge_data_type)(in_entry->baggage);

    if(node->in_edge_is_incoming)
    {
      /*
      set_edge_flow(in_entry,edge_data->flow + flow);
      */
      increment_edge_flow(in_entry,flow);
      next_node_index = in_entry->i;
    }
    else
    {
      /*
      set_edge_flow(in_entry,edge_data->flow - flow);
      */
      decrement_edge_flow(in_entry,flow);
      next_node_index = in_entry->j;
    }

#   if PRINT
      ++path_len;
#   endif
  }

# if PRINT
    printf("augment path_len = %d\n",path_len);
# endif
}


local void random_augment_flow(bool *psuccess,double *pflow)
{
  int new_node_index,node_to_expand_index;
  node_data_type node_to_expand;

  reset_traversal_ids();

  clear_random_nodes_to_expand();

  new_node_index = source_i_index;

  while(new_node_index != dest_j_index)
  {
    if(new_node_index != -1)
    {
      newstart_node_expand_edge(node_data_array[new_node_index]);
      node_data_array[new_node_index]->traversal_id = pending_id;

      insert_random_node_to_expand(new_node_index);
    }

    get_random_node_to_expand(&node_to_expand_index);
    if(node_to_expand_index == -1)
    {
      *pflow = 0.0;
      *psuccess = FALSE;
      return;
    }

    expand_node(&new_node_index,node_to_expand_index);

    node_to_expand = node_data_array[node_to_expand_index];

    increment_node_expand_edge(node_to_expand);
    if(node_expand_edge_is_start(node_to_expand))
    {
      node_to_expand->traversal_id = done_id;
      delete_random_node_to_expand(node_to_expand_index);
    }
  }

  augment_flow_of_path_edges();

  *psuccess = TRUE;
  *pflow = node_data_array[dest_j_index]->augment_in_flow;
}


local void maximize_flow(void)
{
  double flow;
  bool success;
# if PRINT
  int iterations;
# endif

# if PRINT
    printf("starting maximize_flow.\n");
    fflush(stdout);
  
    iterations = 0;
# endif

  node_data_array[source_i_index]->augment_in_flow = WN_FHUGE;
  total_flow = 0.0;

  for(;;)
  {
    random_augment_flow(&success,&flow);
    if(!success)
    {
      break;
    }

    total_flow += flow;

#   if PRINT
      ++iterations;
      printf("maximize_flow: iterations = %d,flow = %lg,total_flow = %lg\n",
	     iterations,flow,total_flow);
      fflush(stdout);
#   endif
  }

# if PRINT
    printf("finish maximize_flow(): iterations = %d,total_flow = %lg\n",
	   iterations,total_flow);
    fflush(stdout);
# endif
}


local void initialize_node_unexpanded_edges_by_cost(int node_index)
{
  node_data_type node_data;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;
  wn_bhandle handle;
  double cost;

  node_data = node_data_array[node_index];

  wn_mkdoublebtree(&(node_data->unexpanded_edges_by_cost));

  for(el=node_data->incoming_edges;el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);
    if(entry->i == entry->j)
    {
      continue;
    }
    edge_data = (edge_data_type)(entry->baggage);

    wn_assert(entry->j == node_index);

    if(edge_data->flow > 0.0)
    {
      cost = -edge_data->cost;

      wn_bins(&handle,node_data->unexpanded_edges_by_cost,(ptr)&cost);

      handle->contents = entry;
      edge_data->j_handle = handle;
    }
  }
  for(el=node_data->outgoing_edges;el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);
    if(entry->i == entry->j)
    {
      continue;
    }
    edge_data = (edge_data_type)(entry->baggage);

    wn_assert(entry->i == node_index);

    if(edge_data->forward_residual_flow > 0.0)
    {
      cost = edge_data->cost;

      wn_bins(&handle,node_data->unexpanded_edges_by_cost,(ptr)&cost);

      handle->contents = entry;
      edge_data->i_handle = handle;
    }
  }
}


local void capacitate_negative_cost_edges(void)
{
  int node_index;
  node_data_type node_data;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;
  double delta_flow;

  for(node_index=0;node_index<len;++node_index)
  {
    node_data = node_data_array[node_index]; 

    for(el=node_data->outgoing_edges;el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);
      if(entry->i == entry->j)
      {
        edge_data = (edge_data_type)(entry->baggage);

	if(edge_data->cost >= 0.0)
	{
	  delta_flow = -edge_data->flow;
          set_edge_flow(entry,0.0);
	}
	else
	{
	  wn_assert(edge_data->cost < 0.0);

          delta_flow = edge_data->forward_residual_flow;
          set_edge_flow(entry,entry->value);
	}

        total_cost += delta_flow*edge_data->cost;

#       if PRINT   
          printf("capacitate_negative_edge_costs: %d->%d total_cost=%lg\n",
		 node_index,node_index,total_cost);
#       endif
      }
    }
  }
}


local void initialize_minimize_cost(void)
{
  int i;
  wn_sparse_matrix_entry capacity_entry,cost_entry;
  wn_sll capacity_el,cost_el;
  edge_data_type edge_data;

  for(i=0;i<len;++i)
  {
    capacity_el = (capacity_mat->i_lists)[i];
    cost_el = (cost_mat->i_lists)[i];

    while((capacity_el != NULL)&&(cost_el != NULL))
    {
      capacity_entry = (wn_sparse_matrix_entry)(capacity_el->contents);
      cost_entry = (wn_sparse_matrix_entry)(cost_el->contents);

      if(capacity_entry->j > cost_entry->j)
      {
	cost_el = cost_el->next;
      }
      else if(cost_entry->j > capacity_entry->j)
      {
	capacity_el = capacity_el->next;
      }
      else
      {
	wn_assert(cost_entry->j == capacity_entry->j);

	edge_data = (edge_data_type)(capacity_entry->baggage);
	edge_data->cost = cost_entry->value;

	cost_el = cost_el->next;
	capacity_el = capacity_el->next;
      }
    }
  }

  node_permutation_array = random_nodes_to_expand;  /* reuse memory */
  node_permutation_pointer = len;
  optimality_flag = FALSE;

  for(i=0;i<len;++i)
  {
    initialize_node_unexpanded_edges_by_cost(i);
  }
}


local double compute_total_cost(void)
{
  double sum;
  /* double value; ** unused - bchapman 041111 */
  int i;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;

  sum = 0.0;

  for(i=0;i<len;++i)
  {
    for(el=(capacity_mat->i_lists)[i];el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);
      edge_data = (edge_data_type)(entry->baggage);

      sum += edge_data->flow*edge_data->cost;
    }
  }

  return(sum);
}


local void reset_node_unexpanded_edges(int node_index)
{
  node_data_type node_data;
  wn_bhandle handle;

  node_data = node_data_array[node_index];

  wn_bget_handle_of_index(&handle,node_data->unexpanded_edges_by_cost,0);

  node_data->best_unexpanded_edge_index = 0;

  if(handle == NULL)
  {
    node_data->best_unexpanded_edge_cost = WN_FHUGE;
    node_data->best_unexpanded_edge = NULL;
  }
  else
  {
    node_data->best_unexpanded_edge_cost = *(double *)(handle->key);
    node_data->best_unexpanded_edge = 
		     (wn_sparse_matrix_entry)(handle->contents);
  }
}


local void increment_node_unexpanded_edges(int node_index)
{
  node_data_type node_data;
  wn_bhandle handle;

  node_data = node_data_array[node_index];

  ++(node_data->best_unexpanded_edge_index);

  wn_bget_handle_of_index(&handle,node_data->unexpanded_edges_by_cost,
			  node_data->best_unexpanded_edge_index);

  if(handle == NULL)
  {
    node_data->best_unexpanded_edge_cost = WN_FHUGE;
    node_data->best_unexpanded_edge = NULL;
  }
  else
  {
    node_data->best_unexpanded_edge_cost = *(double *)(handle->key);
    node_data->best_unexpanded_edge = 
			    (wn_sparse_matrix_entry)(handle->contents);
  }
}


local void insert_node_index_into_wavefront(int node_index)
{
  wn_bhandle handle;
  node_data_type node_data;
  double cost;

  node_data = node_data_array[node_index];

  if(node_data->traversal_id < pending_id)
  {
    node_data->handle = NULL;
  }
  node_data->traversal_id = pending_id;

  wn_assert(node_data->best_unexpanded_edge_cost < WN_FHUGE);

  cost = node_data->best_sum_cost + node_data->best_unexpanded_edge_cost;

  if(node_data->handle == NULL)
  {
    wn_bins(&handle,node_indexes_by_sum_cost,(ptr)&cost);

    handle->contents = (ptr)node_index;
    node_data->handle = handle;
  }
  else
  {
    wn_bmove(node_data->handle,node_indexes_by_sum_cost,(ptr)&cost);
  }

  time_count += 1;
}


local void delete_node_index_from_wavefront(int node_index)
{
  wn_bhandle handle;
  node_data_type node_data;

  node_data = node_data_array[node_index];
  handle = node_data->handle;

  wn_assert(handle != NULL);

  wn_bdel(handle,node_indexes_by_sum_cost);

  node_data->handle = NULL;
}


local void get_best_sum_cost_node_index(bool *psuccess,int *pnode_index)
{
  wn_bhandle handle;

  wn_bget(&handle,node_indexes_by_sum_cost,(ptr)NULL,WN_B_MIN);

  *psuccess = (handle != NULL);

  if(*psuccess)
  {
    *pnode_index = (int) (long) (handle->contents);
  }
}


local void check_for_loop(bool *ploop_detected,int node_index)
{
  node_data_type node_data;

  node_data = node_data_array[node_index];

  if(node_data->loop_node_index == node_index)
  {
    *ploop_detected = TRUE;
    return;
  }

  if(node_data->path_length >= node_data->reset_loop_check_path_length)
  {
    node_data->loop_node_index = node_index;
    node_data->reset_loop_check_path_length *= 2;
  }

  *ploop_detected = FALSE;
}


local void expand_best_node_index(bool *ploop_detected,int node_index)
{
  node_data_type node_data,next_node_data;
  /* wn_sll el; ** unused - bchapman 041111 */
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;
  int next_node_index;
  double next_cost;

  *ploop_detected = FALSE;

  node_data = node_data_array[node_index];
  node_data->node_touched = TRUE;

  next_cost = node_data->best_sum_cost + node_data->best_unexpanded_edge_cost;
  entry = node_data->best_unexpanded_edge;
  edge_data = (edge_data_type)(entry->baggage);
  if(entry->i == node_index)
  {
    wn_assert(edge_data->forward_residual_flow > 0.0);
    next_node_index = entry->j;
  }
  else
  {
    wn_assert(entry->j == node_index);
    wn_assert(edge_data->flow > 0.0);
    next_node_index = entry->i;
  }
  next_node_data = node_data_array[next_node_index];

  if(
      (next_node_data->traversal_id < pending_id)
        ||
      (next_cost < next_node_data->best_sum_cost-TOL)
    )
  {
    next_node_data->in_edge_is_incoming = (entry->j == next_node_index);
    next_node_data->in_entry = entry;
    next_node_data->best_sum_cost = next_cost;
    next_node_data->path_length = node_data->path_length + 1;
    next_node_data->reset_loop_check_path_length = 
				node_data->reset_loop_check_path_length;
    next_node_data->loop_node_index = node_data->loop_node_index;
    next_node_data->node_touched = TRUE;
	  
    check_for_loop(ploop_detected,next_node_index); 
    if(!(*ploop_detected))
    {
      reset_node_unexpanded_edges(next_node_index);
      if(next_node_data->best_unexpanded_edge_cost < WN_FHUGE)
      {
        insert_node_index_into_wavefront(next_node_index);
      }
    }
  }

  increment_node_unexpanded_edges(node_index);
  if(node_data->best_unexpanded_edge_cost < WN_FHUGE)
  {
    insert_node_index_into_wavefront(node_index);
  }
  else
  {
    delete_node_index_from_wavefront(node_index);
  }
}


local void isolate_improving_cycle(int *ploop_node_index,int start_node_index)
{
  int node_index;
  int count,loop_check_count;
  wn_sparse_matrix_entry in_entry;
  edge_data_type edge_data;
  node_data_type node;

  *ploop_node_index = start_node_index;
  loop_check_count = 1;
  count = 0;

  node_index=start_node_index;
  do
  {
    node = node_data_array[node_index];
    in_entry = node->in_entry;
    edge_data = (edge_data_type)(in_entry->baggage);

    if(node->in_edge_is_incoming)
    {
      node_index = in_entry->i;
    }
    else
    {
      node_index = in_entry->j;
    }

    if(node_index == *ploop_node_index)
    {
      return;
    }

    ++count;
    if(count >= loop_check_count)
    {
      count = 0;
      loop_check_count *= 2;
      *ploop_node_index = node_index;
    }
  }
  while(node_index != start_node_index);

  *ploop_node_index = start_node_index;
}


local void update_edge_btrees
(
  wn_sparse_matrix_entry entry,
  double delta_flow,
  bool forward_residual_flow_was_zero,
  bool flow_was_zero
)
{
  int node_index,index_in_btree;
  node_data_type node_data;
  edge_data_type edge_data;
  wn_bhandle handle;
  double cost;
  wn_sparse_matrix_entry new_best_unexpanded_edge;

  edge_data = (edge_data_type)(entry->baggage);

  if(delta_flow < 0.0)
  {
    /*
    if(edge_data->forward_residual_flow == -delta_flow)
    */
    if(forward_residual_flow_was_zero)
    {
      /* insert */
      node_index = entry->i;
      node_data = node_data_array[node_index];
      wn_assert(edge_data->i_handle == NULL);
      cost = edge_data->cost;
      wn_bins(&handle,node_data->unexpanded_edges_by_cost,(ptr)&cost);
      handle->contents = entry;
      edge_data->i_handle = handle;
      wn_bget_index_of_handle(&index_in_btree,
			      node_data->unexpanded_edges_by_cost,
			      handle);
      if(index_in_btree <= node_data->best_unexpanded_edge_index)
      {
	node_data->best_unexpanded_edge_index = index_in_btree;
	node_data->best_unexpanded_edge = entry;
	node_data->best_unexpanded_edge_cost = cost;
      }
    }
    if(edge_data->flow == 0.0)
    {
      /* delete */
      node_index = entry->j;
      node_data = node_data_array[node_index];
      wn_assert(edge_data->j_handle != NULL);
      handle = edge_data->j_handle;
      wn_bget_index_of_handle(&index_in_btree,
			      node_data->unexpanded_edges_by_cost,
			      handle);
      wn_bdel(handle,node_data->unexpanded_edges_by_cost);
      edge_data->j_handle = NULL;
      if(index_in_btree <= node_data->best_unexpanded_edge_index)
      {
	wn_bget_handle_of_index(&handle,node_data->unexpanded_edges_by_cost,
				index_in_btree);
	if(handle != NULL)
	{
	  new_best_unexpanded_edge = (wn_sparse_matrix_entry)(handle->contents);
	  cost = *(double *)(handle->key);

	  node_data->best_unexpanded_edge_index = index_in_btree;
	  node_data->best_unexpanded_edge = new_best_unexpanded_edge;
	  node_data->best_unexpanded_edge_cost = cost;
	}
      }
    }
  }
  else if(delta_flow > 0.0)
  {
    /* if(edge_data->flow == delta_flow) */
    if (flow_was_zero) 
    {
      /* insert */
      node_index = entry->j;
      node_data = node_data_array[node_index];
      wn_assert(edge_data->j_handle == NULL);
      cost = -edge_data->cost;
      wn_bins(&handle,node_data->unexpanded_edges_by_cost,(ptr)&cost);
      handle->contents = entry;
      edge_data->j_handle = handle;
      wn_bget_index_of_handle(&index_in_btree,
			      node_data->unexpanded_edges_by_cost,
			      handle);
      if(index_in_btree <= node_data->best_unexpanded_edge_index)
      {
	node_data->best_unexpanded_edge_index = index_in_btree;
	node_data->best_unexpanded_edge = entry;
	node_data->best_unexpanded_edge_cost = cost;
      }
    }
    if(edge_data->forward_residual_flow == 0.0)
    {
      /* delete */
      node_index = entry->i;
      node_data = node_data_array[node_index];
      wn_assert(edge_data->i_handle != NULL);
      handle = edge_data->i_handle;
      wn_bget_index_of_handle(&index_in_btree,
			      node_data->unexpanded_edges_by_cost,
			      handle);
      wn_bdel(handle,node_data->unexpanded_edges_by_cost);
      edge_data->i_handle = NULL;
      if(index_in_btree <= node_data->best_unexpanded_edge_index)
      {
	wn_bget_handle_of_index(&handle,node_data->unexpanded_edges_by_cost,
				index_in_btree);
	if(handle != NULL)
	{
	  new_best_unexpanded_edge = (wn_sparse_matrix_entry)(handle->contents);
	  cost = *(double *)(handle->key);

	  node_data->best_unexpanded_edge_index = index_in_btree;
	  node_data->best_unexpanded_edge = new_best_unexpanded_edge;
	  node_data->best_unexpanded_edge_cost = cost;
	}
      }
    }
  }
  else
  {
    wn_assert_notreached();
  }
}


local void augment_improving_cycle(double *pmin_flow,double *psum_cost,
				   int start_node_index)
{
# if PRINT
  int path_len;
# endif
  int node_index;
  wn_sparse_matrix_entry in_entry;
  edge_data_type edge_data;
  node_data_type node;

  isolate_improving_cycle(&start_node_index,start_node_index);

# if PRINT
    path_len = 0;
# endif

  /* compute min_flow and sum_cost */
  *pmin_flow = WN_FHUGE;
  *psum_cost = 0.0;

  node_index=start_node_index;
  do
  {
    node = node_data_array[node_index];
    in_entry = node->in_entry;
    edge_data = (edge_data_type)(in_entry->baggage);

    if(node->in_edge_is_incoming)
    {
      *psum_cost += edge_data->cost;
      if(edge_data->forward_residual_flow < *pmin_flow)
      {
	*pmin_flow = edge_data->forward_residual_flow;
      }
      
      node_index = in_entry->i;
    }
    else
    {
      *psum_cost -= edge_data->cost;
      if(edge_data->flow < *pmin_flow)
      {
	*pmin_flow = edge_data->flow;
      }

      node_index = in_entry->j;
    }

#   if PRINT
      ++path_len;
#   endif
  }
  while(node_index != start_node_index);

# if PRINT
    printf("augment path_len = %d\n",path_len);
# endif

  /* augment cycle */
  node_index=start_node_index;
  do
  {
    node = node_data_array[node_index];
    in_entry = node->in_entry;
    edge_data = (edge_data_type)(in_entry->baggage);

    if(node->in_edge_is_incoming)
    {
      bool flow_was_zero;

      flow_was_zero = 
	(edge_data->flow == 0.0);
      /*
      set_edge_flow(in_entry,edge_data->flow + *pmin_flow);
      */
      increment_edge_flow(in_entry,*pmin_flow);
      update_edge_btrees(in_entry,*pmin_flow,FALSE,flow_was_zero);
      node_index = in_entry->i;
    }
    else
    {
      bool forward_residual_flow_was_zero;

      forward_residual_flow_was_zero = 
        (edge_data->forward_residual_flow == 0.0);
      /*
      set_edge_flow(in_entry,edge_data->flow - *pmin_flow);
      */
      decrement_edge_flow(in_entry,*pmin_flow);
      update_edge_btrees(in_entry,-(*pmin_flow),
                         forward_residual_flow_was_zero,FALSE);
      node_index = in_entry->j;
    }
  }
  while(node_index != start_node_index);
}


local void clear_nodes_touched(void)
{
  int i;
  node_data_type node_data;

  for(i=0;i<len;++i)
  {
    node_data = node_data_array[i];

    node_data->node_touched = FALSE;
  }
}


local void find_untouched_node(bool *poptimal,int *pnode_index)
{
  node_data_type node_data;

  for(;;)
  {
    if(node_permutation_pointer >= len)
    {
      if(optimality_flag == TRUE)
      {
	*poptimal = TRUE;
	return;
      }

      node_permutation_pointer = 0;
      wn_random_permutation(node_permutation_array,len);
      clear_nodes_touched();
      optimality_flag = TRUE;
    }

    *pnode_index = node_permutation_array[node_permutation_pointer];
    ++node_permutation_pointer;
    node_data = node_data_array[*pnode_index];
    /*
    if(!(node_data->node_touched))
    */
    if((!(optimality_flag&&node_data->node_touched))
	 &&
       (wn_bcount(node_data->unexpanded_edges_by_cost) > 0))
    {
      *poptimal = FALSE;
      return;
    }
  }
}


local void random_find_improving_cycle
(
  bool *psuccess,
  bool *poptimal,
  double *pflow,
  double *psum_cost
)
{
  int start_node_index;
  int node_index;
  node_data_type start_node_data;
  bool success,loop_detected;

  wn_gpmake("general_free");

  reset_traversal_ids();

  *psuccess = FALSE;
  *pflow = 0.0;
  *psum_cost = 0.0;

  wn_mkdoublebtree(&node_indexes_by_sum_cost);
  time_count = 0.0;

  find_untouched_node(poptimal,&start_node_index);
  if(*poptimal)
  {
#   if PRINT 
      printf("optimal!!!!!!\n");
      fflush(stdout);
#   endif
    wn_gpfree();
    return;
  }

  start_node_data = node_data_array[start_node_index];
  start_node_data->in_entry = NULL;
  start_node_data->best_sum_cost = 0.0;
  start_node_data->path_length = 0;
  start_node_data->reset_loop_check_path_length = 3;
  start_node_data->loop_node_index = start_node_index;
  reset_node_unexpanded_edges(start_node_index);

  insert_node_index_into_wavefront(start_node_index);

  for(;;)
  {
    get_best_sum_cost_node_index(&success,&node_index);
    if(!success)
    {
      *psuccess = FALSE;      

#     if PRINT 
        printf("optimal?\n");
        fflush(stdout);
#     endif

      if(optimality_flag == FALSE)
      {
        node_permutation_pointer = len;
      }

      break;   /* optimal solution found possibly */
    }

    expand_best_node_index(&loop_detected,node_index);

    if(loop_detected||(start_node_data->best_sum_cost < 0.0))
    {
      *psuccess = TRUE;

#if PRINT
      if(loop_detected)
      {
	printf("minimize_cost: negative weight subloop detected.\n");
        fflush(stdout);
      }
#endif
      augment_improving_cycle(pflow,psum_cost,node_index);

      optimality_flag = FALSE;

      break;
    }
    if(time_count >= max_time_count)
    {
      optimality_flag = FALSE;

      break;
    }
  }

  wn_gpfree();
}


local void minimize_cost(void)
{
  bool success,optimal;
  int no_success_count;
  double flow,sum_cost;
# if PRINT
  double delta_cost;
  int iterations;
# endif

  initialize_minimize_cost();

# if PRINT
    total_cost = compute_total_cost();

    printf("starting minimize_cost: total_cost = %lg\n",total_cost);
    fflush(stdout);
  
    iterations = 0;
# endif

  capacitate_negative_cost_edges();

  /*
  for(max_time_count=5.0;;max_time_count*=2.0)
  */
  max_time_count = WN_IHUGE;
  {
    no_success_count = 0;

    for(;;)
    {
      random_find_improving_cycle(&success,&optimal,&flow,&sum_cost);

#     if PRINT
        ++iterations;
#     endif

      if(success)
      {
	no_success_count = 0;

#       if PRINT
          delta_cost = flow*sum_cost;
          total_cost += delta_cost;

          printf("minimize_cost: iterations = %d,flow = %lg,sum_cost = %lg\n",
	         iterations,flow,sum_cost);
          printf("               delta_cost = %lg,total_cost = %lg\n",
		 delta_cost,total_cost);
          printf("               time_count = %lg,max_time_count = %lg\n",
		 time_count,max_time_count);
          fflush(stdout);
#       endif
      }
      else
      {
#       if PRINT
          printf("minimize_cost: iterations = %d fail.\n",iterations);
          fflush(stdout);
#       endif

	if(optimal)
	{
	  goto finished;
	}
#if 0
	else
	{
	  ++no_success_count;
	  if((no_success_count > 6)&&
	     (no_success_count > num_edges/max_time_count))
	  {
	    break;  /* try a bigger search area */
	  }
	}
#endif
      }
    }
  }

finished: ;
  total_cost = compute_total_cost();

# if PRINT
    printf("finish minimize_cost(): iterations = %d,total_cost = %lg\n",
	   iterations,total_cost);
    fflush(stdout);
# endif
}


local void construct_result(wn_sparse_matrix *presult)
{
  int i;
  double flow;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  edge_data_type edge_data;

  wn_make_sparse_matrix(presult,len,len);

  for(i=0;i<len;++i)
  {
    for(el=(capacity_mat->i_lists)[i];el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);
      edge_data = (edge_data_type)(entry->baggage);

      flow = edge_data->flow;
      wn_assert(flow >= 0.0);
      if(flow > 0.0)
      {
        wn_insert_sparse_matrix_value(*presult,flow,i,entry->j);
      }
    }
  }

  wn_sort_sparse_matrix(*presult);
}


void wn_max_flow
(
  double *pflow,
  double *pcost,
  wn_sparse_matrix *presult,
  wn_sparse_matrix passed_capacity_mat,
  wn_sparse_matrix passed_cost_mat,
  int passed_source_i_index,
  int passed_dest_j_index
)
{
  wn_memgp top_group;

  top_group = wn_curgp();
  wn_gpmake("general_free");

  capacity_mat = passed_capacity_mat;
  cost_mat = passed_cost_mat;
  source_i_index = passed_source_i_index;
  dest_j_index = passed_dest_j_index;

  initialize();

  maximize_flow();

  if(cost_mat != NULL)
  {
    minimize_cost();
  }

  wn_gppush(top_group);
  construct_result(presult);
  wn_gppop();

  *pflow = total_flow;
  *pcost = total_cost;

  wn_gpfree();
}



