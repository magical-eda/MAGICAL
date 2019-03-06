/**********************************************************************

make_assignment_problem(pright_side,pmat,costs)

**********************************************************************/
/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor

****************************************************************************/

#include "wnlib.h"
#include "wnmat.h"



extern double random_double_between();


make_assignment_problem(pobjective_vect,pright_side,pmat,costs)

wn_vector *pobjective_vect,*pright_side;
wn_matrix *pmat,costs;

{
  int size,len_i,len_j;
  int i,j;

  wn_assert(costs->len_i == costs->len_j);

  size = costs->len_i;

  len_i = 2*size-1;
  /*
  len_i = 2*size;
  */
  len_j = size*size;

  wn_make_vector(pobjective_vect,len_j);
  wn_make_vector(pright_side,len_i);
  wn_make_matrix(pmat,len_i,len_j);
  
  for(i=0;i<len_i;i++)
  {  
    ((*pright_side)->numbers)[i] = 1.0;
  }
    
  for(j=0;j<len_j;j++)
  {
    ((*pobjective_vect)->numbers)[j] = -(costs->numbers)[j/size][j%size];
  }

  for(i=0;i<size;i++)
  {
    for(j=0;j<len_j;j++)
    {
      ((*pmat)->numbers)[i][j] = (double)(int)(j/size == i);
    }
  }
  for(i=size;i<len_i;i++)
  {
    for(j=0;j<len_j;j++)
    {
      ((*pmat)->numbers)[i][j] = (double)(int)(j%size == (i-size));
    }
  }
}


