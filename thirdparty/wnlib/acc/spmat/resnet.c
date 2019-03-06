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
#include <stdio.h>

extern "C" {
#include "wnlib.h"
#include "wnasrt.h"
#include "wnvect.h"
#include "wnspmat.h"
#include "wnconj.h"
}

#include "resnet.h"



local int count;
local int len;
local wn_sparse_matrix conductance_graph;
local double *stimulus_vect;
local stimulus_type *stimulus_type_vect;


local double function(double v[])
{
  double sum,diff,conductance_sum;
  int i;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  sum = 0.0;

  for(i=0;i<len;++i)
  {
    if(stimulus_type_vect[i] != voltage)
    {
      if(stimulus_type_vect[i] == current)
      {
        /* currents at each node must sum to drive current */
        diff = stimulus_vect[i];
      }
      else
      {
        /* currents at each node must sum to zero */
        diff = 0.0;
      }

      conductance_sum = 0.0;
  
      for(el=(conductance_graph->i_lists)[i];el!=NULL;el=el->next)
      {
        entry = (wn_sparse_matrix_entry)(el->contents);
  
        conductance_sum += entry->value;
      }
  
      diff -= v[i]*conductance_sum;
  
      for(el=(conductance_graph->i_lists)[i];el!=NULL;el=el->next)
      {
        entry = (wn_sparse_matrix_entry)(el->contents);
  
        if(stimulus_type_vect[entry->j] == voltage)
        {
          diff += stimulus_vect[entry->j]*entry->value;
        }
	else
	{
          diff += v[entry->j]*entry->value;
	}
      }
  
      sum += diff*diff;
    }
  }

  printf("ob = %30.30lg\n",sum);

  return(sum);
}


local void gradient(double grad[],double v[])
{
  double diff,conductance_sum;
  int i;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  printf("gradient.  count = %d\n",count);
  ++count;

  wn_zero_vect(grad,len);

  for(i=0;i<len;++i)
  {
    if(stimulus_type_vect[i] != voltage)
    {
      if(stimulus_type_vect[i] == current)
      {
        /* currents at each node must sum to drive current */
        diff = stimulus_vect[i];
      }
      else
      {
        /* currents at each node must sum to zero */
        diff = 0.0;
      }

      conductance_sum = 0.0;
  
      for(el=(conductance_graph->i_lists)[i];el!=NULL;el=el->next)
      {
        entry = (wn_sparse_matrix_entry)(el->contents);
  
        conductance_sum += entry->value;
      }
  
      diff -= v[i]*conductance_sum;
  
      for(el=(conductance_graph->i_lists)[i];el!=NULL;el=el->next)
      {
        entry = (wn_sparse_matrix_entry)(el->contents);
  
        if(stimulus_type_vect[entry->j] == voltage)
        {
          diff += stimulus_vect[entry->j]*entry->value;
        }
	else
	{
          diff += v[entry->j]*entry->value;
	}
      }
  
      grad[i] += 2.0*diff*(-conductance_sum);

      for(el=(conductance_graph->i_lists)[i];el!=NULL;el=el->next)
      {
        entry = (wn_sparse_matrix_entry)(el->contents);
  
        if(stimulus_type_vect[entry->j] != voltage)
        {
          grad[entry->j] += 2.0*diff*entry->value;
	}
      }
    }
  }
}


void solve_conductance_network
(
  int *pcode,
  double voltage_vect[],
  wn_sparse_matrix passed_conductance_graph,   /* assumed to be symmetric */
  double passed_stimulus_vect[],
  stimulus_type passed_stimulus_type_vect[]
)
{
  double val_min;
  int i;

  conductance_graph = passed_conductance_graph;
  stimulus_vect = passed_stimulus_vect;
  stimulus_type_vect = passed_stimulus_type_vect;

  wn_assert(conductance_graph->len_i == conductance_graph->len_j);
  len = conductance_graph->len_i;

  count = 0;

  wn_conj_gradient_method(pcode,&val_min,
		          voltage_vect,len,&function,&gradient,2*len);

  for(i=0;i<len;++i)
  {
    if(stimulus_type_vect[i] == voltage)
    {
      voltage_vect[i] = stimulus_vect[i];
    }
  }
}

