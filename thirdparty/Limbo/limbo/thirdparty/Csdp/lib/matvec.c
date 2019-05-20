/*
 *  Compute y=A*x
 *
 *  We use 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void matvec(A,x,y)
     struct blockmatrix A;
     double *x;
     double *y;
{
  int blk,i,n;
  int p;
  double *ap;
  double scale1;
  double scale2;
  int inc;
  
  /*
   * Work through the blocks one at a time.
   */

  p=1;
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    {
	      y[p]=A.blocks[blk].data.vec[i]*x[p];
	      p++;
	    };
	  break;
	case MATRIX:
	  /*
	   * Call dgemm to do the matrix multiplication.
	   */

	  n=A.blocks[blk].blocksize;
	  ap=A.blocks[blk].data.mat;
          inc=1;

	  scale1=1.0;
	  scale2=0.0;
#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
	  DGEMV("N",&n,&n,&scale1,ap,&n,x+p,&inc,&scale2,y+p,&inc);
#else
	  dgemv("N",&n,&n,&scale1,ap,&n,x+p,&inc,&scale2,y+p,&inc);
#endif
#else
#ifdef CAPSBLAS
	  DGEMV_("N",&n,&n,&scale1,ap,&n,x+p,&inc,&scale2,y+p,&inc);
#else
	  dgemv_("N",&n,&n,&scale1,ap,&n,x+p,&inc,&scale2,y+p,&inc);
#endif
#endif
	  p=p+n;

	  break;
	case PACKEDMATRIX:
	default:
	  printf("matvec illegal block type \n");
	  exit(12);
	};

    };
}


