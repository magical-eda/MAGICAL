/**************************************************************************

wn_random_permutation(permutation,size)
wn_identity_permutation(permutation,size)

wn_permute_permutation(result,perm1,perm2,size)
wn_invert_permutation(inverse,permutation,size)

bool wn_is_valid_permutation(permutation,size)

**************************************************************************/
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
#include "wninr.h"
#include "wnprm.h"



wn_random_permutation(permutation,size)

int permutation[],size;

{
  int *remaining_numbers,top_of_remaining_numbers,selection,count;

  wn_gpmake("no_free");

  remaining_numbers = (int *)wn_zalloc(size*wn_sizeof(int));
  wn_identity_permutation(remaining_numbers,size);

  for(count=0,top_of_remaining_numbers=size;
      count<size;
      count++,top_of_remaining_numbers--)
  {
    selection = wn_random_mod_int(top_of_remaining_numbers);

    permutation[count] = remaining_numbers[selection];
    remaining_numbers[selection] = 
                          remaining_numbers[top_of_remaining_numbers-1];
  }

  wn_gpfree();
}



wn_identity_permutation(permutation,size)

int permutation[],size;

{
  int i;

  for(i=0;i<size;i++)
  {
    permutation[i] = i;
  }
}



wn_permute_permutation(result,perm1,perm2,size)

int result[],perm1[],perm2[],size;

{
  wn_permute_array(result,perm1,perm2,size);
}



wn_invert_permutation(inverse,permutation,size)

int inverse[],permutation[],size;

{
  int i;

  wn_assertmsg(inverse != permutation,
               "wn_invert_permutation: inverse == permutation");

  for(i=0;i<size;i++)
  {
    inverse[permutation[i]] = i;
  }
}



bool wn_is_valid_permutation(permutation,size)

int permutation[],size;

{
  int i,entry;
  int *counts;
  bool ret;

  ret = TRUE;

  wn_gpmake("no_free");

  counts = (bool *)wn_zalloc(size*wn_sizeof(bool));

  for(i=0;i<size;i++)
  {
    entry = permutation[i];

    if(not(wn_inrange(0,<=,entry,<,size)))
    {
      ret = FALSE;

      break;
    }

    counts[permutation[i]]++;
  }

  if(ret == TRUE)
  {
    for(i=0;i<size;i++)
    {
      if(counts[i] != 1)
      {
        ret = FALSE;
  
        break;
      }
    }
  }

  wn_gpfree();

  return(ret);
}



