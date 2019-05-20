/*
 * Free the memory associated with a problem.
 */

#include <stdlib.h>
#include "declarations.h"

void free_prob(n,k,C,a,constraints,X,y,Z)
     int n;
     int k;
     struct blockmatrix C;
     double *a;
     struct constraintmatrix *constraints;
     struct blockmatrix X;
     double *y;
     struct blockmatrix Z;
{
  int i;
  struct sparseblock *ptr;
  struct sparseblock *oldptr;

  /*
   * First, free the vectors of doubles.
   */

  free(y);
  free(a);

  /*
   * Now, the block matrices.
   */

  free_mat(C);
  free_mat(X);
  free_mat(Z);

  /*
   * Finally, get rid of the constraints.
   */

  if (constraints != NULL)
    {
      for (i=1; i<=k; i++)
	{
	  /*
	   * Get rid of constraint i.
	   */
	  
	  ptr=constraints[i].blocks;
	  while (ptr != NULL)
	    {
	      free(ptr->entries);
	      free(ptr->iindices);
	      free(ptr->jindices);
	      oldptr=ptr;
	      ptr=ptr->next;
	      free(oldptr);
	    };
	};
      /*
       * Finally, free the constraints array.
       */

      free(constraints);
    };

}

