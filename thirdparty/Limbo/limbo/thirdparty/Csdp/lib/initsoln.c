/*
 * Build an initial solution for an SDP problem.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "declarations.h"

void initsoln(n,k,C,a,constraints,pX0,py0,pZ0)
     int n;
     int k;
     struct blockmatrix C;
     double *a;
     struct constraintmatrix *constraints;
     struct blockmatrix *pX0;
     double **py0;
     struct blockmatrix *pZ0;
{
  int i,j;
  double alpha,beta;
  double maxnrmA,nrmA;
  double nrmC;
  double scale1, scale2;
  struct sparseblock *ptr;

  /*
   *  First, allocate storage for X0, y0, and Z0.
   */
  alloc_mat(C,pX0);
  alloc_mat(C,pZ0);
  *py0=(double *)malloc(sizeof(double)*(k+1));

  if (py0 == NULL)
    {
      printf("Storage allocation failed!\n");
      exit(10);
    };

  /*
   *  next, pick alpha=n*max_i((1+|a_i|)/(1+||A_i||)).  
   */

  maxnrmA=0.0;
  alpha=0.0;
  for (i=1; i<=k; i++)
    {
      nrmA=0.0;
      ptr=constraints[i].blocks;

      while (ptr != NULL)
	{
	  for (j=1; j<=ptr->numentries; j++)
	    {
	      nrmA += (ptr->entries[j])*(ptr->entries[j]);
	      if (ptr->iindices[j] != ptr->jindices[j])
		nrmA += (ptr->entries[j])*(ptr->entries[j]);
	    };
	  ptr=ptr->next;
	};

      nrmA=sqrt(nrmA);
      if (nrmA > maxnrmA)
	maxnrmA=nrmA;

      if ((1+fabs(a[i]))/(1+nrmA) > alpha)
        alpha=(1+fabs(a[i]))/(1+nrmA);

    };

  alpha=n*alpha;

  /*
   * Next, calculate the F norm of C.
   */

  nrmC=Fnorm(C);

  if (nrmC > maxnrmA)
    {
      beta=(1+nrmC)/sqrt(n*1.0);
    }
  else
    {
      beta=(1+maxnrmA)/sqrt(n*1.0);
    };

  /*
   * Now that we have alpha and beta, make X=100*alpha*I, Z=100*beta*I, y=0.
   */

  make_i(*pX0);
  scale1=0.0;
  scale2=10*alpha;
  mat_mult(scale1,scale2,*pX0,*pX0,*pX0);

  make_i(*pZ0);
  scale1=0.0;
  scale2=10*beta;
  mat_mult(scale1,scale2,*pZ0,*pZ0,*pZ0);

  /*
   * Set y to 0.
   */
  for (i=1; i<=k; i++)
    (*py0)[i]=0;

}



