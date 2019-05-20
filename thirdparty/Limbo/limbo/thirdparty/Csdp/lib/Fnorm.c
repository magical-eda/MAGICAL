/*
  Compute the Frobenius norm of a matrix.
  */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "declarations.h"

double Fnorm(A)
     struct blockmatrix A;
{
  int blk;
  double nrm;
  double temp;

  nrm=0;
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  temp=norm2(A.blocks[blk].blocksize,A.blocks[blk].data.vec+1);
	  nrm += temp*temp;
	  break;
	case MATRIX:
	  temp=norm2(A.blocks[blk].blocksize*A.blocks[blk].blocksize,
		     A.blocks[blk].data.mat);
	  nrm += temp*temp;
	  break;
	case PACKEDMATRIX:
	default:
	  printf("Fnorm illegal block type \n");
	  exit(12);
	};
    };

  nrm=sqrt(nrm);
  return(nrm);
}

/*
 * The Knorm is the sum of the Fnorms of the blocks.
 */

double Knorm(A)
     struct blockmatrix A;
{
  int blk;
  double nrm;
  double temp;

  nrm=0;
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  temp=norm2(A.blocks[blk].blocksize,A.blocks[blk].data.vec+1);
	  nrm += temp;
	  break;
	case MATRIX:
	  temp=norm2(A.blocks[blk].blocksize*A.blocks[blk].blocksize,
		     A.blocks[blk].data.mat);
	  nrm += temp;
	  break;
	case PACKEDMATRIX:
	default:
	  printf("Fnorm illegal block type \n");
	  exit(12);
	};
    };

  return(nrm);
}


double mat1norm(A)
     struct blockmatrix A;
{
  int blk;
  double nrm;
  double temp;

  nrm=0;
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  temp=norm1(A.blocks[blk].blocksize,A.blocks[blk].data.vec+1);
	  nrm += temp;
	  break;
	case MATRIX:
	  temp=norm1(1*A.blocks[blk].blocksize*A.blocks[blk].blocksize,
		     A.blocks[blk].data.mat);
	  nrm += temp;
	  break;
	case PACKEDMATRIX:
	default:
	  printf("Fnorm illegal block type \n");
	  exit(12);
	};
    };

  return(nrm);
}


double matinfnorm(A)
     struct blockmatrix A;
{
  int blk;
  int i;
  double nrm;

  nrm=0;
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=A.blocks[blk].blocksize; i++)
	    {
	      if (fabs(A.blocks[blk].data.vec[i]) > nrm)
		nrm=fabs(A.blocks[blk].data.vec[i]);
	    };
	  break;
	case MATRIX:
	  for (i=0; i<A.blocks[blk].blocksize*A.blocks[blk].blocksize; i++)
	    {
	      if (fabs(A.blocks[blk].data.vec[i]) > nrm)
		nrm=fabs(A.blocks[blk].data.vec[i]);
	    };
	  break;
	case PACKEDMATRIX:
	default:
	  printf("Fnorm illegal block type \n");
	  exit(12);
	};
    };

  return(nrm);
}

