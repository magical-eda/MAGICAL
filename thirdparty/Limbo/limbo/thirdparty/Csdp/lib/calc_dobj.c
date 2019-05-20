/*
  Compute the dual objective function value dobj=a'y.  
  */

#include "declarations.h"

double calc_dobj(k,a,y,constant_offset)
     int k;
     double *a;
     double *y;
     double constant_offset;
{
  double s;
  int incx=1;

  s=0.0;

#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
    s=s+DDOT(&k,a+1,&incx,y+1,&incx);
#else
    s=s+ddot(&k,a+1,&incx,y+1,&incx);
#endif
#else
#ifdef CAPSBLAS
    s=s+DDOT_(&k,a+1,&incx,y+1,&incx);
#else
    s=s+ddot_(&k,a+1,&incx,y+1,&incx);
#endif
#endif

  return(s+constant_offset);
  
}

