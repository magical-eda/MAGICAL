/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#ifndef wnanlH
#define wnanlH



#include "wnlib.h"



typedef enum {WN_ANNEAL_ACCEPT=1,WN_ANNEAL_REJECT=2} anneal_acceptance_type;



extern int wn_anneal_epochs_to_run,wn_anneal_epochs_remaining;
extern double wn_anneal_temperature,
              wn_anneal_objective,
              wn_anneal_accept_rate;  


EXTERN void wn_anneal_std_checkpoint_print(void);

EXTERN void wn_get_anneal_statistics
(
  double *pmean,
  double *psdev,
  double (*pevaluate_random_mutation)(void),
  int iterations
);

EXTERN void wn_measure_anneal_temperature
(
  double *ptemperature,
  double (*pevaluate_random_mutation)(void),
  int iterations
);


EXTERN void wn_anneal_linear_temperature
(
  double (*pevaluate_random_mutation)(void),
  void (*paccept_mutation)(void),
  void (*preject_mutation)(void),
  void (*pcheckpoint)(void),
  int problem_size,
  int stop_run_length,
  int epochs_to_run,
  double start_temperature,
  double fin_temperature
);

EXTERN void wn_anneal_with_sched
(
  double (*pevaluate_random_mutation)(void),
  void (*paccept_mutation)(void),
  void (*preject_mutation)(void),
  void (*pcheckpoint)(void),
  int problem_size,
  int stop_run_length,
  int epochs_to_run,
  double (*ptemperature_function)(double relative_time),
  double start_temperature
);

EXTERN void wn_anneal_from_temperature
(
  double (*pevaluate_random_mutation)(void),
  void (*paccept_mutation)(void),
  void (*preject_mutation)(void),
  void (*pcheckpoint)(void),
  int problem_size,
  int stop_run_length,
  int epochs_to_run,
  double start_temperature
);

EXTERN void wn_anneal
(
  double (*pevaluate_random_mutation)(void),
  void (*paccept_mutation)(void),
  void (*preject_mutation)(void),
  void (*pcheckpoint)(void),
  int problem_size,
  int stop_run_length,
  int epochs_to_run
);

EXTERN void wn_adjust_anneal_window_full
(
  double *pwindow_size,
  double min_window_size,
  double max_window_size,
  double attack_rate,
  double best_acceptance_rate,
  anneal_acceptance_type anneal_acceptance
);

EXTERN void wn_adjust_anneal_window
(
  double *pwindow_size,
  double min_window_size,
  double max_window_size,
  anneal_acceptance_type anneal_acceptance
);


#endif


