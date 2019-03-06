/**********************************************************************

wn_add_up_vector(&result,vect)
wn_add_up_raw_vector(&result,raw_vect,i)

wn_vector_add_vector(&result_vect,vect1,vect2)
wn_raw_vector_add_raw_vector(result_raw_vect,raw_vect1,raw_vect2,len_i)

wn_add_into_vector_vector(to_vect,from_vect)
wn_add_into_raw_vector_raw_vector(to_raw_vect,from_raw_vect,len_i)

wn_add_into_vector_scaled_vector(to_vect,from_vect,scalar)
wn_add_into_raw_vector_scaled_raw_vector(to_raw_vect,from_raw_vect,scalar,len_i)

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



wn_add_up_vector(presult,vect)

double *presult;
wn_vector vect;

{
  wn_add_up_raw_vector(presult,vect->numbers,vect->len_i);
}



wn_add_up_raw_vector(presult,raw_vect,len_i)

double *presult;
register double raw_vect[];
register int len_i;

{
  register int i;
  register double sum;

  sum = 0.0;

  for(i=0;i<len_i;i++)
  {
    sum += raw_vect[i];
  }

  *presult = sum;
}



wn_vector_add_vector(presult_vect,vect1,vect2)

wn_vector *presult_vect,vect1,vect2;

{
  int len_i;

  wn_assertmsg(vect1->len_i == vect2->len_i,"bad lens");

  len_i = vect1->len_i;

  wn_make_vector(presult_vect,len_i);

  wn_raw_vector_add_raw_vector((*presult_vect)->numbers,
                               vect1->numbers,vect2->numbers,
                               len_i);
}



wn_raw_vector_add_raw_vector(result_raw_vect,raw_vect1,raw_vect2,len_i)

register double result_raw_vect[],raw_vect1[],raw_vect2[];
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    result_raw_vect[i] = raw_vect1[i] + raw_vect2[i];
  }
}



wn_add_into_vector_vector(to_vect,from_vect)

wn_vector to_vect,from_vect;

{
  wn_assertmsg(to_vect->len_i == from_vect->len_i,"bad lens");

  wn_add_into_raw_vector_raw_vector(to_vect->numbers,
                                    from_vect->numbers,from_vect->len_i);
}



wn_add_into_raw_vector_raw_vector(to_raw_vect,from_raw_vect,len_i)

register double to_raw_vect[],from_raw_vect[];
register int len_i;

{
  register int i;

  for(i=0;i<len_i;i++)
  {
    to_raw_vect[i] += from_raw_vect[i];
  }
}



wn_add_into_vector_scaled_vector(to_vect,from_vect,scalar)

wn_vector to_vect,from_vect;
double scalar;

{
  wn_assertmsg(to_vect->len_i == from_vect->len_i,"bad lens");

  wn_add_into_raw_vector_scaled_raw_vector(to_vect->numbers,
                                           from_vect->numbers,
                                           scalar,
                                           from_vect->len_i);
}



#if 0  /* wcn: deleted 11/06/89 */
/***************************************************************************

  This must be as fast as possible, because it is the inner loop for most
  n^3 matrix algebra algorithms.  That is why the code is so ugly.

***************************************************************************/
wn_add_into_raw_vector_scaled_raw_vector(to_raw_vect,from_raw_vect,scalar,len_i)

register double *to_raw_vect,*from_raw_vect;
register double scalar;
int len_i;

{
  register double *fin;

  fin = from_raw_vect + len_i;

  while(from_raw_vect != fin)
  {
    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
  }
}
#endif



/***************************************************************************

  This must be as fast as possible, because it is the inner loop for most
  n^3 matrix algebra algorithms.  That is why the code is so ugly.

***************************************************************************/
wn_add_into_raw_vector_scaled_raw_vector(to_raw_vect,from_raw_vect,scalar,len_i)

register double *to_raw_vect,*from_raw_vect;
register double scalar;
int len_i;

{
small:
  switch(len_i)
  {
    case(16):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(15):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(14):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(13):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(12):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(11):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(10):   *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(9):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(8):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(7):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(6):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(5):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(4):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(3):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(2):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(1):    *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
    case(0):    return;
    default:
    {
      wn_assert(len_i > 16);

      do
      {
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));
        *(to_raw_vect++) += (scalar*(*(from_raw_vect++)));

	len_i -= 16;
      }
      while(len_i > 16);

      goto small;
    }
  }
}



