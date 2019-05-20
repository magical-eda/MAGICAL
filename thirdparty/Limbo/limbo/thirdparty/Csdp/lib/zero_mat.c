/*
  Zero out a block matrix.
*/

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void zero_mat(A)
     struct blockmatrix A;
{
  int i,j;
  int blk;

  /*
   * Loop through the blocks, zeroing one at a time.
   */

  for (blk=1; blk<=A.nblocks; blk++)
    {
      /*
       * Zero out block i.
       */

      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    {
	      A.blocks[blk].data.vec[i]=0.0;
	    };
	  break;
	case MATRIX:
#pragma omp parallel for schedule(dynamic,64) default(none) shared(A,blk) private(j,i)
	  for (j=1; j<=A.blocks[blk].blocksize; j++)
	    for (i=1; i<=A.blocks[blk].blocksize; i++)
	      A.blocks[blk].data.mat[ijtok(i,j,A.blocks[blk].blocksize)]=0.0;
	  break;
	default:
	  printf("Illegal block type \n");
	  exit(12);
	};
    };

}


