/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************
******************************************************************************/

#include <stdio.h>
#include "wnlib.h"
#include "wnrtab.h"
#include "wnargp.h"




#define CHAR_MASK  ((1<<8)-1)




main(argc,argv)

int argc;
char *argv[];

{
  static bool ignore_order;
  static wn_arg_format arg_format[] =
  {
    WN_KEY_BOOL(&ignore_order,"x"),
    WN_ARG_TERMINATE
  };

  wn_gpmake("no_free");

  wn_parse_args(argc,argv,arg_format);

  if(ignore_order)
  {
    printf("%d\n",sum_no_order());
  }
  else
  {
    printf("%d\n",sum());
  }

  wn_gpfree();
}



local int sum_no_order()
{
  extern int wn_random_int_table[];
  register int sum,count,c;
  register int *random_int_table;

  random_int_table = wn_random_int_table+WN_RANDOM_INT_TABLE_SIZE;

  count = 0;
  sum = 0;

  while(TRUE)
  {
    c = getchar(); 
    if(c == EOF)
    {
      break;
    }

    sum ^= random_int_table[c];

    ++count;
  }

  return(sum^count);
}



local int sum()
{
  extern int wn_random_int_table[];
  register int sum,count,c;
  register int *random_int_table;

  random_int_table = wn_random_int_table+WN_RANDOM_INT_TABLE_SIZE;

  count = 0;
  sum = 0;

  while(TRUE)
  {
    c = getchar(); 
    if(c == EOF)
    {
      break;
    }

    sum ^= random_int_table[(c^count)&CHAR_MASK];

    ++count;
  }

  return(sum);
}


