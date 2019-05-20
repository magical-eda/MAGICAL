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
#ifndef wnbllH
#define wnbllH

#include "wntype.h"


typedef struct wn_bll_struct *wn_bll;

struct wn_bll_struct
{
  ptr contents;
  wn_bll next,*plist;
};


#define wn_bllins(_plist,_e)  wn_bllins_routine(_plist,(ptr)(_e))
#define wn_blldel(_plist,_e)  wn_blldel_routine(_plist,(ptr)(_e))
#define wn_bllindex(_pcontents,_list,_index) \
                      wn_bllindex_routine((ptr *)(_pcontents),_list,_index)
#define wn_bllget(_pe,_list,_ppredicate) \
                      wn_bllget_routine((ptr *)(_pe),_list,_ppredicate)
#define wn_bllgetmin(_pe,_list,_pcompare_func) \
                      wn_bllgetmin_routine((ptr *)(_pe),_list,_pcompare_func)

#define wn_blleloop(_pel) wn_blleloop_routine(_pel)
#define wn_bllloop(_pel,_pdummy) wn_bllloop_routine((_pel),(ptr *)(_pdummy))


#endif

