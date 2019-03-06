/**********************************************************************

wn_truncate_unreachable_nodes(mat,node)

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
#include "wnary.h"
#include "wnsmat.h"



local bool *node_reached;
local int len;
local wn_stack node_stack;



wn_truncate_unreachable_nodes(mat,node)

wn_sparse_matrix mat;
int node;

{
  wn_gpmake("no_free");

  initialize(mat);

  find_reachable_nodes(mat,node);
  free_unreachable_nodes(mat);

  wn_gpfree();
}



local initialize(mat)

wn_sparse_matrix mat;

{
  wn_assertmsg(mat->len_i == mat->len_j,
	       "wn_truncate_unreachable_nodes: matrix must be square.");
  
  len = mat->len_i;
}



local find_reachable_nodes(mat,node)

wn_sparse_matrix mat;
int node;

{
  int node_to_expand;

  node_reached = (bool *)wn_alloc(len*wn_sizeof(bool));
  wn_arrayfill(node_reached,len,FALSE,bool);
  wn_mkstack(&node_stack);

  note_node_reached(node);

  while(not(wn_stackempty(node_stack)))
  {
    node_to_expand = (int)wn_stacktop(node_stack);
    wn_pop(node_stack);

    expand_node(mat,node_to_expand);
  }
}



local expand_node(mat,node)

wn_sparse_matrix mat;
int node;

{
  wn_sll connected_node_list,el;
  int connected_node;

  connected_node_list = (mat->i_lists)[node];

  for(wn_loopinit(),el=connected_node_list;wn_sllloop(&el,&connected_node);)
  {
    if(not(node_reached[connected_node]))
    {
      note_node_reached(connected_node);
    }
  }
}



local note_node_reached(node)

int node;

{
  node_reached[node] = TRUE; 

  wn_push(node_stack,node);
}



/* this routine is broken and needs to be fixed.  Replace some el's
   with pel's */
local free_unreachable_nodes(mat)

wn_sparse_matrix mat;

{
  int i,j;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  wn_gppush(mat->group);

  for(j=0;j<len;++j)
  {
    if(node_reached[j])
    {
      el = (mat->j_lists)[j];

      while(el != NULL)
      {
	entry = (wn_sparse_matrix_entry)(el->contents);

	if(node_reached[entry->i])
	{
	  el = el->next;
	}
	else
	{
	  wn_slledel(&el);
	}
      }
    }
    else
    {
      wn_freesll_list((mat->j_lists)[j]);

      mat->j_lists[j] = NULL;
    }
  }

  for(i=0;i<len;++i)
  {
    if(node_reached[i])
    {
      el = (mat->i_lists)[i];

      while(el != NULL)
      {
	entry = (wn_sparse_matrix_entry)(el->contents);

	if(node_reached[entry->j])
	{
	  el = el->next;
	}
	else
	{
	  wn_free((ptr)entry);

	  wn_slledel(&el);
	}
      }
    }
    else
    {
      for(wn_loopinit(),el=(mat->i_lists)[i];wn_sllloop(&el,&entry);)
      {
	wn_free((ptr)entry);
      }

      wn_freesll_list((mat->i_lists)[i]);

      mat->i_lists[i] = NULL;
    }
  }

  wn_gppop();
}



