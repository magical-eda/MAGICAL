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

wn_split_graph(&objective,&delta,result,graph,iterations)

**********************************************************************/

#include "wnlib.h"
#include "wnary.h"

#include "wnsmat.h"



local double *split_multiplier_vect,
	     crossing_sum,
	     old_objective;
local int len,number_of_trues;
local wn_sll node_swap_list;



wn_split_graph(pobjective,pdelta,result,graph,iterations)

double *pobjective,*pdelta;
bool result[];
wn_sparse_matrix graph;
double iterations;

{
  wn_gpmake("no_free");

  initialize(result,graph);

  improve_solution(result,graph,iterations);

  compute_objective(pobjective,result,graph);
  *pdelta = *pobjective - old_objective;
  /*
  printf("end_objective = %f\n",(float)(*pobjective));
  */

  wn_gpfree();
}



local initialize(result,graph)

bool result[];
wn_sparse_matrix graph;

{
  double log();
  int i;

  wn_assertmsg(graph->len_i == graph->len_j,
	       "wn_split_graph: matrix must be square.");
  len = graph->len_i;

  number_of_trues = 0;
  for(i=0;i<len;++i)
  {
    if(result[i] == TRUE)
    {
      ++number_of_trues;
    }
  }

  node_swap_list = NULL;
  for(i=0;i<len;++i)
  {
    wn_sllins(&node_swap_list,(ptr)i);
  }

  compute_split_multiplier_vect();

  compute_objective(&old_objective,result,graph);
  /*
  printf("start_objective = %f\n",(float)old_objective);
  */
}



local compute_split_multiplier_vect()
{
  extern double log2();
  int size,bound,i;
  double f1,f2;

  size = len+1;

  split_multiplier_vect = (double *)wn_zalloc(size*wn_sizeof(double));

  split_multiplier_vect[0] = split_multiplier_vect[len] = WN_FHUGE;

  bound = len/2 + 1;

  for(i=1;i<bound;++i)
  {
    f1 = ((double)i)/((double)len);
    f2 = 1.0 - f1;

    split_multiplier_vect[i] = split_multiplier_vect[len-i] = 
	            1.0/(i*(len-i)*(-(f1*log2(f1)+f2*log2(f2))));
  }
}



local compute_objective(pobjective,result,graph)

double *pobjective;
bool result[];
wn_sparse_matrix graph;

{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  bool my_state;
  int i,j;
  double split_multiplier;

  crossing_sum = 0.0;

  for(i=0;i<len;++i)
  {
    my_state = result[i];

    for(el=((graph->i_lists)[i]);el != NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);
      j = entry->j;

      if(j >= i)
      {
	break;
      }

      if(my_state != result[j])  /* edge crosses boundary */
      {
	crossing_sum += entry->value;
      }
    }
  }

  split_multiplier = split_multiplier_vect[number_of_trues];

  if(split_multiplier >= WN_FHUGE)
  {
    *pobjective = WN_FHUGE;
  }
  else
  {
    *pobjective = crossing_sum*split_multiplier;
  }
}



local bool *save_result;
local wn_sparse_matrix save_graph;
local int save_node_to_swap,delta_node_to_swap;
local double delta_crossing_sum,save_crossing_sum,
	     current_objective,delta_objective,save_objective;


local improve_solution(result,graph,iterations)

bool result[];
wn_sparse_matrix graph;
double iterations;

{
  double delta_of_mutation();
  void save_mutation(),accept_saved_mutation();

  current_objective = old_objective;

  save_result = result;
  save_graph = graph;

  wn_anneal((delta_of_mutation),
	    (save_mutation),(accept_saved_mutation),
	    iterations);
}



local double delta_of_mutation()
{
  double delta,new_split_multiplier;

  delta_node_to_swap = wn_random_mod_int(len);

  compute_crossing_sum_delta(&delta,save_result,save_graph,delta_node_to_swap);
  delta_crossing_sum = crossing_sum+delta;

  compute_new_split_multiplier(&new_split_multiplier,
			       save_result,delta_node_to_swap);

  if(new_split_multiplier >= WN_FHUGE)
  {
    delta_objective = WN_FHUGE;
  }
  else
  {
    delta_objective = delta_crossing_sum*new_split_multiplier;
  }

  return(delta_objective-current_objective);
}



local void save_mutation()
{
  save_node_to_swap = delta_node_to_swap;
  save_objective = delta_objective;
  save_crossing_sum = delta_crossing_sum;
}



local void accept_saved_mutation()
{
  if(save_result[save_node_to_swap] == TRUE)
  {
    save_result[save_node_to_swap] = FALSE;
    --number_of_trues;
  }
  else
  {
    save_result[save_node_to_swap] = TRUE;
    ++number_of_trues;
  }
	
  current_objective = save_objective;
  /*
  printf("number_of_trues=%d,current_objective = %f\n",
	 number_of_trues,(float)current_objective);

  printf("current_objectives = %f\n",current_objective);
  */

  crossing_sum = save_crossing_sum;
}



local compute_crossing_sum_delta(pdelta,result,graph,node_to_swap)

double *pdelta;
bool result[];
wn_sparse_matrix graph;
int node_to_swap;

{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  double delta;
  bool my_state;

  delta = 0;
  my_state = result[node_to_swap];

  for(el=(graph->i_lists)[node_to_swap];el != NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    if(result[entry->j] == my_state)
    {
      delta += entry->value;
    }
    else /* different */
    {
      delta -= entry->value;
    }
  }

  *pdelta = delta;
}



local compute_new_split_multiplier(pnew_split_multiplier,result,node_to_swap)

double *pnew_split_multiplier;
bool result[];
int node_to_swap;

{
  if(result[node_to_swap] == TRUE)
  {
    *pnew_split_multiplier = split_multiplier_vect[number_of_trues-1];
  }
  else
  {
    *pnew_split_multiplier = split_multiplier_vect[number_of_trues+1];
  }
}





