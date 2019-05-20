/*************************************************************************

wn_set_crash_print(pcrash_print)
wn_set_crash_abort(pcrash_abort)

wn_assert(cond)
wn_assertmsg(cond,msg)
wn_crash()
wn_crashmsg(msg)

*************************************************************************/
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
#include "wnlib.h"



local bool initialized = FALSE;
local void (*psaved_crash_print)(),(*psaved_crash_abort)();



local void default_crash_print(string)

char string[];

{
#ifdef vms
  fprintf(stderr,"\n\n%s",string);
#else
  fprintf(stderr,"%s",string);
#endif
}



local void default_crash_abort()
{
#ifdef vms
#ifdef DEBUG
  globalvalue SS$_DEBUG;
  LIB$SIGNAL( SS$_DEBUG );
#else
  globalvalue SS$_BUGCHECK;
  LIB$SIGNAL( SS$_BUGCHECK );
#endif
#else
  abort();
#endif
}



local initialize_crash()
{
  if(not(initialized))
  {
    psaved_crash_print = (default_crash_print);
    psaved_crash_abort = (default_crash_abort);

    initialized = TRUE;
  }
}



wn_set_crash_print(pcrash_print)

void (*pcrash_print)(/*string*/);

{
  initialize_crash();

  psaved_crash_print = pcrash_print;
}



wn_set_crash_abort(pcrash_abort)

void (*pcrash_abort)(/*string*/);

{
  initialize_crash();

  psaved_crash_abort = pcrash_abort;
}



wn_assert_routine(file_name,line_num)

char file_name[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: assertion botched -- forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);

  (*psaved_crash_abort)();
}



wn_assertmsg_routine(file_name,line_num,msg)

char file_name[],msg[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: %s\n",file_name,line_num,msg);
  (*psaved_crash_print)(string);
  (void)sprintf(string,"%s[%d]: assertion botched -- forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);

  (*psaved_crash_abort)();
}



wn_crash_routine(file_name,line_num)

char file_name[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: wn_crash() called -- forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);

  (*psaved_crash_abort)();
}



wn_crashmsg_routine(file_name,line_num,msg)

char file_name[],msg[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: %s\n",file_name,line_num,msg);
  (*psaved_crash_print)(string);
  (void)sprintf(string,"%s[%d]: wn_crashmsg() called -- forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);

  (*psaved_crash_abort)();
}



wn_assert_nc_routine(file_name,line_num)

char file_name[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: assertion botched -- NOT forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);
}



wn_assertmsg_nc_routine(file_name,line_num,msg)

char file_name[],msg[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: %s\n",file_name,line_num,msg);
  (*psaved_crash_print)(string);
  (void)sprintf(string,"%s[%d]: assertion botched -- NOT forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);
}



wn_crash_nc_routine(file_name,line_num)

char file_name[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: wn_crash_nc() called -- NOT forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);
}



wn_crashmsg_nc_routine(file_name,line_num,msg)

char file_name[],msg[];
int line_num;

{
  char string[150];

  initialize_crash();

  print_user_message();
  (void)sprintf(string,"%s[%d]: %s\n",file_name,line_num,msg);
  (*psaved_crash_print)(string);
  (void)sprintf(string,"%s[%d]: wn_crashmsg_nc() called -- NOT forcing crash\n",
                file_name,line_num);
  (*psaved_crash_print)(string);
}



local print_user_message()
{
  (*psaved_crash_print)(
      "------------------------------------------------------------\n");
  (*psaved_crash_print)(
      "                       HELP!!!\n");
  (*psaved_crash_print)(
      "This program has encountered a severe internal error.\n");
  (*psaved_crash_print)(
      "Please report the following message to the\n");
  (*psaved_crash_print)(
      "program's developers so that they can fix the problem:\n");
  (*psaved_crash_print)(
      "------------------------------------------------------------\n");
}





