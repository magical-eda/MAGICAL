/*
 * This file contains routines for manipulating block matrices with blocks
 * stored in LAPACK's packed storage scheme.  
 */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void store_packed(A,B)
     struct blockmatrix A,B;
{
  int blk,i,j,n;
  double *p;
  double *q;

  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
	  p=A.blocks[blk].data.vec;
	  q=B.blocks[blk].data.vec;
	  n=A.blocks[blk].blocksize;
	  for (i=1; i<=n; i++)
	    q[i]=p[i];
	  break;
	case MATRIX:
	  p=A.blocks[blk].data.mat;
	  q=B.blocks[blk].data.mat;
	  n=A.blocks[blk].blocksize;
#pragma omp parallel for schedule(dynamic,64) private(i,j) shared(p,q,n)
	  for (j=1; j<=n; j++)
	    for (i=1; i<=j; i++)
	      q[ijtokp(i,j,n)]=p[ijtok(i,j,n)];
	  break;
	default:
	  printf("store_packed illegal block type \n");
	  exit(12);
	};
    }

}

void store_unpacked(A,B)
     struct blockmatrix A,B;
{
  int blk,i,j,n;
  double *p;
  double *q;

  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
	  p=A.blocks[blk].data.vec;
	  q=B.blocks[blk].data.vec;
	  n=A.blocks[blk].blocksize;
	  for (i=1; i<=n; i++)
	    q[i]=p[i];
	  break;
	case PACKEDMATRIX:
	  p=A.blocks[blk].data.mat;
	  q=B.blocks[blk].data.mat;
	  n=A.blocks[blk].blocksize;
#pragma omp parallel for schedule(dynamic,64) private(i,j) shared(p,q,n)
	  for (j=1; j<=n; j++)
	    for (i=1; i<=j; i++)
	      q[ijtok(i,j,n)]=p[ijtokp(i,j,n)];
	  for (j=1; j<n; j++)
	    for (i=j+1; i<=n; i++)
	      q[ijtok(i,j,n)]=q[ijtok(j,i,n)];

	  break;
	default:
	  printf("store_unpacked block type \n");
	  exit(12);
	};
    }

}

/*
 *  Allocate space for a block matrix.  Get strucutre info from A, and
 *  allocate the matrix B with matching structure.
 */




void alloc_mat_packed(A,pB)
     struct blockmatrix A;
     struct blockmatrix *pB;
{
  int blk,n;

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

      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  pB->blocks[blk].blockcategory=A.blocks[blk].blockcategory;
	  pB->blocks[blk].blocksize=A.blocks[blk].blocksize;
	  pB->blocks[blk].data.vec=(double *)malloc(sizeof(double)*(A.blocks[blk].blocksize+1));
	  if (pB->blocks[blk].data.vec == NULL)
	    {
	      printf("Storage allocation failed!\n");
	      exit(10);
	    };
	  break;
	case MATRIX:
	  n=A.blocks[blk].blocksize;
	  pB->blocks[blk].blockcategory=PACKEDMATRIX;
	  pB->blocks[blk].blocksize=n;
	  pB->blocks[blk].data.mat=(double *)malloc(sizeof(double)*n*(n+1)/2);
	  if (pB->blocks[blk].data.mat == NULL)
	    {
	      printf("Storage allocation failed!\n");
	      exit(10);
	    };
	  break;
	default:
	  printf("Illegal block type!\n");
	  exit(12);
	};
    };
}


void free_mat_packed(A)
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
	case PACKEDMATRIX:
	  free(A.blocks[blk].data.mat);
	  break;
	default:
	  printf("Illegal block type!\n");
	  exit(12);
	};
    };

  /*
   * Then free space for the block records.
   */

  free(A.blocks);

}

void triu(A)
     struct blockmatrix A;
{
  int i,j,n;
  int blk;

  for (blk=1; blk <= A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory)
	{
	case DIAG:
	  break;
	case MATRIX:
	  n=A.blocks[blk].blocksize;
#pragma omp parallel for schedule(dynamic,64) private(i,j) shared(A,n)
	  for (j=1; j<n; j++)
	    for (i=j+1; i<=n; i++)
	      A.blocks[blk].data.mat[ijtok(i,j,n)]=0.0;
	  break;
	default:
	  printf("triu illegal block type!\n");
	  exit(12);
	};
    };
  
}     
