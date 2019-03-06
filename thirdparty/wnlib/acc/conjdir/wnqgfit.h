/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnqgfitH
#define wnqgfitH


EXTERN void wn_fit_quadratic_to_grads
(
  double *perror,
  double **passed_m,
  double *passed_b,
  double **passed_vs,
  double **passed_gs,
  int passed_num_pts,
  int passed_num_dim,
  int num_iter
);


#endif

