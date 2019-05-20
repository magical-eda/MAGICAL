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
#ifndef wnmatH
#define wnmatH


EXTERN void wn_print_mat(double **mat,int len_i,int len_j);
EXTERN void wn_enter_mat(double **mat,int len_i,int len_j);

EXTERN void wn_random_mat(double **mat,int len_i,int len_j);
EXTERN void wn_zero_mat(double **mat,int len_i,int len_j);
EXTERN void wn_identity_mat(double **mat,int len);
EXTERN void wn_hilbert_mat(double **mat,int len);

EXTERN void wn_make_mat(double ***pmat,int len_i,int len_j);
EXTERN void wn_free_mat(double **mat,int len_i,int len_j);

EXTERN void wn_copy_mat(double **to_mat,double **from_mat,int len_i,int len_j);
EXTERN void wn_transpose_mat(double **to_mat,double **from_mat,int len_i,int len_j);

EXTERN
void wn_mult_mat_by_vect(double *result_vect,double **mat,double *vect,
			 int len_i,int len_j);
EXTERN
void wn_mult_mats(double **result_mat,double **mat1,double **mat2,
		  int len_i,int len_j,int len_k);

EXTERN void wn_gramm_schmidt(int *pcode,double **mat,int len_i,int len_j);
EXTERN void wn_invert_mat(int *pcode,double **mat,int len_i);
EXTERN void wn_polish_inverse_mat
(
  int *pcode,
  double **mat_inverse,
  double **mat,
  int len,
  int iterations
);
EXTERN void wn_solve_system
(
  int *pcode,
  double *solution,
  double *rhs,
  double **mat_inverse,
  double **mat,
  int len,
  int iterations
);

EXTERN void wn_simplex_loop
(
  int *pcode,
  double **mat,                /* mat[len_i][len_j] */
  double *right_side,          /* right_side[len_i] */
  double *right_side_control,  /*right_side_control[len_i] */
  int *non_zero_vars,          /* non_zero_vars[len_i] */
  int *zero_vars,              /* zero_vars[len_j]  */
  int len_i,
  int len_j
);
EXTERN void wn_simplex_method
(
  int *pcode,
  double *pobjective,
  double *raw_shadow_prices,
  double *raw_solution,
  double *raw_objective_vect,
  double **raw_mat,
  double *raw_right_side,
  int len_i,
  int len_j
);

EXTERN void wn_svd
(
  double **image_basis,  /* image_basis[len_i][len_i] */
  double *d,             /* d[min(len_i, len_j)] */
  double **arg_basis,    /* arg_basis[len_j][len_j] */
  double **in_mat,       /* in_mat[len_i][len_j] */
  int len_i,             /* dimension of matrix **a */
  int len_j  
);


#endif

