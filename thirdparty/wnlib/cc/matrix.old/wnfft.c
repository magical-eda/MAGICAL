/**********************************************************************

            FAST FOURIER TRANSFORM

wn_fft_raw_vector(vector,len_i)
wn_inverse_fft_raw_vector(vector,len_i)

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

#include "wnlib.h"
#include "wncplx.h"
#include "wncmat.h"



local wn_cplx *roots_of_unity,*in_copy;



wn_fft_raw_vector(vector,len_i)

wn_cplx vector[];
int len_i;

{
  conjagate_vector(vector,len_i);
  wn_inverse_fft_raw_vector(vector,len_i);
  conjagate_vector(vector,len_i);
}



local conjagate_vector(vector,len_i)

register wn_cplx *vector;
int len_i;

{
  register wn_cplx number,*fin;

  fin = vector+len_i;

  for(;vector != fin;++vector)
  {
    number = *vector;

    number->imag = -number->imag;
  }
}



wn_inverse_fft_raw_vector(vector,len_i)

wn_cplx vector[];
int len_i;

{
  wn_gpmake("no_free");

  initialize(len_i);

  fft(vector,len_i,1);

  normalize_result(vector,len_i);

  wn_gpfree();
}



local initialize(len_i)

int len_i;

{
  int half_len_i,i;

  wn_assertmsg(len_i >= 0,
	       "wn_fft_raw_vector: number of points must be > 0.");
  wn_assertmsg((len_i == 0) || wn_is_iexp2(len_i),
	       "wn_fft_raw_vector: number of points must be power of 2.");

  half_len_i = len_i/2;

  wn_cplx_make_raw_vector(&roots_of_unity,half_len_i);
  in_copy = (wn_cplx *)wn_zalloc(len_i*wn_sizeof(wn_cplx));

  /* compute roots of unity */
  for(i=0;i<half_len_i;++i)
  {
    wn_polar_to_cplx(roots_of_unity[i],
		     1.0,2.0*WN_PI*((double)i)/((double)len_i));
  }
}



local normalize_result(vector,len_i)

register wn_cplx *vector;
int len_i;

{
  extern double sqrt();
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



local fft(vector,len_i,exponent_increment)

wn_cplx vector[];
int len_i,exponent_increment;

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



local split_vector_into_halves(vector,half_len_i,len_i)

register wn_cplx *vector;
register int half_len_i;
int len_i;

{
  register wn_cplx *vector2,*reg_in_copy;

  wn_blkcpy((ptr)in_copy,(ptr)vector,
	    len_i*wn_sizeof(wn_cplx));

  reg_in_copy = in_copy;
  vector2 = vector + half_len_i;

  *vector = *reg_in_copy;
  *vector2 = *(++reg_in_copy);

  switch(half_len_i)
  {
    case(0):
      wn_crash();

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



local merge_halves(vector,half_len_i,exponent_increment)

register wn_cplx *vector;
register int half_len_i,exponent_increment;

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



