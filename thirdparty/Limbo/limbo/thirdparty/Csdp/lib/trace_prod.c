/*
  Compute the trace of the product of two matrices.  Since we 
  only need the trace, it makes more sense to just compute the diagonal
  of the product and sum the entries..
  */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

double trace_prod(A,B)
     struct blockmatrix A;
     struct blockmatrix B;
{
  int blk;
  double sum;
  int i;
  int j;

  sum=0.0;
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    sum += A.blocks[blk].data.vec[i]*B.blocks[blk].data.vec[i];
	  break;
	case MATRIX:
	  for (j=1; j<=A.blocks[blk].blocksize; j++)
	    for (i=1; i<=A.blocks[blk].blocksize; i++)
	      sum += A.blocks[blk].data.mat[ijtok(i,j,A.blocks[blk].blocksize)]
                    *B.blocks[blk].data.mat[ijtok(j,i,A.blocks[blk].blocksize)];
	  break;
	case PACKEDMATRIX:
	default:
	  printf("trace_prod illegal block type \n");
	  exit(12);
	};
    };
  return(sum);
}

