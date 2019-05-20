/*
  Add a matrix plus a multiple of a second matrix and put the result in a
  third matrix.

    C=A+scale*B

  */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void addscaledmat(A,scale,B,C)
     struct blockmatrix A;
     double scale;
     struct blockmatrix B;
     struct blockmatrix C;
{
  int blk;
  int i,j;

  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:

	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    C.blocks[blk].data.vec[i] = A.blocks[blk].data.vec[i] + scale*B.blocks[blk].data.vec[i];
	  break;
	case MATRIX:
#pragma omp parallel for schedule(dynamic,64) default(none) private(i,j) shared(A,B,C,scale,blk)
	  for (j=1; j<=A.blocks[blk].blocksize; j++)
	    for (i=1; i<=A.blocks[blk].blocksize; i++)
	      C.blocks[blk].data.mat[ijtok(i,j,A.blocks[blk].blocksize)]=
		A.blocks[blk].data.mat[ijtok(i,j,A.blocks[blk].blocksize)]+
		scale*B.blocks[blk].data.mat[ijtok(i,j,A.blocks[blk].blocksize)];
	  break;
	case PACKEDMATRIX:
	default:
	  printf("addscaledmat illegal block type \n");
	  exit(12);
	};
    };


}


