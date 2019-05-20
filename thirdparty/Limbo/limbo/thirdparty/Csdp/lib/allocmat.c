/*
 *  Allocate space for a block matrix.  Get strucutre info from A, and
 *  allocate the matrix B with matching structure.
 */


#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void alloc_mat(A,pB)
     struct blockmatrix A;
     struct blockmatrix *pB;
{
  int blk;

  /*
   * First put up the number of blocks.
   */
  pB->nblocks=A.nblocks;

  /*
   * Then allocate space for the block records.
   */

  pB->blocks=(struct blockrec *)malloc(sizeof(struct blockrec)*(A.nblocks+1));
  if (pB->blocks == NULL)
    {
      printf("Storage allocation failed!\n");
      exit(10);
    };
  /*
   *  Now, fill in the info for each block.
   */

  for (blk=1; blk <=A.nblocks; blk++)
    {
      pB->blocks[blk].blockcategory=A.blocks[blk].blockcategory;
      pB->blocks[blk].blocksize=A.blocks[blk].blocksize;
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  pB->blocks[blk].data.vec=(double *)malloc(sizeof(double)*(A.blocks[blk].blocksize+1));
	  if (pB->blocks[blk].data.vec == NULL)
	    {
	      printf("Storage allocation failed!\n");
	      exit(10);
	    };
	  break;
	case MATRIX:
	  pB->blocks[blk].data.mat=(double *)malloc(sizeof(double)*(A.blocks[blk].blocksize)*(A.blocks[blk].blocksize));
	  if (pB->blocks[blk].data.mat == NULL)
	    {
	      printf("Storage allocation failed!\n");
	      exit(10);
	    };
	  break;
	default:
	  printf("alloc_mat illegal block type!\n");
	  exit(12);
	};
    };
}


void free_mat(A)
     struct blockmatrix A;
{
  int blk;


  /*
   *  First, free the space for  each block.
   */

  for (blk=1; blk <=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  free(A.blocks[blk].data.vec);
	  break;
	case MATRIX:
	  free(A.blocks[blk].data.mat);
	  break;
	default:
	  printf("free_mat illegal block type!\n");
	  exit(12);
	};
    };

  /*
   * Then free space for the block records.
   */

  free(A.blocks);

}

