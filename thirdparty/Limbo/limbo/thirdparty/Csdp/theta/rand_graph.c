/*
  Compute the complement of a graph and output it in our standard
  format.  
 
  Usage:
 
    complement <input graph> <output graph>
 
  
*/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "declarations.h"

/*

 On some systems, RAND_MAX is undefined.  If so, use RAND_MAX=32768

*/

#ifndef RAND_MAX
#define RAND_MAX 32768
#endif

int main(argc,argv)
     int argc;
     char *argv[];
{
  FILE *fidout;
  int i;
  int j;
  int n;
  int m;
  int s;
  double p;
  int *G;

  /*
   * Get the parameters and open up the output file.
   */

  if (argc <= 3)
    {
      printf("Usage: rand_graph <file> <n> <p> [<seed>] \n");
      exit(1);
    };
  if (argc >= 4)
    {
      fidout=fopen(*++argv,"w");
      n=atoi(*++argv);
      p=atof(*++argv);
    };

  /*
   * Get the random number seed if specified.
   */

  if (argc >= 5)
    {
      s=atoi(*++argv);
      srand((unsigned int)s);
    };

  /*
   * Allocate storage for the graph.
   */

  G=(int *)malloc(n*n*sizeof(int));

  /*
   * Now, generate the graph.  
   */

  /*
   * The following kludge is here to deal with a bug that we've
   * encountered in stdlib.h under Solaris 2.5.  The problem is that
   * stdlib.h defines RAND_MAX as 2^15-1, while rand() actually returns
   * numbers between 0 and 2^31-1.  This is a violation of the ANSI
   * standard, but we can get around it by first taking rand() mod
   * RAND_MAX and then dividing by RAND_MAX.  The same code should
   * work fine if RAND_MAX is correctly defined.
   * 
   * Note too that on some systems (for example, SunOS 4.1.X, RAND_MAX
   * isn't defined in stdlib.h.  In such cases, we'll have to define the
   * symbol with -DRAND_MAX=<whatever> in the Makefile.
   *
   */

  m=0;
  for (i=1; i<=n-1; i++)
    for (j=i+1; j<=n; j++)
      if ((1.0*(rand() % RAND_MAX)/RAND_MAX) <= p)
	{
	  G[ijtok(i,j,n)]=1;
	  m++;
	}
      else
	{
	  G[ijtok(i,j,n)]=0;
	};


  /*
   * And print the graph out.
   */

  fprintf(fidout,"%d \n",n);
  fprintf(fidout,"%d \n",m);
  for (i=1; i<=n-1; i++)
    for (j=i+1; j<=n; j++)
      if (G[ijtok(i,j,n)] == 1)
	fprintf(fidout,"%d %d \n",i,j);


  /*
   * Close files and free memory.
   */

  fclose(fidout);
  free(G);

  exit(0);
}





