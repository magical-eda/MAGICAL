/*
  Copy a matrix A to a second matrix B in our blocked format.
 */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void copy_mat(A,B)
     struct blockmatrix A,B;
{
  int blk,i,j;
  double *p;
  double *q;

  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
	  p=A.blocks[blk].data.vec;
	  q=B.blocks[blk].data.vec;
	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    q[i]=p[i];
	  break;
	case MATRIX:
	  p=A.blocks[blk].data.mat;
	  q=B.blocks[blk].data.mat;
#pragma omp parallel for schedule(dynamic,64) private(i,j) shared(p,q)
	  for (j=1; j<=A.blocks[blk].blocksize; j++)
	    for (i=1; i<=A.blocks[blk].blocksize; i++)
	      q[ijtok(i,j,A.blocks[blk].blocksize)]=
		p[ijtok(i,j,A.blocks[blk].blocksize)];
	  break;
	default:
	  printf("copy_mat illegal block type \n");
	  exit(12);
	};
    }
}



