/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor, Bill Chapman

****************************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnargp.h"
#include "wnasrt.h"
#include "wnabs.h"

#include "wnrnd.h"
#include "wnrndd.h"
#include "wnvect.h"
#include "wnmat.h"

#define LEN   100	/* was 4000, but thought that was too big */
#define LO_ALMOST_EQUAL(a, b) (wn_abs((a)-(b)) < 0.00001)


local void mat_test_invert(void)
{
  double **mat,**mat2,**mat3;
  int code;
  int i, j;

  wn_gpmake("general_free");

  wn_make_mat(&mat, LEN,LEN);
  wn_make_mat(&mat2,LEN,LEN);
  wn_make_mat(&mat3,LEN,LEN);

  wn_random_mat(mat,LEN,LEN);

  wn_copy_mat(mat2,mat,LEN,LEN);

  wn_invert_mat(&code,mat2,LEN);
  wn_assert(code == WN_SUCCESS);

  wn_mult_mats(mat3,mat,mat2,LEN,LEN,LEN);

  /* mat3 should now be the identity matrix */
  for (i = 0;  i < LEN;  ++i)
  {
    for (j = 0;  j < LEN;  ++j)
    {
      wn_assert(LO_ALMOST_EQUAL(mat3[i][j], i == j ? 1.0 : 0.0));
  } }

  wn_mult_mats(mat3,mat2,mat,LEN,LEN,LEN);

  /* mat3 should now be the identity matrix */
  for (i = 0;  i < LEN;  ++i)
  {
    for (j = 0;  j < LEN;  ++j)
    {
      wn_assert(LO_ALMOST_EQUAL(mat3[i][j], i == j ? 1.0 : 0.0));
  } }

  wn_free_mat(mat, LEN,LEN);
  wn_free_mat(mat2,LEN,LEN);
  wn_free_mat(mat3,LEN,LEN);

  wn_gpfree();
} /* mat_test_invert */


local double a_row0[] = { 2, 3 };
local double a_row1[] = { 5, 7 };
local double *a_mat[] = { a_row0, a_row1 };

local double b_row0[] = { 4, 2 };
local double b_row1[] = { 3, 2 };
local double *b_mat[] = { b_row0, b_row1 };

local double c_row0[] = { 17, 10 };
local double c_row1[] = { 41, 24 };
local double *c_mat[] = { c_row0, c_row1 };

local double a_vec[] = { 4, 3 };
local double c_vec[] = { 17, 41 };

local void mat_test_2sies()
{
  static double t_row0[2], t_row1[2];
  static double *t_mat[] = { t_row0, t_row1 };
  double t_vec[2];
  double **a_inv_mat;
  int code;
  int i, j;

  wn_gpmake("no_free");

  wn_mult_mats(t_mat, a_mat, b_mat, 2, 2, 2);

  /* t should now EXACTLY equal c */
  for (i = 0;  i < 2;  ++i)
  {
    for (j = 0;  j < 2;  ++j)
    {
      wn_assert(t_mat[i][j] == c_mat[i][j]);
  } }

  wn_mult_mat_by_vect(t_vec, a_mat, a_vec, 2, 2);

  for (i = 0;  i < 2;  ++i)
  {
    wn_assert(t_vec[i] == c_vec[i]);
  }

  wn_make_mat(&a_inv_mat, 2, 2);
  wn_copy_mat(a_inv_mat, a_mat, 2, 2);
  wn_invert_mat(&code,a_inv_mat, 2);
  wn_assert(WN_SUCCESS == code);

  t_vec[0] = t_vec[1] = -1;
  wn_solve_system(&code, t_vec, c_vec, a_inv_mat, a_mat, 2, 5);

  /* t_vec should now == a_vec, don't know if it's exact tho */
  for (i = 0;  i < 2;  ++i)
  {
    wn_assert(LO_ALMOST_EQUAL(t_vec[i], a_vec[i]));
  }

  wn_gpfree();
} /* mat_test_2sies */


local void mat_test_gramm_schmidt(void)
{
  double **mat, **mat_orig;
  int ilen, jlen, code;		/* ilen rows, jlen columns */
  int a, b;
  double *sum_vect, mul, mul2;
  int trial;

  for (trial = 0;  trial < 1000;  ++trial) {
    wn_gpmake("no_free");    /* temp memory group */

      jlen = (unsigned) wn_random_int() % 20    +  1;
      ilen = (unsigned) wn_random_int() % jlen  +  1;

      wn_make_mat(&mat_orig, ilen, jlen);
      wn_random_mat(mat_orig, ilen, jlen);

      wn_make_mat(&mat, ilen, jlen);
      for (a = 0;  a < ilen;  ++a) {
	for (b = 0;  b < jlen;  ++b) {
	  mat[a][b] = mat_orig[a][b];
	}
      }

      wn_gramm_schmidt(&code, mat, ilen, jlen);
      wn_assert(WN_SUCCESS == code);

      /* first ilen vects should be orthogonal */
      for (a = 0;  a < ilen;  ++a) {
	for (b = a+1;  b < ilen;  ++b) {
	  wn_assert(LO_ALMOST_EQUAL(0.0, wn_dot_vects(mat[a], mat[b], jlen)));
	}
      }

      /* let's make the gramm_schmidt be unit vectors, to be simple */
      for (a = 0;  a < ilen;  ++a) {
	mul2 = wn_dot_vects(mat[a], mat[a], jlen);
	mul = 1/sqrt(mul2);
	for (b = 0;  b < jlen;  ++b) {
	  mat[a][b] *= mul;
	}
	wn_assert(LO_ALMOST_EQUAL(1.0, wn_dot_vects(mat[a], mat[a], jlen)));
      }

      /* check again orthogonality */
      for (a = 0;  a < ilen;  ++a) {
	for (b = a+1;  b < ilen;  ++b) {
	  wn_assert(LO_ALMOST_EQUAL(0.0, wn_dot_vects(mat[a], mat[b], jlen)));
	}
      }

      /* make a vector in the space spanned by the original vectors */
      wn_make_vect(&sum_vect, jlen);
      wn_zero_vect(sum_vect, jlen);
      for (a = 0;  a < ilen;  ++a) {
	wn_add_scaled_vect(sum_vect, mat_orig[a], wn_flat_distribution(),
	/**/							jlen);
      }

      /* OK, sum_vect is reached from the original matrix of vectors */
      /* can we reach it from mat? */
      for (a = 0;  a < ilen;  ++a) {
	mul = wn_dot_vects(sum_vect, mat[a], jlen);
	wn_add_scaled_vect(sum_vect, mat[a], -mul, jlen);
      }
      wn_assert(LO_ALMOST_EQUAL(0.0, wn_dot_vects(sum_vect, sum_vect, jlen)));

    wn_gpfree();
  } /* for trial */
} /* mat_test_gramm_schmidt */


/***************************************************************
**	Big simplex problem.  Don't know how to evaluate what's
**  a reasonable outcome, just running code copied from the
**  examples without doing any output to see if it blows up. */

local void mat_test_big_simplex()
{
  double val,objective,*solution_vect,*objective_vect,*right_side,**mat;
  int vars,eqs;
  int len_i,len_j,code,i,j;

  wn_gpmake("no_free");    /* temp memory group */

  vars = LEN;
  eqs = LEN;

  len_i = eqs;
  len_j = vars+eqs;

  wn_make_mat(&mat,len_i,len_j);

  wn_make_vect(&solution_vect,len_j);
  wn_make_vect(&objective_vect,len_j);
  wn_make_vect(&right_side,len_i);

  for(j=0;j<len_j;++j)
  {
    val = wn_normal_distribution();
    /*
    if(val < 0.0)
    {
      val = -val;
    }
    */

    objective_vect[j] = val; 
    /*
    objective_vect[j] = 1.0;
    */
  }

  for(i=0;i<len_i;++i)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    right_side[i] = val; 
  }

  for(i=0;i<len_i;++i)
  {
    wn_zero_vect(mat[i],len_j);

    for(j=0;j<vars;++j)
    {
      val = wn_normal_distribution();
      if(val < 0.0)
      {
	val = -val;
      }

      mat[i][j] = val;
    }

    mat[i][vars+i] = 1;
  }

  wn_simplex_method(&code,&objective,NULL,solution_vect,
		    objective_vect,mat,right_side,
		    len_i,len_j);
  wn_assert(WN_SUCCESS == code);

  wn_gpfree();
} /* mat_test_big_simplex */


/* again, just copied from the examples and muffled */

local void mat_test_big_simplex_loop(void)
{
  /* double objective; */
  double val,*solution_vect,*objective_vect,
	 *right_side,*right_side_control,**mat;
  /* int vars,eqs; */
  int len_i,len_j,code,i,j;
  int *zero_vars,*non_zero_vars;

  wn_gpmake("no_free");    /* temp memory group */

  len_j = LEN;
  len_i = 3*len_j;

  wn_make_mat(&mat,len_i,len_j);

  wn_make_vect(&solution_vect,len_j);
  wn_make_vect(&objective_vect,len_j);
  wn_make_vect(&right_side,len_i);
  wn_make_vect(&right_side_control,len_i);

  zero_vars = (int *)wn_zalloc(len_j*sizeof(int));
  non_zero_vars = (int *)wn_zalloc(len_i*sizeof(int));

  for(j=0;j<len_j;++j)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    objective_vect[j] = val; 
  }

  for(i=0;i<len_i;++i)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    right_side[i] = val; 
    right_side_control[i] = val;
  }

  for(i=0;i<len_i;++i)
  for(j=0;j<len_j;++j)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    mat[i][j] = val;
  }

  /*
  printf("mat =\n");
  wn_print_mat(mat,len_i,len_j);
  printf("rhs =\n");
  wn_print_vect(right_side,len_i);
  printf("ob =\n");
  wn_print_vect(objective_vect,len_j);
  */

  wn_simplex_loop(&code,mat,right_side,right_side_control,
		  non_zero_vars,zero_vars,
		  len_i,len_j);
  wn_assert(WN_SUCCESS == code);

  /*
  printf("code = %d\n",code);
  */
  /*
  wn_print_vect(solution_vect,len_j);
  */

  wn_gpfree();
} /* mat_test_big_simplex_loop */


/****************************************************************
**	small simplex.  I never studied the simplex method in any
**  detail.  This example is copied out of a textbook.  This
**  example intends to pose the following problem:

    maximize	  s[0] + 9*s[1] +   s[2]

such that

		  s[0] + 2*s[1] + 3*s[2] <=  9
		3*s[0] + 2*s[1] + 2*s[2] <= 15

and s[0], s[1], s[2] are all non-negative.  s[] is described below
as solution[]. -- Bill  Chapman.  The wnsplx routines require
that constraints be equalities.  We change the constraints to
equalities by adding slack variables, leading to the following
formulation:

    maximize	  s[0] + 9*s[1] +   s[2]

such that

		  s[0] + 2*s[1] + 3*s[2] + s[3] ==  9
		3*s[0] + 2*s[1] + 2*s[2] + s[4] == 15

*/
void mat_test_small_simplex()
{
  static double ss_row0[] = { 1, 2, 3, 1, 0 };
  static double ss_row1[] = { 3, 2, 2, 0, 1 };
  static double *ss_mat[] = { ss_row0, ss_row1 };
  int code;
  static double ss_rhs[] = { 9, 15 };

  static double ss_objective_vect[] = { 1, 9, 1, 0, 0 };

  double solution[5];
  double objective;

  wn_simplex_method(&code, &objective, NULL, solution, ss_objective_vect,
  /**/		ss_mat, ss_rhs, 2, 5);
  wn_assert(WN_SUCCESS == code);

  /* at this point, it returns the feasible but suboptimal solution
  ** { 3, 3, 0 }.  Textbook said { 0, 4.5, 0 }, which yields a higher
  ** objective.  Don't know what I'm doing wrong. */
  /*
  printf("small simplex solution: %f %f %f\n", solution[0], solution[1],
  							    solution[2]);
  */

  wn_assert(LO_ALMOST_EQUAL(0,   solution[0]));
  wn_assert(LO_ALMOST_EQUAL(4.5, solution[1]));
  wn_assert(LO_ALMOST_EQUAL(0,   solution[2]));
} /* mat_test_small_simplex */


/****************************************************************
**	small simplex 2.  This is another simplex example from
**  another textbook.

    maximize	  s[0] + 2*s[1]

such that

		  s[0] - 2*s[1] <=   2
		2*s[0] -   s[1] <=   7
		4*s[0] +   s[1] <=  29
	       -2*s[0] + 3*s[1] <=  17
	       -3*s[0] + 2*s[1] <=   8

and s[0], s[1] are both non-negative.  s[] is described below
as solution[]. -- Bill  Chapman.  The wnsplx routines require
that constraints be equalities.  We change the constraints to
equalities by adding slack variables, leading to the following
formulation:

    maximize	  s[0] + 2*s[1]

such that

		  s[0] - 2*s[1] + s[2] ==   2
		2*s[0] -   s[1] + s[3] ==   7
		4*s[0] +   s[1] + s[4] ==  29
	       -2*s[0] + 3*s[1] + s[5] ==  17
	       -3*s[0] + 2*s[1] + s[6] ==   8

 */

void mat_test_small_simplex_2()
{
  static double ss_row0[] = {  1, -2, 1, 0, 0, 0, 0 };
  static double ss_row1[] = {  2, -1, 0, 1, 0, 0, 0 };
  static double ss_row2[] = {  4,  1, 0, 0, 1, 0, 0 };
  static double ss_row3[] = { -2,  3, 0, 0, 0, 1, 0 };
  static double ss_row4[] = { -3,  2, 0, 0, 0, 0, 1 };
  static double *ss_mat[] = { ss_row0, ss_row1, ss_row2, ss_row3, ss_row4 };

  static double ss_rhs[] = { 2, 7, 29, 17, 8 };

  static double ss_objective_vect[] = { 1, 2, 0, 0, 0, 0, 0 };

  double solution[7];
  double objective;
  int code;

  wn_simplex_method(&code, &objective, NULL, solution, ss_objective_vect,
  /**/		ss_mat, ss_rhs, 5, 7);

  /* at this point, the simplex has told us the problem is infeasible,
  ** when the solution { 5, 9 } given in the textbook clearly satisfies
  ** all the inequalities, as does { 0, 0 } */

  /*
  printf("small simplex 2 return code: %d\n", code);
  */
  wn_assert(WN_SUCCESS == code);

  wn_assert(LO_ALMOST_EQUAL(5, solution[0]));
  wn_assert(LO_ALMOST_EQUAL(9, solution[1]));
} /* mat_test_small_simplex_2 */


int main(int argc,char *argv[])
{
  static bool small_simplex;
  static bool small_simplex2;
  /*
  static wn_arg_format arg_format_array[] =
  {
    WN_KEY_BOOL(&small_simplex,  "s"),
    WN_KEY_BOOL(&small_simplex2, "t"),
    WN_ARG_TERMINATE
  };

  wn_parse_args(argc,argv,arg_format_array);
  */
  small_simplex = TRUE;
  small_simplex2 = TRUE;

#if 0
  fprintf(stderr,
  /**/  "testing matrix stuff... (for small simplex: -s for 1, -t for 2)\n");
#endif
  fprintf(stderr,
          "testing matrix stuff...\n");

  mat_test_invert();
  mat_test_2sies();
  mat_test_gramm_schmidt();
  mat_test_big_simplex();
  mat_test_big_simplex_loop();
  if (small_simplex)
  {
    mat_test_small_simplex();
  }
  if (small_simplex2)
  {
    mat_test_small_simplex_2();
  }

  fprintf(stderr,"  ok!!!!!!\n");

  return(0);
} /* main */
