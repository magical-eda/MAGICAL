/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnmem.h"
#include "wnvect.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnffln.h"



void wn_ffln_make
(
  wn_ffln_type *pffln,
  int num_inputs,
  int num_outputs,
  int num_total_nodes
)
{
  wn_assert(num_inputs >= 0);
  wn_assert(num_outputs >= 0);
  wn_assert(num_inputs + num_outputs <= num_total_nodes);

  *pffln = (wn_ffln_type)wn_zalloc(sizeof(struct wn_ffln_type_struct));

  (*pffln)->group = wn_curgp();
  (*pffln)->num_inputs = num_inputs;
  (*pffln)->num_outputs = num_outputs;
  (*pffln)->num_total_nodes = num_total_nodes;

  wn_make_vect(&((*pffln)->vals),num_total_nodes);
  (*pffln)->nodes = (wn_ffln_node_type *)wn_zalloc(
                      num_total_nodes*sizeof(wn_ffln_node_type)); 
}


void wn_ffln_setup_node
(
  wn_ffln_type ffln,
  int node_index,
  int num_edges,
  double coefs[],
  int from_indexes[]
)
{
  int i;
  wn_ffln_node_type node;

  wn_gppush(ffln->group);

  wn_assert(node_index < ffln->num_total_nodes);

  if((ffln->nodes)[node_index] == NULL)
  {
    node = (wn_ffln_node_type)
      wn_zalloc(sizeof(struct wn_ffln_node_type_struct));
    (ffln->nodes)[node_index] = node;
  }
  else
  {
    node = (ffln->nodes)[node_index];
    wn_assert(node->num_edges == 0);
  }

  node->num_edges = num_edges;

  if(num_edges > 0)
  {
    /*
    wn_make_vect(&(node->coefs),num_edges);
    */
    node->coefs = (float *)wn_zalloc(num_edges*sizeof(float));
    node->pvals = (double **)wn_zalloc(num_edges*sizeof(double *));

    if(coefs != NULL)
    {
      /*
      wn_copy_vect((node->coefs),coefs,num_edges);
      */
      for(i=0;i<num_edges;++i)
      {
	(node->coefs)[i] = coefs[i];
      }

      for(i=0;i<num_edges;++i)
      {
        wn_assert(from_indexes[i] >= 0);
        wn_assert(from_indexes[i] < node_index);

        (node->pvals)[i] = &((ffln->vals)[from_indexes[i]]);
      }
    }
  }

  wn_gppop();
}


void wn_ffln_generate
(
  wn_ffln_type ffln,
  int num_edges,
  double coef_mean,
  double coef_sdev
)
{
  int node_index,from_node_index,edge_index;
  double *coefs;
  double prob;
  int *from_indexes;
  int m1_node_index,m2_node_index,min_node_index;

  wn_gpmake("no_free");

  wn_make_vect(&coefs,num_edges);
  from_indexes = (int *)wn_zalloc(num_edges*sizeof(int));

  m1_node_index = ffln->num_inputs;
  m2_node_index = ffln->num_total_nodes - ffln->num_outputs;

  for(node_index = m1_node_index;node_index < m2_node_index;++node_index)
  {
    min_node_index = wn_max(m1_node_index,node_index - ffln->num_inputs);

    prob = ((double)m1_node_index)/
           (m1_node_index + node_index - min_node_index); 

    for(edge_index=0;edge_index<num_edges;++edge_index)
    {
      if(wn_true_with_probability(prob))
      {
        from_node_index = wn_random_int_between(0,m1_node_index);
      }
      else
      {
        from_node_index = wn_random_int_between(min_node_index,node_index);
      }

      coefs[edge_index] = coef_mean + coef_sdev*wn_normal_distribution();
      from_indexes[edge_index] = from_node_index;
    }

    wn_ffln_setup_node(ffln,node_index,num_edges,coefs,from_indexes);
  }

  for(node_index = m2_node_index;node_index < ffln->num_total_nodes;
      ++node_index)
  {
    min_node_index = wn_max(m1_node_index,m2_node_index - ffln->num_inputs);

    prob = ((double)m1_node_index)/
           (m1_node_index + m2_node_index - min_node_index); 

    for(edge_index=0;edge_index<num_edges;++edge_index)
    {
      if(wn_true_with_probability(prob))
      {
        from_node_index = wn_random_int_between(0,m1_node_index);
      }
      else
      {
        from_node_index = wn_random_int_between(min_node_index,m2_node_index);
      }

      coefs[edge_index] = coef_mean + coef_sdev*wn_normal_distribution();
      from_indexes[edge_index] = from_node_index;
    }

    wn_ffln_setup_node(ffln,node_index,num_edges,coefs,from_indexes);
  }

  wn_gpfree();
}


local void print_node(wn_ffln_type ffln,int node_index)
{
  wn_ffln_node_type node;

  printf("    node %d, value = %lg:\n",node_index,(ffln->vals)[node_index]);

  node = (ffln->nodes)[node_index];

  if(node == NULL)
  {
    printf("      NULL\n");
  }
  else
  {
    int i;

    for(i=0;i<node->num_edges;++i)
    {
      printf("      i=%d,coef=%lg,from_index=%d\n",
             i,(node->coefs)[i],
             (node->pvals)[i] - ffln->vals);
    }
  }
}


void wn_ffln_print(wn_ffln_type ffln)
{
  int i;

  printf("ffln: num_inputs=%d,num_outputs=%d,num_total_nodes=%d\n",
         ffln->num_inputs,ffln->num_outputs,ffln->num_total_nodes);

  printf("  inputs:\n");
  for(i=0;i<ffln->num_inputs;++i)
  {
    print_node(ffln,i);
  }

  printf("  mid:\n");
  for(i=ffln->num_inputs;i<ffln->num_total_nodes - ffln->num_outputs;++i)
  {
    print_node(ffln,i);
  }

  printf("  outputs:\n");
  for(i=ffln->num_total_nodes - ffln->num_outputs;i<ffln->num_total_nodes;++i)
  {
    print_node(ffln,i);
  }
}


local void load_in_vect(wn_ffln_type ffln,double *in)
{
  wn_copy_vect((ffln->vals),in,ffln->num_inputs);
}


local void extract_out_vect(wn_ffln_type ffln,double *out)
{
  wn_copy_vect(out,
               &((ffln->vals)[ffln->num_total_nodes - ffln->num_outputs]),
               ffln->num_outputs);
}


#if 0
local void feed_forward(wn_ffln_type ffln)
{
  int node_index,edge_index;
  wn_ffln_node_type node;
  double sum;

  for(node_index = ffln->num_inputs;node_index < ffln->num_total_nodes;
      ++node_index)
  {
    node = (ffln->nodes)[node_index];

    sum = 0.0;

    for(edge_index = 0;edge_index < node->num_edges;++edge_index)
    {
      sum += (node->coefs)[edge_index]*(*((node->pvals)[edge_index]));
    }

    (ffln->vals)[node_index] = sum;
  }
}
#endif


#if 1
local void feed_forward(wn_ffln_type ffln)
{
  int node_index,edge_index;
  wn_ffln_node_type node;
  double sum;

  for(node_index = ffln->num_inputs;node_index < ffln->num_total_nodes;
      ++node_index)
  {
    node = (ffln->nodes)[node_index];

    switch(node->num_edges)
    {
      case(0):
        sum = 0.0;
        break;
      case(1):
        sum = (node->coefs)[0]*(*((node->pvals)[0]));
        break;
      case(2):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1]));
        break;
      case(3):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2]));
        break;
      case(4):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3]));
        break;
      case(5):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3])) +
              (node->coefs)[4]*(*((node->pvals)[4]));
        break;
      case(6):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3])) +
              (node->coefs)[4]*(*((node->pvals)[4])) +
              (node->coefs)[5]*(*((node->pvals)[5]));
        break;
      case(7):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3])) +
              (node->coefs)[4]*(*((node->pvals)[4])) +
              (node->coefs)[5]*(*((node->pvals)[5])) +
              (node->coefs)[6]*(*((node->pvals)[6]));
        break;
      case(8):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3])) +
              (node->coefs)[4]*(*((node->pvals)[4])) +
              (node->coefs)[5]*(*((node->pvals)[5])) +
              (node->coefs)[6]*(*((node->pvals)[6])) +
              (node->coefs)[7]*(*((node->pvals)[7]));
        break;
      case(9):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3])) +
              (node->coefs)[4]*(*((node->pvals)[4])) +
              (node->coefs)[5]*(*((node->pvals)[5])) +
              (node->coefs)[6]*(*((node->pvals)[6])) +
              (node->coefs)[7]*(*((node->pvals)[7])) +
              (node->coefs)[8]*(*((node->pvals)[8]));
        break;
      case(10):
        sum = (node->coefs)[0]*(*((node->pvals)[0])) +
              (node->coefs)[1]*(*((node->pvals)[1])) +
              (node->coefs)[2]*(*((node->pvals)[2])) +
              (node->coefs)[3]*(*((node->pvals)[3])) +
              (node->coefs)[4]*(*((node->pvals)[4])) +
              (node->coefs)[5]*(*((node->pvals)[5])) +
              (node->coefs)[6]*(*((node->pvals)[6])) +
              (node->coefs)[7]*(*((node->pvals)[7])) +
              (node->coefs)[8]*(*((node->pvals)[8])) +
              (node->coefs)[9]*(*((node->pvals)[9]));
        break;
      default:
        sum = 0.0;
        for(edge_index = 0;edge_index < node->num_edges;++edge_index)
        {
          sum += (node->coefs)[edge_index]*(*((node->pvals)[edge_index]));
        }
        break;
    }

    (ffln->vals)[node_index] = sum;
  }
}
#endif


void wn_ffln_mult_vect
(
  double *out,
  wn_ffln_type ffln,
  double *in
)
{
  load_in_vect(ffln,in);

  feed_forward(ffln);
  
  extract_out_vect(ffln,out);
}

