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
#ifndef wnrootH
#define wnrootH


#include "wnlib.h"


EXTERN void wn_findroot_raw_bracketed
(
  int *pcode,
  double *pf0,
  double *pf1,
  double *pf2,
  double *px0,
  double *px1,
  double *px2,
  double (*pfunction)(double x),
  int max_iterations
);

EXTERN void wn_findroot_bracketed
(
  int *pcode,
  double *px1,
  double *pf1,
  double x0,
  double f0,
  double x2,
  double f2,
  double (*pfunction)(double x),
  int max_iterations
);

EXTERN void wn_findroot
(
  int *pcode,
  double *px1,
  double *pf1,
  double x0,
  double f0,
  double x2,
  double f2,
  double (*pfunction)(double x),
  int max_iterations
);


EXTERN void wn_findroot_stop(void);

EXTERN void wn_findroot_getrange
(
  bool *pbracketed,
  double *px0,
  double *pf0,
  double *px2,
  double *pf2
);

#endif

