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
#ifndef resnetH
#define resnetH



enum stimulus_type {none=0,voltage,current};

EXTERN void solve_conductance_network
(
  int *pcode,
  double voltage_vect[],
  wn_sparse_matrix conductance_graph,   /* assumed to be symmetric */
  double stimulus_vect[],
  stimulus_type stimulus_type_vect[]
);



#endif

