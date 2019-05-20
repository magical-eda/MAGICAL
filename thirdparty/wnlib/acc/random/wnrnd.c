/**********************************************************************

wn_seed_random_number_generator(seed)

int wn_random_int()

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

  Will Naylor -- original
  Bill Chapman -- 3/3/2004 -- sped up wn_random_int()
  Will Naylor -- 18/3/2004 -- rewrite to make cryptographically strong
  Ross Donelly - 19/3/2004 -- speed up by 25%

****************************************************************************/

#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnmemb.h"
#include "wnrtab.h"
#include "wnrnd.h"

#define WN_RANDOM_WIDTH           17
#define NUM_FEEDBACK_MUNGE_FUNCS  283  /* prime number */
#define NUM_OUT_MUNGE_BLOCKS      17
#define NUM_OUT_MUNGE_FUNCS       (NUM_OUT_MUNGE_BLOCKS*WN_RANDOM_WIDTH) /*289*/
#define WINDOW_SIZE               587  /* size of state machine */
#define STATE_BUFFER_SIZE         (8009/*prime*/ + WINDOW_SIZE)

#define CHAR_MASK                 ((1<<8)-1)


int wn_random_ints_buf[WN_RANDOM_WIDTH] =
{  /* initialize for seed = 0 */ 
(int)0x3a7ad7b3, (int)0x4a0d9cc8, (int)0xa1815edb, (int)0x584384e6,
(int)0x60052b24, (int)0x211df882, (int)0x5ca0a589, (int)0xc69dc7c9,
(int)0x2a31f839, (int)0xda9ca2e8, (int)0x6fdb7e70, (int)0x3092c785,
(int)0xa6c492f3, (int)0x0c9b8336, (int)0x7ddb50d1, (int)0x88e78651,
(int)0xfa63a631
};
int wn_random_ints_count = 0;  /* number of random ints in buffer */

local int saved_bits;
local int number_of_saved_bits = 0;


local int state_buffer[STATE_BUFFER_SIZE] =   
{
  #include "wnrds.h"  /* state for seed 0 */
};
local int *state_pointer = state_buffer + 3304; /* initialize for seed = 0 */
local int *state_buffer_top = state_buffer + STATE_BUFFER_SIZE;

struct munge_func_struct
{
  int constant;      
  int lin_tap1,lin_tap2;    /* linear taps */
  int quad_tap1,quad_tap2;  /* quadratic taps */
};

local struct munge_func_struct 
feedback_munge_array[NUM_FEEDBACK_MUNGE_FUNCS] = 
{
  #include "wnrdf.h"  /* feedback taps and constants */
};
local struct munge_func_struct *feedback_munge_pointer = 
      feedback_munge_array + 282;  /* initialize for seed = 0 */
local struct munge_func_struct *feedback_munge_array_top = 
	  feedback_munge_array + NUM_FEEDBACK_MUNGE_FUNCS;  
local int out_munge_num = 3; /* initialize for seed = 0 */


void wn_rand_print_state(void)
{
  int i;

  printf("int wn_random_ints_buf[WN_RANDOM_WIDTH] =\n");
  printf("{  /* initialize for seed = 0 */ \n");
  for(i=0;  i < WN_RANDOM_WIDTH;  ++i)
  {
    printf("(int)0x%8x%s", 
	   (unsigned) wn_random_ints_buf[i], i % 4 != 3 ? ", " : ",\n");
  }
  printf("}\n");
  printf("int wn_random_ints_count = %d;  /* number of random ints in buffer */\n",
  wn_random_ints_count);
  printf("\n");

  printf("local int *state_pointer = state_buffer + %d; /* initialize for seed = 0 */\n",
	 state_pointer - state_buffer);
  printf("\n");
  printf("local struct munge_func_struct *feedback_munge_pointer = \n");
  printf("      feedback_munge_array + %d;  /* initialize for seed = 0 */\n",
	 feedback_munge_pointer - feedback_munge_array);
  printf("\n");
  printf("local int out_munge_num = %d; /* initialize for seed = 0 */\n",
	 out_munge_num);
  printf("\n");

  printf("local int state_buffer[STATE_BUFFER_SIZE] =\n");
  printf("{  /* initialize for seed = 0 */\n");
  for(i=0;  i < STATE_BUFFER_SIZE;  ++i)
  {
    printf("(int)0x%8x%s", (unsigned) state_buffer[i],
    /**/					i % 4 != 3 ? ", " : ",\n");
  }
  printf("}\n");
}


void wn_seed_random_number_generator(int seed)
{
  int i;

  for(i=0;i<STATE_BUFFER_SIZE;++i)
  {
    state_buffer[i] = seed ^ i;
  }

  wn_random_ints_count = 0;  
  state_pointer = state_buffer;
  feedback_munge_pointer = feedback_munge_array;
  out_munge_num = 0;

  /*
  printf("state 1:\n");
  wn_rand_print_state();
  */

  for(i = 0;  (size_t) i < 2*sizeof(int)*WN_RANDOM_WIDTH*STATE_BUFFER_SIZE;
  /**/									++i)
  {
    (void)wn_random_int();
  }

  number_of_saved_bits = 0;
}


/*
void mix_up_bit_planes() \
*/
#define mix_up_bit_planes() \
{ \
  state_pointer[0] ^= wn_random_int_table[ \
    (state_pointer[1]) & ((1<<11)-1)]; \
  state_pointer[2] ^= wn_random_int_table[ \
    (state_pointer[3] >> 1*11) & ((1<<11)-1)]; \
  state_pointer[4] ^= wn_random_int_table[ \
    (state_pointer[5] >> 2*11) & ((1<<10)-1)]; \
}


/*
void decrement_state_pointer() \
*/
#define decrement_state_pointer() \
{ \
  if(state_pointer == state_buffer) \
  { \
    state_pointer = state_buffer_top-1 - WINDOW_SIZE; \
    wn_memcpy((ptr)state_pointer,(ptr)state_buffer, \
              WINDOW_SIZE*sizeof(int)); \
    \
    mix_up_bit_planes(); \
  } \
  \
  --state_pointer; \
  \
}

/*
void increment_feedback_munge_pointer() \
*/
#define increment_feedback_munge_pointer() \
{ \
  ++feedback_munge_pointer; \
  \
  if(feedback_munge_pointer == feedback_munge_array_top) \
  { \
    feedback_munge_pointer = feedback_munge_array; \
  } \
}

#define munge(p,m) \
( \
  (m)->constant ^ p[(m)->lin_tap1] ^ p[(m)->lin_tap2] ^ \
  ( p[(m)->quad_tap1] & p[(m)->quad_tap2] ) \
)

int wn_random_ints_fill_buf()
{
  int feedback, ret;

  /* compute output bits */
  switch (out_munge_num)
  {
#include "wnrdo.h"
  }
  wn_random_ints_count = WN_RANDOM_WIDTH - 1;  /* wn_random_ints_buf is 1 less than full */
  ++out_munge_num;
  if (out_munge_num == NUM_OUT_MUNGE_BLOCKS)
  {
    out_munge_num = 0;
  }

  /* compute feedback bits */
  feedback = munge(state_pointer,feedback_munge_pointer);
  increment_feedback_munge_pointer();

  /* stuff feedback bits back into shift register */
  decrement_state_pointer();
  (*state_pointer) ^= feedback;

  return(ret);
}


/* subroutine version of the macro wn_random_int() */
int wn_random_int_func(void)
{
  return(wn_random_int());
}


int wn_random_n_bits(int n)
{
  int ret;

  if(n <= number_of_saved_bits)
  {
    ret = saved_bits&((1<<n)-1);
    saved_bits >>= n;
    number_of_saved_bits -= n;

    return(ret);
  }
  else /* n > number_of_saved_bits */
  {
    int bit_deficit;

    if (n >= 32)
    {
      wn_assert(n == 32);
      return wn_random_int();
    }
    bit_deficit = n - number_of_saved_bits;

    ret = saved_bits&((1<<number_of_saved_bits)-1);
    ret <<= bit_deficit;

    saved_bits = wn_random_int();
    ret |= ( saved_bits&((1<<bit_deficit)-1) );
    saved_bits >>= bit_deficit;
    number_of_saved_bits = 32 - bit_deficit;

    return(ret);
  }
}

/* this version is faster if n>10 */
int wn_random_n_bits_alt(int n)
{
  if (n >= 32)
  {
    wn_assert(n == 32);
    return wn_random_int();
  }
  else
  {
    return wn_random_int() & ((1u << n) - 1u);
  }
}


int wn_random_bit(void)
{
  int ret;

  if(number_of_saved_bits < 1)
  {
    saved_bits = wn_random_int();
    number_of_saved_bits = 32;
  }

  ret = (int)(saved_bits&1);
  saved_bits >>= 1;
  --number_of_saved_bits;

  return(ret);
}
