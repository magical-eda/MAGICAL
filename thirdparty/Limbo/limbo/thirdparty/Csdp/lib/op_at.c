/*
  Compute A'(y).
  */

#include <stdlib.h>
#include "declarations.h"


void op_at(k,y,constraints,result)
     int k;
     double *y;
     struct constraintmatrix *constraints;
     struct blockmatrix result;
{
  int i,j;
#ifndef BIT64
  int p,q;
#else
  long int p,q;
#endif
  int blk;
  double ent;
  struct sparseblock *ptr;

  zero_mat(result);

  for (i=1; i<=k; i++)
    {
      if (y[i] == 0.0)
	{
	  continue;
	};

      ptr=constraints[i].blocks;

      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  if (result.blocks[blk].blockcategory  == DIAG)
	    {
	      for (j=1; j<=ptr->numentries; j++)
		{
		  ent=ptr->entries[j];
		  p=ptr->iindices[j];
		  result.blocks[blk].data.vec[p] += y[i]*ent;
		};
	    }
	  else
	    {
	      for (j=1; j<=ptr->numentries; j++)
		{
		  ent=ptr->entries[j];
		  p=ijtok(ptr->iindices[j],ptr->jindices[j],ptr->blocksize);
		  q=ijtok(ptr->jindices[j],ptr->iindices[j],ptr->blocksize);

		  result.blocks[blk].data.mat[p] += y[i]*ent;
		  if (p != q)
		    result.blocks[blk].data.mat[q] += y[i]*ent;
		};

	    };
	  ptr=ptr->next;
	};
    };

}

