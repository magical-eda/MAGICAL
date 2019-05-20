#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "declarations.h"

/*
 * Stuff for keeping track of time.
 */


#ifdef USEGETTIME
#include <stddef.h>      /* definition of NULL */
#include <sys/time.h>   /* definition of timeval struct and protyping of gettime
			   ofday */

extern double starttime;
extern double opotime;
extern double factortime;
extern double othertime;
extern double totaltime;
extern double othertime1;
extern double othertime2;
extern double othertime3;
extern struct timeval tp;
extern double t1;
extern double t2;
extern double starttime;
extern double endtime;

#endif

int main(argc,argv)
     int argc;
     char *argv[];
{
  int ret;
  int n;
  int k;
  struct blockmatrix C;
  double *a;
  struct constraintmatrix *constraints;
  struct blockmatrix X,Z;
  double *y;
  double pobj,dobj;

#ifdef USEGETTIME
  gettimeofday(&tp, NULL);
  starttime=(double)tp.tv_sec+(1.e-6)*tp.tv_usec;
#endif

  /*
   * Check that we have enough arguments.
   */

  if ((argc < 2) || (argc > 4)) 
    {
      printf("CSDP 6.1.1\n");
      printf(" \n");
      printf("Usage: \n");
      printf("\n");
      printf("csdp <input problem> [<final solution>] [<initial solution>]\n");
      exit(1);
    };


  /*
   * First, read the problem in. 
   */
  ret=read_prob(argv[1],&n,&k,&C,&a,&constraints,1);

  if (ret != 0)
    {
      printf("Giving up.\n");
      exit(10);
    };

  if (argc == 4)
    {
      ret=read_sol(argv[3],n,k,C,&X,&y,&Z);
      if (ret != 0)
	{
	  printf("Giving up.\n");
	  exit(10);
	};
    }
  else
    {
      initsoln(n,k,C,a,constraints,&X,&y,&Z);
    };

  /*
   * Call the solver.
   */

  ret=easy_sdp(n,k,C,a,constraints,0.0,&X,&y,&Z,&pobj,&dobj);

  /*
   * Write out the solution if necessary.
   */
  if (argc >= 3)
    write_sol(argv[2],n,k,X,y,Z);

  /*
   * Free up memory.
   */

  free_prob(n,k,C,a,constraints,X,y,Z);

#ifdef USEGETTIME
  gettimeofday(&tp, NULL);
  endtime=(double)tp.tv_sec+(1.e-6)*tp.tv_usec;
  totaltime=endtime-starttime;
  othertime=totaltime-opotime-factortime;

  printf("Elements time: %f \n",opotime);
  printf("Factor time: %f \n",factortime);
  printf("Other time: %f \n",othertime);
  printf("Total time: %f \n",totaltime);
#endif

  return(ret);
}

