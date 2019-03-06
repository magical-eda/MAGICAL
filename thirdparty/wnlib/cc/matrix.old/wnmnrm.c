/**********************************************************************

wn_norm_squared_vector(&result,vect)
wn_norm_squared_raw_vector(&result,raw_vect,len_i)

wn_norm_vector(&result,vect)
wn_norm_raw_vector(&result,raw_vect,len_i)

wn_unit_vector(&result_vect,vect)
wn_unit_raw_vector(result_raw_vect,raw_vect,len_i)

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
#include "wnmat.h"



wn_norm_squared_vector(presult,vect)

double *presult;
wn_vector vect;

{
  wn_norm_squared_raw_vector(presult,vect->numbers,vect->len_i);
}



wn_norm_squared_raw_vector(presult,raw_vect,len_i)

double *presult,raw_vect[];
int len_i;

{
  wn_raw_vector_dot_raw_vector(presult,raw_vect,raw_vect,len_i);
}



wn_norm_vector(presult,vect)

double *presult;
wn_vector vect;

{
  wn_norm_raw_vector(presult,vect->numbers,vect->len_i);
}



wn_norm_raw_vector(presult,raw_vect,len_i)

double *presult,raw_vect[];
int len_i;

{
  extern double sqrt();
  double norm_squared;

  wn_norm_squared_raw_vector(&norm_squared,raw_vect,len_i);

  *presult = sqrt(norm_squared);
}



wn_unit_vector(presult_vect,vect)

wn_vector *presult_vect,vect;

{
  int len_i;

  len_i = vect->len_i;

  wn_make_vector(presult_vect,len_i);
  wn_unit_raw_vector((*presult_vect)->numbers,vect->numbers,len_i);
}



wn_unit_raw_vector(result_raw_vect,raw_vect,len_i)

double result_raw_vect[],raw_vect[];
int len_i;

{
  double norm;

  wn_norm_raw_vector(&norm,raw_vect,len_i);
  wn_copy_raw_vector(result_raw_vect,raw_vect,len_i);
  wn_multiply_into_raw_vector_scalar(result_raw_vect,1.0/norm,len_i);
}



