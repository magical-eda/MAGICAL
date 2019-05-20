/*
  Write out a problem in SDPA format.
*/

#include <stdio.h>
#include <stdlib.h>
#include "declarations.h"

int write_prob(fname,n,k,C,a,constraints)
     char *fname;
     int n;
     int k;
     struct blockmatrix C;
     double *a;
     struct constraintmatrix *constraints;
{
  FILE *fid;
  int i;
  int j;
  int blk;
  struct sparseblock *p;

  /*
   * Open up the file for output.
   */
  fid=fopen(fname,"w");

  if (fid == (FILE *) NULL)
    {
      printf("Couldn't open problem file for writing! \n");

      exit(11);
    };


  /*
   * Write out the basic problem size info.
   */
  fprintf(fid,"%d \n",k);
  fprintf(fid,"%d \n",C.nblocks);
  for (i=1; i<=C.nblocks; i++)
    {
      switch (C.blocks[i].blockcategory)
	{
	case DIAG:
	  fprintf(fid,"%d ",-C.blocks[i].blocksize);
	  break;
	case MATRIX:
	  fprintf(fid,"%d ",C.blocks[i].blocksize);
	  break;
	default:
	  printf("Illegal block type!\n");
	  exit(12);
	};

    };

  fprintf(fid,"\n");

  /*
   * Write out the a vector.  (c in SDPA terminology)
   */
  for (i=1; i<=k; i++)
    fprintf(fid,"%.18e ",a[i]);
  fprintf(fid,"\n");

  /*
   *  Write out the C matrix (F0 in SDPA)
   */

  for (blk=1; blk<=C.nblocks; blk++)
    {
      switch (C.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=C.blocks[blk].blocksize; i++)
	    {
	      if (C.blocks[blk].data.vec[i] != 0.0)
		fprintf(fid,"0 %d %d %d %.18e \n",blk,i,i,C.blocks[blk].data.vec[i]);
	    };
	  break;
	case MATRIX:
	  for (i=1; i<=C.blocks[blk].blocksize; i++)
	    for (j=i; j<=C.blocks[blk].blocksize; j++)
	      {
		if (C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)] != 0.0 )
		  fprintf(fid,"0 %d %d %d %.18e \n",blk,i,j,C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)]);
	      };
	  break;
	default:
	  printf("Illegal block type!\n");
	  exit(12);
	};
    };

  /*
   *  Now, start writing out the A_i (F_k in SDPA notation)
   */

  for (i=1; i<=k; i++)
    {
      p=constraints[i].blocks;
      while (p != NULL)
	{
	  for (j=1; j<=p->numentries; j++)
	    {
	      fprintf(fid,"%d %d %d %d %.18e \n",i,p->blocknum,
		      p->iindices[j],
		      p->jindices[j],
		      p->entries[j]);
	    };
	  p=p->next;
	};
    };

  /*
   * Close up the file and get out.
   */
    
  fclose(fid);
  return(0);
}










