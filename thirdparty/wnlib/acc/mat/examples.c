/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/*
  Self-test for wn_svd (singular value decomposition method)
  Last modified: July 25, 2003 by bogdan@synopsys.com
*/


#include <stdlib.h>
#include <stdio.h>

#include "wnabs.h"
#include "wnasrt.h"
#include "wnlib.h"
#include "wnmat.h"
#include "wnmax.h"
#include "wnmem.h"
#include "wnrnd.h"
#include "wnrndd.h"
#include "wnvect.h"


#define MACHEPS        2.2e-16
#define DISPLAY        FALSE          /* turns on matrix display
					 (intended for low dimensions only) */


local double wn_l1norm_mat(double **a, int len_i, int len_j)
{
  int i, j;
  double ret;

  ret = 0.0;
  for(i = 0; i < len_i; ++i)
  {
    for(j = 0; j < len_j; ++j)
    {
      ret += wn_abs(a[i][j]);
    }
  }
  return(ret);
}


local double wn_supnorm_mat(double **a, int len_i, int len_j)
{
  int i, j;
  double ret, tmp;

  ret = 0.0;
  for(i = 0; i < len_i; ++i)
  {
    tmp = 0.0;
    for(j = 0; j < len_j; ++j)
    {
      tmp += wn_abs(a[i][j]);
    }
    ret = wn_max(ret, tmp);
  }
  return(ret);
}


int main(int argc, char *argv[])
{
  int m, n, limit, i, j, seed;
  double norm1_error;
  double *d;
  double **a, **b, **u, **v, **utr, **vtr, **im, **in, **diag, **a_tmp;

  if(argc != 4)
  {
    fprintf(stderr,"Usage: %s dim1 dim2 seed\n", argv[0]);
    exit(-1);
  }
  m = atoi(argv[1]);
  n = atoi(argv[2]);
  seed = atoi(argv[3]);
  wn_seed_random_number_generator(seed);

  /* Calculate number of singular values */
  limit = wn_min(m, n);

  wn_gpmake("no_free");
  wn_make_mat(&a, m, n);
  wn_make_mat(&b, m, n);
  wn_make_mat(&u, m, m);
  wn_make_mat(&v, n, n);
  wn_make_vect(&d, limit);

  /* Generate matrix */
  for(i = 0; i < m; ++i)
  {
    for(j = 0; j < n; ++j)
    {
      b[i][j] = wn_normal_distribution();
    }
  }

  /* Copy matrix to wn_svd input (which will be destroyed)*/
  wn_copy_mat(a, b, m, n);

#if DISPLAY
  printf("\n");
  printf("Input:\n");
  printf("A =\n");
  wn_print_mat(a, m, n);
#endif

  wn_svd(u, d, v, a, m, n);

#if DISPLAY
  printf("\n");
  printf("Solution:\n");
  /*
  printf("U =\n");
  wn_print_mat(u, m, m);
  printf("V =\n");
  wn_print_mat(v, n, n);
  */
  printf("Singular values = ");
  wn_print_vect(d, limit);
#endif

#if DISPLAY
  printf("\n");
  printf("Verification:\n");
#endif
  wn_make_mat(&utr, m, m);
  wn_make_mat(&diag, m, n);
  wn_zero_mat(diag, m, n);
  for(i = 0; i < limit; ++i)
  {
    diag[i][i] = d[i];
  }
  /*
  printf("Diagonal matrix of singular values =\n");
  wn_print_mat(diag, m, m);
  */
  wn_transpose_mat(utr, u, m, m);
  /*
  printf("Multiplying with\n");
  wn_print_mat(utr, m, m);
  */
  wn_mult_mats(a, utr, diag, m, m, n);
  /*
  printf("Multiplication result =\n");
  wn_print_mat(a, m, n);
  printf("Multiplying with\n");
  wn_print_mat(v, n, n);
  */
  wn_mult_mats(diag, a, v, m, n, n);

#if DISPLAY
  printf("A as obtained by SVD =\n");
  wn_print_mat(diag, m, n);
#endif

  for(i = 0; i < m; ++i)
  {
    for(j = 0; j < n; ++j)
    {
      diag[i][j] -= b[i][j];
    }
  }

#if DISPLAY
  printf("Error =\n");
  wn_print_mat(diag, m, n);
  printf("Sup_norm of error = %lg\n", norm1_error);
#endif

  /* Check l1-norm of the error */
  norm1_error = wn_l1norm_mat(diag, m, n);
  if(norm1_error >= MACHEPS * 
     wn_l1norm_mat(v, n, n)*wn_supnorm_mat(u, m, m)*wn_l1norm_mat(b, m, n))
  {
    printf("WARNING: seed = %d, SVD reconstruction error = %lg\n",
	   seed, norm1_error);
  }

  /* Check orthogonality of U and V */
  wn_make_mat(&vtr, n, n);
  wn_make_mat(&in, n, n);
  wn_make_mat(&im, m, m);
  wn_transpose_mat(vtr, v, n, n);

  wn_mult_mats(im, utr, u, m, m, m);
  wn_mult_mats(in, vtr, v, n, n, n);

  for(i = 0; i < m; ++i)
  {
    im[i][i] -= 1.0;
  }
  norm1_error = wn_l1norm_mat(im, m, m);

#if DISPLAY
  printf("U^T * U - I =\n");
  wn_print_mat(im, m, m);
  printf("l1-norm of U^T * U - I = %lg.\n", norm1_error);
#endif

  if(norm1_error >= 5.0 * MACHEPS *
     wn_supnorm_mat(u, m, m) * wn_l1norm_mat(u, m, m))
  {
    printf("WARNING: seed = %d, SVD U-orthogonality error = %lg\n",
	   seed, norm1_error);
    printf("         bound = 5 * %lg * %lg * %lg = %lg.\n",
	   MACHEPS, wn_supnorm_mat(u, m, m), wn_l1norm_mat(u, m, m),
	   5.0 * MACHEPS * wn_supnorm_mat(u, m, m) * wn_l1norm_mat(u, m, m));
  }

  for(j = 0; j < n; ++j)
  {
    in[j][j] -= 1.0;
  }
  norm1_error = wn_l1norm_mat(in, n, n);

#if DISPLAY
  printf("V^T * V - I =\n");
  wn_print_mat(in, n, n);
  printf("l1-norm of V^T * V - I = %lg.\n", norm1_error);
#endif

  if(norm1_error >= 5.0 * MACHEPS *
     wn_supnorm_mat(v, n, n) * wn_l1norm_mat(v, n, n))
  {
    printf("WARNING: seed = %d, SVD V-orthogonality error = %lg\n",
	   seed, norm1_error);
    printf("         bound = 5 * %lg * %lg * %lg = %lg.\n",
	   MACHEPS, wn_supnorm_mat(v, n, n), wn_l1norm_mat(v, n, n),
	   5.0 * MACHEPS * wn_supnorm_mat(v, n, n) * wn_l1norm_mat(v, n, n));
  }

  for(i = 0; i < limit; ++i)
  {
    if(
       (d[i] < 0.0)
       ||
       ( (i < limit - 1) && (d[i + 1] > d[i]) )
      )
    {
      break;
    }
  }
  if(i < limit)
  {
    printf("WARNING: seed = %d, SVD sorting error.\n");
  }

  wn_gpfree();
  return(0);
}
