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
 *  We use dgemm to do the actual work on normal dense blocks.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include "declarations.h"

void mat_mult(scale1,scale2,A,B,C)
     double scale1,scale2;
     struct blockmatrix A,B,C;
{
  int blk,i,n;
  double *ap;
  double *bp;
  double *cp;

  /*
   * In theory, the BLAS ensures that if scale2=0, then C will not be 
   * accessed before being written to.  In practice, this is not always 
   * true, so we initilize C to zeros for safety.
   */

  if (scale2 == 0.0)
    zero_mat(C);


  /*
   * Work through the blocks one at a time.
   */
  
  for (blk=1; blk<=A.nblocks; blk++)
    {
      switch (A.blocks[blk].blockcategory) 
	{
	case DIAG:
          if (scale2 != 0.0)
	    {
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		{
		  C.blocks[blk].data.vec[i]=scale1*A.blocks[blk].data.vec[i]
		    *B.blocks[blk].data.vec[i]
		    +scale2*C.blocks[blk].data.vec[i];
		};
	    }
	  else
	    {
	      for (i=1; i<=A.blocks[blk].blocksize; i++)
		{
		  C.blocks[blk].data.vec[i]=scale1*A.blocks[blk].data.vec[i]
		    *B.blocks[blk].data.vec[i];
		};
	    };
	  break;
	case MATRIX:
	  /*
	   * Call dgemm to do the matrix multiplication.
	   */
	  
	  n=A.blocks[blk].blocksize;
	  ap=A.blocks[blk].data.mat;
	  bp=B.blocks[blk].data.mat;
	  cp=C.blocks[blk].data.mat;

	  mat_mult_raw(n,scale1,scale2,ap,bp,cp);
	  break;
	default:
	  printf("mat_mult illegal block type!\n");
	  exit(12);
	};

    };
}

void mat_mult_raw(n,scale1,scale2,ap,bp,cp)
     int n;
     double scale1;
     double scale2;
     double *ap;
     double *bp;
     double *cp;
{
#ifdef NOUNDERBLAS
#ifdef CAPSBLAS
	  DGEMM("N","N",&n,&n,&n,&scale1,ap,&n,bp,&n,&scale2,cp,&n);
#else
	  dgemm("N","N",&n,&n,&n,&scale1,ap,&n,bp,&n,&scale2,cp,&n);
#endif
#else
#ifdef CAPSBLAS
	  DGEMM_("N","N",&n,&n,&n,&scale1,ap,&n,bp,&n,&scale2,cp,&n);
#else
	  dgemm_("N","N",&n,&n,&n,&scale1,ap,&n,bp,&n,&scale2,cp,&n);
#endif
#endif

}

#ifdef USEATLAS


void mat_mult_rawatlas(n,scale1,scale2,ap,bp,cp)
     int n;
     double scale1;
     double scale2;
     double *ap;
     double *bp;
     double *cp;
{
enum CBLAS_ORDER {CblasRowMajor=101, CblasColMajor=102 };
enum CBLAS_TRANSPOSE {CblasNoTrans=111, CblasTrans=112, CblasConjTrans=113,
		      AtlasConj=114};

void  ATL_dgemm(enum CBLAS_TRANSPOSE transa,enum CBLAS_TRANSPOSE transb,
                int m,int n, int k,double scale1,double *ap,int l,double *bp,
                int p,double scale2,double *cp,int q);

 ATL_dgemm(CblasNoTrans,CblasNoTrans,n,n,n,scale1,ap,n,bp,n,scale2,cp,n);
}

#endif

