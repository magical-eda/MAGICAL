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
#ifndef wnspmatH
#define wnspmatH



#include "wnlib.h"
#include "wnmem.h"
#include "wnsll.h"




typedef struct wn_sparse_matrix_struct
{
  int len_i,len_j;
  wn_sll *i_lists,*j_lists;

  wn_memgp group;
} *wn_sparse_matrix;


typedef struct wn_sparse_matrix_entry_struct
{
  double value;

  int i,j;

  ptr baggage;
} *wn_sparse_matrix_entry;



#include "wnspmk.h"
#include "wnspcon.h"
#include "wnspio.h"
#include "wnsprnd.h"
#include "wnspcpy.h"
#include "wnspcnt.h"
#include "wnstrn.h"




#endif
