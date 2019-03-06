/****************************************************************

wn_strupcase(s)
wn_strlowcase(s)

wn_blkupcase(blk,len)
wn_blklowcase(blk,len)

wn_charupcase(&c)
wn_charlowcase(&c)

* old routines *
wn_upcase_string(string)
wn_lowcase_string(string)
wn_upcase_char(&c)
wn_lowcase_char(&c)

****************************************************************/
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
#include <ctype.h>
#include "wnlib.h"



local char upcase_map_array[256],lowcase_map_array[256];



local initialize()
{
  static bool initialized = FALSE;

  if(not(initialized))
  {
    initialized = TRUE;

    wn_load_map_array(upcase_map_array,
		      "abcdefghijklmnopqrstuvwxyz",
		      "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    wn_load_map_array(lowcase_map_array,
		      "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		      "abcdefghijklmnopqrstuvwxyz");
  }
}



wn_blkupcase(string,len)

register char *string;
int len;

{
  initialize();

  wn_blkmap(string,len,upcase_map_array);
}



wn_blklowcase(string,len)

register char *string;
int len;

{
  initialize();

  wn_blkmap(string,len,lowcase_map_array);
}



wn_strupcase(string)

register char *string;

{
  initialize();

  wn_strmap(string,upcase_map_array);
}



wn_strlowcase(string)

register char *string;

{
  initialize();

  wn_strmap(string,lowcase_map_array);
}



wn_charupcase(pc)

char *pc;

{
  initialize();

  *pc = upcase_map_array[*pc];
}



wn_charlowcase(pc)

char *pc;

{
  initialize();

  *pc = lowcase_map_array[*pc];
}



/* old routines supported, for compatibility */

#if 1
wn_upcase_string(string)
char *string;
{
  wn_strupcase(string);
}

wn_lowcase_string(string)
char *string;
{
  wn_strlowcase(string);
}

wn_upcase_char(pc)
char *pc;
{
  wn_charupcase(pc);
}

wn_lowcase_char(pc)
char *pc;
{
  wn_charlowcase(pc);
}
#endif

