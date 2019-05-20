/*
  Read in a solution in SDPA sparse format.  
*/

#include <stdio.h>
#include <stdlib.h>
#include "declarations.h"

void skip_to_end_of_line();

int read_sol(fname,n,k,C,pX,py,pZ)
     char *fname;
     int n;
     int k;
     struct blockmatrix C;
     struct blockmatrix *pX;
     double **py;
     struct blockmatrix *pZ;
{
  FILE *fid;
  int i;
  int indexi;
  int indexj;
  int blkno;
  int matno;
  double ent;
  int ret;

  /*
   * Allocate storage.
   */

  alloc_mat(C,pX);
  alloc_mat(C,pZ);
  *py=(double *)malloc(sizeof(double)*(k+1));
  if (*py == NULL)
    {
      printf("Storage allocation failed!\n");
      exit(10);
    };

  /*
   *  Open the file for reading.
   */ 

  fid=fopen(fname,"r");
 
  if (fid == (FILE *) NULL)
    {
      printf("Couldn't open solution file for reading. \n");
      exit(11);
    };


  /*
   *  Read in y.
   */

  for (i=1; i<=k; i++)
    {
      ret=fscanf(fid,"%le",&((*py)[i]));
      if (ret != 1)
	{
	  printf("Reading solution failed, while reading y. ret=%d\n",ret);
	  return(1);
	};
    };


  skip_to_end_of_line(fid);

  /*
   *  Initialize X and Z to 0.
   */

  zero_mat(*pX);
  zero_mat(*pZ);

  /*
   *  Read in the rest of the data.
   */

  do {
    ret=fscanf(fid,"%d %d %d %d %le",&matno,&blkno,&indexi,&indexj,&ent);

    if ((ret != 5) && (ret != EOF))
      {
	printf("Bad line in solution file: %d %d %d %d %e\n",
	       matno,blkno,indexi,indexj,ent);
	fclose(fid);
	return(1);
      };

    if (matno == 1)
      {
	switch (pZ->blocks[blkno].blockcategory)
	  {
	  case DIAG:
	    pZ->blocks[blkno].data.vec[indexi]=ent;
	    break;
	  case MATRIX:
	    pZ->blocks[blkno].data.mat[ijtok(indexi,indexj,pZ->blocks[blkno].blocksize)]=ent;
	    pZ->blocks[blkno].data.mat[ijtok(indexj,indexi,pZ->blocks[blkno].blocksize)]=ent;
	    break;
	  default:
	    printf("Illegal block type! \n");
	    exit(12);
	  };
      }
    else
      {
	switch (pX->blocks[blkno].blockcategory)
	  {
	  case DIAG:
	    pX->blocks[blkno].data.vec[indexi]=ent;
	    break;
	  case MATRIX:
	    pX->blocks[blkno].data.mat[ijtok(indexi,indexj,pX->blocks[blkno].blocksize)]=ent;
	    pX->blocks[blkno].data.mat[ijtok(indexj,indexi,pX->blocks[blkno].blocksize)]=ent;
	    break;
	  default:
	    printf("Illegal block type! \n");
	    exit(12);
	  };
      };
  } while (ret != EOF);

  fclose(fid);
  return(0);
}





