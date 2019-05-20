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
#ifndef wnmqfH
#define wnmqfH


#include "wnlib.h"

#include "wnffln.h"



extern int wn_minimize_quad_form_verbose;



/*
  minimize f(x) = (1/2)*x^t*A*x + g*x
  whose gradient is
           grad(x) = A*x + g
*/
void wn_minimize_quad_form
(
  int *pcode,
  double *pob,     
  double *palpha,  /* recommend line search len of final iteration */
  double *p,       /* search direction (negative search direction) */
  double *x,       /* current best solution */
  double *g,
  wn_ffln_type A,
  int num_iterations
);



#endif

