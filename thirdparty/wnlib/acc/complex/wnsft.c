/**********************************************************************

            SLOW FOURIER TRANSFORM

wn_sft_vect(vector,len_i)
wn_inverse_sft_vect(vector,len_i)

**********************************************************************/
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

#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"

#include "wncplx.h"
#include "wnfft.h"



local wn_cplx *roots_of_unity,*in_copy;




void wn_inverse_sft_vect(wn_cplx vector[],int len_i)
{
  int i,j,exponent;
  struct wn_cplx_struct prod_struct;
  wn_cplx sum,prod;
  double norm_factor;

  wn_gpmake("no_free");

  wn_cplx_make_vect(&roots_of_unity,len_i);
  wn_cplx_make_vect(&in_copy,len_i);

  /* compute roots of unity */
  for(i=0;i<len_i;++i)
  {
    wn_polar_to_cplx(roots_of_unity[i],
		     1.0,2.0*WN_PI*((double)i)/((double)len_i));
  }

  wn_cplx_copy_vect(in_copy,vector,len_i);

  norm_factor = 1.0/sqrt((double)len_i);
  prod = &prod_struct;

  for(i=0;i<len_i;i++)
  {
    sum = vector[i];

    sum->real = 0.0;
    sum->imag = 0.0;

    exponent = 0;

    for(j=0;j<len_i;++j)
    {   /* this is the inner loop */
      wn_cplx_multiply(prod,in_copy[j],roots_of_unity[exponent]);

      sum->real += prod->real;
      sum->imag += prod->imag;

      exponent +=i;
      if(exponent >= len_i)
      {
	exponent -= len_i;
      }
    }

    sum->real *= norm_factor;
    sum->imag *= norm_factor;
  }

  wn_gpfree();
}


void wn_sft_vect(wn_cplx vector[],int len_i)
{
  wn_cplx_conjugate_vect(vector,len_i);

  wn_inverse_sft_vect(vector,len_i);

  wn_cplx_conjugate_vect(vector,len_i);
}



