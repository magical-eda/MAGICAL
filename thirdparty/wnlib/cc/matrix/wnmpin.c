/**********************************************************************

wn_polish_inverse_mat(&code,mat_inverse,mat,len_i,iterations)
wn_solve_system(&code,solution,rhs,mat_inverse,mat,len,iterations)

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
#include "wnabs.h"
#include "wnmat.h"



#define THRESH  0.001



local void test_identity(pcode,ident,len)

int *pcode;
double **ident;
int len;

{
  int i,j;
  double diff;

  for(i=0;i<len;++i)
  for(j=0;j<len;++j)
  {
    if(i == j)
    {
      diff = ident[i][j]-1.0;
    }
    else
    {
      diff = ident[i][j];
    }

    diff = wn_abs(diff);

    if(not(diff <= THRESH))  /* must express this way to catch NaN */
    {
      *pcode = WN_SINGULAR;
      return;
    }
  }

  *pcode = WN_SUCCESS;
}


void wn_polish_inverse_mat(pcode,mat_inverse,mat,len,iterations)

int *pcode;
double **mat_inverse,**mat;
int len,iterations;

{
  double **tmp1,**tmp2;
  int i;

  wn_gpmake("no_free");
  wn_gplabel("polish invert matrix group");

  wn_make_mat(&tmp1,len,len);
  wn_make_mat(&tmp2,len,len);

  for(i=0;i<iterations;++i)
  {
    wn_mult_mats(tmp1,mat_inverse,mat,len,len,len);
    wn_mult_mats(tmp2,tmp1,mat_inverse,len,len,len);
    wn_mult_mat_by_scalar(tmp2,-1.0,len,len);
    wn_mult_mat_by_scalar(mat_inverse,2.0,len,len);
    wn_add_mat(mat_inverse,tmp2,len,len);
  }

  wn_mult_mats(tmp1,mat_inverse,mat,len,len,len);
  test_identity(pcode,tmp1,len);

  wn_gpfree();
}


local void test_solution(pcode,rhs_mult,rhs,len)

int *pcode;
double *rhs_mult,*rhs;
int len;

{
  int i;
  double diff;

  for(i=0;i<len;++i)
  {
    diff = rhs_mult[i]-rhs[i];
    diff = wn_abs(diff);

    if(not(diff <= THRESH))  /* must express this way to catch NaN */
    {
      *pcode = WN_SINGULAR;
      return;
    }
  }

  *pcode = WN_SUCCESS;
}


void wn_solve_system(pcode,solution,rhs,mat_inverse,mat,len,iterations)

int *pcode;
double *solution,*rhs,
       **mat_inverse,**mat;
int len,iterations;

{
  double *tmp,*tmp2;
  int i,j;

  wn_gpmake("no_free");
  wn_gplabel("solve system group");

  wn_make_vect(&tmp,len);
  wn_make_vect(&tmp2,len);

  wn_mult_mat_by_vect(solution,mat_inverse,rhs,len,len,len);

  for(i=0;i<iterations;++i)
  {
    wn_mult_mat_by_vect(tmp,mat,solution,len,len,len);

    for(j=0;j<len;++j)
    {
      tmp[j] -= rhs[j];
    }

    wn_mult_mat_by_vect(tmp2,mat_inverse,tmp,len,len,len);

    for(j=0;j<len;++j)
    {
      tmp2[j] = -tmp2[j];
    } 

    wn_add_vect(solution,tmp2,len);
  }

  wn_mult_mat_by_vect(tmp,mat,solution,len,len,len);
  test_solution(pcode,tmp,rhs,len);

  wn_gpfree();
}


