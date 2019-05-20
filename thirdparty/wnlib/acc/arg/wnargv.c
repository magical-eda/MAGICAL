/*****************************************************************

wn_parse_into_args(&argc,&argv,string)

wn_argscpy(&argv,argc,argv)
wn_freeargs(argc,argv)

*****************************************************************/
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
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "wnlib.h"
#include "wnmem.h"
#include "wnargp.h"

#define MAX_ARGS          20
#define MAX_STRING_LEN    500


#define IN_BLANK_SPACE     1
#define IN_ID              2
#define IN_QUOTE_STRING    3



void wn_parse_into_args(int *pargc, char **pargv[], char *string)
{
  int argc,state;
  char *pc;
  static char string_copy[MAX_STRING_LEN],*(argv[MAX_ARGS]);

  strcpy(string_copy, string);

  state = IN_BLANK_SPACE;

  argc = 0;

  for(pc = string_copy;*pc != '\0';pc++)
  {
    if(state == IN_BLANK_SPACE)
    {
      if(!(isspace(*pc)))
      {
        if(*pc == '\"')
        {
          argv[argc] = pc + 1;
          state = IN_QUOTE_STRING;
        }
        else
        {
          argv[argc] = pc;
          state = IN_ID;
        }
	++argc;
      }
    }
    else if(state == IN_ID)
    {
      if(isspace(*pc))
      {
        *pc = '\0';
        state = IN_BLANK_SPACE;
      }
    }
    else if(state == IN_QUOTE_STRING)
    {
      if(*pc == '\"')
      {
        *pc = '\0';
        state = IN_BLANK_SPACE;
      }
    }
  }

  *pargc = argc;
  *pargv = argv;
}



void wn_argscpy(char **pargv[], int argc, char *argv[])
{
  int a;

  *pargv = (char **)wn_zalloc((argc+1)*sizeof(char *));

  for(a=0;a<argc;a++)
  {
    /* can't use stracpy, it's in str which arg is below */

    (*pargv)[a] = (char *) wn_alloc(strlen(argv[a])+1);
    strcpy((*pargv)[a], argv[a]);
  }
}



void wn_freeargs(int argc, char *argv[])
{
  int a;

  for(a=0;a<argc;a++)
  {
    wn_free((ptr)(argv[a]));
  }

  wn_free((ptr)argv);
}
