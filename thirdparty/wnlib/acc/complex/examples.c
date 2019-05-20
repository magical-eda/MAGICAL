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

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnfft.h"



#define SIZE  (1<<15)


local void print_vect(wn_cplx vect[],int size)
{
  int i;

  printf("[");

  for(i=0;i<size;++i)
  {
    printf(" ");
    wn_cplx_print(vect[i]);
  }

  printf("]\n");
}


int main(void)
{
  wn_cplx *vect;

  wn_cplx_make_vect(&vect,SIZE);

  vect[0]->real = 1.0;
  vect[1]->real = 1.0;
  vect[2]->real = 1.0;

  /*
  print_vect(vect,SIZE);
  */
  print_vect(vect,20);

  wn_fft_vect(vect,SIZE);

  print_vect(vect,20);
  /*
  */

  wn_inverse_fft_vect(vect,SIZE);

  print_vect(vect,20);
  /*
  */

  return(0);
}


