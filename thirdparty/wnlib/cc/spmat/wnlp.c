/**********************************************************************

wn_longest_path(&code,&len,&result,length_mat,start_node,fin_node)

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
#include "wninr.h"
#include "wnsmat.h"



local int len_i;
local wn_memgp top_group;

local bool *node_visited;
local int *path,*best_path;
local double best_score;
local wn_sll *i_lists;
local int start_node,fin_node;



wn_longest_path(pcode,plen,presult,length_mat,passed_start_node,passed_fin_node)

int *pcode;
double *plen;
wn_sll *presult;          /* list of edges */
wn_sparse_matrix length_mat;
int passed_start_node,passed_fin_node;

{
  top_group = wn_curgp();
  wn_gpmake("no_free");

  initialize(length_mat,passed_start_node,passed_fin_node);

  main_loop(pcode);

  if(*pcode == WN_SUCCESS)
  {
    grab_results(plen,presult);
  }

  wn_gpfree();
}



local initialize(length_mat,passed_start_node,passed_fin_node)

wn_sparse_matrix length_mat;
int passed_start_node,passed_fin_node;

{
  start_node = passed_start_node;
  fin_node = passed_fin_node;

  wn_assertmsg(length_mat->len_i == length_mat->len_j,
               "wn_longest_path: matrix not square.");
  len_i = length_mat->len_i;
  wn_assertmsg(wn_inrange(0,<=,start_node,<,len_i),
               "wn_longest_path: start_node out of range.");
  wn_assertmsg(wn_inrange(0,<=,fin_node,<,len_i),
               "wn_longest_path: fin_node out of range.");

  node_visited = (bool *)wn_alloc(len_i*wn_sizeof(bool));
  path = (int *)wn_alloc((len_i+1)*wn_sizeof(int));
  best_path = (int *)wn_alloc((len_i+1)*wn_sizeof(int));

  wn_arrayfill(node_visited,len_i,FALSE,bool);
  wn_arrayfill(path,(len_i+1),-1,int);
  wn_arrayfill(best_path,(len_i+1),-1,int);

  best_score = -WN_FHUGE;

  i_lists = length_mat->i_lists;
}



local main_loop(pcode)

int *pcode;

{
  trace_path_from_node(start_node,-1,0.0);

  if(best_path[0] == -1)
  {
    *pcode = WN_INFEASIBLE;
  }
  else
  {
    *pcode = WN_SUCCESS;
  }
}



local trace_path_from_node(node,depth,score)

int node,depth;
double score;

{
  ++depth;
  path[depth] = node;

  if(node == fin_node)
  {
    if(score > best_score)
    {
      copy_path(best_path,path);

      best_score = score;
    }
  }
  else
  {
    wn_sll el;
    wn_sparse_matrix_entry entry;
    int next_node;

    node_visited[node] = TRUE;
  
    for(el=i_lists[node];el != NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);
      next_node = entry->j;

      if(not(node_visited[next_node]))
      {
	trace_path_from_node(next_node,depth,score+entry->value);
      }
    }
  
    node_visited[node] = FALSE;
  }

  path[depth] = -1;
}



local copy_path(out,in)

register int *out,*in;

{
  register int node;

  do
  {
    node = *in;

    *out = node;

    ++out;  ++in;
  }
  while(node != -1);
}



local grab_results(plen,presult)

double *plen;
wn_sll *presult;

{
  int i,from,to;
  wn_sparse_matrix_entry entry;

  wn_gppush(top_group);

  *plen = best_score;

  *presult = NULL;
  i = 0;

  while(TRUE)
  {
    from = best_path[i];
    ++i;
    to = best_path[i];

    if(to == -1)
    {
      break;
    }

    get_entry(&entry,from,to);

    wn_sllins(presult,entry);
  }

  wn_sllrev(presult);

  wn_gppop();
}



local get_entry(pentry,from,to)

wn_sparse_matrix_entry *pentry;
int from,to;

{
  wn_sll el;
  wn_sparse_matrix_entry entry;

  for(wn_loopinit(),el=i_lists[from];wn_sllloop(&el,&entry);)
  {
    if(entry->j == to)
    {
      *pentry = entry;

      return;
    }
  }

  wn_crash();
}






