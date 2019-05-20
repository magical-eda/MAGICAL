/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************

void wn_resistance_series_reduce(&total,r1,r2)
void wn_resistance_parallel_reduce(&total,r1,r2)
void wn_resistance_y_to_delta(&r23,&r13,&r12,r1,r2,r3)
void wn_resistance_delta_to_y(&r1,&r2,&r3,r23,r13,r12)

void wn_path_series_reduce(&total,r1,r2)
void wn_max_path_parallel_reduce(&total,r1,r2)
void wn_min_path_parallel_reduce(&total,r1,r2)
void wn_min_path_y_to_delta(&r23,&r13,&r12,r1,r2,r3)
void wn_max_path_delta_to_y(&r1,&r2,&r3,r23,r13,r12)

******************************************************************************/

#include "wnlib.h"
#include "wnmax.h"



void wn_resistance_series_reduce(ptotal,r1,r2)

double *ptotal,r1,r2;

{
  *ptotal = r1+r2;
}



void wn_resistance_parallel_reduce(ptotal,r1,r2)

double *ptotal,r1,r2;

{
  *ptotal = r1*r2/(r1+r2);
}



void wn_resistance_y_to_delta(pr23,pr13,pr12,r1,r2,r3)

double *pr23,*pr13,*pr12,r1,r2,r3;

{
  double sum;

  sum = r1*r2+r1*r3+r2*r3;

  *pr23 = sum/r1;
  *pr13 = sum/r2;
  *pr12 = sum/r3;
}



void wn_resistance_delta_to_y(pr1,pr2,pr3,r23,r13,r12)

double *pr1,*pr2,*pr3,r23,r13,r12;

{
  double denom;

  denom = r23+r13+r12;

  *pr1 = (r13*r12)/denom;
  *pr2 = (r23*r12)/denom;
  *pr3 = (r13*r23)/denom;
}



void wn_path_series_reduce(ptotal,r1,r2)

double *ptotal,r1,r2;

{
  *ptotal = r1+r2;
}



void wn_max_path_parallel_reduce(ptotal,r1,r2)

double *ptotal,r1,r2;

{
  *ptotal = wn_max(r1,r2);
}



void wn_min_path_parallel_reduce(ptotal,r1,r2)

double *ptotal,r1,r2;

{
  *ptotal = wn_min(r1,r2);
}



void wn_path_y_to_delta(pr23,pr13,pr12,r1,r2,r3)

double *pr23,*pr13,*pr12,r1,r2,r3;

{
  *pr23 = r2+r3;
  *pr13 = r1+r3;
  *pr12 = r1+r2;
}



void wn_path_delta_to_y(pr1,pr2,pr3,r23,r13,r12)

double *pr1,*pr2,*pr3,r23,r13,r12;

{
  *pr1 = (-r23+r13+r12)/2.0;
  *pr2 = ( r23-r13+r12)/2.0;
  *pr3 = ( r23+r13-r12)/2.0;
}





