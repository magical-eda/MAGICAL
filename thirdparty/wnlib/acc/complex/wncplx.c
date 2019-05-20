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

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnsqr.h"
#include "wncplx.h"



void wn_cplx_make(wn_cplx *pnumber)
{
  *pnumber = (wn_cplx)wn_zalloc(sizeof(struct wn_cplx_struct));

  (*pnumber)->real = 0.0;
  (*pnumber)->imag = 0.0;
}


void wn_cplx_copy(wn_cplx out,wn_cplx in)
{
  out->real = in->real;
  out->imag = in->imag;
}


void wn_cplx_enter(wn_cplx c)
{
  double real,imag;

  scanf("%lf,%lf",&real,&imag);

  c->real = real;
  c->imag = imag;
}


void wn_cplx_print(wn_cplx c)
{
  printf("(%lf,%lf)",c->real,c->imag);
}


void wn_cplx_add(wn_cplx result,wn_cplx c1,wn_cplx c2)
{
  result->real = c1->real + c2->real;
  result->imag = c1->imag + c2->imag;
}


void wn_cplx_subtract(wn_cplx result,wn_cplx c1,wn_cplx c2)
{
  result->real = c1->real - c2->real;
  result->imag = c1->imag - c2->imag;
}


void wn_cplx_multiply(wn_cplx result,wn_cplx c1,wn_cplx c2)
{
  result->real = (c1->real*c2->real) - (c1->imag*c2->imag);
  result->imag = (c1->real*c2->imag) + (c1->imag*c2->real);
}


void wn_cplx_divide(wn_cplx result,wn_cplx c1,wn_cplx c2)
{
  struct wn_cplx_struct r2_struct;
  wn_cplx r2 = &r2_struct;

  wn_cplx_reciprical(r2,c2);
  wn_cplx_multiply(result,c1,r2);
}


void wn_cplx_reciprical(wn_cplx result,wn_cplx c)
{
  double norm_squared;

  norm_squared = wn_cplx_norm_squared(c);

  result->real = c->real / norm_squared;
  result->imag = -(c->imag / norm_squared);
}


double wn_cplx_norm_squared(wn_cplx c)
{
  return(wn_square(c->real) + wn_square(c->imag));
}


double wn_cplx_norm(wn_cplx c)
{
  return(sqrt(wn_cplx_norm_squared(c)));
}


void wn_cplx_to_polar(double *pr,double *ptheta,wn_cplx c)
{
  *pr = wn_cplx_norm(c);
  *ptheta = atan2(c->imag,c->real);
}


void wn_polar_to_cplx(wn_cplx c,double r,double theta)
{
  c->real = r*cos(theta);
  c->imag = r*sin(theta);
}


void wn_cplx_make_vect(wn_cplx **pvector,int len_i)
{
  int i;

  *pvector = (wn_cplx *)wn_zalloc(len_i*sizeof(wn_cplx));

  for(i=0;i<len_i;i++)
  {
    wn_cplx_make(&((*pvector)[i]));
  }
}


void wn_cplx_copy_vect(wn_cplx out[],wn_cplx in[],int len_i)
{
  int i;

  for(i=0;i<len_i;++i)
  {
    wn_cplx_copy(out[i],in[i]);
  }
}


void wn_cplx_conjugate_vect
(
  register wn_cplx *vector,
  int len_i
)
{
  register wn_cplx number,*fin;

  fin = vector+len_i;

  for(;vector != fin;++vector)
  {
    number = *vector;

    number->imag = -number->imag;
  }
}


