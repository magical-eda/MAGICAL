/*
  Compute the complement of a graph and output it in our standard
  format.  
 
  Usage:
 
    complement <input graph> <output graph>
 
  
*/
#include <stdio.h>
#include <stdlib.h>
#include "declarations.h"

int main(argc,argv)
     int argc;
     char *argv[];
{
  FILE *fidin;
  FILE *fidout;
  int i;
  int j;
  int n;
  int m;
  int start;
  int finish;
  int *G;

  /*
   * Check for the right number of arguments.
   */
  
  if ((argc < 3) || (argc >4))
    {
      printf("Usage: complement <input graph> <output graph> \n");
      exit(1);
    }
  /*
   * Open the input and output files.
   */

  fidin=fopen(*++argv,"r");
  fidout=fopen(*++argv,"w");

  /*
   * Get the size parameters.
   */

  fscanf(fidin,"%d",&n);
  fscanf(fidin,"%d",&m);

  /*
   * Allocate the array.
   */

  G=(int *)malloc(n*n*sizeof(int));

  /*
   * Now, work the G array including every edge in the graph.
   */

  for (j=1; j<=n; j++)
    for (i=1; i<=n; i++)
      if (i == j)
	G[ijtok(i,j,n)]=0;
      else
	G[ijtok(i,j,n)]=1;

  /*
   * Read in the edges, and zero them out in G.
   */

  for (i=1; i<=m; i++)
    {
      fscanf(fidin,"%d %d",&start,&finish);
      G[ijtok(start,finish,n)]=0;
      G[ijtok(finish,start,n)]=0;
    };

  /*
   * Print out the resulting graph.
   */

  fprintf(fidout,"%d \n",n);
  fprintf(fidout,"%d \n",n*(n-1)/2-m);
  for (i=1; i<=n-1; i++)
    for (j=i+1; j<=n; j++)
      if (G[ijtok(i,j,n)] == 1)
	fprintf(fidout,"%d %d \n",i,j);

  /*
   * Close files and free memory.
   */

  fclose(fidin);
  fclose(fidout);
  free(G);

  exit(0);

}


