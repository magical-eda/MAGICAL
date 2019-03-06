/**********************************************************************

wn_trans_problem_feasible(&code,&result,
                          cost_mat,
                          i_capacities,j_capacities)

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
#include "wnmem.h"
#include "wnswap.h"
#include "wnmax.h"
#include "wnbtr.h"
#include "wnbtrl.h"

#include "wnspmat.h"

#include "wntrn.h"


#define I_PERIF   1
#define J_PERIF   2


typedef struct perif_data_struct *perif_data;

struct perif_data_struct
{
  bool no_capacity_left;
  double remaining_capacity;
  int index,remaining_entry_count;
  int perif_type;
  wn_bhandle handle;
};


local wn_memgp top_group;
local int len_i,len_j,
	  number_of_basics_still_needed;
local perif_data *i_perif_array,*j_perif_array;
local wn_sparse_matrix first_result;
local wn_btree perif_queue;



local double total_capacity
(
  double capacities[],
  int len
)
{
  int i;
  double total;

  total = 0.0;

  for(i=0;i<len;++i)
  {
    total += capacities[i];
  }

  return(total);
}


local void make_perif_array
(
  perif_data **pperif_array,
  int perif_type,
  wn_sll entry_lists[],
  double capacities[],
  int len
)
{
  int i;
  perif_data perif;

  *pperif_array = (perif_data *)wn_zalloc(len*sizeof(perif_data));

  for(i=0;i<len;++i)
  {
    perif = (perif_data)wn_zalloc(sizeof(struct perif_data_struct));

    perif->perif_type = perif_type;
    perif->no_capacity_left = FALSE;
    perif->remaining_capacity = capacities[i];
    perif->remaining_entry_count = wn_sllcount(entry_lists[i]);
    perif->index = i;

    (*pperif_array)[i] = perif;
  }
}


local void initialize
(
  wn_sparse_matrix cost_mat,
  double i_capacities[],
  double j_capacities[]
)
{
  top_group = wn_curgp();
  wn_gpmake("no_free");

  len_i = cost_mat->len_i;
  len_j = cost_mat->len_j;

  wn_assert(total_capacity(i_capacities,len_i) == 
	    total_capacity(j_capacities,len_j));

  number_of_basics_still_needed = len_i + len_j - 1;

  make_perif_array(&i_perif_array,I_PERIF,cost_mat->i_lists,i_capacities,len_i);
  make_perif_array(&j_perif_array,J_PERIF,cost_mat->j_lists,j_capacities,len_j);

  wn_make_sparse_matrix(&first_result,len_i,len_j);
}


local void make_perif_queue(void)
{
  wn_mkintbtree(&perif_queue);
}


local void insert_perif_array_into_perif_queue
(
  perif_data perif_array[],
  int len
)
{
  int i;
  wn_bhandle handle;
  perif_data perif;

  for(i=0;i<len;++i)
  {
    perif = perif_array[i];

    wn_bins(&handle,perif_queue,(ptr)perif->remaining_entry_count);
    handle->contents = (ptr)perif;
    perif->handle = handle;
  }
}


local void insert_perifs_into_perif_queue(void)
{
  insert_perif_array_into_perif_queue(i_perif_array,len_i);
  insert_perif_array_into_perif_queue(j_perif_array,len_j);
}


local void initialize_approximate(void)
{
  make_perif_queue();

  insert_perifs_into_perif_queue();
}


local void get_best_entry_of_i_perif
(
  wn_sparse_matrix_entry *pentry,
  wn_sparse_matrix cost_mat,
  perif_data perif
)
{
  wn_sll el;
  wn_sparse_matrix_entry entry,best_entry;
  int best_remaining_entry_count,remaining_entry_count;
  perif_data j_perif;

  best_entry = NULL;
  best_remaining_entry_count = WN_IHUGE;

  for(el=(cost_mat->i_lists)[perif->index];el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    j_perif = j_perif_array[entry->j];

    if(!(j_perif->no_capacity_left))
    {
      remaining_entry_count = j_perif->remaining_entry_count;

      if(remaining_entry_count < best_remaining_entry_count)
      {
        best_entry = entry;
        best_remaining_entry_count = remaining_entry_count;
      }
    }    
  }

  *pentry = best_entry;
}


local void get_best_entry_of_j_perif
(
  wn_sparse_matrix_entry *pentry,
  wn_sparse_matrix cost_mat,
  perif_data perif
)
{
  wn_sll el;
  wn_sparse_matrix_entry entry,best_entry;
  int best_remaining_entry_count,remaining_entry_count;
  perif_data i_perif;

  best_entry = NULL;
  best_remaining_entry_count = WN_IHUGE;

  for(el=(cost_mat->j_lists)[perif->index];el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    i_perif = i_perif_array[entry->i];

    if(!(i_perif->no_capacity_left))
    {
      remaining_entry_count = i_perif->remaining_entry_count;

      if(remaining_entry_count < best_remaining_entry_count)
      {
        best_entry = entry;
        best_remaining_entry_count = remaining_entry_count;
      }
    }    
  }

  *pentry = best_entry;
}


local void get_best_entry_of_perif
(
  wn_sparse_matrix_entry *pentry,
  wn_sparse_matrix cost_mat,
  perif_data perif
)
{
  switch(perif->perif_type)
  {
    case(I_PERIF):
      get_best_entry_of_i_perif(pentry,cost_mat,perif);
      break;
    case(J_PERIF):
      get_best_entry_of_j_perif(pentry,cost_mat,perif);
      break;
    default:
      wn_assert_notreached();
  }
}


local void get_best_entry
(
  wn_sparse_matrix_entry *pentry,
  wn_sparse_matrix cost_mat
)
{
  wn_bhandle handle;
  perif_data best_perif;

  for(;;)
  {
    wn_bget(&handle,perif_queue,NULL,WN_B_MIN);

    if(handle == NULL)
    {
      *pentry = NULL;

      break;
    }
    else
    {
      best_perif = (perif_data)(handle->contents);

      get_best_entry_of_perif(pentry,cost_mat,best_perif);

      if(*pentry == NULL)
      {
	wn_bdel(handle,perif_queue);   /* delete empty perif */
      }
      else
      {
	break;
      }
    }
  }
}


local void i_perif_is_saturated
(
  perif_data i_perif,
  wn_sparse_matrix cost_mat
)
{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  int i,j;
  perif_data j_perif;

  i = i_perif->index;
  i_perif->no_capacity_left = TRUE;
  wn_bdel(i_perif->handle,perif_queue);

  for(el=(cost_mat->i_lists)[i];el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    j = entry->j;
    j_perif = j_perif_array[j];

    if(!(j_perif->no_capacity_left))
    {
      --(j_perif->remaining_entry_count);
      wn_bmove(j_perif->handle,perif_queue,(ptr)j_perif->remaining_entry_count);
    }
  }
}


local void j_perif_is_saturated
(
  perif_data j_perif,
  wn_sparse_matrix cost_mat
)
{
  wn_sll el;
  wn_sparse_matrix_entry entry;
  int i,j;
  perif_data i_perif;

  j = j_perif->index;
  j_perif->no_capacity_left = TRUE;
  wn_bdel(j_perif->handle,perif_queue);

  for(el=(cost_mat->j_lists)[j];el!=NULL;el=el->next)
  {
    entry = (wn_sparse_matrix_entry)(el->contents);

    i = entry->i;
    i_perif = i_perif_array[i];

    if(!(i_perif->no_capacity_left))
    {
      --(i_perif->remaining_entry_count);
      wn_bmove(i_perif->handle,perif_queue,(ptr)i_perif->remaining_entry_count);
    }
  }
}


local void add_entry_to_result
(
  wn_sparse_matrix_entry entry,
  wn_sparse_matrix cost_mat
)
{
  double flow;
  perif_data i_perif,j_perif;
  int i,j;

  i = entry->i;
  j = entry->j;

  i_perif = i_perif_array[i];
  j_perif = j_perif_array[j];

  flow = wn_min(i_perif->remaining_capacity,j_perif->remaining_capacity);

  if(flow > 0.0)
  {
    wn_insert_sparse_matrix_value(first_result,flow,i,j);
  }

  --number_of_basics_still_needed;

  i_perif->remaining_capacity -= flow;
  j_perif->remaining_capacity -= flow;

  if(i_perif->remaining_capacity == 0.0)
  {
    if(j_perif->remaining_capacity == 0.0)
    {
      if(i_perif->remaining_entry_count < j_perif->remaining_entry_count)
      {
	i_perif_is_saturated(i_perif,cost_mat);
      }
      else
      {
	j_perif_is_saturated(j_perif,cost_mat);
      }
    }
    else
    {
      wn_assert(j_perif->remaining_capacity > 0.0);

      i_perif_is_saturated(i_perif,cost_mat);
    }
  }
  else
  {
    wn_assert(i_perif->remaining_capacity > 0.0);

    if(j_perif->remaining_capacity == 0.0)
    {
      j_perif_is_saturated(j_perif,cost_mat);
    }
    else
    {
      wn_assert_notreached();
    }
  }
}


local bool capacity_still_remains(void)
{
  int i;
  perif_data perif;

  for(i=0;i<len_i;++i)
  {
    perif = i_perif_array[i];

    if(!(perif->no_capacity_left))
    {
      if(perif->remaining_capacity > 0.0)
      {
	return(TRUE);
      }
    }
  }

  return(FALSE);
}


local void main_approximate_loop(wn_sparse_matrix cost_mat)
{
  wn_sparse_matrix_entry entry;

  while(number_of_basics_still_needed > 0)
  {
    get_best_entry(&entry,cost_mat);

    if(entry == NULL)
    {
      break;
    }

    add_entry_to_result(entry,cost_mat);
  }
}


local void approximate
(
  int *pcode,
  wn_sparse_matrix cost_mat
)
{
  initialize_approximate();

  main_approximate_loop(cost_mat);

  if(capacity_still_remains())
  {
    *pcode = WN_INFEASIBLE;
  }
  else
  {
    *pcode = WN_SUBOPTIMAL;
  }
}


local void compute_feasibility_cost_mat
(
  wn_sparse_matrix *pfeasibility_cost_mat,
  wn_sparse_matrix cost_mat
)
{
  int i,j;
  wn_sll el;
  wn_sparse_matrix_entry entry;
  perif_data i_perif,j_perif;
  double flow;

  wn_make_sparse_matrix(pfeasibility_cost_mat,len_i,len_j);

  for(i=0;i<len_i;++i)
  {
    for(el=(cost_mat->i_lists)[i];el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      j = entry->j;

      wn_insert_sparse_matrix_value(*pfeasibility_cost_mat,0.0,i,j);
    }
  }

  i=0;
  j=0;
  i_perif = i_perif_array[i];
  j_perif = j_perif_array[j];

  while(i < len_i)
  {
    if(i_perif->remaining_capacity == 0.0)
    {
      ++i;
      i_perif = i_perif_array[i];
    }
    else if(j_perif->remaining_capacity == 0.0)
    {
      ++j;
      j_perif = j_perif_array[j];

      wn_assert(j < len_j);
    }
    else  /* capacity remains in both */
    {
      flow = wn_min(i_perif->remaining_capacity,
		    j_perif->remaining_capacity);

      wn_assert(flow > 0.0);

      i_perif->remaining_capacity -= flow;
      j_perif->remaining_capacity -= flow;

      wn_insert_sparse_matrix_value(*pfeasibility_cost_mat,1.0,i,j);
      wn_insert_sparse_matrix_value(first_result,flow,i,j);
    }
  }

  wn_assert((i == len_i)&&(j <= len_j));

  wn_sort_sparse_matrix(*pfeasibility_cost_mat);
  wn_sort_sparse_matrix(first_result);
}


local void simplex_solve
(
  int *pcode,
  wn_sparse_matrix cost_mat
)
{
  int code;
  wn_sparse_matrix feasibility_cost_mat;
  double objective,delta;

  compute_feasibility_cost_mat(&feasibility_cost_mat,cost_mat);

  wn_trans_problem_simplex_improve(&code,&objective,&delta,
				   &first_result,first_result,
				   feasibility_cost_mat,
				   WN_IHUGE);

  wn_assert(code == WN_SUCCESS);

  if(objective == 0.0)
  {
    *pcode = WN_SUBOPTIMAL;
  }
  else
  {
    wn_assert(objective > 0.0);

    *pcode = WN_INFEASIBLE;
  }
}


local void finish(void)
{
  wn_gpfree();
}


void wn_trans_problem_feasible
(
  int *pcode,
  wn_sparse_matrix *presult,
  wn_sparse_matrix cost_mat,
  double i_capacities[],
  double j_capacities[]
)
{
  int code;

  initialize(cost_mat,i_capacities,j_capacities);

  approximate(&code,cost_mat);

  if(code == WN_SUBOPTIMAL)
  {
    wn_sort_sparse_matrix(first_result);
  }
  else
  {
    simplex_solve(&code,cost_mat);
  }

  if(code == WN_SUBOPTIMAL)
  {
    wn_gppush(top_group);
    wn_copy_sparse_matrix(presult,first_result);
    wn_gppop();

    *pcode = WN_SUBOPTIMAL;
  }
  else
  {
    *presult = NULL;

    *pcode = WN_INFEASIBLE;
  }

  finish();
}








