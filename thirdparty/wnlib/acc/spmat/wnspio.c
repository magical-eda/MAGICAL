/**********************************************************************

wn_enter_sparse_matrix(pmat)

wn_print_sparse_matrix(mat)
wn_print_symmetric_sparse_matrix(mat)

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

#include <stdlib.h>
#include <stdio.h>

#include "wnlib.h"
#include "wneq.h"
#include "wnspmat.h"
#include "wnspio.h"



local void get_entry(double *pvalue,int *pi,int *pj,bool *pfinished)
{
  char string[200];

  *pfinished = FALSE;

  printf("new value ***********\n");

  printf("enter i: ");
  scanf("%s",string);
  if(wn_streqnc(string,"quit"))
  {
    *pfinished = TRUE;
    return;
  }
  sscanf(string,"%d",pi);

  printf("enter j: ");
  scanf("%s",string);
  if(wn_streqnc(string,"quit"))
  {
    *pfinished = TRUE;
    return;
  }
  sscanf(string,"%d",pj);

  printf("enter value: ");
  scanf("%s",string);
  if(wn_streqnc(string,"quit"))
  {
    *pfinished = TRUE;
    return;
  }
  *pvalue = atof(string);
}


void wn_enter_sparse_matrix(wn_sparse_matrix *pmat)
{
  int len_i,len_j,i,j;
  bool finished;
  double value;

  printf("enter len_i: ");
  scanf("%d",&len_i);
  printf("enter len_j: ");
  scanf("%d",&len_j);

  wn_make_sparse_matrix(pmat,len_i,len_j);

  for(;;)
  {
    get_entry(&value,&i,&j,&finished);

    if(finished)
    {
      break;
    }

    wn_insert_sparse_matrix_value(*pmat,value,i,j);
  }

  wn_sort_sparse_matrix(*pmat);
}


void wn_print_sparse_matrix_entry(wn_sparse_matrix_entry entry)
{
  printf("entry[%d][%d] = %lg\n",entry->i,entry->j,entry->value);
}


void wn_print_sparse_matrix(wn_sparse_matrix mat)
{
  int i;
  wn_sll list,el;
  wn_sparse_matrix_entry entry;

  printf("---------------------------------\n");

  printf(" %d * %d sparse matrix\n",mat->len_i,mat->len_j);

  for(i=0;i<mat->len_i;i++)
  {
    list = (mat->i_lists)[i];

    for(el=list;el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      wn_print_sparse_matrix_entry(entry);
    }
  }

  printf("---------------------------------\n");
}


void wn_print_symmetric_sparse_matrix(wn_sparse_matrix mat)
{
  int i;
  wn_sll list,el;
  wn_sparse_matrix_entry entry;

  printf("---------------------------------\n");

  printf(" %d * %d symmetric sparse matrix\n",mat->len_i,mat->len_j);

  for(i=0;i<mat->len_i;i++)
  {
    list = (mat->i_lists)[i];

    for(el=list;el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      if(i <= entry->j)
      {
        printf("entry[%d][%d] = %f\n",entry->i,entry->j,(float)(entry->value));
      }
    }
  }

  printf("---------------------------------\n");
}




