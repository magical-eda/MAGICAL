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
#ifndef wnvectH
#define wnvectH


#include "wnlib.h"


EXTERN void wn_print_vect(double vect[],int len);
EXTERN void wn_enter_vect(double vect[],int len);
EXTERN void wn_random_vect(double vect[],int len);
EXTERN void wn_zero_vect(double vect[],int len);

EXTERN void wn_make_vect(double **pvect,int len);
EXTERN void wn_free_vect(double *vect,int len);

EXTERN void wn_copy_vect(double to_vect[],double from_vect[],int len);

EXTERN double wn_dot_vects(double *vect1,double *vect2,int len);
EXTERN double wn_norm2_vect(double *vect,int len);
EXTERN double wn_norm_vect(double *vect,int len);
EXTERN void wn_unit_vect(double vect[],int len);
EXTERN double wn_dist2_vect(double v1[],double v2[],int len);
EXTERN double wn_dist_vect(double v1[],double v2[],int len);

EXTERN 
void wn_add_scaled_vect(double *to_vect,double *from_vect,double scalar,
			int len);
EXTERN
void wn_add_vect_and_scaled_vect(double *out,
				 double *v1,double *v2,double s2,
			         int len);
EXTERN void wn_add_scaled_vects
(
  double *out,
  double *v1,
  double s1,
  double *v2,
  double s2,
  int len
);
EXTERN void wn_add_3_scaled_vects
(
  double *out,
  double *v1,
  double s1,
  double *v2,
  double s2,
  double *v3,
  double s3,
  int len
);
EXTERN void wn_subtract_vect
(
  double *to_vect,
  double *from_vect,
  int len
);
EXTERN void wn_scale_vect(double *vect,double scalar,int len);

EXTERN void wn_multiply_vect_by_vect(double *v1, double *v2, int len);
EXTERN void wn_divide_vect_by_vect(double *v1, double *v2, int len);
EXTERN void wn_add_scaled_vects_and_dot_and_norm
(
  double *out,
  double *pnorm2_out,
  double *pdot_v1_out,
  double *v1,
  double s1,
  double *v2,
  double s2,
  int len
);

#endif

