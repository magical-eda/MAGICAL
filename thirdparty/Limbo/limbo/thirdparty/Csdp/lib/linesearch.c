/*
  Find the largest value for alpha<=1  such that X+alpha*dX is PD.
  Return the optimal X+alpha*dX in work1.  If the search fails, 
  return alpha=-1.0.  
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "declarations.h"

#define LANCZOSITS 30

double linesearch(n,dX,work1,work2,work3,cholinv,q,z,workvec,
		  stepfrac,start,printlevel)
     int n;
     struct blockmatrix dX,work1,work2,work3,cholinv;
     double *q;
     double *z;
     double *workvec;
     double stepfrac;
     double start;
     int printlevel;
{
  int i,j,jj;
  double alpha;
  double scale1;
  double scale2;
  int inc;
  double lalpha[LANCZOSITS+1];
  double lbeta[LANCZOSITS+1];
  double lbeta2[LANCZOSITS+1];
  double evalues[LANCZOSITS+2];
  double maxeigs[LANCZOSITS+1];
  double reorth[LANCZOSITS+1];
  double maxeig;
  int maxn,blk,method;
  double *lanczosvectors;

  /*
   * Allocate space for storing the Lanczos vectors.
   */

  lanczosvectors=(double *) malloc((LANCZOSITS+1)*n*sizeof(double));
  if (lanczosvectors==NULL)
    {
      printf("Storage Allocation Failed!\n");
      exit(10);
    };

  /*
   * First, figure out which method to use.  If maxn is big, then use 
   * lots of matrix/vector mults.  If maxn is small, then multiply the
   * matrices once and for all.
   */

  maxn=0;
  for (blk=1; blk<=work1.nblocks; blk++)
    {
      if ((work1.blocks[blk].blocksize > maxn) && 
	  (work1.blocks[blk].blockcategory==MATRIX))
	maxn=work1.blocks[blk].blocksize;
    };

  if (maxn > 6*LANCZOSITS)
    {
      method=1;     /* matrix vector mults. */
      if (printlevel >= 4)
	{
	  printf("linesearch method 1 \n");
	};

    }
  else
    {
      method=2;     /* matrix matrix mults. */
      if (printlevel >= 4)
	{
	  printf("linesearch method 2 \n");
	};
    };

  /*
   *
   */

  if (method==1)
    {
      scale1=-1.0;
      zero_mat(work1);
      store_unpacked(cholinv,work3);
      triu(work3);
      addscaledmat(work1,scale1,work3,work2);
      trans(work2);
    }
  else
    {
      /*
       * method=2.
       */
      /*
       * First, multiply dX*cholinv.  Store it in work3.
       */
      scale1=1.0;
      scale2=0.0;
      store_unpacked(cholinv,work2);
      triu(work2);
      mat_mult(scale1,scale2,dX,work2,work3);
      
      /*
       * Now, find R^{-T}
       */
      
      trans(work2);
      
      scale1=-1.0;
      scale2=0.0;
      mat_mult(scale1,scale2,work2,work3,work1);
    };

  /*
   *  Initialize q.
   */

  for (i=1; i<=n; i++)
    q[i]=1.0/sqrt(n*1.0);

  for (i=1; i<=n; i++)
    lanczosvectors[ijtok(i,1,n)]=q[i];

  /*
   *  Next, perform the Lancoz iterations.
   */

  maxeig=-1.0e200;
  for (j=1; j<=LANCZOSITS; j++)
    {
      maxeigs[j]=-1.0e100;

      if (method == 1)
	{
	  matvec(work3,q,z);
	  matvec(dX,z,workvec);
	  matvec(work2,workvec,z);
	}
      else
	{
	  matvec(work1,q,z);
	};

      lalpha[j]=0.0;
      for (i=1; i<=n; i++)
	lalpha[j]=lalpha[j]+q[i]*z[i];

      /*
       * We'll use BLAS routines to do the reorthogonalization.  First,
       * Compute reorth=lanczosvectors'*z.  Then compute 
       * z=z-lanczosvectors*reorth.
       *
       * lanczosvectors is of size n by j, with lda=n.
       */

      scale1=1.0;
      scale2=0.0;
      inc=1;
      
#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
      DGEMV("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#else
      dgemv("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#endif
#else
#ifdef CAPSBLAS
      DGEMV_("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#else
      dgemv_("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#endif
#endif

      scale1=-1.0;
      scale2=1.0;
      inc=1;

#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
      DGEMV("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#else
      dgemv("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#endif
#else
#ifdef CAPSBLAS
      DGEMV_("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#else
      dgemv_("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#endif
#endif
	  
      scale1=1.0;
      scale2=0.0;
      inc=1;
      
#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
      DGEMV("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#else
      dgemv("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#endif
#else
#ifdef CAPSBLAS
      DGEMV_("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#else
      dgemv_("T",&n,&j,&scale1,lanczosvectors,&n,z+1,&inc,&scale2,reorth+1,&inc);
#endif
#endif

      scale1=-1.0;
      scale2=1.0;
      inc=1;

#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
      DGEMV("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#else
      dgemv("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#endif
#else
#ifdef CAPSBLAS
      DGEMV_("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#else
      dgemv_("N",&n,&j,&scale1,lanczosvectors,&n,reorth+1,&inc,&scale2,z+1,&inc);
#endif
#endif
	  

      /*
       * Compute the norm of z.
       */

      lbeta[j]=norm2(n,z+1);
      if (fabs(lbeta[j])<1.0e-16)
	{
	  if (printlevel >= 3)
	    printf("Small beta[j]\n");
	  j=j-1;
	  jj=j;
	  goto DONEEARLY;
	};

      for (i=1; i<=n; i++)
        q[i]=z[i]/lbeta[j];

      /*
       * Store the Lanczos vector.
       */

      for (i=1; i<=n; i++)
	lanczosvectors[ijtok(i,j+1,n)]=q[i];

      if (j>=5)
	{
	  /*
	   * Now, get ready to call qreig to get the eigenvalues.
	   */

	  for (i=1; i<=j-1; i++)
	    lbeta2[i]=lbeta[i]*lbeta[i];
	  for (i=1; i<=j; i++)
	    evalues[i]=lalpha[i];

	  qreig(j,evalues,lbeta2);
	  
	  maxeigs[j]=-1.0e100;

	  for (i=1; i<=j; i++)
	    {
	      if (printlevel >= 4)
		printf ("qreig evalue %e \n",evalues[i]);
	      if (evalues[i] > maxeigs[j])
		maxeigs[j]=evalues[i];
	    };

	  if (maxeigs[j] > maxeig)
	    maxeig=maxeigs[j];

	};

      /*
       * Now, decide whether or not to stop.  
       */

      if ((j>=7) && (maxeigs[j] <= 1/(3*start)) && 
	  (fabs((maxeigs[j]-maxeigs[j-2])/(0.000001+fabs(maxeigs[j]))) < 0.2))
	{
	  if (printlevel >= 4)
	    printf("Stopping on <1/3s j=%d \n",j);
	  jj=j;
	  goto DONEEARLY;
	};

      if ((j>=8) && (fabs((maxeigs[j]-maxeigs[j-2])/(0.000001+fabs(maxeigs[j]))) < 0.02))
	{
	  if (printlevel >= 4)
	    printf("Stopping here, on tightness j=%d \n",j);
	  maxeig=maxeig+0.01*fabs(maxeig);
	  jj=j;
	  goto DONEEARLY;
	};
    };

  jj=LANCZOSITS;

 DONEEARLY:

  if (printlevel >= 4)
    {
      for (i=1; i<=jj; i++)
	printf("maxeigs[%d]=%e \n",i,maxeigs[i]);
      printf("maxeig %e \n",maxeig);
    };

  if (printlevel >= 4)
    printf("Lancoz converged after %d iters\n",jj);

  if (printlevel >= 3)
    {
      if (maxeig >0.0)
	printf("eigsearch: alpha=%e \n",stepfrac/maxeig);
      else
	printf("eigsearch: alpha=+Inf\n");
    };

  if (((stepfrac/maxeig) < start) && (maxeig > 0))
    alpha=stepfrac/maxeig;
  else
    alpha=start;

  free(lanczosvectors);

  return(alpha);
}

