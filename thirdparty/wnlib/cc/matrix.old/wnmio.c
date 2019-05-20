/**********************************************************************

wn_enter_vector(&vect)
wn_enter_raw_vector(raw_vect,len_i)

wn_enter_matrix(pmat)
wn_enter_raw_matrix(raw_mat,len_i,len_j)


wn_print_vector(vect)
wn_print_raw_vector(raw_vect,len_i)

wn_print_matrix(mat)
wn_print_raw_matrix(raw_mat,len_i,len_j);

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



wn_enter_vector(pvect)

wn_vector *pvect;

{
  int len_i;

  printf("enter len: ");
  scanf("%d",&len_i);

  wn_make_vector(pvect,len_i);

  wn_enter_raw_vector((*pvect)->numbers,len_i);
}



wn_enter_raw_vector(raw_vect,len_i)

double raw_vect[];

{
  int i;

  for(i=0;i<len_i;i++)
  {
    printf("enter element[%d]: ",i);
    read_double(&(raw_vect[i]));
  }
}



wn_enter_matrix(pmat)

wn_matrix *pmat;

{
  int len_i,len_j;

  printf("enter len_i: ");
  scanf("%d",&len_i);
  printf("enter len_j: ");
  scanf("%d",&len_j);

  wn_make_matrix(pmat,len_i,len_j);

  wn_enter_raw_matrix((*pmat)->numbers,len_i,len_j);
}



wn_enter_raw_matrix(raw_mat,len_i,len_j)

double **raw_mat;
int len_i,len_j;

{
  int i,j;

  for(i=0;i<len_i;i++)  for(j=0;j<len_j;j++)
  {
    printf("enter element[%d][%d]: ",i,j);
    read_double(&(raw_mat[i][j]));
  }
}



local read_double(pd)

double *pd;

{
  float f;

  scanf("%f",&f);

  *pd = f;
}



wn_print_vector(vect)

wn_vector vect;

{
  wn_print_raw_vector(vect->numbers,vect->len_i);
}



wn_print_raw_vector(raw_vect,len_i)

double raw_vect[];
int len_i;

{
  int i;

  printf("[ ");

  for(i=0;i<len_i;i++)
  {
    printf("%f ",raw_vect[i]);
  }

  printf("]\n");
}



wn_print_matrix(mat)

wn_matrix mat;

{
  wn_print_raw_matrix(mat->numbers,mat->len_i,mat->len_j);
}



wn_print_raw_matrix(raw_mat,len_i,len_j)

double **raw_mat;
int len_i,len_j;

{
  int i;

  for(i=0;i<len_i;i++)
  {
    wn_print_raw_vector(raw_mat[i],len_j);
  }
}

