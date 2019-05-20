/*
 * Compute a matrix representation of the operator 
 *
 * O(.)=A(inv(Z)*A'(.)*X) 
 *
 * The ith colum of the result is O(e_i), where e_i is the vector with a 1 in
 * position i and 0's elsewhere.  
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef USEOPENMP
#include <omp.h>
#endif
#include "declarations.h"


void
op_o(k, constraints, byblocks, Zi, X, O, work1, work2)
  int             k;
  struct constraintmatrix *constraints;
  struct sparseblock **byblocks;
  struct blockmatrix Zi;
  struct blockmatrix X;
  double         *O;
  struct blockmatrix work1;
  struct blockmatrix work2;
{
  int             i, j;
  int             ii, jj;
  int             ldam;
  int             p, q, r, s;

  struct sparseblock *ptri;
  struct sparseblock *ptrj;

  int             blocknum;
  int             blocksize;
  double          contrib;
  double         *Ziblk;
  double         *Xblk;
  double         *workblk;
  double         *work2blk;
  double          enti, entj, scale1, scale2;

  int             max_blknum = 0, max_blksize = 0;
  int             thread_num;
  /* static double **work; // modified by Yibo Lin for external threading safe */
  double **work; 

  int max_threads;

  /*
   * Get the maximum number of threads.
   */

#ifdef USEOPENMP
  max_threads=omp_get_max_threads();
#else
  max_threads=1;
#endif

  /* Allocate more memory for the number of work matrices needed. */

  /* Find the largest nondiagonal block */
  for (blocknum = 1; blocknum <= X.nblocks; blocknum++) 
    {
      if (X.blocks[blocknum].blockcategory != DIAG
	  && X.blocks[blocknum].blocksize > max_blksize) 
	{
	  max_blknum = blocknum;
	  max_blksize = X.blocks[blocknum].blocksize;
	};
    };
    

  /*
   * If we have at least one dense block, we must allocate memory for work
   * matrices. 
   */

  if (max_blknum > 0) 
    {
      work = (double **) malloc(sizeof(double *) *
				(max_threads * 2 + 1));
      if (work == NULL) 
	{
	  printf("Failed to allocate memory for parallel execution (1)!\n");
	  printf("omp_get_max_threads() was %d \n",max_threads);
	  exit(10);
	}
      work[1] = work1.blocks[max_blknum].data.mat;
      work[2] = work2.blocks[max_blknum].data.mat;
      for (i = 1; i < max_threads; i++) 
	{
	  work[i * 2 + 1] = (double *) malloc(sizeof(double) *
					      X.blocks[max_blknum].blocksize *
					      X.blocks[max_blknum].blocksize);
	  work[i * 2 + 2] = (double *) malloc(sizeof(double) *
					      X.blocks[max_blknum].blocksize *
					      X.blocks[max_blknum].blocksize);
	  if (work[i * 2 + 1] == NULL || work[i * 2 + 2] == NULL) 
	    {
	      printf("Failed to allocate memory for parallel execution (2)!\n");
	      printf("max_blksize is %d \n",max_blksize);
	      printf("omp_get_max_threads() was %d \n",max_threads);
	      exit(10);
	    }; 
	}; 
    }; 


  /*
   * Work out the leading dimension for the array.  Note that we may not want
   * to use k itself, for cache issues. 
   */
  if ((k % 2) == 0)
    ldam = k + 1;
  else
    ldam = k;

  /*
   * First, zero out the O matrix. 
   */
#pragma omp parallel for schedule(dynamic,64) default(none) shared(O,ldam,k) private(j,i)
  for (j = 1; j <= k; j++)
    for (i = 1; i <= k; i++)
      O[ijtok(i, j, ldam)] = 0.0;

  /* Loop over i, then the blocks, then j */
#ifdef USEOPENMP
#ifdef SETNUMTHREADS
  omp_set_num_threads(omp_get_max_threads());
#endif
#endif

#pragma omp parallel for schedule(dynamic,64) default(none) shared(k,constraints,byblocks,Zi,X,O,work1, work2, work, ldam) private(j, ii, jj, blocksize, p, q, r, s, ptri, ptrj, blocknum, contrib, Ziblk, Xblk, workblk, work2blk, enti, entj, scale1, scale2, thread_num)
  for (i = 1; i <= k; i++) 
    {

#ifdef USEOPENMP
#ifdef SETNUMTHREADS
      /* Only use one thread in the inner loop. */
      omp_set_num_threads(1);
#endif
#endif

      ptri = constraints[i].blocks;
      while (ptri != NULL) 
	{
	  blocknum = ptri->blocknum;
	  blocksize = ptri->blocksize;
	  
	  /* Diagonal blocks */
	  if (ptri->issparse == 1 &&
	      X.blocks[blocknum].blockcategory == DIAG)
	    {
	      Ziblk = Zi.blocks[blocknum].data.vec;
	      Xblk = X.blocks[blocknum].data.vec;
	      
	      ptrj = ptri;
	      while (ptrj != NULL) 
		{
		  j = ptrj->constraintnum;
		  /*
		   * Do the contribution from constraints i and j of this block. 
		   */
		  contrib = 0.0;
		  p = 1;
		  q = 1;
		  while ((p <= ptri->numentries) &&
			 (q <= ptrj->numentries)) 
		    {
		      if (ptri->iindices[p] < ptrj->iindices[q]) 
			{
			  p = p + 1;
			} 
		      else 
			{
			  if (ptri->iindices[p] > ptrj->iindices[q]) 
			    {
			      q = q + 1;
			    } 
			  else 
			    {
			      /*
			       * Match! 
			       */
			      contrib += ptri->entries[p] * ptrj->entries[q] *
				Ziblk[ptri->iindices[p]] *
				Xblk[ptri->iindices[p]];
			      p = p + 1;
			      q = q + 1;
			    };
			};
		    };
		  
		  O[ijtok(i, j, ldam)] += contrib;
		  
		  /* Next j */
		  ptrj = ptrj->nextbyblock;
		}
	    }
	  /* Sparse matrix block */
	  else if (ptri->issparse == 1 &&
		   X.blocks[blocknum].blockcategory == MATRIX)
	    {
	      Ziblk = Zi.blocks[blocknum].data.mat;
	      Xblk = X.blocks[blocknum].data.mat;
	      
	      ptrj = ptri;
	      while (ptrj != NULL) 
		{
		  
		  /* Only process sparse-sparse pairs. */
		  if (ptrj->issparse == 1) 
		    {
		      j = ptrj->constraintnum;
		      
		      /*
		       * The following prefetch seems to give about a 5% performance
		       * improvement on certain problems. e.g. truss8 on a 1200 Mhz
		       * Athlon. 
		       *
		       * It won't be compiled unless we're using gcc. 
		       */
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3))
		      __builtin_prefetch(ptrj->nextbyblock, 0, 3);
#endif
#endif
		      
		      /*
		       * Do the contribution from constraints i and j of this block. 
		       */
		      contrib = 0.0;
		      for (ii = 1; ii <= ptri->numentries; ii++) 
			{
			  
			  enti = ptri->entries[ii];
			  p = ptri->iindices[ii];
			  q = ptri->jindices[ii];
			  
			  /*
			   * We'll keep the p==q test outside of the inner loop. 
			   *
			   * In what follows, we've made use of the symmetry of Ziblk and
			   * Xblk, by permuting all array indices so that p and q are
			   * last. This means that we stay in columns p and q of Ziblk
			   * and Xblk as much as possible, improving locality. 
			   *
			   */
			  
			  if (p == q) 
			    {
			      for (jj = 1; jj <= ptrj->numentries; jj++) 
				{
				  entj = ptrj->entries[jj];
				  r = ptrj->iindices[jj];
				  s = ptrj->jindices[jj];
				  
				  if (r == s) 
				    {
				      /* here p==q, r==s */
				      contrib += enti * entj *
					Ziblk[ijtok(r, q, blocksize)] *
					Xblk[ijtok(s, p, blocksize)];
				    } else {
				      /* here p=q, r!=s */
				      contrib += enti * entj *
					(Ziblk[ijtok(r, q, blocksize)] *
					 Xblk[ijtok(s, p, blocksize)]
					 +
					 Ziblk[ijtok(s, q, blocksize)] *
					 Xblk[ijtok(r, p, blocksize)]);
				    }
				}
			    } 
			  else 
			    {		/* p!= q */
			      for (jj = 1; jj <= ptrj->numentries; jj++) 
				{
				  entj = ptrj->entries[jj];
				  r = ptrj->iindices[jj];
				  s = ptrj->jindices[jj];
				  
				  if (r == s) 
				    {
				      /* p!=q, but r=s */
				      contrib += enti * entj *
					(Ziblk[ijtok(r, q, blocksize)] *
					 Xblk[ijtok(s, p, blocksize)]
					 +
					 Ziblk[ijtok(r, p, blocksize)] *
					 Xblk[ijtok(s, q, blocksize)]);
				    } 
				  else 
				    {
				      /* here, p!=q and r!=s */
				      contrib += enti * entj *
					(Ziblk[ijtok(r, q, blocksize)] *
					 Xblk[ijtok(s, p, blocksize)] +
					 Ziblk[ijtok(r, p, blocksize)] *
					 Xblk[ijtok(s, q, blocksize)] +
					 Ziblk[ijtok(s, q, blocksize)] *
					 Xblk[ijtok(r, p, blocksize)] +
					 Ziblk[ijtok(s, p, blocksize)] *
					 Xblk[ijtok(r, q, blocksize)]);
				    }
				}
			    }
			}
		      
		      O[ijtok(i, j, ldam)] += contrib;
		    }
		  ptrj = ptrj->nextbyblock;
		}
	    }
	  /* Dense matrix block */
	  else 
	    {
	      /*
	       * put this block into a work matrix. 
	       */
	      
#ifdef USEOPENMP
	      thread_num = omp_get_thread_num();
#else
	      thread_num=0;
#endif
	      workblk = work[thread_num * 2 + 1];
	      work2blk = work[thread_num * 2 + 2];
	      Xblk = X.blocks[blocknum].data.mat;
	      Ziblk = Zi.blocks[blocknum].data.mat;
	      for (ii = 0; ii <= blocksize * blocksize - 1; ii++) 
		{
		  workblk[ii] = 0.0;
		}
	      for (ii = 1; ii <= ptri->numentries; ii++) 
		{
		  enti = ptri->entries[ii];
		  p = ptri->iindices[ii];
		  q = ptri->jindices[ii];
		  workblk[ijtok(p, q, blocksize)] = enti;
		  if (p != q)
		    workblk[ijtok(q, p, blocksize)] = enti;
		}

	      /*
	       * Now, multiply out Zi*work*X. 
	       */
	      scale1 = 1.0;
	      scale2 = 0.0;

#ifdef USEATLAS
	      mat_mult_rawatlas(blocksize, scale1, scale2, Ziblk, workblk, work2blk);
	      mat_mult_rawatlas(blocksize, scale1, scale2, work2blk, Xblk, workblk);
#else
	      mat_mult_raw(blocksize, scale1, scale2, Ziblk, workblk, work2blk);
	      mat_mult_raw(blocksize, scale1, scale2, work2blk, Xblk, workblk);
#endif	      

	      ptrj = byblocks[blocknum];
	      while (ptrj != NULL) 
		{
		  j = ptrj->constraintnum;
		
		  /*
		   * Compute if block j is sparse; block j is dense, but block i has
		   * more elements; or, if both blocks have the same number of
		   * elements, and the constraint number of block i is less than or
		   * equal to j 
		   */
		  if (ptrj->issparse == 1 ||
		      ptri->numentries > ptrj->numentries ||
		      (ptri->numentries == ptrj->numentries && i <= j))
		    {
		      contrib = 0.0;
		      
		      /*
		       * Another GCC prefetch for improved performance. It won't be
		       * compiled unless we're using gcc. 
		       */
#ifdef __GNUC__
#if (((__GNUC__ == 3) && (__GNUC_MINOR__ > 1)) || (__GNUC__ > 3))
		      __builtin_prefetch(ptrj->nextbyblock, 0, 3);
#endif
#endif
		      
		      for (ii = 1; ii <= ptrj->numentries; ii++) 
			{
			  entj = ptrj->entries[ii];
			  p = ijtok(ptrj->iindices[ii], ptrj->jindices[ii], blocksize);
			  q = ijtok(ptrj->jindices[ii], ptrj->iindices[ii], blocksize);
			  contrib += entj * workblk[p];
			  if (p != q) 
			    {
			      contrib += entj * workblk[q];
			    };
			};
		      
		      /*
		       * Depending on the relative values of i and j, this may leave
		       * the update below the diagonal.  However, we'll go back through
		       * and pickup such updates at the end.  By doing this, we 
		       * ensure that there are no conflicts between threads- each row
		       * of O belongs to only one thread of gang handling the for i 
		       * loop, so we don't need to use #pragma atomic here.
		       */
		      
		      O[ijtok(i,j,ldam)] +=contrib;
		      
		    }
		  ptrj = ptrj->nextbyblock;
		}
	    }
	  
	  ptri = ptri->next;
	}
    }

  /* Make sure the number of threads is at maximum. */
#ifdef USEOPENMP
#ifdef SETNUMTHREADS
  omp_set_num_threads(omp_get_max_threads());
#endif
#endif

  /*
   * Go back and pick up any updates that were left below the diagonal.
   */

#pragma omp parallel for schedule(dynamic,64) default(none) shared(O,ldam,k) private(j,i)
  for (j = 2; j <= k; j++)
    for (i = 1; i < j; i++)
      O[ijtok(i, j, ldam)] += O[ijtok(j,i,ldam)];

  /*
   * Free storage allocated for parallel work space.
   */

  if (max_blknum>0)
    {
      for (i=1; i< max_threads; i++)
	{
	  free(work[i*2+1]);
	  free(work[i*2+2]);
	};
      free(work);
    };

}
