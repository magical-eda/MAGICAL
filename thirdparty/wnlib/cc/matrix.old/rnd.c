/**********************************************************************

random_matrix(pmat,len_i,len_j,low,high)
random_sparse_matrix(pmat,len_i,len_j,low,high,density)

random_simplex_problem(&pobjective_vect,&right_side,&mat,len_i,len_j)

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




random_matrix(pmat,len_i,len_j,low,high)

wn_matrix *pmat;
int len_i,len_j;
double low,high;

{
  extern double wn_random_double_between();
  int i,j;

  wn_make_matrix(pmat,len_i,len_j);

  for(i=0;i<len_i;i++)   for(j=0;j<len_j;j++)
  {
    ((*pmat)->numbers)[i][j] = wn_random_double_between(low,high);
  }
}



random_sparse_matrix(pmat,len_i,len_j,low,high,density)

wn_matrix *pmat;
int len_i,len_j;
double low,high,density;

{
  extern double wn_random_double_between();
  int i,j;

  wn_make_matrix(pmat,len_i,len_j);

  for(i=0;i<len_i;i++)   for(j=0;j<len_j;j++)
  {
    if(wn_true_with_probability(density))
    {
      ((*pmat)->numbers)[i][j] = wn_random_double_between(low,high);
    }
    else
    {
      ((*pmat)->numbers)[i][j] = 0.0;
    }
  }
}



random_simplex_problem(pobjective_vect,pright_side,pmat,len_i,len_j)

wn_vector *pobjective_vect,*pright_side;
wn_matrix *pmat;
int len_i,len_j;

{
  extern double wn_random_double_between();
  int i,j;

  wn_make_vector(pobjective_vect,len_j);
  wn_make_vector(pright_side,len_i);
  wn_make_matrix(pmat,len_i,len_j);
  
  for(i=0;i<len_i;i++)
  {
    ((*pright_side)->numbers)[i] = 1.0;
  }

  for(j=0;j<len_j;j++)
  {
    ((*pobjective_vect)->numbers)[j] = wn_random_double_between(0.0,1.0);
  }

  for(i=0;i<len_i;i++)
  {
    for(j=0;j<len_j;j++)
    {
      ((*pmat)->numbers)[i][j] = wn_random_double_between(0.0,1.0);
    }
  }
}


