/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnswap.h"

#include "wnconj.h"



local double **m;
local double *b;
local double **vs;
local double **gs;
local int num_pts;
local int num_dim;

local double *vect;
local int vect_size;


local void symmetrize_m(void)
{
  int i,j;
  double val;

  for(i=0;i<num_dim;++i)
  for(j=0;j<i;++j)
  {
    val = 0.5*(m[i][j] + m[j][i]);

    m[i][j] = m[j][i] = val;
  }
}


local void ij_to_index(int *pindex,int i,int j)
{
  if(i > j)
  {
    wn_swap(i,j,int);
  }

  *pindex = (j*(j+1))/2;
  (*pindex) += i;
}


local void initialize(void)
{
  int i,j;
  int index;

  symmetrize_m();

  vect_size = num_dim + (num_dim*(num_dim+1))/2;

  printf("vect_size=%d\n",vect_size);

  wn_make_vect(&vect,vect_size);

  for(i=0;i<num_dim;++i)
  {
    vect[i] = b[i];
  }

  for(i=0;i<num_dim;++i)
  for(j=0;j<=i;++j)
  {
    ij_to_index(&index,i,j);

    vect[num_dim + index] = m[i][j];
  }
}


local void finish(void)
{
  int i,j;
  int index;

  for(i=0;i<num_dim;++i)
  {
    b[i] = vect[i];
  }

  for(i=0;i<num_dim;++i)
  for(j=0;j<=i;++j)
  {
    ij_to_index(&index,i,j);

    m[i][j] = m[j][i] = vect[num_dim + index];
  }
}


local double funcgrad(double grad[],double x[])
{
  int i,j;
  int pt_index;
  int index;
  double *v,*g;
  double f,m_val,d_m_val,b_val,d_b_val,diff;
  double g_val_predict;
  int count;

  f = 0.0;
  wn_zero_vect(grad,vect_size);
  count = 0;

  for(pt_index=0;pt_index<num_pts;++pt_index)
  {
    v = vs[pt_index];
    g = gs[pt_index];

    for(i=0;i<num_dim;++i)
    {
      if(count >= 25)
      {
	goto quit;
      }
      ++count;

      b_val = x[i];

      g_val_predict = b_val;

      for(j=0;j<num_dim;++j)
      {
        ij_to_index(&index,i,j);

        m_val = x[num_dim + index];

	g_val_predict += m_val*v[j];
      }

      diff = g_val_predict - g[i];
      
      f += diff*diff;

      for(j=0;j<num_dim;++j)
      {
        ij_to_index(&index,i,j);

	/*
        m_val = m[num_dim + index];

	g_val_predict += m_val*v[j];
	*/

	d_m_val = 2.0*v[j]*diff;

	grad[num_dim + index] += d_m_val;
      }

      d_b_val = 2.0*diff;

      grad[i] += d_b_val;
    }
  }

quit:

  /*
  printf("f=%lg\n",f);
  */

  return(f);
}


void wn_fit_quadratic_to_grads
(
  double *perror,
  double **passed_m,
  double *passed_b,
  double **passed_vs,
  double **passed_gs,
  int passed_num_pts,
  int passed_num_dim,
  int num_iter
)
{
  int code;

  wn_gpmake("no_free");

  m = passed_m;
  b = passed_b;
  vs = passed_vs;
  gs = passed_gs;
  num_pts = passed_num_pts;
  num_dim = passed_num_dim;

  initialize();

  wn_conj_funcgrad_method(&code,perror,vect,vect_size,&funcgrad,
			  (bool (*)(double *,double))NULL,
			  1.0, WNCONJ_DISTANCE, 0, 2.0, num_iter);

  finish();

  wn_gpfree();
}


