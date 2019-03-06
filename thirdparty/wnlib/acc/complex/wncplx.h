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
#ifndef wncplxH
#define wncplxH


typedef struct wn_cplx_struct *wn_cplx;
struct wn_cplx_struct
{
  double real,imag;
};


EXTERN void wn_cplx_make(wn_cplx *pnumber);
EXTERN void wn_cplx_copy(wn_cplx out,wn_cplx in);

EXTERN void wn_cplx_enter(wn_cplx c);
EXTERN void wn_cplx_print(wn_cplx c);

EXTERN void wn_cplx_add(wn_cplx result,wn_cplx c1,wn_cplx c2);
EXTERN void wn_cplx_subtract(wn_cplx result,wn_cplx c1,wn_cplx c2);
EXTERN void wn_cplx_multiply(wn_cplx result,wn_cplx c1,wn_cplx c2);
EXTERN void wn_cplx_divide(wn_cplx result,wn_cplx c1,wn_cplx c2);
EXTERN void wn_cplx_reciprical(wn_cplx result,wn_cplx c);

EXTERN double wn_cplx_norm_squared(wn_cplx c);
EXTERN double wn_cplx_norm(wn_cplx c);

EXTERN void wn_cplx_to_polar(double *pr,double *ptheta,wn_cplx c);
EXTERN void wn_polar_to_cplx(wn_cplx c,double r,double theta);

EXTERN void wn_cplx_make_vect(wn_cplx **pvector,int len_i);
EXTERN void wn_cplx_copy_vect(wn_cplx out[],wn_cplx in[],int len_i);
EXTERN void wn_cplx_conjugate_vect
(
  register wn_cplx *vector,
  int len_i
);


#endif

