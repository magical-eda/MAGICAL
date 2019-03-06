/**********************************************************************

wn_listen(volume)

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

#include <signal.h>
#include <fcntl.h>

#include "wnlib.h"
#include "wnabs.h"



#define INTERVAL  100   /* in microseconds */

local int save_volume,audio_fd;



wn_listen(volume)

int volume;

{
  void output_state();

  save_volume = volume;

  audio_fd = open("/dev/audio",O_WRONLY,-1);

  signal(SIGALRM,(output_state));

  ualarm(INTERVAL, INTERVAL);
}



local void output_state()
{
  char upper_stack;

  output_state2(&upper_stack);
}



local output_state2(pupper_stack)

char *pupper_stack;

{
  char state;

  measure_state(&state,pupper_stack);

  state = wn_abs(state)%save_volume;

  wn_write(audio_fd,&state,1);
}



local measure_state(pstate,pupper_stack)

char *pstate,*pupper_stack;

{
  int direction;
  char stack;
  register char state,*stack_ptr,*start,*fin;

  direction = pupper_stack-(&stack);
  if(direction < 0)
  {
    direction = -1;
  }
  else if(direction > 0)
  {
    direction = 1;
  }
  else
  {
    wn_crash();
  }

  start = pupper_stack;
  switch(direction)
  {
    case(-1):
    {
      fin = pupper_stack-50;
    } break;
    case(1):
    {
      fin = pupper_stack+50;
    } break;
    default:
      wn_crash();
  }

  state = '\0';

  for(stack_ptr=start;stack_ptr!=fin;stack_ptr += direction)
  {
    state += *stack_ptr;
  }

  *pstate = state;
}
