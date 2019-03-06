/****************************************************************

wn_strupcase(s)
wn_strlowcase(s)

wn_memupcase(mem,len)
wn_memlowcase(mem,len)

wn_charupcase(&c)
wn_charlowcase(&c)

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
#include "wnmap.h"

#include "wncase.h"



#define MASK (255)

local char upcase_map_array[256],lowcase_map_array[256];



local void initialize(void)
{
  static bool initialized = FALSE;

  if(!initialized)
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


void wn_memupcase(register char *string,int len)
{
  initialize();

  wn_memmap(string,len,upcase_map_array);
}


void wn_memlowcase(register char *string,int len)
{
  initialize();

  wn_memmap(string,len,lowcase_map_array);
}


void wn_strupcase(register char *string)
{
  initialize();

  wn_strmap(string,upcase_map_array);
}


void wn_strlowcase(register char *string)
{
  initialize();

  wn_strmap(string,lowcase_map_array);
}


void wn_charupcase(char *pc)
{
  initialize();

  *pc = upcase_map_array[*pc & MASK];	/* make char non-negative */
}


void wn_charlowcase(char *pc)
{
  initialize();

  *pc = lowcase_map_array[*pc & MASK];	/* make char non-negative */
}
