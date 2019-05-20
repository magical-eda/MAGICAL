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
#include "wnswap.h"
#include "wnvect.h"
#include "wnpoly.h"

#include "wnconj.h"



/********************************************************************
Fit parabola to 3 points (x,y).  Parabola is of the form

  y = a*(x-x0)^2 + b

Return a, x0, b.
********************************************************************/
void wn_fit_parabola_3p
(
  int *pcode,
  double *pa,double *px0,double *pb,
  double x1,double y1,
  double x2,double y2,
  double x3,double y3
)
{
  double x12,x23,dx21,dx32,dx2312,dy12,dy23,ddy,diff;

  /* sort the x's */
  if(!(x1 < x3))
  {
    wn_swap(x1,x3,double);
    wn_swap(y1,y3,double);
  }
  if(x2 < x1)
  {
    wn_swap(x1,x2,double);
    wn_swap(y1,y2,double);
  }
  else if(x3 < x2) 
  {
    wn_swap(x2,x3,double);
    wn_swap(y2,y3,double);
  }

  dx21 = x2-x1;
  dx32 = x3-x2;

  if(!((x1 != x3)&&(dx21 != 0.0)&&(dx32 != 0.0)))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  x12 = 0.5*(x1+x2);
  x23 = 0.5*(x2+x3);

  dx2312 = x23-x12;
  if(!(dx2312 != 0.0))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  dy12 = (y2-y1)/dx21;
  dy23 = (y3-y2)/dx32;

  ddy = dy23-dy12;
  *pa = 0.5*ddy/dx2312;

  if(ddy != 0.0)
  {
    *px0 = (dy23*x12-dy12*x23)/ddy;

    diff = x2-(*px0);
    *pb = y2-(*pa)*diff*diff;
  }
  else
  {
    *px0 = 0.0;
    *pb = y2;
  }

  *pcode = WN_SUCCESS;
}


/********************************************************************
Fit parabola to 2 points (x,y) and a derivative at one point.  
Parabola is of the form

  y = a(x-x0)^2 + b

Return a, x0, b.
********************************************************************/
void wn_fit_parabola_2pd
(
  int *pcode,
  double *pa,double *px0,double *pb,
  double x1,double y1,double dy1,
  double x2,double y2
)
{
  double x12,dx21,dx121,dy12,ddy,diff;

  dx21 = x2-x1;

  if(!(dx21 != 0.0))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  x12 = 0.5*(x1+x2);

  dy12 = (y2-y1)/dx21;

  ddy = dy12-dy1;

  dx121 = x12-x1;
  if(!(dx121 != 0.0))
  {
    *pcode = WN_SINGULAR;
    return;
  }
  *pa = 0.5*ddy/dx121;

  if(ddy != 0.0)
  {
    *px0 = (dy12*x1-dy1*x12)/ddy;

    diff = x1-(*px0);
    *pb = y1-(*pa)*diff*diff;
  }
  else
  {
    *px0 = 0.0;
    *pb = y1;
  }

  *pcode = WN_SUCCESS;
}


/********************************************************************
Fit parabola to derivatives at 2 points.
Parabola is of the form

  y = a(x-x0)^2 + b

Return a, x0.  b is impossible to compute so it is not returned.
********************************************************************/
void wn_fit_parabola_2d
(
  int *pcode,
  double *pa,double *px0,
  double x1,double dy1,
  double x2,double dy2
)
{
  double dx,ddy;

  dx = x2-x1;

  if(!(dx != 0.0))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  ddy = dy2-dy1;

  if(!(ddy != 0.0))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  *pa = 0.5*ddy/dx;
  *px0 = (dy2*x1-dy1*x2)/ddy;

  *pcode = WN_SUCCESS;
}


/********************************************************************
Fit parabola to 3 points (x,y).  Parabola is of the form

  y = a*x^2 + b*x + c

Return a, b, c.
********************************************************************/
void wn_fit_traditional_parabola_3p
(
  int *pcode,
  double *pa,double *pb, double *pc,
  double x1,double y1,
  double x2,double y2,
  double x3,double y3
)
{
  double x12,x23,x13,dx21,dx32,dx31,dx2312,dy21,dy32,dy31,ddy;

  /* sort the x's */
  if(!(x1 < x3))
  {
    wn_swap(x1,x3,double);
    wn_swap(y1,y3,double);
  }
  if(x2 < x1)
  {
    wn_swap(x1,x2,double);
    wn_swap(y1,y2,double);
  }
  else if(x3 < x2) 
  {
    wn_swap(x2,x3,double);
    wn_swap(y2,y3,double);
  }

  dx21 = x2-x1;
  dx32 = x3-x2;
  dx31 = x3-x1;

  if(!((x1 != x3)&&(dx21 != 0.0)&&(dx32 != 0.0)&&(dx31 != 0)))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  x12 = 0.5*(x1+x2);
  x23 = 0.5*(x2+x3);
  x13 = 0.5*(x1+x3);

  dx2312 = x23-x12;
  if(!(dx2312 != 0.0))
  {
    *pcode = WN_SINGULAR;
    return;
  }

  dy21 = (y2-y1)/dx21;
  dy32 = (y3-y2)/dx32;
  dy31 = (y3-y1)/dx31;

  ddy = dy32-dy21;
  *pa = 0.5*ddy/dx2312;

  *pb = dy31 - 2.0*(*pa)*x13;

  *pc = y2 - (*pa)*x2*x2 - (*pb)*x2;

  *pcode = WN_SUCCESS;
}


/********************************************************************
Fit parabola to 2 points (x,y), given that curvature a is already
known.  Parabola is of the form

  y = a*x^2 + b*x + c

Return b, c.
********************************************************************/
void wn_fit_traditional_parabola_2pa
(
  int *pcode,
  double *pb, double *pc,
  double a,
  double x1,double y1,
  double x2,double y2
)
{
  double dx,sx,mult_dxsx;

  /* sort the x's */
  if(x2 < x1)
  {
    wn_swap(x1,x2,double);
    wn_swap(y1,y2,double);
  }

  dx = x2-x1;

  if(dx == 0.0)
  {
    *pcode = WN_SINGULAR;
    return;
  }

  sx = x2+x1;
  mult_dxsx = dx*sx;

  *pb = ((y2-y1) - a*mult_dxsx)/dx;
  *pc = 0.5*((y2+y1) - a*(x2*x2+x1*x1) - (*pb)*sx);

  *pcode = WN_SUCCESS;
}


/********************************************************************
Convert traditional parabola of the form

  y = a*x^2 + b*x + c

to centered parabola of form 

  y = a*(x-x0)^2 + b

Return x0, b.
********************************************************************/
void wn_convert_parabola_traditional_to_centered
(
  int *pcode,
  double *px0,double *pb,
  double a,
  double b,
  double c
)
{
  if(a == 0.0)
  {
    *pcode = WN_SINGULAR;
    return;
  }

  *px0 = -0.5*b/a;
  *pb = c - 0.25*b*b/a;

  *pcode = WN_SUCCESS;
}


/********************************************************************
Fit traditional cubic to  2 points (x,y) and derivatives
Cubic is of the form

  y = c3*x^3 + c2*x^2 + c1*x + c0

Return c3, c2, c1, c0
********************************************************************/
void wn_fit_cubic_2p2d
(
  int *pcode,
  double coef[4],
  double x1,double y1,double dy1,
  double x2,double y2,double dy2
)
{
  double c[4];
  double m,b;
  double xform[2];
  double accum[5],next_accum[5];
  int i,j;

  *pcode = WN_SUCCESS;

  if(x1 == x2)
  {
    *pcode = WN_SINGULAR;
    return;
  }

  /* compute mapping from x space to -1 +1 space */
  m = 2.0/(x2-x1);
  b = 1.0 - m*x2;

  dy1 /= m;
  dy2 /= m;

  /* compute coefs assuming x1==-1,x2==1 */
  c[2] = (1.0/4.0)*(dy2-dy1);
  c[0] = (1.0/2.0)*(y2+y1) - c[2];
  c[3] = (1.0/2.0)*(y1+dy1 + c[2] - c[0]);
  c[1] = dy2 - 3.0*c[3] - 2.0*c[2];

  /* use mapping to transform cubic */
  wn_zero_vect(coef,4);
  wn_zero_vect(accum,4);
  accum[0] = 1.0;

  xform[0] = b;
  xform[1] = m;

  for(i=0;i<4;++i)
  {
    for(j=0;j<=i;++j)
    {
      coef[j] += c[i]*accum[j];
    }

    wn_mult_polys(next_accum,accum,i+1,xform,2);
    wn_copy_vect(accum,next_accum,i+2);
  }

  *pcode = WN_SUCCESS;
}
