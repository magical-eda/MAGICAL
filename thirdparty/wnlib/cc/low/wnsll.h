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
#ifndef wnsllH
#define wnsllH

#include "wntype.h"


typedef struct wn_sll_struct *wn_sll;

struct wn_sll_struct
{
  ptr contents;
  wn_sll next;
};


#define wn_sllins(_plist,_e)  wn_sllins_routine(_plist,(ptr)(_e))
#define wn_slldel(_plist,_e)  wn_slldel_routine(_plist,(ptr)(_e))
#define wn_sllindex(_pcontents,_list,_index) \
                      wn_sllindex_routine((ptr *)(_pcontents),_list,_index)
#define wn_sllget_index(_pindex,_list,_e) \
		      wn_sllget_index_routine(_pindex,_list,(ptr)(_e))
#define wn_sllget(_pe,_list,_ppredicate) \
                      wn_sllget_routine((ptr *)(_pe),_list,_ppredicate)
#define wn_sllgetmin(_pe,_list,_pcompare_func) \
                      wn_sllgetmin_routine((ptr *)(_pe),_list,_pcompare_func)

#define wn_slleloop(_pel) wn_slleloop_routine(_pel)
#define wn_sllloop(_pel,_pdummy) wn_sllloop_routine((_pel),(ptr *)(_pdummy))


#endif

