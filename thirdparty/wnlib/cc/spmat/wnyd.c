/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************

wn_y_delta_reduce(preduced_mat,mat,significant_nodes,
                  ppassed_series_reduce,ppassed_parallel_reduce,
		  ppassed_y_to_delta,ppassed_delta_to_y)

******************************************************************************/

#include "wnlib.h"
#include "wnswap.h"
#include "wnsmat.h"



local wn_memgp top_group;
local void (*pseries_reduce)(/*total,v1,v2*/),
           (*pparallel_reduce)(/*total,v1,v2*/),
           (*py_to_delta)(/*&v23,&v13,&v12,v1,v2,v3*/),
           (*pdelta_to_y)(/*&v1,&v2,&v3,v23,v13,v12*/);
local bool y_to_delta_reduce_allowed,delta_to_y_reduce_allowed;
local int len;

typedef struct node_data_struct
{
  int i,degree;
  bool significant;

  wn_bll edge_list;
  
  wn_bll degree_el,node_list_el;
} *node_data;

typedef struct edge_connection_data_struct
{
  node_data nodei,nodej;
} *edge_connection_data;

typedef struct edge_data_struct
{
  edge_connection_data connection;

  double weight;

  wn_bll eli,elj,edge_list_el;
} *edge_data;

local wn_htab edges_by_connection,significant_node_table;
local wn_bll degree1_nodes,degree2_nodes,degree3_nodes,
	     node_list,edge_list;



wn_y_delta_reduce(preduced_mat,mat,significant_nodes,
                  ppassed_series_reduce,ppassed_parallel_reduce,
		  ppassed_y_to_delta,ppassed_delta_to_y)

wn_sparse_matrix *preduced_mat,mat;
wn_set significant_nodes;
void (*ppassed_series_reduce)(/*total,v1,v2*/),
     (*ppassed_parallel_reduce)(/*total,v1,v2*/),
     (*ppassed_y_to_delta)(/*&v23,&v13,&v12,v1,v2,v3*/),
     (*ppassed_delta_to_y)(/*&v1,&v2,&v3,v23,v13,v12*/);

{
  top_group = wn_curgp();
  wn_gpmake("no_free");

  initialize(ppassed_series_reduce,ppassed_parallel_reduce,
	     ppassed_y_to_delta,ppassed_delta_to_y);
  build_graph_for_mat(mat,significant_nodes);

  reduce_graph();

  build_mat_for_graph(preduced_mat);

  wn_gpfree();
}



local initialize(ppassed_series_reduce,ppassed_parallel_reduce,
	         ppassed_y_to_delta,ppassed_delta_to_y)

void (*ppassed_series_reduce)(/*total,v1,v2*/),
     (*ppassed_parallel_reduce)(/*total,v1,v2*/),
     (*ppassed_y_to_delta)(/*&v23,&v13,&v12,v1,v2,v3*/),
     (*ppassed_delta_to_y)(/*&v1,&v2,&v3,v23,v13,v12*/);

{
  int hash_connection();
  bool connections_eq();
  extern void wn_ptrcpy(),wn_do_nothing();

  pseries_reduce = ppassed_series_reduce;
  pparallel_reduce = ppassed_parallel_reduce;
  py_to_delta = ppassed_y_to_delta;
  pdelta_to_y = ppassed_delta_to_y;

  y_to_delta_reduce_allowed = (py_to_delta != NULL);
  delta_to_y_reduce_allowed = (pdelta_to_y != NULL);

  wn_mkhtab(&edges_by_connection,
	    (hash_connection),(connections_eq),
	    (wn_ptrcpy),(wn_do_nothing));

  degree1_nodes = NULL;
  degree2_nodes = NULL;
  degree3_nodes = NULL;

  node_list = NULL;
  edge_list = NULL;
}



local int hash_connection(connection)

edge_connection_data connection;

{
  return(wn_blkhash((ptr)connection,
		    wn_sizeof(wn_structof(edge_connection_data))));
}



local bool connections_eq(c1,c2)

edge_connection_data c1,c2;

{
  return(
	  (c1->nodei == c2->nodei)
	    &&
	  (c1->nodej == c2->nodej)
	);
}



local build_graph_for_mat(mat,significant_nodes)

wn_sparse_matrix mat;
wn_set significant_nodes;

{
  wn_htab node_table;

  wn_assert(mat->len_i == mat->len_j);

  len = mat->len_i;

  build_significant_node_table(significant_nodes);
  make_nodes(&node_table);
  make_edges(node_table,mat);

  put_nodes_in_degree_lists();
}



local build_significant_node_table(significant_nodes)

wn_set significant_nodes;

{
  int i;

  wn_mkinthtab(&significant_node_table);

  for(wn_loopinit();wn_setloop(significant_nodes,&i);)
  {
    wn_hins(NULL,significant_node_table,i);    
  }
}



local make_nodes(pnode_table)

wn_htab *pnode_table;

{
  int i;
  ptr dummy;
  bool success;
  node_data node;

  wn_mkinthtab(pnode_table);

  for(i=0;i<len;++i)
  {
    make_node(&node);

    node->i = i;
    node->significant = wn_hget(&dummy,significant_node_table,i);

    success = wn_hins(node,*pnode_table,i);
    wn_assert(success);
  }
}



local make_node(pnode)

node_data *pnode;

{
  *pnode = wn_znew(node_data);

  (*pnode)->i = -1;

  insert_node_into_node_list(*pnode);
}



local insert_node_into_node_list(node)

node_data node;

{
  wn_bllins(&node_list,node);
  node->node_list_el = node_list;
}



local delete_node(node)

node_data node;

{
  delete_node_from_node_list(node);

  wn_free((ptr)node);
}



local delete_node_from_node_list(node)

node_data node;

{
  wn_blledel(node->node_list_el);
}



local make_edges(node_table,mat)

wn_htab node_table;
wn_sparse_matrix mat;

{
  int i;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  for(i=0;i<len;++i)
  {
    for(wn_loopinit(),el=(mat->i_lists)[i];wn_sllloop(&el,&entry);)
    {
      if(entry->j > i)
      {
	install_initial_edge(node_table,entry->value,i,entry->j);
      }
    }
  }
}



local install_initial_edge(node_table,weight,i,j)

wn_htab node_table;
double weight;
int i,j;

{
  edge_data edge;
  node_data nodei,nodej;

  get_node(&nodei,node_table,i);
  get_node(&nodej,node_table,j);

  make_edge(&edge,weight,nodei,nodej);
  insert_edge_into_node_lists(edge);
}



local get_node(pnode,node_table,i)

node_data *pnode;
wn_htab node_table;
int i;

{
  bool success;

  success = wn_hget(pnode,node_table,i);
  wn_assert(success);
}



local make_edge(pedge,weight,nodei,nodej)

edge_data *pedge;
double weight;
node_data nodei,nodej;

{
  edge_connection_data connection;

  make_connection(&connection,nodei,nodej);

  *pedge = wn_znew(edge_data);

  (*pedge)->connection = connection;
  (*pedge)->weight = weight;

  insert_edge_into_edge_list(*pedge);
}



local insert_edge_into_edge_list(edge)

edge_data edge;

{
  wn_bllins(&edge_list,edge);
  edge->edge_list_el = edge_list;
}



local make_connection(pconnection,nodei,nodej)

edge_connection_data *pconnection;
node_data nodei,nodej;

{
  if((long int)nodei > (long int)nodej)
  {
    wn_swap(nodei,nodej,node_data);
  }

  *pconnection = wn_new(edge_connection_data);

  (*pconnection)->nodei = nodei;
  (*pconnection)->nodej = nodej;
}



local delete_edge(edge)

edge_data edge;

{
  delete_connection(edge->connection);

  delete_edge_from_edge_list(edge);

  wn_free((ptr)edge);
}



local delete_connection(connection)

edge_connection_data connection;

{
  wn_free((ptr)connection);
}



local delete_edge_from_edge_list(edge)

edge_data edge;

{
  wn_blledel(edge->edge_list_el);
}



local insert_edge_into_node_lists(edge)

edge_data edge;

{
  node_data nodei,nodej;
  edge_connection_data connection;

  connection = edge->connection;

  nodei = connection->nodei;
  nodej = connection->nodej;

  wn_bllins(&(nodei->edge_list),edge);
  edge->eli = nodei->edge_list;
  ++(nodei->degree);

  wn_bllins(&(nodej->edge_list),edge);
  edge->elj = nodej->edge_list;
  ++(nodej->degree);
}



local put_nodes_in_degree_lists()
{
  node_data node;
  wn_bll el;

  for(wn_loopinit(),el=node_list;wn_bllloop(&el,&node);)
  {
    if(not(node->significant))
    {
      switch(node->degree)
      {
        case(1):
        {
	  insert_node_into_degree_list(&degree1_nodes,node);
        } break;
        case(2):
        {
	  insert_node_into_degree_list(&degree2_nodes,node);
        } break;
        case(3):
        {
	  insert_node_into_degree_list(&degree3_nodes,node);
        } break;
      }
    }
  }
}



local insert_node_into_degree_list(pdegree_list,node)

wn_bll *pdegree_list;
node_data node;

{
  wn_bllins(pdegree_list,node);
  node->degree_el = *pdegree_list;
}



local delete_node_from_degree_list(node)

node_data node;

{
  wn_blledel(node->degree_el);
}



local build_mat_for_graph(preduced_mat)

wn_sparse_matrix *preduced_mat;

{
  wn_bll el;
  edge_data edge;
  edge_connection_data connection;

  allocate_node_ids();

  wn_gppush(top_group);

  wn_make_sparse_matrix(preduced_mat,len,len);

  for(wn_loopinit(),el=edge_list;wn_bllloop(&el,&edge);)
  {
    connection = edge->connection;

    wn_insert_sparse_matrix_value(*preduced_mat,edge->weight,
				  connection->nodei->i,
				  connection->nodej->i);
  }

  wn_sort_sparse_matrix(*preduced_mat);
  wn_symmetrize_sparse_matrix(*preduced_mat);

  wn_gppop();
}



local allocate_node_ids()
{
  wn_bll el;
  node_data node;
  int i;

  i = -1;

  for(wn_loopinit(),el=node_list;wn_bllloop(&el,&node);)
  {
    if(not(node->significant))
    {
      get_new_node_id(&i);

      node->i = i;
    }
  }
}



local get_new_node_id(pi)

int *pi;

{
  ptr dummy;

  while(TRUE)
  {
    ++(*pi);

    if(not(wn_hget(&dummy,significant_node_table,*pi)))
    {
      break;
    }
  }
}



local reduce_graph()
{
  parallel_reduce_start_graph();

  y_to_delta_reduce_loop();

  /* add these later 
  delta_to_y_reduce_loop();
  y_to_delta_reduce_loop();
  */
}



local parallel_reduce_start_graph()
{
  edge_data edge;
  wn_bll el;

  for(wn_loopinit(),el=edge_list;wn_bllloop(&el,&edge);)
  {
    try_to_parallel_reduce_new_edge(edge);
  }
}



local try_to_parallel_reduce_new_edge(edge)

edge_data edge;

{
  edge_data first_edge;
  edge_connection_data connection;
  bool success;

  connection = edge->connection;

  success = wn_hins(edge,edges_by_connection,connection);

  if(not(success))  /* another edge between these nodes */
  {
    success = wn_hget(&first_edge,edges_by_connection,connection);
    wn_assert(success);

    (*pparallel_reduce)(&(first_edge->weight),
			first_edge->weight,edge->weight);
    parallel_delete_edge(edge);
  }
}



local parallel_delete_edge(edge)

edge_data edge;

{
  edge_connection_data connection;

  wn_blledel(edge->eli);
  wn_blledel(edge->elj);

  connection = edge->connection;

  decrement_node_degree(connection->nodei);
  decrement_node_degree(connection->nodej);

  delete_edge(edge);
}



local decrement_node_degree(node)

node_data node;

{
  --(node->degree);

  if(not(node->significant))
  {
    switch(node->degree)
    {
      case(0):
      {
	delete_node_from_degree_list(node);

        delete_node(node);
      } break;
      case(1):
      {
	delete_node_from_degree_list(node);

	insert_node_into_degree_list(&degree1_nodes,node);
      } break;
      case(2):
      {
	delete_node_from_degree_list(node);

	insert_node_into_degree_list(&degree2_nodes,node);
      } break;
      case(3):
      {
	insert_node_into_degree_list(&degree3_nodes,node);
      } break;
    }
  }
}



local increment_node_degree(node)

node_data node;

{
  ++(node->degree);

  if(not(node->significant))
  {
    switch(node->degree)
    {
      case(0):
      case(1):
	wn_crash();
      case(2):
      {
	delete_node_from_degree_list(node);

	insert_node_into_degree_list(&degree2_nodes,node);
      } break;
      case(3):
      {
	delete_node_from_degree_list(node);

	insert_node_into_degree_list(&degree3_nodes,node);
      } break;
      case(4):
      {
	delete_node_from_degree_list(node);
      } break;
    }
  }
}



local y_to_delta_reduce_loop()
{
  while(TRUE)
  {
    if(not(wn_bllempty(degree1_nodes)))
    {
      delete_orphan_node((node_data)(degree1_nodes->contents));
    }
    else if(not(wn_bllempty(degree2_nodes)))
    {
      series_reduce_node((node_data)(degree2_nodes->contents));
    }
    else if(
	     y_to_delta_reduce_allowed
	       &&
	     not(wn_bllempty(degree3_nodes))
	   )
    {
      y_to_delta_node((node_data)(degree3_nodes->contents));
    }
    else /* no further reduction possible */
    {
      break;
    }
  }
}



local delete_orphan_node(node)

node_data node;

{
  edge_data edge;
  wn_bll el;

  wn_assert(node->degree == 1);

  for(wn_loopinit(),el=node->edge_list;wn_bllloop(&el,&edge);)
  {
    full_delete_edge(edge);
  }
}



local full_delete_edge(edge)

edge_data edge;

{
  edge_connection_data connection;
  bool success;

  wn_blledel(edge->eli);
  wn_blledel(edge->elj);

  connection = edge->connection;

  success = wn_hdel(edges_by_connection,connection);
  wn_assert(success);

  decrement_node_degree(connection->nodei);
  decrement_node_degree(connection->nodej);

  delete_edge(edge);
}



local series_reduce_node(node)

node_data node;

{
  edge_data edge1,edge2;
  node_data node1,node2;
  wn_bll el;
  double new_weight;

  wn_assert(node->degree == 2);

  el = node->edge_list;
  edge1 = (edge_data)(el->contents);  
  el = el->next;
  edge2 = (edge_data)(el->contents);  
  wn_assert(el->next == NULL);

  get_other_node(&node1,edge1,node);
  get_other_node(&node2,edge2,node);

  (*pseries_reduce)(&new_weight,edge1->weight,edge2->weight);

  full_add_edge(new_weight,node1,node2);

  full_delete_edge(edge1);
  full_delete_edge(edge2);
}



local get_other_node(pother_node,edge,node)

node_data *pother_node,node;
edge_data edge;

{
  edge_connection_data connection;

  connection = edge->connection;

  if(node == connection->nodei)
  {
    *pother_node = connection->nodej;
  }
  else if(node == connection->nodej)
  {
    *pother_node = connection->nodei;
  }
  else
  {
    wn_crash();
  }
}



local full_add_edge(weight,nodei,nodej)

double weight;
node_data nodei,nodej;

{
  edge_data edge;
  edge_connection_data connection;

  make_edge(&edge,weight,nodei,nodej);

  connection = edge->connection;

  nodei = connection->nodei;
  nodej = connection->nodej;

  wn_bllins(&(nodei->edge_list),edge);
  edge->eli = nodei->edge_list;
  increment_node_degree(nodei);

  wn_bllins(&(nodej->edge_list),edge);
  edge->elj = nodej->edge_list;
  increment_node_degree(nodej);

  try_to_parallel_reduce_new_edge(edge);
}



local y_to_delta_node(node)

node_data node;

{
  edge_data edge1,edge2,edge3;
  node_data node1,node2,node3;
  wn_bll el;
  double w23,w13,w12;

  wn_assert(node->degree == 3);

  el = node->edge_list;
  edge1 = (edge_data)(el->contents);  
  el = el->next;
  edge2 = (edge_data)(el->contents);  
  el = el->next;
  edge3 = (edge_data)(el->contents);  
  wn_assert(el->next == NULL);

  get_other_node(&node1,edge1,node);
  get_other_node(&node2,edge2,node);
  get_other_node(&node3,edge3,node);

  (*py_to_delta)(&w23,&w13,&w12,
		 edge1->weight,edge2->weight,edge3->weight);

  full_add_edge(w23,node2,node3);
  full_add_edge(w13,node1,node3);
  full_add_edge(w12,node1,node2);

  full_delete_edge(edge1);
  full_delete_edge(edge2);
  full_delete_edge(edge3);
}



