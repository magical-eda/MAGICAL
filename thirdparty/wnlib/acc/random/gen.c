/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHORS:

  Will Naylor

*****************************************************************************

Generate taps, coefficients, and constants
for random number generators.

****************************************************************************/
#include <stdlib.h>

#include "wnlib.h"
#include "wnasrt.h"

#include "wnrndt.h"



local int random_int(void)
{
  return(wn_true_random_int());
  /*
  return(random());
  */
}


local bool any_taps_are_equal(int lin_tap1,int lin_tap2,
			      int quad_tap1,int quad_tap2)
{
  return(
	  (lin_tap1 == lin_tap2)
	    ||
	  (lin_tap1 == quad_tap1)
	    ||
	  (lin_tap1 == quad_tap2)
	    ||
	  (lin_tap2 == quad_tap1)
	    ||
	  (lin_tap2 == quad_tap2)
	    ||
	  (quad_tap1 == quad_tap2)
        );
}


local void print_munge_func_struct_initializer(int window_size)
{
  int constant;
  int lin_tap1,lin_tap2;
  int quad_tap1,quad_tap2;

  constant = random_int();

  do
  {
    lin_tap1 = ((unsigned)random_int())%window_size;
    lin_tap2 = ((unsigned)random_int())%window_size;
    quad_tap1 = ((unsigned)random_int())%window_size;
    quad_tap2 = ((unsigned)random_int())%window_size;
  } while(any_taps_are_equal(lin_tap1,lin_tap2,quad_tap1,quad_tap2));

  printf("  {(int)0x%0.8lx,%9d,%9d,%9d,%9d},\n",
	 constant,
	 lin_tap1,lin_tap2,quad_tap1,quad_tap2);
  fflush(stdout);
}


int main(int argc,char *argv[])
{
  int i;

  printf("feedback_munge_array[] = \n");
  printf("{\n");
  printf("/* constant        lin_tap1  lin_tap2  quad_tap1 quad_tap2*/\n");
  printf("/* --------------- --------- --------- --------- ---------*/\n");
  for(i=0;i<283;++i)
  {
    print_munge_func_struct_initializer(587);
  }
  printf("}\n");
  printf("\n");
  printf("out_munge_array[] = \n");
  printf("{\n");
  printf("/* constant        lin_tap1  lin_tap2  quad_tap1 quad_tap2*/\n");
  printf("/* --------------- --------- --------- --------- ---------*/\n");
  for(i=0;i<289;++i)
  {
    print_munge_func_struct_initializer(587);
  }
  printf("}\n");
}
