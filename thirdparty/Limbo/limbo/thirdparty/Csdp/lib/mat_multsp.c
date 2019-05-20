/*
 *  Compute C=scale1*A*B+scale2*C.  
 *  Note that C must consist of dense matrix and vector blocks- no sparse
 *  blocks or eye's or other special cases.
 *
 *  A and B can have blocks of all supported types.  Unsupported types
 *  generate exit(1).
 *
 *  It is assumed that all three matrices are of compatible block strucutre.
 *
 *  This version of mat_mult is specialized to deal with matrices B
 *  which have nonzero structure given by fill.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

#ifdef USEOPENMP
#include <omp.h>
#endif

/*
 * This #define parameter determines how sparse a block must be to be
 * considered sparse.  You can change it by compiling with 
 * -DSPARSELIM=<your value>
 *
 * 0.01 was selected without much tuning.  On maxG11, performance is 
 * better with SPARSELIM=0.01 than 0.0, but it isn't clear whether this
 * should be made larger.  
 *
 * For that matter, it isn't clear whether there should be separate
 * limits for each type of sparse matrix multiply.
 */
#ifndef SPARSELIMA
#define SPARSELIMA 0.01
#endif
#ifndef SPARSELIMB
#define SPARSELIMB 0.01
#endif
#ifndef SPARSELIMC
#define SPARSELIMC 0.01
#endif

void mat_multspb(scale1,scale2,A,B,C,fill)
     double scale1,scale2;
     struct blockmatrix A,B,C;
     struct constraintmatrix fill;
{
  int blk,i,ii,j;
  int blksize,p,q;
  struct sparseblock *ptr;
  double temp;
  int total_threads;
  int thread_num;

  if (scale2 == 0.0)
    {
      zero_mat(C);

      /*
       * if scale1 also is zero, then we just zero'd out C.
       */

      if (scale1 == 0.0)
	return;

      /*
       * Now, multiply A*B and add it in.
       */

      ptr=fill.blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;
	  
	  switch (A.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]=scale1*A.blocks[blk].data.vec[i]*
		  B.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
	      blksize=ptr->blocksize;

	      /*
	       * If this matrix is fairly dense, then don't bother with
	       * This approach.
	       */
	      if (ptr->numentries/(blksize*blksize*1.0) > SPARSELIMB)
		{
		  /*
		   * A dense block.  Do it the old fashioned way.
		   */ 
		  mat_mult_raw(blksize,scale1,scale2,A.blocks[blk].data.mat,
			       B.blocks[blk].data.mat,C.blocks[blk].data.mat);
		}
	      else
		{
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3)) 
		  __builtin_prefetch(ptr->next,0,3);
#endif
#endif

#ifdef USEOPENMP
#pragma omp parallel default(none) private(i,ii,p,q,thread_num,total_threads,temp) shared(ptr,A,B,C,blk,blksize,scale1)
 {
   total_threads=omp_get_num_threads();
   thread_num=omp_get_thread_num();
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      q=ptr->jindices[ii];
		      if ((q % total_threads)==thread_num)
			{
			  p=ptr->iindices[ii];
			  temp=scale1*B.blocks[blk].data.mat[ijtok(p,q,blksize)];
			  for (i=1; i<=blksize; i++)
			    C.blocks[blk].data.mat[ijtok(i,q,blksize)]+=temp*
			      A.blocks[blk].data.mat[ijtok(i,p,blksize)];
			};

		    };

 }
#pragma omp barrier
#else
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      temp=scale1*B.blocks[blk].data.mat[ijtok(p,q,blksize)];
		      for (i=1; i<=blksize; i++)
			C.blocks[blk].data.mat[ijtok(i,q,blksize)]+=temp*
			  A.blocks[blk].data.mat[ijtok(i,p,blksize)];
		    };


#endif

		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);
	    };
	  /*
	   * Move on to the next block.
	   */
	  ptr=ptr->next;
	};
    }
  else
    {
      /*
       * First, scale C by the scale 2 factor.
       */
      for (blk=1; blk<=C.nblocks; blk++)
	{
	  switch (C.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=C.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]=scale2*C.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
#pragma omp parallel for schedule(dynamic,64) default(none) private(i,j) shared(blk,C,scale2)
	      for (j=1; j<=C.blocks[blk].blocksize; j++)
		for (i=1; i<=C.blocks[blk].blocksize; i++)
		  C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)]=
		    scale2*C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)];
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);

	    };
	};

      /*
       * if scale1 is zero, then we're done.
       */

      if (scale1 == 0.0)
	return;

      /*
       * Now, multiply A*B and add it in.
       */

      ptr=fill.blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  switch (A.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]+=scale1*A.blocks[blk].data.vec[i]*
		  B.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
	      blksize=ptr->blocksize;

	      /*
	       * If this matrix is fairly dense, then don't bother with
	       * This approach.
	       */
	      if (ptr->numentries/(blksize*blksize*1.0) > SPARSELIMB)
		{
		  /*
		   * A dense block.  Do it the old fashioned way.
		   */ 
		  mat_mult_raw(blksize,scale1,1.0,A.blocks[blk].data.mat,
			       B.blocks[blk].data.mat,C.blocks[blk].data.mat);
		}
	      else
		{
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3)) 
		  __builtin_prefetch(ptr->next,0,3);
#endif
#endif
#ifdef USEOPENMP
#pragma omp parallel default(none) private(i,ii,p,q,thread_num,total_threads,temp) shared(ptr,A,B,C,blk,blksize,scale1)
 {
   total_threads=omp_get_num_threads();
   thread_num=omp_get_thread_num();
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      q=ptr->jindices[ii];
		      if ((q % total_threads)==thread_num)
			{
			  p=ptr->iindices[ii];
			  temp=scale1*B.blocks[blk].data.mat[ijtok(p,q,blksize)];
			  for (i=1; i<=blksize; i++)
			    C.blocks[blk].data.mat[ijtok(i,q,blksize)]+=temp*
			      A.blocks[blk].data.mat[ijtok(i,p,blksize)];
			};

		    };

 }
#pragma omp barrier
#else

		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      temp=scale1*B.blocks[blk].data.mat[ijtok(p,q,blksize)];
		      for (i=1; i<=blksize; i++)
			C.blocks[blk].data.mat[ijtok(i,q,blksize)]+=temp*
			  A.blocks[blk].data.mat[ijtok(i,p,blksize)];
		    };
#endif
		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);
	    };

	  /*
	   * Move on to the next block.
	   */
	  ptr=ptr->next;
	};
    };

}

/*
 *  This version of mat_mult is specialized for sparse A matrices.
 */

void mat_multspa(scale1,scale2,A,B,C,fill)
     double scale1,scale2;
     struct blockmatrix A,B,C;
     struct constraintmatrix fill;
{
  int blk,i,j,ii;
  int blksize,p,q;
  struct sparseblock *ptr;
  double temp;
  int total_threads;
  int thread_num;

  if (scale2 == 0.0)
    {
      zero_mat(C);

      /*
       * if scale1 also is zero, then we just zero'd out C.
       */

      if (scale1 == 0.0)
	return;

      /*
       * Now, multiply A*B and add it in.
       */

      ptr=fill.blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  switch (A.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]=scale1*A.blocks[blk].data.vec[i]*
		  B.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
	      blksize=ptr->blocksize;

	      /*
	       * If this matrix is fairly dense, then don't bother with
	       * This approach.
	       */
	      if (ptr->numentries/(blksize*blksize*1.0) > SPARSELIMA)
		{
		  /*
		   * A dense block.  Do it the old fashioned way.
		   */ 
		  mat_mult_raw(blksize,scale1,scale2,A.blocks[blk].data.mat,
			       B.blocks[blk].data.mat,C.blocks[blk].data.mat);
		}
	      else
		{
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3)) 
		  __builtin_prefetch(ptr->next,0,3);
#endif
#endif

#ifdef USEOPENMP
#pragma omp parallel default(none) private(i,ii,p,q,thread_num,total_threads,temp) shared(ptr,A,B,C,blk,blksize,scale1)
 {
   total_threads=omp_get_num_threads();
   thread_num=omp_get_thread_num();

		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      if ((p % total_threads) == thread_num)
			{
			  q=ptr->jindices[ii];
			  temp=scale1*A.blocks[blk].data.mat[ijtok(p,q,blksize)];
			  for (i=1; i<=ptr->blocksize; i++)
			    C.blocks[blk].data.mat[ijtok(p,i,blksize)]+=temp*
			      B.blocks[blk].data.mat[ijtok(i,q,blksize)];
			};


		    };
 }
#pragma omp barrier
#else
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      temp=scale1*A.blocks[blk].data.mat[ijtok(p,q,blksize)];
		      for (i=1; i<=ptr->blocksize; i++)
			C.blocks[blk].data.mat[ijtok(p,i,blksize)]+=temp*
			  B.blocks[blk].data.mat[ijtok(i,q,blksize)];

		    };
#endif
		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);
	    };

	  /*
	   * Move on to the next block.
	   */
	  ptr=ptr->next;
	};
    }
  else
    {
      /*
       * First, scale C by the scale 1 factor.
       */

      for (blk=1; blk<=C.nblocks; blk++)
	{
	  switch (C.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=C.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]=scale2*C.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
#pragma omp parallel for default(none) schedule(dynamic,64) private(i,j) shared(blk,C,scale2)
	      for (j=1; j<=C.blocks[blk].blocksize; j++)
		for (i=1; i<=C.blocks[blk].blocksize; i++)
		  C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)]=
		    scale2*C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)];
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);

	    };
	};

      /*
       * if scale1 is zero, then we're done.
       */

      if (scale1 == 0.0)
	return;

      /*
       * Now, multiply A*B and add it in.
       */


      ptr=fill.blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  switch (A.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]+=scale1*A.blocks[blk].data.vec[i]*
		  B.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
	      blksize=ptr->blocksize;

	      /*
	       * If this matrix is fairly dense, then don't bother with
	       * This approach.
	       */
	      if (ptr->numentries/(blksize*blksize*1.0) > SPARSELIMA)
		{
		  /*
		   * A dense block.  Do it the old fashioned way.
		   */ 
		  mat_mult_raw(blksize,scale1,1.0,A.blocks[blk].data.mat,
			       B.blocks[blk].data.mat,C.blocks[blk].data.mat);
		}
	      else
		{
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3)) 
		  __builtin_prefetch(ptr->next,0,3);
#endif
#endif

#ifdef USEOPENMP
#pragma omp parallel default(none) private(i,ii,p,q,thread_num,total_threads,temp) shared(ptr,A,B,C,blk,blksize,scale1)
 {
   total_threads=omp_get_num_threads();
   thread_num=omp_get_thread_num();

		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      if ((p % total_threads) == thread_num)
			{
			  q=ptr->jindices[ii];
			  temp=scale1*A.blocks[blk].data.mat[ijtok(p,q,blksize)];
			  for (i=1; i<=ptr->blocksize; i++)
			    C.blocks[blk].data.mat[ijtok(p,i,blksize)]+=temp*
			      B.blocks[blk].data.mat[ijtok(i,q,blksize)];
			};


		    };
 }
#pragma omp barrier
#else
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      temp=scale1*A.blocks[blk].data.mat[ijtok(p,q,blksize)];
		      for (i=1; i<=ptr->blocksize; i++)
			C.blocks[blk].data.mat[ijtok(p,i,blksize)]+=temp*
			  B.blocks[blk].data.mat[ijtok(i,q,blksize)];
		    };
#endif

		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);
	    };

	  /*
	   * Move on to the next block.
	   */

	  ptr=ptr->next;
	};
    };

}


/*
 *  This version of mat_mult is specialized for sparse C matrices.
 *  It only generates the elements in the result C corresponding 
 *  to elements described in fill.  
 */

void mat_multspc(scale1,scale2,A,B,C,fill)
     double scale1,scale2;
     struct blockmatrix A,B,C;
     struct constraintmatrix fill;
{
  int blk,i,j,ii;
  int blksize,p,q;
  struct sparseblock *ptr;
  double temp;

  /*
   * To protect against bad implementations of the BLAS that don't handle
   * scale2=0 in dgemv well.
   */

  if (scale2 == 0.0)
    {

      /*
       * To protect against bad implementations of the BLAS that don't handle
       * scale2=0 in dgemv well.
       */

      zero_mat(C);


      /*
       * Now, multiply A*B and add it in.
       */

      ptr=fill.blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  switch (A.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]=scale1*A.blocks[blk].data.vec[i]*
		  B.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
	      blksize=ptr->blocksize;

	      /*
	       * If this matrix is fairly dense, then don't bother with
	       * This approach.
	       */
	      if (ptr->numentries/(blksize*blksize*1.0) > SPARSELIMC)
		{
		  /*
		   * A dense block.  Do it the old fashioned way.
		   */ 
		  mat_mult_raw(blksize,scale1,scale2,A.blocks[blk].data.mat,
			       B.blocks[blk].data.mat,C.blocks[blk].data.mat);
		}
	      else
		{
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3)) 
		  __builtin_prefetch(ptr->next,0,3);
#endif
#endif
#pragma omp parallel for schedule(dynamic,64) default(none) private(i,ii,p,q,temp) shared(ptr,A,B,C,blk,blksize,scale1)
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      temp=0;
		      for (i=1; i<=ptr->blocksize; i++)
			temp+=
			  A.blocks[blk].data.mat[ijtok(i,p,blksize)]*
			  B.blocks[blk].data.mat[ijtok(i,q,blksize)];
		      C.blocks[blk].data.mat[ijtok(p,q,blksize)]=temp*scale1;
		    };
		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);
	    };


	  /*
	   * Move on to the next block.
	   */
	  ptr=ptr->next;
	};
    }
  else
    {
      /*
       * First, scale C by the scale 2 factor.
       */
      for (blk=1; blk<=C.nblocks; blk++)
	{
	  switch (C.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=C.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]=scale2*C.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
#pragma omp parallel for default(none) schedule(dynamic,64) private(i,j) shared(blk,C,scale2)
	      for (j=1; j<=C.blocks[blk].blocksize; j++)
		for (i=1; i<=C.blocks[blk].blocksize; i++)
		  C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)]=
		    scale2*C.blocks[blk].data.mat[ijtok(i,j,C.blocks[blk].blocksize)];
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);

	    };
	};
      /*
       * Now, multiply A*B and add it in.
       */

      ptr=fill.blocks;
      while (ptr != NULL)
	{
	  blk=ptr->blocknum;

	  switch (A.blocks[blk].blockcategory)
	    {
	    case DIAG:
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		C.blocks[blk].data.vec[i]+=scale1*A.blocks[blk].data.vec[i]*
		  B.blocks[blk].data.vec[i];
	      break;
	    case MATRIX:
	      blksize=ptr->blocksize;

	      /*
	       * If this matrix is fairly dense, then don't bother with
	       * This approach.
	       */
	      if (ptr->numentries/(blksize*blksize*1.0) > SPARSELIMC)
		{
		  /*
		   * A dense block.  Do it the old fashioned way.
		   */ 
		  mat_mult_raw(blksize,scale1,scale2,A.blocks[blk].data.mat,
			       B.blocks[blk].data.mat,C.blocks[blk].data.mat);
		}
	      else
		{
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3)) 
		  __builtin_prefetch(ptr->next,0,3);
#endif
#endif
		  for (ii=1; ii<=ptr->numentries; ii++)
		    {
		      /*
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      for (i=1; i<=ptr->blocksize; i++)
			C.blocks[blk].data.mat[ijtok(p,q,blksize)]+=scale1*
			  A.blocks[blk].data.mat[ijtok(p,i,blksize)]*
			  B.blocks[blk].data.mat[ijtok(i,q,blksize)];
		      */
		      p=ptr->iindices[ii];
		      q=ptr->jindices[ii];
		      temp=0;
		      for (i=1; i<=ptr->blocksize; i++)
			temp+=
			  A.blocks[blk].data.mat[ijtok(i,p,blksize)]*
			  B.blocks[blk].data.mat[ijtok(i,q,blksize)];
		      C.blocks[blk].data.mat[ijtok(p,q,blksize)]+=temp*scale1;
		    };
		};
	      break;
	    case PACKEDMATRIX:
	    default:
	      printf("mat_multsp illegal block type \n");
	      exit(12);
	    };

	  /*
	   * Move on to the next block.
	   */
	  ptr=ptr->next;
	};

    };
}





