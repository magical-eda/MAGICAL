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

#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnrndd.h"
#include "wnspmat.h"
#include "wnsprnd.h"



#define NORMALIZE  (1.0/(2.0*(1.0 - 1.0/WN_E)))



void wn_random_sparse_matrix
(
  wn_sparse_matrix *pmat,
  int len_i,
  int len_j,
  double low,
  double high,
  double density
)
{
  int i,j;
  double value,dist;

  wn_assert((0.0<=density)&&(density<=1.0));

  i = j = 0;

  wn_make_sparse_matrix(pmat,len_i,len_j);

  if(density == 0.0)
  {
    return;
  }

  for(;;)
  {
    if(density == 1.0)
    {
      dist = 0.0;
    }
    else
    {
      dist = wn_exponential_distribution()*(1.0/density - 1)*NORMALIZE;
    }
   
    j += (int)dist;

    while(j >= len_j)
    {
      j -= len_j;
      ++i;

      if(i >= len_i)
      {
        wn_sort_sparse_matrix(*pmat);

        return;
      }
    }

    value = wn_random_double_between(low,high);

    wn_insert_sparse_matrix_value(*pmat,value,i,j);

    ++j;
  }
}


void wn_random_symmetric_sparse_matrix
(
  wn_sparse_matrix *pmat,
  int len_i,
  double low,
  double high,
  double density
)
{
  int i,j;
  double value,dist;

  wn_assert((0.0<=density)&&(density<=1.0));

  i = j = 0;

  wn_make_sparse_matrix(pmat,len_i,len_i);

  if(density == 0.0)
  {
    return;
  }

  for(;;)
  {
    if(density == 1.0)
    {
      dist = 0.0;
    }
    else
    {
      dist = wn_exponential_distribution()*(1.0/density - 1)*NORMALIZE;
    }
   
    j += (int)dist;

    while(j >= i)
    {
      j -= i;
      i++;

      if(i >= len_i)
      {
        wn_sort_sparse_matrix(*pmat);

        return;
      }
    }

    value = wn_random_double_between(low,high);

    wn_assert(j<i);

    wn_insert_sparse_matrix_value(*pmat,value,i,j);
    wn_insert_sparse_matrix_value(*pmat,value,j,i);

    j++;
  }
}


