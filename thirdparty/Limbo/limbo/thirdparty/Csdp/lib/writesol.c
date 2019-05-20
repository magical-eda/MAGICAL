/*
  Write out a solution in SDPA format.
*/

#include <stdio.h>
#include <stdlib.h>
#include "declarations.h"

int write_sol(fname,n,k,X,y,Z)
     char *fname;
     int n;
     int k;
     struct blockmatrix X;
     double *y;
     struct blockmatrix Z;
{
  FILE *fid;
  int i;
  int j;
  int blk;
  double ent;

  /*
   * Open the output file for writing.
   */

  fid=fopen(fname,"w");

  if (fid == (FILE *) NULL)
    {
      printf("Failed to open output file for writing solution. \n");
      exit(11);
    };

  /*
   * Print out y (x0 in SDPA notation)
   */
  for (i=1; i<=k; i++)
    fprintf(fid,"%.18e ",y[i]);

  fprintf(fid,"\n");

  /*
   * Next, print out Z (X in SDPA notation)
   */

  for (blk=1; blk<=Z.nblocks; blk++)
    {
      switch (Z.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=Z.blocks[blk].blocksize; i++)
	    {
	      ent=Z.blocks[blk].data.vec[i];
	      if (ent != 0.0)
		fprintf(fid,"1 %d %d %d %.18e \n",blk,i,i,ent);
	    };
	  break;
	case MATRIX:
	  for (i=1; i<=Z.blocks[blk].blocksize; i++)
	    for (j=i; j<=Z.blocks[blk].blocksize; j++)
	      {
		ent=Z.blocks[blk].data.mat[ijtok(i,j,Z.blocks[blk].blocksize)];
		if (ent != 0.0)
		  fprintf(fid,"1 %d %d %d %.18e \n",blk,i,j,ent);
	      };	  
	  break;
	default:
	  printf("Illegal block type!\n");
	  exit(12);
	};
    };


  /*
   *  Next, print out X (Y in SDPA notation)
   */

  for (blk=1; blk<=X.nblocks; blk++)
    {
      switch (X.blocks[blk].blockcategory)
	{
	case DIAG:
	  for (i=1; i<=X.blocks[blk].blocksize; i++)
	    {
	      ent=X.blocks[blk].data.vec[i];
	      if (ent != 0.0)
		fprintf(fid,"2 %d %d %d %.18e \n",blk,i,i,ent);
	    };
	  break;
	case MATRIX:
	  for (i=1; i<=X.blocks[blk].blocksize; i++)
	    for (j=i; j<=X.blocks[blk].blocksize; j++)
	      {
		ent=X.blocks[blk].data.mat[ijtok(i,j,X.blocks[blk].blocksize)];
		if (ent != 0.0)
		  fprintf(fid,"2 %d %d %d %.18e \n",blk,i,j,ent);
	      };	  
	  break;
	case PACKEDMATRIX:
	default:
	  printf("writesol Invalid Block Type!\n");
	  exit(12);
	};
    };

  /*
   *  Close up and quit.
   */ 

  fclose(fid);
  return(0);
}








