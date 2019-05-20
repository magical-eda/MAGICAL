/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/**********************************************************************

**********************************************************************/

#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"

#include "wnanl.h"



#define BEST_ACCEPTANCE_RATE (0.455)     /* best for parabolic objective */
#define USUAL_ATTACK_RATE    (0.05)



void wn_adjust_anneal_window_full
(
  double *pwindow_size,
  double min_window_size,
  double max_window_size,
  double attack_rate,
  double target_acceptance_rate,
  anneal_acceptance_type anneal_acceptance
)
{
  wn_assert(*pwindow_size >= 0.0);
  wn_assert(0.0 < attack_rate);

  wn_assert(0.0 < target_acceptance_rate);
  wn_assert(target_acceptance_rate < 1.0);

  switch(anneal_acceptance)
  {
    default:
      wn_assert_notreached();
      break;
    case(WN_ANNEAL_ACCEPT):
      (*pwindow_size) *= exp(attack_rate*(1.0-target_acceptance_rate));
      if(*pwindow_size > max_window_size)
      {
	*pwindow_size = max_window_size;
      }
      break;
    case(WN_ANNEAL_REJECT):
      (*pwindow_size) *= exp(-attack_rate*target_acceptance_rate);
      if(*pwindow_size < min_window_size)
      {
	*pwindow_size = min_window_size;
      }
      break;
  }
}


void wn_adjust_anneal_window
(
  double *pwindow_size,
  double min_window_size,
  double max_window_size,
  anneal_acceptance_type anneal_acceptance
)
{
  wn_adjust_anneal_window_full(pwindow_size,
                               min_window_size,max_window_size,
                               USUAL_ATTACK_RATE,BEST_ACCEPTANCE_RATE,
                               anneal_acceptance);
}


