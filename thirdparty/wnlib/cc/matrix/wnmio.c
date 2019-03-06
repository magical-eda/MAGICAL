/**********************************************************************

wn_enter_vect(vect,len_i)
wn_enter_mat(mat,len_i,len_j)

wn_print_vect(vect,len_i)
wn_print_mat(mat,len_i,len_j)

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



local void read_double(pd)

double *pd;

{
  double f;

  scanf("%lf",&f);

  *pd = f;
}


void wn_enter_vect(vect,len_i)

double vect[];
int len_i;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    printf("enter element[%d]: ",i);
    read_double(&(vect[i]));
  }
}


void wn_enter_mat(mat,len_i,len_j)

double **mat;
int len_i,len_j;

{
  int i,j;

  for(i=0;i<len_i;++i)  
  for(j=0;j<len_j;++j)
  {
    printf("enter element[%d][%d]: ",i,j);
    read_double(&(mat[i][j]));
  }
}


void wn_print_vect(vect,len_i)

double vect[];
int len_i;

{
  int i;

  printf("[ ");

  for(i=0;i<len_i;i++)
  {
    printf("%f ",vect[i]);
  }

  printf("]\n");
}


void wn_print_mat(mat,len_i,len_j)

double **mat;
int len_i,len_j;

{
  int i;

  printf("-----------\n");

  for(i=0;i<len_i;i++)
  {
    wn_print_vect(mat[i],len_j);
  }

  printf("-----------\n");
}

