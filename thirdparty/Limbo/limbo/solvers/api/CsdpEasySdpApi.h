/**
 * @file   CsdpEasySdpApi.h
 * @brief  this file is a modified version of easysdp.c in Csdp package.  
 *         Original version does not provide control over parameters and print level.  
 *         The function limbo::solvers::easy_sdp_ext passes two additional parameters for better flexibility.  
 *
 * @author Yibo Lin
 * @date   Oct 2014
 */
/*
 *  This is an easy to call version of the sdp routine.  It takes as
 *  input a problem (n,k,C,a,constraints,constant_offset), and an 
 *  initial solution (X,y,Z), allocates working storage, and calls sdp() 
 *  to solve the problem.  The solution is returned in X,y,Z,pobj,dobj, and 
 *  the return code from sdp is returned as the return value from easy_sdp.
 *
 */

#ifndef LIMBO_SOLVERS_CSDPEASYSDPAPI_H
#define LIMBO_SOLVERS_CSDPEASYSDPAPI_H

// must define NOSHORTS to forbid the usage of unsigned shorts in Csdp 
#define NOSHORTS

// necessary since we are linking C library 
#ifdef __cplusplus
extern "C" {
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limbo/thirdparty/Csdp/include/declarations.h>
#ifdef __cplusplus
}
#endif

/// namespace for Limbo 
namespace limbo 
{ 
/// namespace Limbo.Solvers
namespace solvers 
{

/// @brief API to call Csdp solver.  
/// 
/// This is a dummy template, 
/// so I do not need to write a .c file. 
/// See the documentation of Csdp for details on how to describe an SDP problem. 
/// 
/// @tparam T dummy data type, any type works 
/// @param n, k, C, a, constraints, constant_offset as input problem 
/// @param pX, py, pZ as initial solution and final solution  
/// @param ppobj as primal objective 
/// @param pdobj as dual objective 
/// @param params pass customized parameters to control the solver 
/// @param printlevel verbose level in printing 
/// @return the return code from sdp
template <typename T>
int easy_sdp_ext(
        // this part is same as original version 
        int n, 
        int k, 
        struct blockmatrix C, 
        double *a, 
        struct constraintmatrix *constraints, 
        double constant_offset, 
        struct blockmatrix *pX,
        double **py, 
        struct blockmatrix *pZ, 
        double *ppobj, 
        double *pdobj, 
        // newly added parameters 
        struct paramstruc const& params, 
        int const& printlevel
        )
{
  int ret;
  struct constraintmatrix fill;
  //struct paramstruc params; // commence out because we pass it as a parameter
  struct blockmatrix work1;
  struct blockmatrix work2;
  struct blockmatrix work3;
  struct blockmatrix bestx;
  struct blockmatrix bestz;
  struct blockmatrix Zi;
  struct blockmatrix dZ;
  struct blockmatrix dX;
  struct blockmatrix cholxinv;
  struct blockmatrix cholzinv;
  double *workvec1;
  double *workvec2;
  double *workvec3;
  double *workvec4;
  double *workvec5;
  double *workvec6;
  double *workvec7;
  double *workvec8;
  double *diagO; 
  double *Fp;
  double *O;
  double *dy;
  double *dy1;
  double *rhs;
  double *besty;
  //int printlevel; // commence out because we pass it as a parameter
  int ldam;
  struct sparseblock **byblocks;
  struct sparseblock *ptr;
  struct sparseblock *oldptr;
  int i;
  int j;
  //int blk; // commence out because compiler reports set but unused variable 
  struct sparseblock *p;
  struct sparseblock *q;
  struct sparseblock *prev=NULL;
  double gap;
  int nnz;

   /*
    *  Initialize the parameters.
    */

   //initparams(&params,&printlevel); // commence out because we pass it as a parameter

  /*
   *  Allocate working storage
   */

  alloc_mat(C,&work1);
  alloc_mat(C,&work2);
  alloc_mat(C,&work3);
  alloc_mat_packed(C,&bestx);
  alloc_mat_packed(C,&bestz);
  alloc_mat_packed(C,&cholxinv);
  alloc_mat_packed(C,&cholzinv);

  besty=(double *)malloc(sizeof(double)*(k+1));
   if (besty == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec1=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec1=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec1 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec2=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec2=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec2 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec3=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec3=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec3 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec4=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec4=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec4 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec5=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec5=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec5 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec6=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec6=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec6 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec7=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec7=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec7 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   if (n > k)
     {
       workvec8=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       workvec8=(double *)malloc(sizeof(double)*(k+1));
     };
   if (workvec8 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };


   if (n > k)
     {
       diagO=(double *)malloc(sizeof(double)*(n+1));
     }
   else
     {
       diagO=(double *)malloc(sizeof(double)*(k+1));
     };
   if (diagO == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };



   rhs=(double*)malloc(sizeof(double)*(k+1));
   if (rhs == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   dy=(double*)malloc(sizeof(double)*(k+1));
   if (dy == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   dy1=(double*)malloc(sizeof(double)*(k+1));
   if (dy1 == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   Fp=(double*)malloc(sizeof(double)*(k+1));
   if (Fp == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   /*
    *  Work out the leading dimension for the array.  Note that we may not
    *  want to use k itself, for cache issues.
    */
   if ((k % 2) == 0)
     ldam=k+1;
   else
     ldam=k;

   O=(double*)malloc(sizeof(double)*ldam*ldam);
   if (O == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   alloc_mat(C,&Zi);
   alloc_mat(C,&dZ);
   alloc_mat(C,&dX);

   /*
    *  Fill in lots of details in the constraints data structure that haven't
    *  necessarily been done before now.
    */

   /*
    * Set up the cross links used by op_o
    * While we're at it, determine which blocks are sparse and dense.
    */

   /*
    * Next, setup issparse and NULL out all nextbyblock pointers.
    */

   for (i=1; i<=k; i++)
     {
       p=constraints[i].blocks;
       while (p != NULL)
	 {
	   /*
	    * First, set issparse.
	    */
	   if (((p->numentries) > 0.25*(p->blocksize)) && ((p->numentries) > 15))
	     {
	       p->issparse=0;
	     }
	   else
	     {
	       p->issparse=1;
	     };
	   
	   if (C.blocks[p->blocknum].blockcategory == DIAG)
	     p->issparse=1;
	   
	   /*
	    * Setup the cross links.
	    */
	   
	   p->nextbyblock=NULL;
	   p=p->next;
	 };
     };
   
   /*
    * Now, cross link.
    */
   for (i=1; i<=k; i++)
     {
       p=constraints[i].blocks;
       while (p != NULL)
	 {
	   if (p->nextbyblock == NULL)
	     {
	       //blk=p->blocknum; // commence out because compiler reports set but unused variable 
	       
	       /*
		* link in the remaining blocks.
		*/
	       for (j=i+1; j<=k; j++)
		 {
		   q=constraints[j].blocks;
		   
		   while (q != NULL)
		     {
		       if (q->blocknum == p->blocknum)
			 {
			   if (p->nextbyblock == NULL)
			     {
			       p->nextbyblock=q;
			       q->nextbyblock=NULL;
			       prev=q;
			     }
			   else
			     {
			       prev->nextbyblock=q;
			       q->nextbyblock=NULL;
			       prev=q;
			     };
			   break;
			 };
		       q=q->next;
		     };
		 };
	     };
	   p=p->next;
	 };
     };

   /*
    * If necessary, print out information on sparsity of blocks.
    */
   
   if (printlevel >= 4)
     {
       for (i=1; i<=k; i++)
	 {
	   p=constraints[i].blocks;
	   while (p != NULL)
	     {
	       printf("%d,%d,%d,%d \n",i,p->blocknum,p->issparse,p->numentries);
	       p=p->next;
	     };
	 };
     };
   
   /*
    * Allocate space for byblocks pointers.
    */

   byblocks=(struct sparseblock **)malloc((C.nblocks+1)*sizeof(struct sparseblock *));
   if (byblocks == NULL)
     {
       printf("Storage Allocation Failed!\n");
       exit(10);
     };

   for (i=1; i<=C.nblocks; i++)
     byblocks[i]=NULL;

   /*
    * Fill in byblocks pointers.
    */
   for (i=1; i<=k; i++)
     {
       ptr=constraints[i].blocks;
       while (ptr != NULL)
	 {
	   if (byblocks[ptr->blocknum]==NULL)
	     {
	       byblocks[ptr->blocknum]=ptr;
	     };
	   ptr=ptr->next;
	 };
     };

   /*
    *  Compute "fill".  This data structure tells us which elements in the
    *  block diagonal matrix have corresponding elements in one of the
    *  constraints, and which constraint this element first appears in.
    *
    */

   makefill(k,C,constraints,&fill,work1,printlevel);

   /*
    * Compute the nonzero structure of O.
    */

   nnz=structnnz(n,k,C,constraints);

   if (printlevel >= 3)
     printf("Structural density of O %d, %e \n",nnz,nnz*1.0/(k*k*1.0));

   /*
    * Sort entries in diagonal blocks of constraints.
    */

   sort_entries(k,C,constraints);

   /*
    *  Now, call sdp().
    */

   ret=sdp(n,k,C,a,constant_offset,constraints,byblocks,fill,*pX,*py,*pZ,
	   cholxinv,cholzinv,ppobj,pdobj,work1,work2,work3,workvec1,
	   workvec2,workvec3,workvec4,workvec5,workvec6,workvec7,workvec8,
	   diagO,bestx,besty,bestz,Zi,O,rhs,dZ,dX,dy,dy1,Fp,
	   printlevel,params);

   if (printlevel >= 1)
     {
       if (ret==0)
	 printf("Success: SDP solved\n");
       if (ret==1)
	 printf("Success: SDP is primal infeasible\n");
       if (ret==2)
	 printf("Success: SDP is dual infeasible\n");
       if (ret==3)
	 printf("Partial Success: SDP solved with reduced accuracy\n");
       if (ret >= 4)
	 printf("Failure: return code is %d \n",ret);

       if (ret==1)
	 {
	   op_at(k,*py,constraints,work1);
	   addscaledmat(work1,-1.0,*pZ,work1);
	   printf("Certificate of primal infeasibility: a'*y=%.5e, ||A'(y)-Z||=%.5e\n",-1.0,Fnorm(work1));
	 };

       if (ret==2)
	 {
	   op_a(k,constraints,*pX,workvec1);
	   printf("Certificate of dual infeasibility: tr(CX)=%.5e, ||A(X)||=%.5e\n",trace_prod(C,*pX),norm2(k,workvec1+1));
	 };

       if ((ret==0) || (ret>=3))
	 {
	   if (printlevel >= 3)
	     {
	       printf("XZ Gap: %.7e \n",trace_prod(*pZ,*pX));
	       gap=*pdobj-*ppobj;
	       printf("Real Gap: %.7e \n",gap);
	     };

	   if (printlevel >= 1)
	     {
               gap=*pdobj-*ppobj;

	       printf("Primal objective value: %.7e \n",*ppobj);
	       printf("Dual objective value: %.7e \n",*pdobj);
	       printf("Relative primal infeasibility: %.2e \n",
		      pinfeas(k,constraints,*pX,a,workvec1));
	       printf("Relative dual infeasibility: %.2e \n",
		      dinfeas(k,C,constraints,*py,*pZ,work1));
	       printf("Real Relative Gap: %.2e \n",gap/(1+fabs(*pdobj)+fabs(*ppobj)));
	       printf("XZ Relative Gap: %.2e \n",trace_prod(*pZ,*pX)/(1+fabs(*pdobj)+fabs(*ppobj)));

	       printf("DIMACS error measures: %.2e %.2e %.2e %.2e %.2e %.2e\n",
		      pinfeas(k,constraints,*pX,a,workvec1)*(1+norm2(k,a+1))/
		      (1+norminf(k,a+1)),
		      0.0,
		      dimacserr3(k,C,constraints,*py,*pZ,work1),
		      0.0,
		      gap/(1+fabs(*pdobj)+fabs(*ppobj)),
		      trace_prod(*pZ,*pX)/(1+fabs(*pdobj)+fabs(*ppobj)));
	     };
	 };

     };

   /*
    *  Now, free up all of the storage.
    */

   free_mat(work1);
   free_mat(work2);
   free_mat(work3);
   free_mat_packed(bestx);
   free_mat_packed(bestz);
   free_mat_packed(cholxinv);
   free_mat_packed(cholzinv);

   free_mat(Zi);
   free_mat(dZ);
   free_mat(dX);

   free(besty);
   free(workvec1);
   free(workvec2);
   free(workvec3);
   free(workvec4);
   free(workvec5);
   free(workvec6);
   free(workvec7);
   free(workvec8);
   free(rhs);
   free(dy);
   free(dy1);
   free(Fp);
   free(O);
   free(diagO);
   free(byblocks);
 
   /*
    * Free up the fill data structure.
    */

   ptr=fill.blocks;
   while (ptr != NULL)
     {
       free(ptr->entries);
       free(ptr->iindices);
       free(ptr->jindices);
       oldptr=ptr;
       ptr=ptr->next;
       free(oldptr);
     };


  /*
   * Finally, free the constraints array.
   */

   return(ret);

}

}} // namespace limbo // namespace solvers

#endif
