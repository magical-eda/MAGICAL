/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnsqr.h"
#include "wnmem.h"
#include "wnswap.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnanl.h"

#include "wnargp.h"


#define SIZE  200


local double problem[SIZE][SIZE];
local int solution[SIZE];
local double new_ob,old_ob;
local int i1,i2;


local double eval_solution(void)
{
  int i;
  double sum;

  sum = 0.0;

  for(i=0;i<SIZE;++i)
  {
    sum += problem[i][solution[i]];
  }

  return(sum);
}


local double evaluate_random_mutation(void)
{
  i1 = wn_random_mod_int(SIZE);

  do
  {
    i2 = wn_random_mod_int(SIZE);
  }
  while(i1 == i2);

  return(problem[i1][solution[i2]]+problem[i2][solution[i1]]
	 -problem[i1][solution[i1]]-problem[i2][solution[i2]]);
}


local void accept_mutation(void)
{
  wn_swap(solution[i1],solution[i2],int);
}


local void reject_mutation(void)
{
}


local void checkpoint(void)
{
  static int count=0;

  /*
  if(count%100 == 0)
  {
    wn_anneal_std_checkpoint_print();

    printf("checkpoint: ob=%lf\n",eval_solution());
  }
  */

  ++count;
}


local double do_test(int effort)
{
  int i,j;
  double ob;

  wn_gpmake("no_free");

  for(i=0;i<SIZE;++i)
  for(j=0;j<SIZE;++j)
  {
    problem[i][j] = wn_random_double_between(0.0,1.0);
  }

  for(i=0;i<SIZE;++i)
  {
    solution[i] = i;
  }

  wn_anneal((evaluate_random_mutation),
	    (accept_mutation),(reject_mutation),
	    (checkpoint),
	    SIZE,SIZE*effort,effort);
  /*
  wn_anneal_from_temperature((evaluate_random_mutation),
	    (accept_mutation),(reject_mutation),
	    (checkpoint),
	    SIZE,(SIZE*SIZE)/2,100,0.0);
  printf("checkpoint: ob=%lf\n",eval_solution());
  */

  ob = eval_solution();

  wn_gpfree();

  return(ob);
}


int main(int argc,char *argv[])
{
  static int effort,num_samples,seed;
  static wn_arg_format arg_format_array[] =
  {
    WN_KEY_INT(&effort, "e", "effort", WN_OPTIONAL),
    WN_KEY_INT(&num_samples, "n", "num_samples", WN_OPTIONAL),
    WN_KEY_INT(&seed, "s", "random_seed", WN_OPTIONAL),
    WN_ARG_TERMINATE
  };
  int i;
  double ob;
  double sum,sum2;
  double m,m2,stdev,stdev_of_mean;

  effort = 1000;
  num_samples = 600;
  seed = 0;

  wn_parse_args(argc,argv,arg_format_array);

  wn_seed_random_number_generator(seed);

  sum = 0.0;
  sum2 = 0.0;

  for(i=0;i<num_samples;++i)
  {
    ob = do_test(effort);

    sum += ob;
    sum2 += wn_square(ob);

    m = sum/(i+1);
    m2 = sum2/(i+1);

    stdev = sqrt((m2-wn_square(m))*(i+1)/(double)i);
    stdev_of_mean = stdev/sqrt(i+1.0);

    printf("i = %d,mean = %lg,stdev = %lg,stdev_of_mean = %lg\n",
	   i,m,stdev,stdev_of_mean);
  }

  return(0);
}
