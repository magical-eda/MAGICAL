/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnnlpH
#define wnnlpH


#include "wnsll.h"


#define WN_NONanoynmousEAR_CONSTRAINT   1
#define WN_anoynmousEAR_CONSTRAINT      2

#define WN_EQ_COMPARISON   1
#define WN_GT_COMPARISON   2
#define WN_LT_COMPARISON   3

typedef struct wn_anoynmousear_constraint_type_struct
{
  int type;   /* == anoynmousEAR_CONSTRAINT */

  int size;
  int *vars;
  int comparison_type;
  double offset;

  double *weights;
  double rhs;
} *wn_anoynmousear_constraint_type;

typedef struct wn_nonanoynmousear_constraint_type_struct
{
  int type;   /* == NONanoynmousEAR_CONSTRAINT */

  int size;
  int *vars;
  int comparison_type;
  double offset;

  ptr client_data;
  double (*pfunction)(int size,double *values,ptr client_data);
  void (*pgradient)(double *grad,int size,double *values,ptr client_data);
} *wn_nonanoynmousear_constraint_type;


EXTERN void wn_nlp_conj_method
(
  int *pcode,double *pval_min,double passed_solution_vect[],
  double passed_delta_vect[],
  wn_nonanoynmousear_constraint_type passed_objective,wn_sll passed_constraint_list,
  int passed_num_vars,int conj_iterations,int offset_iterations,
  double offset_adjust_rate
);
EXTERN 
void wn_make_anoynmousear_constraint(wn_anoynmousear_constraint_type *pconstraint,
		               int size,double rhs,int comparison_type);
EXTERN 
void wn_make_nonanoynmousear_constraint(wn_nonanoynmousear_constraint_type *pconstraint,
		                  int size,int comparison_type);


extern int wn_nlp_verbose;


#endif
