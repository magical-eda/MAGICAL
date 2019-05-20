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
#include "wncplx.h"


#define WN_SUCCESS        0
#define WN_SINGULAR       (-1)
#define WN_UNBOUNDED      (-2)
#define WN_BAD_ARGS       (-3)
#define WN_INFEASIBLE     (-4)


typedef wn_cplx *wn_cplx_raw_vector;

typedef struct wn_cplx_vector_struct *wn_cplx_vector;
struct wn_cplx_vector_struct
{
  int len_i;

  wn_cplx_raw_vector numbers;
};


typedef wn_cplx **wn_cplx_raw_matrix;

typedef struct wn_cplx_matrix_struct *wn_cplx_matrix;
struct wn_cplx_matrix_struct
{
  int len_i,len_j;

  wn_cplx_raw_matrix numbers;
};


