/*
  Compute the primal objective function value pobj=Trace(C*X)  Since we
  only need the trace, it makes more sense to just compute the diagonal
  of the product and sum the entries..
  */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

double calc_pobj(C,X,constant_offset)
     struct blockmatrix C;
     struct blockmatrix X;
     double constant_offset;
{
  int blk;
  int i,j;
  double pobj;

  pobj=constant_offset;

  for (blk=1; blk<=C.nblocks; blk++)
    {
      switch (C.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=C.blocks[blk].blocksize; i++)
	    {
	      pobj += C.blocks[blk].data.vec[i]*X.blocks[blk].data.vec[i];
	    };
	  break;
	case MATRIX:
	  for (j=1; j<=C.blocks[blk].blocksize; j++)
	    for (i=1; i<=C.blocks[blk].blocksize; i++)
	      pobj += C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)]*X.blocks[blk].data.mat[ijtok(j,i,C.blocks[blk].blocksize)];
          break;
	default:
	  printf("calc_pobj illegal block type!\n");
	  exit(12);
	};
    };

  return(pobj);
	 
}
