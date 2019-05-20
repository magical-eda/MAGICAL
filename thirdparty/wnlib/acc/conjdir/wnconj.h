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
#ifndef wnconjH
#define wnconjH


#include "wnlib.h"
#include "wncons.h"
#include "wnparvect.h"


#include "wnconjd.h"

typedef enum {WN_CONJ_ILLEGAL = 0,
	      WN_CONJ_DISTANCE, 
	      WN_CONJ_CURVATURE, 
	      WN_CONJ_MUL_FACTOR,
              WN_CONJ_NUM_ENTRIES} wn_jump_len_type;

EXTERN double wn_penalty(double x);
EXTERN double wn_dpenalty(double x);
EXTERN double wn_d2penalty(double x);

EXTERN double wn_barrier(double x);
EXTERN double wn_dbarrier(double x);

EXTERN double wn_barrier2(double x);
EXTERN double wn_dbarrier2(double x);

EXTERN double wn_clamp(double x);
EXTERN double wn_dclamp(double x);

EXTERN void wn_conj_gradient_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  int len,
  double (*pfunction)(double vect[]),
  void (*pgradient)(double grad[],double vect[]),
  int max_iterations
);
EXTERN void wn_conj_gradient_diff_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  double delta_vect[],
  int len,
  double (*pfunction)(double vect[]),
  int max_iterations
);
EXTERN void wn_conj_funcgrad_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  int len,
  double (*pfuncgrad)(double grad[],double vect[]),
  bool (*pterminate)(double vect[],double best_f,int iteration),
  double first_jump_len,
  wn_jump_len_type jump_len_flag,
  int no_reset_run_len,
  double reset_expand_factor,
  int max_iterations
);
EXTERN void wn_conj_funcgrad_method_parallel
(
  int *pcode,
  double *pval_min,
  double vect[],
  int num_vars,
  double (*pfuncgrad)(double grad[],double vect[]),
  bool (*pterminate)(double vect[],double best_f,int iteration),
  double first_step_len,
  wn_jump_len_type jump_len_flag,
  int no_reset_run_len,
  double reset_expand_factor,
  int max_iterations,
  wn_parallel_callback par_cb,
  int num_threads
);
EXTERN void wn_lbfgs_funcgrad_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  int num_vars,
  double (*pfuncgrad)(double grad[],double vect[]),
  bool (*pterminate)(double vect[],double best_f),
  double first_jump_len,
  int max_depth, 
  int max_iterations
);

EXTERN void wn_force_conj_gradient_stop(void);

EXTERN void wn_numerical_gradient
(
  double grad[],
  double vect[],
  double delta_vect[],
  int len,
  double (*pfunction)(double vect[])
);


EXTERN void wn_fit_parabola_3p
(
  int *pcode,
  double *pa,double *px0,double *pb,
  double x1,double y1,
  double x2,double y2,
  double x3,double y3
);
EXTERN void wn_fit_parabola_2pd
(
  int *pcode,
  double *pa,double *px0,double *pb,
  double x1,double y1,double dy1,
  double x2,double y2
);
EXTERN void wn_fit_parabola_2d
(
  int *pcode,
  double *pa,double *px0,
  double x1,double dy1,
  double x2,double dy2
);
EXTERN void wn_fit_traditional_parabola_3p
(
  int *pcode,
  double *pa,double *pb, double *pc,
  double x1,double y1,
  double x2,double y2,
  double x3,double y3
);
EXTERN void wn_fit_traditional_parabola_2pa
(
  int *pcode,
  double *pb, double *pc,
  double a,
  double x1,double y1,
  double x2,double y2
);
EXTERN void wn_convert_parabola_traditional_to_centered
(
  int *pcode,
  double *px0,double *pb,
  double a,
  double b,
  double c
);
EXTERN void wn_fit_cubic_2p2d
(
  int *pcode,
  double coef[4],
  double x1,double y1,double dy1,
  double x2,double y2,double dy2
);

EXTERN double wn_clip_f(double);

EXTERN void wn_minimize_1d
(
  int *pcode,
  double *pval_min,
  double *px_min,
  double (*pfunction)(double x),
  int max_iterations
);
EXTERN void wn_minimize_1d_raw
(
  int *pcode,
  double *pf0,
  double *pf1,
  double *pf2,
  double *px0,
  double *px1,
  double *px2,
  double f_goal,
  double (*pfunction)(double x),
  int max_improve_iterations,
  int max_total_iterations
);


#endif

