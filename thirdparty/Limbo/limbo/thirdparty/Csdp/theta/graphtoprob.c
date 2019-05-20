/*
 
  Compute the Lovasz Theta number of a graph.  Usage is 
 
      theta <input graph>  
 
  where the input graph is in the following format:

     n                      # of nodes.
     m                      # of edges.
     i1 j1                  first edge.  
     i2 j2
      ...
     im jm                  last edge.


*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "declarations.h"

int main(argc,argv)
     int argc;
     char *argv[];
{
  int i;
  int j;
  int n;
  int m;
  FILE *fid;
  int start,finish;
  int temp;

  /*
   * Storage for the problem.
   */

  struct blockmatrix C;
  struct blockmatrix X,Z;
  double *y;
  double *a;
  struct constraintmatrix *constraints;

  /*
   * Check for an argument.
   */

  if ((argc < 3) || (argc >3)) 
    {
      printf("Usage: \n");
      printf("\n");
      printf("graphtoprob <graph> <problem file> \n");
      exit(1);
    };

  /*
   * Open up the file and read n and m.  Check the problem size.
   */

  fid=fopen(argv[1],"r");
  fscanf(fid,"%d",&n);
  fscanf(fid,"%d",&m);

  printf("Graph is of size %d %d \n",n,m);

  /*
   *  Setup the objective function and the constraints.
   */

  C.nblocks=1;
  C.blocks=malloc(2*sizeof(struct blockrec));
  if (C.blocks == NULL)
    {
      printf("Couldn't allocate storage for C!\n");
      exit(1);
    };

  C.blocks[1].blockcategory=MATRIX;
  C.blocks[1].blocksize=n;
  C.blocks[1].data.mat=malloc(n*n*sizeof(double));
  if (C.blocks[1].data.mat == NULL)
    {
      printf("Couldn't allocate storage for C!\n");
      exit(1);
    };

  for (i=1; i<=n; i++)
    for (j=1; j<=n; j++)
      C.blocks[1].data.mat[ijtok(i,j,n)]=1.0;

  /*
   * Just to keep free_prob happy.
   */

  alloc_mat(C,&X);
  alloc_mat(C,&Z);
  y=(double *)malloc(2*sizeof(double));

  /*
   *  There are m+1 constraints.
   */

  constraints=(struct constraintmatrix *)malloc(						(m+2)*sizeof(struct constraintmatrix));
  if (constraints==NULL)
    {
      printf("Failed to allocate storage for constraints!\n");
      exit(1);
    };

  a=(double *)malloc((m+2)*sizeof(double));
  if (a==NULL)
    {
      printf("Failed to allocate storage for a!\n");
      exit(1);
    };

  /*
   *  Constraint 1 says that Trace(X)=1.
   */

  a[1]=1.0;
  constraints[1].blocks=(struct sparseblock *)malloc(sizeof(struct sparseblock));

  constraints[1].blocks->blocknum=1;
  constraints[1].blocks->numentries=n;
  constraints[1].blocks->blocksize=n;
  constraints[1].blocks->constraintnum=1;
  constraints[1].blocks->next=NULL;
  constraints[1].blocks->nextbyblock=NULL;

  constraints[1].blocks->entries=(double *) malloc((n+1)*sizeof(double));
#ifdef NOSHORTS
  constraints[1].blocks->iindices=(int *) malloc((n+1)*sizeof(int));
  constraints[1].blocks->jindices=(int *) malloc((n+1)*sizeof(int));
#else
  constraints[1].blocks->iindices=(unsigned short *) malloc((n+1)*sizeof(unsigned short));
  constraints[1].blocks->jindices=(unsigned short *) malloc((n+1)*sizeof(unsigned short));
#endif

  for (i=1; i<=n; i++)
    {
      constraints[1].blocks->entries[i]=1.0;
      constraints[1].blocks->iindices[i]=i;
      constraints[1].blocks->jindices[i]=i;
    };

  /*
   *  Constraints 2 through m+1 enforce X(i,j)=0 when (i,j) is an edge.
   */

  for (i=2; i<=m+1; i++)
    {
      a[i]=0.0;
      fscanf(fid,"%d %d",&start,&finish);
      if (start > finish)
	{
	  temp=start;
	  start=finish;
	  finish=temp;
	};

      constraints[i].blocks=(struct sparseblock *)
	malloc(sizeof(struct sparseblock));

      constraints[i].blocks->blocknum=1;
      constraints[i].blocks->numentries=1;
      constraints[i].blocks->blocksize=n;
      constraints[i].blocks->constraintnum=i;
      constraints[i].blocks->next=NULL;
      constraints[i].blocks->nextbyblock=NULL;
      
      constraints[i].blocks->entries=(double *) malloc((2)*sizeof(double));
#ifdef NOSHORTS
      constraints[i].blocks->iindices=(int *) malloc((2)*sizeof(int));
      constraints[i].blocks->jindices=(int *) malloc((2)*sizeof(int));
#else
      constraints[i].blocks->iindices=(unsigned short *) malloc((2)*sizeof(unsigned short));
      constraints[i].blocks->jindices=(unsigned short *) malloc((2)*sizeof(unsigned short));
#endif

      constraints[i].blocks->entries[1]=1.0;
      constraints[i].blocks->iindices[1]=start;
      constraints[i].blocks->jindices[1]=finish;

    };

  /*
   * Close the input file.
   */

  fclose(fid);

  /*
   *  Write out the problem.
   */

  write_prob(argv[2],n,m+1,C,a,constraints);

  free_prob(n,m+1,C,a,constraints,X,y,Z);
  exit(0);

}








