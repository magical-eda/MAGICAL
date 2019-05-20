/*
 * Attempt to tweak a solution with negative gap so that the gap is 
 * closer to 0.  Do this by moving from y to y+sa, where s is picked 
 * by linesearch to keep Z PD.  
 *
 * To calculate dZ, 
 *   dy=a
 *   dZ=A'(dy)
 * 
 * To get a change of dual objective equal to -gap,
 *
 *   -gap=s*a'*a
 *      s=-gap/(a'*a) 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "declarations.h"


void tweakgap(n,k,a,constraints,gap,Z,dZ,y,dy,work1,work2,work3,work4,workvec1,
	      workvec2,workvec3,workvec4,printlevel)
     int n;
     int k;
     double *a;
     struct constraintmatrix *constraints;
     double gap;
     struct blockmatrix Z,dZ;
     double *y,*dy;
     struct blockmatrix work1,work2,work3,work4;
     double *workvec1,*workvec2,*workvec3,*workvec4;
     int printlevel;
{
  int i;
  double norma;
  double alpha;

  norma=norm2(k,a+1);

  for (i=1; i<=k; i++)
    dy[i]=a[i];
 
  op_at(k,dy,constraints,dZ);

  alpha=linesearch(n,dZ,work1,work2,work3,work4,workvec1,workvec2,
		   workvec3,1.0,-gap/(norma*norma),0);

  if (printlevel >= 2)
    printf("tweak: alpha is %e \n",alpha);

  for (i=1; i<=k; i++)
    y[i]=y[i]+alpha*dy[i];

  addscaledmat(Z,alpha,dZ,Z);

}
