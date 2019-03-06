/**********************************************************************

            FAST FOURIER TRANSFORM

wn_fft_vect(vector,len_i)
wn_inverse_fft_vect(vector,len_i)

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
#include <string.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnmemb.h"
#include "wncplx.h"
#include "wnfft.h"



local wn_cplx *roots_of_unity,*in_copy;



local bool is_iexp2(int num)
{
  switch(num)
  {
    case(0):
      return(FALSE);

    case(1):
    case(1<<1):
    case(1<<2):
    case(1<<3):
    case(1<<4):
    case(1<<5):
    case(1<<6):
    case(1<<7):
    case(1<<8):
    case(1<<9):
    case(1<<10):
    case(1<<11):
    case(1<<12):
    case(1<<13):
    case(1<<14):
    case(1<<15):
    case(1<<16):
    case(1<<17):
    case(1<<18):
    case(1<<19):
    case(1<<20):
    case(1<<21):
    case(1<<22):
    case(1<<23):
    case(1<<24):
    case(1<<25):
    case(1<<26):
    case(1<<27):
    case(1<<28):
    case(1<<29):
    case(1<<30):
      return(TRUE);
  }

  return(FALSE);
}


local void initialize(int len_i)
{
  int half_len_i,i;

  wn_assert(len_i >= 0);
  wn_assert((len_i == 0) || is_iexp2(len_i));

  half_len_i = len_i/2;

  wn_cplx_make_vect(&roots_of_unity,half_len_i);
  in_copy = (wn_cplx *)wn_zalloc(len_i*sizeof(wn_cplx));

  /* compute roots of unity */
  for(i=0;i<half_len_i;++i)
  {
    wn_polar_to_cplx(roots_of_unity[i],
		     1.0,2.0*WN_PI*((double)i)/((double)len_i));
  }
}


local void normalize_result(register wn_cplx vector[],int len_i)
{
  register double norm_factor;
  register wn_cplx number,*fin;

  norm_factor = 1.0/sqrt((double)len_i);

  fin = vector+len_i;

  for(;vector != fin;++vector)
  {
    number = *vector;

    number->real *= norm_factor;
    number->imag *= norm_factor;
  }
}


local void split_vector_into_halves
(
  register wn_cplx *vector,
  register int half_len_i,
  int len_i
)
{
  register wn_cplx *vector2,*reg_in_copy;

  wn_memcpy((char *)in_copy,(char *)vector,
	    len_i*sizeof(wn_cplx));

  reg_in_copy = in_copy;
  vector2 = vector + half_len_i;

  *vector = *reg_in_copy;
  *vector2 = *(++reg_in_copy);

  switch(half_len_i)
  {
    case(0):
      wn_assert_notreached();

    default:
      for(;half_len_i > 8;half_len_i -= 8)
      {
        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);

        *(++vector) = *(++reg_in_copy);
        *(++vector2) = *(++reg_in_copy);
      }

    case(8):
      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);

      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);

      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);

      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);

    case(4):
      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);

      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);

    case(2):
      *(++vector) = *(++reg_in_copy);
      *(++vector2) = *(++reg_in_copy);
    case(1):
      ;
  }
}


local void merge_halves
(
  register wn_cplx *vector,
  register int half_len_i,
  register int exponent_increment
)
{
  struct wn_cplx_struct even_struct,odd_struct;
  register wn_cplx even,odd;
  register wn_cplx *vector2;
  register int i,exponent;

  even = &even_struct;
  odd = &odd_struct;

  vector2 = vector + half_len_i;

  exponent = 0;

  for(i=0;i<half_len_i;++i)
  {
    wn_cplx_copy(even,*vector);
    wn_cplx_multiply(odd,*vector2,roots_of_unity[exponent]);

    (*vector)->real = even->real + odd->real; 
    (*vector)->imag = even->imag + odd->imag; 

    (*vector2)->real = even->real - odd->real; 
    (*vector2)->imag = even->imag - odd->imag; 

    exponent += exponent_increment;

    ++vector;
    ++vector2;
  }
}


local void fft(wn_cplx vector[],int len_i,int exponent_increment)
{
  switch(len_i)
  {
    case(0):
    case(1):
      return;
    default:
    {
      int half_len_i,new_exponent_increment;

      half_len_i = len_i/2;
      new_exponent_increment = 2*exponent_increment;

      split_vector_into_halves(vector,half_len_i,len_i);

      fft(vector,half_len_i,new_exponent_increment);
      fft(vector+half_len_i,half_len_i,new_exponent_increment);

      merge_halves(vector,half_len_i,exponent_increment);
    } break;
  }
}


void wn_inverse_fft_vect(wn_cplx vector[],int len_i)
{
  wn_gpmake("no_free");

  initialize(len_i);

  fft(vector,len_i,1);

  normalize_result(vector,len_i);

  wn_gpfree();
}


void wn_fft_vect(wn_cplx vector[],int len_i)
{
  wn_cplx_conjugate_vect(vector,len_i);

  wn_inverse_fft_vect(vector,len_i);

  wn_cplx_conjugate_vect(vector,len_i);
}



