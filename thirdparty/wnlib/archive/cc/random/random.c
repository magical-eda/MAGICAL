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

#include <stdlib.h>
#include <stdio.h>

#include "wnlib.h"
#include "wnargp.h"
#include "wnmem.h"
#include "wnhash.h"
#include "wnrndd.h"



int count,seed,min,max;



local void parse_args(int argc,char *argv[])
{
  static char *count_string,
              *seed_string,
              *min_string,
              *max_string;
  static wn_arg_format arg_format[] =
  {
    WN_KEY_STRING(&count_string,"count","count",WN_OPTIONAL), 
    WN_POS_STRING(&seed_string,"seed",WN_REQUIRED),
    WN_POS_STRING(&min_string,"min",WN_REQUIRED),
    WN_POS_STRING(&max_string,"max",WN_REQUIRED),
    WN_ARG_TERMINATE
  };

  count_string = NULL;

  wn_parse_args(argc,argv,arg_format);

  if(count_string == NULL)
  {
    count = 1;
  }
  else
  {
    count = atoi(count_string);
  }

  seed = wn_strhash(seed_string);
  min = atoi(min_string); 
  max = atoi(max_string);
}


void main(int argc,char *argv[])
{
  int i;
  int num;

  wn_gpmake("no_free");

  parse_args(argc,argv);

  wn_seed_random_number_generator(seed);

  for(i=0;i<count;++i)
  {
    num = wn_random_int_between(min,max+1);
    printf("%d\n",num);
  }

  wn_gpfree();
}



