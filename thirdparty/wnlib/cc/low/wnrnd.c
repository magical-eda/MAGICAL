/**********************************************************************

wn_seed_random_number_generator(seed)

int wn_random_int()

int wn_random_mod_int(mod)

int wn_random_n_bits(n)

int wn_random_bit()

**********************************************************************/
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
#include "wnrtab.h"



#define BUFFER_SIZE 2000
#define STATE_SIZE  300

local bool seeded=FALSE;
local int pointer=0;
local int state[BUFFER_SIZE];

local int saved_bits,number_of_saved_bits;



local int hash_int(i)

int i;

{
  return(wn_blkhash(&i,sizeof(int)));
}


wn_seed_random_number_generator(seed)

int seed;

{
  int i;

  seeded = TRUE;

  for(i=0;i<STATE_SIZE;++i)
  {
    state[i] = hash_int(i+seed);
  }

  pointer = STATE_SIZE;

  number_of_saved_bits = 0;
}


long int wn_random_int()
{
  extern int wn_random_int_table[];
  register int *p;
  register int ret;

  if(not(seeded))
  {
    wn_seed_random_number_generator(0);
  }

  p = &(state[pointer]);

  {
    register char *pc;

    pc = (char *)p;

    ret = wn_random_int_table[WN_RANDOM_INT_TABLE_SIZE + 
			 ((pc[-82]&pc[-555])^(pc[-495]|pc[-500])^pc[-388])
			     ];
  }

  ret ^= (p[-175]^p[-159]);
  ret -= (p[-158]^p[-248]);
  ret ^= (p[-219]+p[-140]);
  ret -= (p[-165]-p[-299]);
  ret += (p[-168]-p[-161]);
  ret ^= (p[-246]-p[-261]);

  ret ^= (p[-155]|p[-230]);
  ret ^= (p[-54]&p[-221]);
  ret ^= (p[-79]&p[-236]);
  ret -= (p[-33]&p[-148]);
  ret ^= (p[-270]&p[-77]);
  ret += (p[-5]|p[-238]);
  ret ^= (p[-55]|p[-273]);
  ret ^= (p[-87]&p[-24]);
  ret ^= (p[-148]|p[-197]);
  ret ^= (p[-243]|p[-109]);
  ret += (p[-102]&p[-234]);
  ret ^= (p[-246]|p[-108]);
  ret += (p[-152]|p[-17]);
  ret += (p[-248]&p[-286]);
  ret ^= (p[-272]&p[-64]);

  *p = ret;

  ++pointer;
  if(pointer >= BUFFER_SIZE)
  {
    wn_blkcpy(state,&(state[BUFFER_SIZE-STATE_SIZE]),STATE_SIZE*sizeof(int));
    pointer = STATE_SIZE;
  }

  return(ret);
}



int wn_random_n_bits(n)

int n;

{
  int ret;

  if(number_of_saved_bits < n)
  {
    saved_bits = wn_random_int();
    number_of_saved_bits = 32;
  }

  ret = saved_bits&((1<<n)-1);
  saved_bits >>= n;
  number_of_saved_bits -= n;

  return(ret);
}



int wn_random_bit()
{
  int ret;

  if(number_of_saved_bits < 1)
  {
    saved_bits = wn_random_int();
    number_of_saved_bits = 32;
  }

  ret = saved_bits&1;
  saved_bits >>= 1;
  --number_of_saved_bits;

  return(ret);
}


#if 0
int wn_random_mod_int(mod)

int mod;

{
  int ret;

  ret = wn_random_int();

  if(ret < 0)
  {
    ret = -ret;
  }

  return(ret%mod);
}
#endif



unsigned int wn_random_mod_int(mod)

unsigned int mod;

{
  int num_bits;
  unsigned int ret;

  num_bits = wn_ilog2(mod)+1;

  do
  {
    ret = (unsigned int)wn_random_n_bits(num_bits);
  }
  while(ret >= mod);

  return(ret);
}

