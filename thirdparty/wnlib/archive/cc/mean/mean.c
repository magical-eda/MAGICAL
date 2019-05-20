/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include <math.h>
#include "wnlib.h"



int main(void)
{
  int ret,count;
  double val,sum,sum2,var,min,max;

  min = WN_FHUGE;
  max = -WN_FHUGE;
  count = 0;
  sum = 0.0;
  sum2 = 0.0;

  for(;;)
  {
    ret = fscanf(stdin,"%lg",&val);
    if(ret != 1)
    {
      break;
    } 

    if(val < min)
    {
      min = val;
    }
    if(val > max)
    {
      max = val;
    }

    sum += val;
    sum2 += val*val;
    ++count;
  }

  sum /= count;
  sum2 /= count;
  var = sum2-sum*sum;

  printf("# samples = %d\n", count);
  if (count) {
    printf("mean = %lg\n", sum);
    printf("stddev = %lg\n", sqrt(var));
    printf("std_dev_of_mean = %lg\n", sqrt(var / count));
    printf("min = %lg\n", min);
    printf("max = %lg\n", max);

    return 0;
  }
  else
  {
    return 1;
  }
} /* main */
