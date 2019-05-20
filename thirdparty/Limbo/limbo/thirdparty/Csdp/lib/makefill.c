/*
 *  This routine creates the fill data structure, which includes information
 *  on all elements of X which are touched by elements of the various 
 *  constraint matrices in computing A(X).  This is helpful in speeding
 *  up the op_o operation.
 *
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "declarations.h"

void makefill(k,C,constraints,pfill,work1,printlevel)
     int k;
     struct blockmatrix C;
     struct constraintmatrix *constraints;
     struct constraintmatrix *pfill;
     struct blockmatrix work1;
     int printlevel;
{
  int i,j,p,q,blk,blksize;
  struct sparseblock *ptr;

  /*
   * Now, zero out the work matrix.
   */
  zero_mat(work1);
  /*
   * Put 1's in all diagonal positions, plus nonzero elements of C.
   */
  for (blk=1; blk<=C.nblocks; blk++)
    {
      switch(C.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=C.blocks[blk].blocksize; i++)
	    work1.blocks[blk].data.vec[i]=1.0;
	  break;
	case MATRIX:
	  for (i=1; i<=C.blocks[blk].blocksize; i++)
	    for (j=1; j<=C.blocks[blk].blocksize; j++)
	      {
		if ((C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)] != 0.0) || (i == j))
		  work1.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)]=1.0;
	      };
	  break;
	case PACKEDMATRIX:
	default:
	  printf("makefill illegal block type \n");
	  exit(12);
	};
    };

  /*
   * Now, work through the constraints.
   */

  for (i=1; i<=k; i++)
    {
      ptr=constraints[i].blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  switch(C.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      break;
	    case MATRIX:
	      for (j=1; j<=ptr->numentries; j++)
		{
		  p=ptr->iindices[j];
		  q=ptr->jindices[j];
		  work1.blocks[blk].data.mat[ijtok(p,q,C.blocks[blk].blocksize)]=1.0;
		  work1.blocks[blk].data.mat[ijtok(q,p,C.blocks[blk].blocksize)]=1.0;
		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("addscaledmat illegal block type \n");
	      exit(12);
	    };  

	  ptr=ptr->next;
	};
    };
  /*
   * At this point, we have 1's in all fill positions in work1.  
   */

  /*
   * Allocate space for each block.
   */

  ptr=(struct sparseblock *)malloc(sizeof(struct sparseblock));
  if (ptr == NULL)
    {
      printf("Storage Allocation Failed!\n");
      exit(10);
    };
  pfill->blocks=ptr;
  ptr->next=NULL;
  ptr->blocknum=1;
  ptr->numentries=0;
  ptr->blocksize=C.blocks[1].blocksize;

  for (i=2; i<=C.nblocks; i++)
    {
      ptr->next=(struct sparseblock *)malloc(sizeof(struct sparseblock));
      if (ptr->next == NULL)
	{
	  printf("Storage Allocation Failed!\n");
	  exit(10);
	};
      ptr=ptr->next;
      ptr->blocknum=i;
      ptr->numentries=0;
      ptr->blocksize=C.blocks[i].blocksize;
    };
  ptr->next=(struct sparseblock *)NULL;

  /*
   * Go through each block of work1, filling out pfill as we go.
   */

  ptr=pfill->blocks;

  while (ptr != NULL)
    {
      blk=ptr->blocknum;

      switch(C.blocks[blk].blockcategory)
	{
	case DIAG:
	  ptr->numentries=ptr->blocksize;
	  ptr->entries=(double *) malloc((ptr->blocksize+1)*sizeof(double));
	  if (ptr->entries == NULL)
	    {
	      printf("Storage Allocation Failed!\n");
	      exit(10);
	    };
#ifdef NOSHORTS
	  ptr->iindices=(int *) malloc((ptr->blocksize+1)*sizeof(int));
#else
	  ptr->iindices=(unsigned short *) malloc((ptr->blocksize+1)*sizeof(unsigned short));
#endif

	  if (ptr->iindices == NULL)
	    {
	      printf("Storage Allocation Failed!\n");
	      exit(10);
	    };

#ifdef NOSHORTS
	  ptr->jindices=(int *) malloc((ptr->blocksize+1)*sizeof(int));
#else
	  ptr->jindices=(unsigned short *) malloc((ptr->blocksize+1)*sizeof(unsigned short));
#endif
	  if (ptr->jindices == NULL)
	    {
	      printf("Storage Allocation Failed!\n");
	      exit(10);
	    };

	  for (j=1; j<=ptr->numentries; j++)
	    {
	      ptr->entries[j]=1.0;
	      ptr->iindices[j]=j;
	      ptr->jindices[j]=j;
	    };
	  break;

	  /*
	   * This is a matrix block.
	   */
	case MATRIX:
	  blksize=C.blocks[blk].blocksize;
	  ptr->numentries=0;
	  for (i=1; i<= blksize; i++)
	    for (j=1; j<=blksize; j++)
	      {
		if (work1.blocks[blk].data.mat[ijtok(i,j,blksize)] == 1.0)
		  {
		    ptr->numentries=ptr->numentries+1;
		  };
	      };

	  ptr->entries=(double *)malloc((ptr->numentries+1)*sizeof(double));
	  if (ptr == NULL)
	    {
	      printf("Storage Allocation Failed!\n");
	      exit(10);
	    };

#ifdef NOSHORTS
	  ptr->iindices=(int *) malloc((ptr->numentries+1)*sizeof(int));
#else
	  ptr->iindices=(unsigned short *) malloc((ptr->numentries+1)*sizeof(unsigned short));
#endif

	  if (ptr->iindices == NULL)
	    {
	      printf("Storage Allocation Failed!\n");
	      exit(10);
	    };

#ifdef NOSHORTS
	  ptr->jindices=(int *) malloc((ptr->numentries+1)*sizeof(int));
#else
	  ptr->jindices=(unsigned short *) malloc((ptr->numentries+1)*sizeof(unsigned short));
#endif

	  if (ptr->jindices == NULL)
	    {
	      printf("Storage Allocation Failed!\n");
	      exit(10);
	    };

          ptr->numentries=0;
	  for (i=1; i<= blksize; i++)
	    for (j=1; j<=blksize; j++)
	      {
		if (work1.blocks[blk].data.mat[ijtok(i,j,blksize)] == 1.0)
		  {
		    ptr->numentries=ptr->numentries+1;
		    ptr->entries[ptr->numentries]=1.0;
		    ptr->iindices[ptr->numentries]=i;
		    ptr->jindices[ptr->numentries]=j;
		  };
	      };


	  break;

	case PACKEDMATRIX:
	default:
	  printf("makefill illegal block type \n");
	  exit(12);
	};

      ptr=ptr->next;
    };

  /*
   * Print out information about fill.
   */

  ptr=pfill->blocks;
  while (ptr != NULL)
    {
      blk=ptr->blocknum;
      if (printlevel >= 3)
	printf("Block %d, Size %d, Fill %d, %.2f \n",blk,C.blocks[blk].blocksize,ptr->numentries,100.0*ptr->numentries/(C.blocks[blk].blocksize*C.blocks[blk].blocksize*1.0));
      ptr=ptr->next;
    };
}



