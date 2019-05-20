/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/


#include "wnlib.h"
#include "wnmem.h"
#include "wnrndd.h"
#include "wnvect.h"
#include "wnmat.h"

#include "wnconj.h"


#define  SIZE    20


local int total_count,count;

local double s;

local double **cost_mat,**solution_mat,**grad_mat;

local double *solution_vect,*grad_vect;


local void xlate_to_mat(double **mat,double *vect)
{
  int i,j,count;

  count = 0;

  for(i=0;i<SIZE;++i)
  for(j=0;j<SIZE;++j)
  {
    mat[i][j] = vect[count];

    ++count;
  }
}


local void xlate_to_vect(double *vect,double **mat)
{
  int i,j,count;

  count = 0;

  for(i=0;i<SIZE;++i)
  for(j=0;j<SIZE;++j)
  {
    vect[count] = mat[i][j];

    ++count;
  }
}


local void initialize(void)
{
  int i,j;

  wn_gpmake("no_free"); 

  wn_make_vect(&solution_vect,SIZE*SIZE);
  wn_make_vect(&grad_vect,SIZE*SIZE);

  wn_make_mat(&solution_mat,SIZE,SIZE);
  wn_make_mat(&grad_mat,SIZE,SIZE);
  wn_make_mat(&cost_mat,SIZE,SIZE);

  for(i=0;i<SIZE;++i)
  for(j=0;j<SIZE;++j)
  {
    cost_mat[i][j] = wn_flat_distribution();
  }
}


local double function(double v[])
{
  double ret,sum,diff;
  int i,j;

  xlate_to_mat(solution_mat,v);

  ret = 0.0;

  for(i=0;i<SIZE;++i)
  for(j=0;j<SIZE;++j)
  {
    ret += wn_penalty(s*solution_mat[i][j]);
    ret += wn_penalty(s*(1.0-solution_mat[i][j]));
    ret += cost_mat[i][j]*solution_mat[i][j];
  }

  for(i=0;i<SIZE;++i)
  {
    sum = 0.0;

    for(j=0;j<SIZE;++j)
    {
      sum += solution_mat[i][j];
    }

    diff = sum - 1.0;

    ret += s*diff*diff;
  }

  for(j=0;j<SIZE;++j)
  {
    sum = 0.0;

    for(i=0;i<SIZE;++i)
    {
      sum += solution_mat[i][j];
    }

    diff = sum - 1.0;

    ret += s*diff*diff;
  }

  /*
  wn_print_mat(solution_mat,SIZE,SIZE);
  */
  printf("ob = %lf\n",ret);

  return(ret);
}


local void gradient(double grad[],double v[])
{
  double sum,diff;
  int i,j;

  printf("gradient, total_count=%d,count=%d.\n",total_count,count);
  /*
  */

  ++count;
  ++total_count;

  xlate_to_mat(solution_mat,v);

  for(i=0;i<SIZE;++i)
  for(j=0;j<SIZE;++j)
  {
    grad_mat[i][j] = s*wn_dpenalty(s*solution_mat[i][j]) + 
		     -s*wn_dpenalty(s*(1.0-solution_mat[i][j])) +
		     cost_mat[i][j];
  }

  for(i=0;i<SIZE;++i)
  {
    sum = 0.0;

    for(j=0;j<SIZE;++j)
    {
      sum += solution_mat[i][j];
    }

    diff = sum - 1.0;

    for(j=0;j<SIZE;++j)
    {
      grad_mat[i][j] += 2.0*s*diff;
    }
  }

  for(j=0;j<SIZE;++j)
  {
    sum = 0.0;

    for(i=0;i<SIZE;++i)
    {
      sum += solution_mat[i][j];
    }

    diff = sum - 1.0;

    for(i=0;i<SIZE;++i)
    {
      grad_mat[i][j] += 2.0*s*diff;
    }
  }

  xlate_to_vect(grad,grad_mat);
}


void main(void)
{
  int code,iterations;
  double val_min;

  initialize();

  total_count = 0;

  s = 64.0;
  /*
  for(;;)
  for(s=1.0;;s *= 2)
  */
  {
  /*
  printf("enter s iterations: ");
  scanf("%lf %d",&s,&iterations);
  iterations = WN_IHUGE;
  iterations = SIZE;
  */
  iterations = WN_IHUGE;
  printf("s = %lf\n",s);

  count = 0;

  wn_conj_gradient_method(&code,&val_min,
		          solution_vect,SIZE*SIZE,(function),(gradient),
			  iterations);

  printf("final result: code = %d   ",code);
  printf("    ob = %lf\n",val_min);
  printf("total_count=%d,count=%d.\n",total_count,count);
  /*
  xlate_to_mat(solution_mat,solution_vect);
  wn_print_mat(solution_mat,SIZE,SIZE);
  */
  }

  wn_gpfree();
}
