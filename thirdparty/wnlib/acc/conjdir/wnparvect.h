/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnparvectH
#define wnparvectH

typedef void *(*wn_parallel_func)(void*);
typedef void **wn_parallel_args;
typedef void (*wn_parallel_callback)(wn_parallel_func, wn_parallel_args,
					char *debug_name);
typedef void (*wn_parallel_vect_operation_func)(int, int, int, void *);
typedef struct wn_parvect_context_struct *wn_parvect_context;

EXTERN
wn_parvect_context wn_parvect_init(wn_parallel_callback par_cb, 
				   int num_threads, int num_vars);
EXTERN void wn_parvect_free(wn_parvect_context context);

EXTERN
void wn_parallelize_vector_operation(wn_parvect_context context,
	  wn_parallel_vect_operation_func operation_func, 
	  void *user_data, int len, char *operation_name);

EXTERN
void wn_add_vect_and_scaled_vect_par(wn_parvect_context context,
				     double *out, double *v1, double *v2,
				     double s2, int len);
EXTERN
void wn_add_scaled_vects_par(wn_parvect_context context,
			     double *out, double *v1, double s1, 
			     double *v2, double s2, int len);
EXTERN
void wn_add_3_scaled_vects_par(wn_parvect_context context,
			       double *out, double *v1, double s1,
			       double *v2, double s2,
			       double *v3, double s3, int len);
EXTERN
double wn_norm2_vect_par(wn_parvect_context context,
			 double *v1, int len);
EXTERN
double wn_dot_vects_par(wn_parvect_context context,
			double *v1, double *v2, int len);
EXTERN
void wn_multiply_vect_by_vect_par(wn_parvect_context context,
				  double *v1, double *v2, int len);
EXTERN
void wn_divide_vect_by_vect_par(wn_parvect_context context,
				double *v1, double *v2, int len);
EXTERN
void wn_zero_vect_par(wn_parvect_context context, double *v1, int len);
EXTERN
void wn_copy_vect_par(wn_parvect_context context,
		      double *v1, double *v2, int len);
EXTERN
double wn_scaled_max_diff_vect_par(wn_parvect_context context, 
				   double *v1, double *v2, double *s, int len);
EXTERN
void wn_make_vect_par(wn_parvect_context context, double **v1, int len);

EXTERN
void wn_add_scaled_vects_and_dot_and_norm_par(wn_parvect_context context,
                             double *out,
                             double *pnorm2_out, double *pdot_v1_out,
			     double *v1, double s1, 
			     double *v2, double s2, int len);

#endif
