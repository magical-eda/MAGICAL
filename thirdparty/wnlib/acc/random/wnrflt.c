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

#include "wnlib.h"

#include "wnrnd.h"
#include "wnrndd.h"



#define RANGE_BITS  30
#define RANGE       (1<<RANGE_BITS)


double wn_flat_distribution_slow(void)
{
  double ret;

  ret = (double)wn_random_n_bits(RANGE_BITS);

  ret += 0.5;

  ret = ret/((double)RANGE);

  return(ret);
}

/* faster version -- rossd */

#define INV_RANDOM_INT_RANGE        (1.0 / 4294967296.0)

double wn_flat_distribution(void)
{
  return (0.5 + (unsigned)wn_random_int()) * INV_RANDOM_INT_RANGE;
}
