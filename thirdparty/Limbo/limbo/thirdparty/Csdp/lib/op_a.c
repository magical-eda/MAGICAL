 /*
  Compute A(X).
  */

#include <stdlib.h>
#include "declarations.h"


void op_a(k,constraints,X,result)
     int k;
     struct constraintmatrix *constraints;
     struct blockmatrix X;
     double *result;
{
  int i,j;
  int p,q;
  int blk;
  double ent;
  double *mat;
  double *vec;
  int nume;
  struct sparseblock *ptr;
  double contrib;

#pragma omp parallel for schedule(dynamic,64) default(none) private(i,contrib,ptr,blk,nume,vec,j,p,mat,ent,q) shared(k,constraints,result,X)
  for (i=1; i<=k; i++)
    {
      result[i]=0.0;

      contrib=0.0;
      ptr=constraints[i].blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;
	  nume=ptr->numentries;

	  if (X.blocks[blk].blockcategory == DIAG)
	    {
	      vec=X.blocks[blk].data.vec;
	      for (j=1; j<=nume; j++)
		{
		  ent=ptr->entries[j];
		  p=ptr->iindices[j];
		  contrib += ent*vec[p];
		};
	    }
	  else
	    {
	      mat=X.blocks[blk].data.mat;
	      for (j=1; j<=nume; j++)
		{
		  ent=ptr->entries[j];
		  p=ijtok(ptr->iindices[j],ptr->jindices[j],ptr->blocksize);
		  q=ijtok(ptr->jindices[j],ptr->iindices[j],ptr->blocksize);

		  if (p == q)
		    {
		      contrib += ent*mat[p];
		    }
		  else
		    {
		      contrib += ent*(mat[p]+mat[q]);
		    };

		};

	    };


	  ptr=ptr->next;
	};

      result[i] += contrib;

    };
}



