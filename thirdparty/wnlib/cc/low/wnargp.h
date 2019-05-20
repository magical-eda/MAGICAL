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



#define WN_TERMINATE_TYPE     0
#define WN_POS_STRING_TYPE    1
#define WN_KEY_BOOL_TYPE      2
#define WN_KEY_STRING_TYPE    3


#define WN_REQUIRED            TRUE
#define WN_OPTIONAL            FALSE




typedef struct
{
  int type;
  bool required;
  char *keyword,*arg_name;
  ptr value_pointer;

  bool positional_arg_present;  /* used only for positional string */
} wn_arg_format;



#define  WN_POS_STRING(_value_pointer,_arg_name,_required) \
      {WN_POS_STRING_TYPE,(_required),NULL,(_arg_name),\
                          (ptr)(_value_pointer)}

#define  WN_KEY_BOOL(_value_pointer,_keyword) \
      {WN_KEY_BOOL_TYPE,WN_OPTIONAL,(_keyword),NULL,(ptr)(_value_pointer)}

#define  WN_KEY_STRING(_value_pointer,_keyword,_arg_name,_required) \
      {WN_KEY_STRING_TYPE,(_required),(_keyword),(_arg_name),\
                          (ptr)(_value_pointer)}

#define WN_ARG_TERMINATE    {0,FALSE,NULL,NULL,NULL}

