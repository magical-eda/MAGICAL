/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this file is provided free of charge
  to the author's consulting clients.  It is in the
  public domain and therefore may be used by anybody for
  any purpose.

AUTHOR:

  Will Naylor

****************************************************************************/

#include "wnlib.h"

#include "wnrnd.h"
#include "wnrndd.h"



#define RANGE_BITS  30
#define RANGE       (1<<RANGE_BITS)


double wn_flat_distribution(void)
{
  double ret;

  ret = (double)wn_random_n_bits(RANGE_BITS);

  ret += 0.5;

  ret = ret/((double)RANGE);

  return(ret);
}

