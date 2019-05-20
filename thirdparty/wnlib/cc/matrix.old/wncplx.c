/**********************************************************************

wn_cplx_make(pnumber)
wn_cplx_enter(c)
wn_cplx_print(c)

wn_cplx_add(result,c1,c2)
wn_cplx_subtract(result,c1,c2)
wn_cplx_multiply(result,c1,c2)
wn_cplx_divide(result,c1,c2)
wn_cplx_reciprical(result,c)

double wn_cplx_norm_squared(c)
double wn_cplx_norm(c)
wn_cplx_to_polar(pr,ptheta,c)
wn_polar_to_cplx(c,r,theta)

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
#include "wnsqr.h"
#include "wncplx.h"



wn_cplx_make(pnumber)

wn_cplx *pnumber;

{
  *pnumber = wn_new(wn_cplx);

  (*pnumber)->real = 0.0;
  (*pnumber)->imag = 0.0;
}



wn_cplx_copy(out,in)

register wn_cplx out,in;

{
  out->real = in->real;
  out->imag = in->imag;
}



wn_cplx_enter(c)

wn_cplx c;

{
  float real,imag;

  scanf("%f,%f",&real,&imag);

  c->real = (double)real;
  c->imag = (double)imag;
}



wn_cplx_print(c)

wn_cplx c;

{
  printf("(%f,%f)",(float)(c->real),(float)(c->imag));
}



wn_cplx_add(result,c1,c2)

wn_cplx result,c1,c2;

{
  result->real = c1->real + c2->real;
  result->imag = c1->imag + c2->imag;
}



wn_cplx_subtract(result,c1,c2)

wn_cplx result,c1,c2;

{
  result->real = c1->real - c2->real;
  result->imag = c1->imag - c2->imag;
}



wn_cplx_multiply(result,c1,c2)

register wn_cplx result,c1,c2;

{
  result->real = (c1->real*c2->real) - (c1->imag*c2->imag);
  result->imag = (c1->real*c2->imag) + (c1->imag*c2->real);
}



wn_cplx_divide(result,c1,c2)

wn_cplx result,c1,c2;

{
  struct wn_cplx_struct r2_struct;
  wn_cplx r2 = &r2_struct;

  wn_cplx_reciprical(r2,c2);
  wn_cplx_multiply(result,c1,r2);
}



wn_cplx_reciprical(result,c)

wn_cplx result,c;

{
  double wn_cplx_norm_squared();
  double norm_squared;

  norm_squared = wn_cplx_norm_squared(c);

  result->real = c->real / norm_squared;
  result->imag = -(c->imag / norm_squared);
}



double wn_cplx_norm_squared(c)

wn_cplx c;

{
  return(wn_square(c->real) + wn_square(c->imag));
}



double wn_cplx_norm(c)

wn_cplx c;

{
  extern double sqrt();

  return(sqrt(wn_cplx_norm_squared(c)));
}



wn_cplx_to_polar(pr,ptheta,c)

double *pr,*ptheta;
wn_cplx c;

{
  extern double atan2();

  *pr = wn_cplx_norm(c);
  *ptheta = atan2(c->imag,c->real);
}



wn_polar_to_cplx(c,r,theta)

wn_cplx c;
double r,theta;

{
  extern double sin(),cos();

  c->real = r*cos(theta);
  c->imag = r*sin(theta);
}


