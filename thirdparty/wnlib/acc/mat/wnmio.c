/**********************************************************************

wn_enter_mat(mat,len_i,len_j)
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
#include <stdio.h>

#include "wnlib.h"
#include "wnvect.h"

#include "wnmat.h"



void wn_enter_mat(double **mat,int len_i,int len_j)
{
  int i,j;

  for(i=0;i<len_i;++i)  
  for(j=0;j<len_j;++j)
  {
    printf("enter element[%d][%d]: ",i,j);
    scanf("%lf",&(mat[i][j]));
  }
}


void wn_print_mat(double **mat,int len_i,int len_j)
{
  int i;

  printf("-----------\n");

  for(i=0;i<len_i;i++)
  {
    wn_print_vect(mat[i],len_j);
  }

  printf("-----------\n");
}

