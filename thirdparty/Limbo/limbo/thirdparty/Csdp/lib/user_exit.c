/*
 * User exit routine for psd.  This version of the routine simply returns
 * 0, so that psd will not stop.
 */

#include "declarations.h"


#ifdef USESIGTERM

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int sigterm_signaled=0;

/*
 * This sets sigterm_signaled to 1.  The next time
 * user_exit runs, it will see this and return 1, so CSDP will stop.
 */

void catch_sigterm(signal)
     int signal;
{
  sigterm_signaled=1;
}

int user_exit(n,k,C,a,dobj,pobj,constant_offset,constraints,X,y,Z,params)
     int n;
     int k;
     struct blockmatrix C;
     double *a;
     double dobj;
     double pobj;
     double constant_offset;
     struct constraintmatrix *constraints;
     struct blockmatrix X;
     double *y;
     struct blockmatrix Z;     
     struct paramstruc params;
{
  signal(SIGTERM,catch_sigterm);
  if (sigterm_signaled==1)
    return(1);
  else
    return(0);

}

#else

int user_exit(n,k,C,a,dobj,pobj,constant_offset,constraints,X,y,Z,params)
     int n;
     int k;
     struct blockmatrix C;
     double *a;
     double dobj;
     double pobj;
     double constant_offset;
     struct constraintmatrix *constraints;
     struct blockmatrix X;
     double *y;
     struct blockmatrix Z;     
     struct paramstruc params;
{
  return(0);
}

#endif
