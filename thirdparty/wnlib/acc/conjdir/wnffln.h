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
#ifndef wnfflnH
#define wnfflnH


#include "wnlib.h"




typedef struct wn_ffln_node_type_struct *wn_ffln_node_type;
typedef struct wn_ffln_type_struct *wn_ffln_type;


struct wn_ffln_node_type_struct
{
  int num_edges;
  float *coefs;  /* num_edges coefs */
  double **pvals;  /* num_edges pointers to val */
};


struct wn_ffln_type_struct
{
  wn_memgp group;

  int num_inputs,num_outputs,num_total_nodes;
  double *vals;
  wn_ffln_node_type *nodes; 
};



void wn_ffln_make
(
  wn_ffln_type *pffln,
  int num_inputs,
  int num_outputs,
  int num_total_nodes
);
void wn_ffln_setup_node
(
  wn_ffln_type ffln,
  int node_index,
  int num_edges,
  double coefs[],
  int from_indexes[]
);

void wn_ffln_generate
(
  wn_ffln_type ffln,
  int num_edges,
  double coef_mean,
  double coef_sdev
);

void wn_ffln_print(wn_ffln_type ffln);

void wn_ffln_mult_vect
(
  double *out,
  wn_ffln_type ffln,
  double *in
);


#endif

