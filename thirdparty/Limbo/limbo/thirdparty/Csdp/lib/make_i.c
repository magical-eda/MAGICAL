/*
  Make an identity matrix.
 */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void make_i(A)
     struct blockmatrix A;
{
  int blk,i,j;
  double *p;

  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
	  p=A.blocks[blk].data.vec;
	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    p[i]=1.0;
	  break;
	case MATRIX:
	  p=A.blocks[blk].data.mat;
#pragma omp parallel for schedule(dynamic,64) default(none) private(i,j) shared(blk,p,A)
	  for (j=1; j<=A.blocks[blk].blocksize; j++)
	    for (i=1; i<=A.blocks[blk].blocksize; i++)
	      p[ijtok(i,j,A.blocks[blk].blocksize)]=0.0;

	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    p[ijtok(i,i,A.blocks[blk].blocksize)]=1.0;
	  break;
	default:
	  printf("make_i illegal block type\n");
	  exit(12);
	};
    }
}






