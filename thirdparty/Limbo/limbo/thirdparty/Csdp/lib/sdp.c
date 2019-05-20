/*  
  Solve a semidefinite programming problem using the algorithm of Helmberg,
  Rendl, Vanderbei, and Wolkowicz.  

  Note: This version of the code uses predictor correct steps.

  Usage:
 

  Return codes:
 
    0             Success.
    1             Success.  The problem is primal infeasibile, and we
                            have a certificate.
    2             Success.  The problem is dual infeasible, and we have 
                            a certificate.

    3.            Partial Success.  Didn't reach full accuracy.

    4             Failure: Maximum iterations reached.
    5             Failure: Stuck at edge of primal feasibility.
    6             Failure: Stuck at edge of dual feasibility.
    7             Failure: Lack of progress
    8             Failure: X, Z, or O was singular.
    9             Failure: Detected NaN or Inf values.


  Notes on data storage:  All "2-d" arrays are stored in Fortran style,
  column major order.  macros in index.h are used to handle indexing into
  C 1-d vectors.  All "1-d" arrays are stored in C vectors, with the first
  element of the vector (index 0) unused.  We always start indexing in 
  both one and two dimensional arrays with 1, ala Fortran.  This makes
  the C code somewhat clearer, but requires us to pass v+1 into a Fortran
  subroutine instead of just passing v.  

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "declarations.h"

#ifdef USEGETTIME
/*
 * Stuff for keeping track of time.
 */


#include <stddef.h>      /* definition of NULL */
#include <sys/time.h>   /* definition of timeval struct and protyping of gettime
			   ofday */

double opotime=0.0;
double factortime=0.0;
double totaltime=0.0;
double othertime=0.0;
double othertime1=0.0;
double othertime2=0.0;
double othertime3=0.0;
struct timeval tp;
double t1=0.0;
double t2=0.0;
double starttime=0.0;
double endtime=0.0;

#endif

int sdp(n,k,C,a,constant_offset,constraints,byblocks,fill,X,y,Z,cholxinv,
	cholzinv,pobj,dobj,work1,work2,work3,workvec1,workvec2,workvec3,
	workvec4,workvec5,workvec6,workvec7,workvec8,diagO,bestx,besty,bestz,
	Zi,O,rhs,dZ,dX,dy,dy1,Fp,printlevel,parameters)
     int n;
     int k;
     struct blockmatrix C;
     double *a;
     double constant_offset;
     struct constraintmatrix *constraints;
     struct sparseblock **byblocks;
     struct constraintmatrix fill;
     struct blockmatrix X;
     double *y;
     struct blockmatrix Z;     
     struct blockmatrix cholxinv;
     struct blockmatrix cholzinv;
     double *pobj;
     double *dobj;
     struct blockmatrix work1;     
     struct blockmatrix work2;     
     struct blockmatrix work3;     
     double *workvec1;
     double *workvec2;
     double *workvec3;
     double *workvec4;
     double *workvec5;
     double *workvec6;
     double *workvec7;
     double *workvec8;
     double *diagO;
     struct blockmatrix bestx;
     double *besty;
     struct blockmatrix bestz;
     struct blockmatrix Zi;
     double *O;
     double *rhs;
     struct blockmatrix dZ;
     struct blockmatrix dX;
     double *dy;
     double *dy1;
     double *Fp;
     int printlevel;
     struct paramstruc parameters;
{
  double gap;
  double relgap;
  double mu=1.0e30;
  double muplus;
  double oldmu;
  double muk;
  double gamma;
  double alphap,alphap1;
  double alphad,alphad1;
  double scale1;
  double scale2;
  double nrmx;
  double relpinfeas=1.0e10;
  double reldinfeas=1.0e10;
  double newrelpinfeas;
  double newreldinfeas;
  double limrelpinfeas;
  double bestrelpinfeas;
  double limreldinfeas;
  double bestreldinfeas;
  double maxrelinfeas=1.0e100;
  double oldmaxrelinfeas=1.0e100;
  double newpobj;
  double newdobj;
  int iter=0;
  int m;
  int ret;
  int i;
  int j;
  int info;
  int ldam;
  int retries=0;
  int retcode=0;
  double bestmeas;
  int lastimprove=0;
  double diagnrm;
  double diagfact=0.0;
  double diagadd;
  double ent;

  /*
   * Stuff for instrumentation of iterative refinement.
   */
  double relerr1;
  double relerr2=0.0;

  int refinements;
  double besterr;
  double relerr;

  /*
   * Stuff for iterative refinements.
   */
  
  int lastimproverefinement;
  double mindiag;

  /*
   *
   */

  int ispfeasprob;
  int isdfeasprob;
  
  double normC;
  double norma;

  /*
   * Amount by which to perturb
   */

  double perturbfac;

  /*
   * Sticky flags for primal and dual feasibility.
   */

  int probpfeas=0;
  int probdfeas=0;

  /*
   *
   *
   */

  double pinfeasmeas;
  double dinfeasmeas;

  /*
   * Stuff for adjusting the step fraction.
   */

  double mystepfrac;
  double minalpha;

  /*
   *
   */
  double newalphap;

  /*
   * Stuff for keeping track of best solutions.
   */

#define BASIZE 100
  double bestarray[BASIZE+1];

  /*
   * Used in checking whether the primal-dual affine step gives us a new
   * best solution.
   */
  
  double affgap,affpobj,affdobj,affrelgap,affrelpinfeas,affreldinfeas;

  /*
   * Precompute norms of a and C, so that we don't keep doing this 
   * again and again.
   *
   */

  norma=norm2(k,a+1);
  normC=Fnorm(C);
  if (parameters.perturbobj>0)
    /*    perturbfac=parameters.perturbobj*1.0e-6*normC/sqrt(1.0*n); */
    perturbfac=1.0e-6*normC/sqrt(1.0*n);
  else
    perturbfac=0.0;

  /*
   * Determine whether or not this is a feasibility problem. 
   *
   */
  
  if (normC==0.0)
    {
      if (printlevel >= 1)
	printf("This is a pure primal feasibility problem.\n");
      ispfeasprob=1;
    }
  else
    ispfeasprob=0;

  if (norma==0.0)
    {
      if (printlevel >= 1)
	printf("This is a pure dual feasibility problem.\n");
      isdfeasprob=1;
    }
  else
    isdfeasprob=0;

  /*
   * For historical reasons, we use m for the total number of constraints.
   * In this version of the code, k is also the number of constraints. 
   */

  m=k;

  /*
   *  Work out the leading dimension for the array.  Note that we may not
   *  want to use k itself, for cache issues.
   */
  if ((k % 2) == 0)
    ldam=k+1;
  else
    ldam=k;

  /*
   * Compute Cholesky factors of X and Z.
   */

  copy_mat(X,work1);
  ret=chol(work1);
  
  /*
   * If the matrix was singular, then just return with an error.
   */
  
  if (ret != 0)
    {
      if (printlevel >= 1)
	printf("X was singular!\n");
      retcode=8;
      goto RETURNBEST;
    };
  
  chol_inv(work1,work2);
  store_packed(work2,cholxinv);
  
  copy_mat(Z,work1);
  ret=chol(work1);
  
  /*
   * If the matrix was singular, then just return with an error.
   */
	   
  if (ret != 0)
    {
      if (printlevel >= 1)
	printf("Z was singular!\n");
      retcode=8;
      goto RETURNBEST;
    };
  
  chol_inv(work1,work2);
  store_packed(work2,cholzinv);
  
  /*
    Compute Zi.
  */
  
  copy_mat(work2,work1);
  trans(work1);
  scale1=1.0;
  scale2=0.0;
  mat_mult(scale1,scale2,work2,work1,Zi);

  if (printlevel >= 4)
    printf("Fnorm of Zi is %e \n",Fnorm(Zi));
  
  /*
    Compute primal and dual objective values.
   */

  *pobj=calc_pobj(C,X,constant_offset);
  *dobj=calc_dobj(k,a,y,constant_offset);
  if (parameters.usexzgap==0)
    {
      gap=*dobj-*pobj;
      if (gap < 0.0)
	gap=0.0;
      relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
    }
  else
    {
      gap=trace_prod(X,Z);
      relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
    };

  if (printlevel >= 3)
    {
      printf("constant offset is %e \n",constant_offset);
      printf("Fnorm of X is %e \n",Fnorm(X));
      printf("Fnorm of C is %e \n",normC);
    };

  if (printlevel >= 4)
    {
      printf("pobj is %e \n",*pobj);
      printf("dobj is %e \n",*dobj);
      printf("gap is %e \n",gap);
    };

  relpinfeas=pinfeas(k,constraints,X,a,workvec1);
  bestrelpinfeas=relpinfeas;
  reldinfeas=dinfeas(k,C,constraints,y,Z,work2);
  bestreldinfeas=reldinfeas;

  if (relpinfeas < parameters.axtol )
    probpfeas=1;

  if (reldinfeas < parameters.atytol)
    probdfeas=1;


  oldmaxrelinfeas=maxrelinfeas;
  if (relpinfeas > reldinfeas)
    {
      maxrelinfeas=relpinfeas;
    }
  else
    {
      maxrelinfeas=reldinfeas;
    };

  /*
   * Record this solution as the best solution.
   */

  bestmeas=1.0e100;
  store_packed(X,bestx);
  store_packed(Z,bestz);
  for (i=1; i<=k; i++)
    besty[i]=y[i];

  /*
    Initialize the big loop.
    */
  
  alphap=0.0;
  alphad=0.0;

  /*
    Print out some status information.
    */

  if (printlevel >= 1)
    {
      printf("Iter: %2d Ap: %.2e Pobj: % .7e Ad: %.2e Dobj: % .7e \n",iter,alphap,*pobj,alphad,*dobj);
      fflush(stdout);
    };

  while ((relgap > parameters.objtol) || (relpinfeas > parameters.axtol) || (reldinfeas > parameters.atytol))
	 {



	   /*
	    * Call the user exit routine, and let the user stop the process
	    * if he wants to.
	    */
	   if (user_exit(n,k,C,a,*dobj,*pobj,constant_offset,constraints,
			 X,y,Z,parameters) == 1)
	     {
	       return(0);
	     }

	   bestarray[iter % BASIZE]=bestmeas;

	   /*
	    * Compute the stepfrac to be used for this iteration.
	    */

	   if (iter > 1)
	     {
	       if (alphap > alphad)
		 minalpha=alphad;
	       else
		 minalpha=alphap;
	     }
	   else
	     {
	       minalpha=1.0;
	     };

	   mystepfrac=parameters.minstepfrac+minalpha*(parameters.maxstepfrac-parameters.minstepfrac);
	   if (printlevel >= 3)
	     printf("mystepfrac is %e \n",mystepfrac);

	   /*
	    *  Print out information on primal/dual feasibility.
	    */

	   if (printlevel >= 2)
	     {
	       printf("Relative primal infeasibility is %.7e \n",relpinfeas);
	       printf("Relative dual infeasibility: %.7e \n",reldinfeas);
	       printf("Relative duality gap is %.7e \n",relgap);
	       printf("XZ relative duality gap is %.7e \n",trace_prod(X,Z)/(1+fabs(*dobj)));
	     };

	   /*
	    * If this is a feasibility problem, and we've got feasibility,
	    * then we're done!
	    */

	   if ((ispfeasprob==1) && (relpinfeas < parameters.axtol))
	     {
	       if (printlevel >= 3)
		 printf("Got primal feasibility, so stopping.\n");
	       
	       for (i=1; i<=k; i++)
		 y[i]=0.0;
 
	       alphad=parameters.atytol/(sqrt(n*1.0)*200);
	       make_i(work1);
	       if (alphad*trace_prod(X,work1) > parameters.objtol)
		 alphad=0.005*parameters.objtol/trace_prod(X,work1);
	       zero_mat(work2);
	       addscaledmat(work2,alphad,work1,Z);

	       relpinfeas=pinfeas(k,constraints,X,a,workvec1);
	       if (relpinfeas < bestrelpinfeas)
		 bestrelpinfeas=relpinfeas;
	       reldinfeas=dinfeas(k,C,constraints,y,Z,work2);
	       if (reldinfeas < bestreldinfeas)
		 bestreldinfeas=reldinfeas;

	       *pobj=calc_pobj(C,X,constant_offset);
	       *dobj=calc_dobj(k,a,y,constant_offset);

	       if (parameters.usexzgap==0)
		 {
		   gap=*dobj-*pobj;
		   if (gap < 0.0)
		     gap=0.0;
		   relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
		 }
	       else
		 {
		   gap=trace_prod(X,Z);
		   if (gap < 0.0)
		     gap=0.0;
		   relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
		 };

	       bestmeas=relpinfeas/parameters.axtol;

	       store_packed(X,bestx);
	       store_packed(Z,bestz);
	       for (i=1; i<=k; i++)
		 besty[i]=y[i];

	       retcode=0;
	       goto RETURNBEST;

	     };


	   if ((isdfeasprob==1) && (reldinfeas < parameters.atytol))
	     {
	       if (printlevel >= 3)
		 printf("Got dual feasibility, so stopping.\n");

	       make_i(work1);
	       zero_mat(work2);
	       op_a(k,constraints,work1,workvec1);
	       alphap=parameters.atytol/(200.0*norm2(k,workvec1+1));
	       if (alphap*trace_prod(work1,Z) > parameters.objtol)
		 alphap=0.005*parameters.objtol/trace_prod(work1,Z);
	       addscaledmat(work2,alphap,work1,X);

	       relpinfeas=pinfeas(k,constraints,X,a,workvec1);
	       if (relpinfeas < bestrelpinfeas)
		 bestrelpinfeas=relpinfeas;
	       reldinfeas=dinfeas(k,C,constraints,y,Z,work2);
	       if (reldinfeas < bestreldinfeas)
		 bestreldinfeas=reldinfeas;

	       *pobj=calc_pobj(C,X,constant_offset);
	       *dobj=calc_dobj(k,a,y,constant_offset);

	       if (parameters.usexzgap==0)
		 {
		   gap=*dobj-*pobj;
		   if (gap < 0.0)
		     gap=0.0;
		   relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
		 }
	       else
		 {
		   gap=trace_prod(X,Z);
		   if (gap < 0.0)
		     gap=0.0;
		   relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
		 };

	       bestmeas=reldinfeas/parameters.atytol;

	       store_packed(X,bestx);
	       store_packed(Z,bestz);
	       for (i=1; i<=k; i++)
		 besty[i]=y[i];
	       if (printlevel >= 3)
		 printf("New best solution, %e \n",bestmeas);

	       retcode=0;
	       goto RETURNBEST;

	     };

	   /*
	    *  Check for primal or dual infeasibility.
	    */


	   op_at(k,y,constraints,work1);
	   addscaledmat(work1,-1.0,Z,work1);

	   pinfeasmeas=-(*dobj)/Fnorm(work1);
	   if (printlevel >= 2)
	     printf("-a'*y/||A'(y)-Z|| is %e \n",pinfeasmeas);

	   if ((probpfeas==0) &&
	       (pinfeasmeas > parameters.pinftol))
	     {
	       if (printlevel >= 1)
		 printf("Declaring primal infeasibility.\n");
	       for (i=1; i<=k; i++)
		 y[i]=-y[i]/(*dobj);
	       zero_mat(work1);
	       addscaledmat(work1,-1.0/(*dobj),Z,work1);
	       copy_mat(work1,Z);
	       retcode=1;
	       goto RETURNCERT;
	     };

	   op_a(k,constraints,X,workvec1);
	   dinfeasmeas=trace_prod(C,X)/norm2(k,workvec1+1);
	   if (printlevel >= 2)
	     printf("<C,X>/||A(X)||=%e\n",dinfeasmeas);
	   if ((probdfeas==0) && 
	       (dinfeasmeas>parameters.dinftol))
	     {
	       if (printlevel >= 1)
		 printf("Declaring dual infeasibility.\n");
	       zero_mat(work1);
	       addscaledmat(work1,1.0/trace_prod(C,X),X,work1);
	       copy_mat(work1,X);
	       retcode=2;
	       goto RETURNCERT;
	     };

	   /*
	    * Print out the norm(X) for debugging purposes.
	    */

	   if (printlevel >= 3)
	     {
	       nrmx=Fnorm(X);
	       printf("Fnorm of X is %e \n",nrmx);
	     };




	   /*
	     Now, compute the system matrix.  
	   */

#ifdef USEGETTIME
	   gettimeofday(&tp,NULL);
	   t1=(double)tp.tv_sec+(1.0e-6)*tp.tv_usec;
#endif

	   op_o(k,constraints,byblocks,Zi,X,O,work1,work2);

#ifdef USEGETTIME
	   gettimeofday(&tp,NULL);
	   t2=(double)tp.tv_sec+(1.0e-6)*tp.tv_usec;
	   opotime=opotime+t2-t1;
#endif

	   /*
	    * Print out the actual density of Z and X.
	    */
	   if ((iter==5) && (printlevel >= 3))
	     {
	       printf("Actual density of O %e\n",actnnz(k,ldam,O)/(1.0*k*k));
	       for (j=1; j<=X.nblocks; j++)
		 {
		   if (X.blocks[j].blockcategory==MATRIX)
		     {
		       printf("density X block %d, %e \n",j,actnnz(X.blocks[j].blocksize,
							   X.blocks[j].blocksize,
							   X.blocks[j].data.mat)/
			      (1.0*X.blocks[j].blocksize*X.blocks[j].blocksize));

		       printf("density Z block %d, %e \n",j,actnnz(Z.blocks[j].blocksize,
							   Z.blocks[j].blocksize,
							   Z.blocks[j].data.mat)/
			      (1.0*Z.blocks[j].blocksize*Z.blocks[j].blocksize));

		       printf("bandwidth Z block %d, %d/%d \n",j,bandwidth(Z.blocks[j].blocksize,Z.blocks[j].blocksize,Z.blocks[j].data.mat),Z.blocks[j].blocksize);
			      
		     };
		 };
	     };

	   /*
	     Save a copy of O in the lower diagonal for later use.
	    */

	   for (i=1; i<=k-1; i++)
	     for (j=i; j<=k; j++)
	       O[ijtok(j,i,ldam)]=O[ijtok(i,j,ldam)];
	   
	   for (i=1; i<=k; i++)
	     diagO[i]=O[ijtok(i,i,ldam)];

	   mindiag=1.0e30;

	   for (i=1; i<=k; i++)
	     {
	       if (diagO[i] < mindiag)
		 {
		   mindiag=diagO[i];
		 };

	     };
	   /*
	     This is where we come if factorization failed or the system
	     was so badly conditioned that we didn't get a usable solution.
	    */

	   diagnrm=0.0;

	   for (i=1; i<=k; i++)
	     {
	       ent=diagO[i];
	       diagnrm = diagnrm + ent*ent;
	     };
	   diagnrm=sqrt(diagnrm);


	 RETRYFACTOR:

	   /*
	     Now, let's make sure that O isn't singular.  
	    */




	   diagadd=1.0e-17*diagfact*diagnrm/sqrt(k*1.0);

	   while ((diagadd + mindiag) <= 0.0)
	     {
	       retries++;
	       if (diagfact==0.0)
		 {
		   diagfact=0.1;
		   diagadd=1.0e-17*diagfact*diagnrm/sqrt(k*1.0);
		 }
	       else
		 {
		   diagfact=diagfact*10.0;
		   diagadd=diagadd*10.0;
		 };
	     };

	   if (printlevel >= 3)
	     printf("diagnrm is %e, adding diagadd %e \n",diagnrm,diagadd);

	   for (i=1; i<=k; i++)
	     O[ijtok(i,i,ldam)] += diagadd;

	   /*
	    * Scale the O matrix.
	    */

	   for (i=1; i<=k; i++)
	     workvec8[i]=1.0/sqrt(O[ijtok(i,i,ldam)]);

	   for (i=1; i<=k; i++)
	     {
	       if (workvec8[i] > 1.0e30)
		 workvec8[i]=1.0e30;
	     };

#pragma omp parallel for schedule(dynamic,64) default(none) shared(O,ldam,k,workvec8) private(i,j)
	  for (j=1; j<=k; j++)
	     for (i=1; i<=j; i++)
	       O[ijtok(i,j,ldam)]=O[ijtok(i,j,ldam)]*(workvec8[i]*workvec8[j]);

	   /*
	     Next, compute the cholesky factorization of the system matrix.
	   */

#ifdef USEGETTIME
	   gettimeofday(&tp,NULL);
	   t1=(double)tp.tv_sec+(1.0e-6)*tp.tv_usec;
#endif

#ifdef NOUNDERLAPACK
  #ifdef CAPSLAPACK
	   DPOTRF("U",&m,O,&ldam,&info);
  #else
	   dpotrf("U",&m,O,&ldam,&info);
  #endif
#else
  #ifdef CAPSLAPACK
	   DPOTRF_("U",&m,O,&ldam,&info);
  #else
	   dpotrf_("U",&m,O,&ldam,&info);
  #endif
#endif

#ifdef USEGETTIME
	   gettimeofday(&tp,NULL);
	   t2=(double)tp.tv_sec+(1.0e-6)*tp.tv_usec;
	   factortime=factortime+t2-t1;
#endif

	   if (info != 0) 
	     {
	       if (printlevel >= 3)
		 printf("Factorization of the system matrix failed!\n");

	       if (retries < 15)
		 {
		   if (retries == 0)
		     diagfact=0.1;
		   retries=retries+1;
		   diagfact=diagfact*10.0;
#pragma omp parallel for schedule(dynamic,64) private(i,j) shared(O,k,ldam)
		   for (i=1; i<=k-1; i++)
		     for (j=i; j<=k; j++)
		       O[ijtok(i,j,ldam)]=O[ijtok(j,i,ldam)];
		   for (i=1; i<=k; i++)
		     O[ijtok(i,i,ldam)]=diagO[i];
		   goto RETRYFACTOR;
		 }
	       else
		 {
		   if (printlevel >= 1) 
		     printf("Factorization of the system matrix failed, giving up. \n");

		   /*
		    * Tighten up the solution as much as possible.
		    */
		   
		   retcode=8;
		   goto RETURNBEST;
		   
		 };
	     };

	       
	   /*
	     Compute the rhs vector.
	   */

	   for (i=1; i<=k; i++)
	     rhs[i]=-a[i];
	   
	   /*
	    * Add in the corrections for Fd.
	    */

	   /*
	     his section is where Fd is computed and put into
	     The rhs.

	     To save storage, work2 is used instead of a variable named
	     Fd.
 
             dX will be used as a work variable where needed in place
	     of old work2 usage.
	   */

	   op_at(k,y,constraints,work1);      

	   addscaledmat(Z,1.0,C,work2);

	   if ((bestmeas > 1.0e3) && (parameters.perturbobj>0))
	     {
	       if (printlevel >= 3)
		 printf("Perturbing C.\n");
	       make_i(work3);
	       addscaledmat(work2,-perturbfac,work3,work2);
	     };

	   if ((bestmeas < 1.0e3) && (parameters.perturbobj>0))
	     {
	       if (printlevel >= 3)
		 printf("Perturbing C.\n");
	       make_i(work3);
	       addscaledmat(work2,-perturbfac*pow(bestmeas/1000.0,1.5),work3,work2);
	     };



	   addscaledmat(work2,-1.0,work1,work2);

	   scale1=1.0;
	   scale2=0.0;



	   mat_multspb(scale1,scale2,Zi,work2,work3,fill);
	   mat_multspc(scale1,scale2,work3,X,dX,fill);


	   
	   op_a(k,constraints,dX,workvec1);



	   for (i=1; i<=k; i++)
	     rhs[i] = rhs[i]+workvec1[i];

	   for (i=1; i<=k; i++)
	     workvec1[i]=rhs[i];

	   if (printlevel >=3)
	     {
	       printf("Fnorm of Fd is %e \n",Fnorm(work2));
	       printf("Norm of rhs is %e \n",norm2(m,rhs+1));
	     };

	   /*
	     Solve the system of equations for dy.
	   */

	   /*
	    * First, scale
	    */

	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]*workvec8[i];

	   info=solvesys(k,ldam,O,workvec1);

	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]*workvec8[i];

	   if (info != 0)
	     {
	       if (printlevel >= 1)
		 printf("Solving for dy failed! \n");
	       retcode=8;
	       goto RETURNBEST;
	     };

	   /*
	    * Do iterative refinement.
	    */


	   if ((iter>1) && (relerr2 > parameters.axtol) && 
	       (parameters.fastmode==0))
	     {
	       op_at(k,workvec1,constraints,work1);
	       mat_multspa(1.0,0.0,work1,X,dX,fill);
	       mat_multspc(1.0,0.0,Zi,dX,work1,fill);
	       op_a(k,constraints,work1,workvec2);
	       for (i=1; i<=k; i++)
		 workvec2[i]=rhs[i]-workvec2[i];
	       
	       relerr=norm2(k,workvec2+1)/(1.0+norm2(k,rhs+1));
	       besterr=relerr;
	       for (i=1; i<=k; i++)
		 workvec4[i]=workvec1[i];
	       
	       if (printlevel >= 3)
		 {
		   printf("refinement: Before relative error in Ody=r is %e \n",besterr);
		   fflush(stdout);
		 };
	       
	       refinements=0;
	       lastimproverefinement=0;
	       
	       while ((refinements < 20) && (refinements-lastimproverefinement < 3) 
		      && (besterr > 1.0e-14))
		 {
		   refinements++;
		   
		   for (i=1; i<=k; i++)
		     workvec3[i]=workvec2[i]*workvec8[i];
		   
		   info=solvesys(k,ldam,O,workvec3);
		   
		   for (i=1; i<=k; i++)
		     workvec3[i]=workvec3[i]*workvec8[i];
		   
		   for (i=1; i<=k; i++)
		     workvec1[i]=workvec1[i]+workvec3[i];
		   
		   op_at(k,workvec1,constraints,work1);
		   mat_multspa(1.0,0.0,work1,X,dX,fill);
		   mat_multspc(1.0,0.0,Zi,dX,work1,fill);
		   op_a(k,constraints,work1,workvec2);
		   for (i=1; i<=k; i++)
		     workvec2[i]=rhs[i]-workvec2[i];
		   
		   relerr=norm2(k,workvec2+1)/(1.0+norm2(k,rhs+1));
		   if (relerr < besterr)
		     {
		       lastimproverefinement=refinements;
		       besterr=relerr;
		       for (i=1; i<=k; i++)
			 workvec4[i]=workvec1[i];
		     };
		   if (printlevel >= 4)
		     {
		       printf("refinement: During relative error in Ody=r is %e \n",besterr);
		       fflush(stdout);
		     };
		   
		 };
	       
	       if (printlevel >= 3)
		 printf("refinement: After relative error in Ody=r is %e, %d \n",besterr,lastimproverefinement);
	       for (i=1; i<=k; i++)
		 workvec1[i]=workvec4[i];
	       
	     };


	   /*
	    * Extract dy.
	    */

	   for (i=1; i<=k; i++)
	     dy[i]=workvec1[i];

	   if (printlevel >= 3)
	     printf("Norm of dy is %e \n",norm2(k,dy+1));

	   /*
	     Compute dZ
	   */

	   op_at(k,dy,constraints,dZ);

	   /*
	    * Note:  At this point, dZ only has A'(dy), not -Fd
	    */
	   /*
	     Compute Zi*A'(dy), and save it in a temp variable for later use.
	   */

	   scale1=1.0;
	   scale2=0.0;

	   mat_multspb(scale1,scale2,Zi,dZ,work1,fill);

	   if (printlevel >= 4)
	     printf("Fnorm of work1 is %e \n",Fnorm(work1));

	   /*
	    * Now, update dZ to include -Fd
	    */

	   if (printlevel >= 3)
	     {
	       printf("Before Fd, Fnorm of dZ is %e \n",Fnorm(dZ));
	       fflush(stdout);
	     };

	   addscaledmat(dZ,-1.0,work2,dZ);

	   if (printlevel >= 3)
	     {
	       printf("After Fd, Fnorm of dZ is %e \n",Fnorm(dZ));
	       fflush(stdout);
	     };


	   /*
	    * Now, we've got dZ in dZ, and Zi*A'(dy) in work1, 
            * and Zi*Fd in work3
	    */

	   /*
	     Compute dX=-X+Zi*Fd*X-temp*X;

             First, put I-Zi*Fd+work1 in workn2.  Then multiply -work2*X, and
             put the result in dX.

	   */
	   make_i(work2);

	   addscaledmat(work2,-1.0,work3,work2);

	   addscaledmat(work2,1.0,work1,work2);

	   scale1=-1.0;
	   scale2=0.0;
	   mat_mult(scale1,scale2,work2,X,dX);

	   sym_mat(dX);

	   if (printlevel >= 3)
	     {
	       printf("Fnorm of dX is %e \n",Fnorm(dX));
	       printf("Fnorm of dZ is %e \n",Fnorm(dZ));
	       fflush(stdout);
	     };




	   /*
	    * Next, determine mu.
	    */

	   if (relpinfeas < parameters.axtol)
	     alphap1=linesearch(n,dX,work1,work2,work3,cholxinv,workvec4,
				workvec5,workvec6,mystepfrac,1.0,
				printlevel);
	   else
	     alphap1=linesearch(n,dX,work1,work2,work3,cholxinv,workvec4,
				workvec5,workvec6,mystepfrac,1.0,
				printlevel);

	   if (reldinfeas < parameters.atytol)
	     alphad1=linesearch(n,dZ,work1,work2,work3,cholzinv,workvec4,
				workvec5,workvec6,mystepfrac,1.0,
				printlevel);
	   else
	     alphad1=linesearch(n,dZ,work1,work2,work3,cholzinv,workvec4,
				workvec5,workvec6,mystepfrac,1.0,
				printlevel);

	   oldmu=mu;

	   /* Here, work1 holds X+alphap1*dX, work2=Z+alphad1*dZ */

	   addscaledmat(X,alphap1,dX,work1);
	   addscaledmat(Z,alphad1,dZ,work2);
	   for (i=1; i<=k; i++)
	     workvec1[i]=y[i]+alphad1*dy[i];


	   /*
	    * Check to see whether this affine solution is the best yet.
	    * The test is somewhat expensive, so don't do it unless
	    * we're close to done.
	    */

	   if ((bestmeas <1.0e4) && (parameters.affine==0))
	     {
	       /*
		* Verify that the new X and Z are Cholesky factorizable.
		*/

	       copy_mat(work1,work3);
	       ret=chol(work3);
	       while (ret != 0)
		 {
		   if (printlevel >=3)
		     printf("Affine eigsearch missed: adjusting alphap1\n");
		   alphap1=alphap1*0.9;
		   addscaledmat(X,alphap1,dX,work1);
		   copy_mat(work1,work3);
		   ret=chol(work3);
		 };

	       copy_mat(work2,work3);
	       ret=chol(work3);
	       while (ret != 0)
		 {
		   if (printlevel >=3)
		     printf("Affine eigsearch missed: adjusting alphad1\n");
		   alphad1=alphad1*0.9;
		   addscaledmat(Z,alphad1,dZ,work2);
		   for (i=1; i<=k; i++)
		     workvec1[i]=y[i]+alphad1*dy[i];
		   copy_mat(work2,work3);
		   ret=chol(work3);
		 };

	       /*
		* Now, check the quality of this solution.
		*/

	       affpobj=calc_pobj(C,work1,constant_offset);
	       affdobj=calc_dobj(k,a,workvec1,constant_offset);

	       /*
		* run user exit to check if the affine solution is good enough
		*/
	       if (user_exit(n,k,C,a,affdobj,affpobj,constant_offset,constraints,
			     work1,workvec1,work2,parameters) == 1) 
		 {
		   *dobj=affdobj;
		   *pobj=affpobj;
		   copy_mat(work1,X);
		   copy_mat(work2,Z);
		   for(i=1; i<=k; i++)
		     y[i]=workvec1[i];
		   if(printlevel>=3)
		     printf("Affine step good enough, exiting\n");
		   return(0);
		 };

	       if (parameters.usexzgap==0)
		 {
		   affgap=affdobj-affpobj;
		   if (affgap < 0)
		     affgap=0.0;
		   affrelgap=affgap/(1.0+fabs(affpobj)+fabs(affdobj));
		 }
	       else
		 {
		   affgap=trace_prod(work1,work2);
		   if (affgap < 0)
		     affgap=0.0;
		   affrelgap=affgap/(1.0+fabs(affpobj)+fabs(affdobj));
		 };

	       affreldinfeas=dinfeas(k,C,constraints,workvec1,work2,work3);
	       affrelpinfeas=pinfeas(k,constraints,work1,a,workvec4);
	       
	       if (printlevel >= 3)
		 {
		   printf("affpobj is %e \n",affpobj);
		   printf("affdobj is %e \n",affdobj);
		   printf("affrelgap is %e \n",affrelgap);
		   printf("affrelpinfeas is %e \n",affrelpinfeas);
		   printf("affreldinfeas is %e \n",affreldinfeas);
		 };
	       
	       if ((affrelgap/parameters.objtol < bestmeas) &&
		   (affrelpinfeas/parameters.axtol <bestmeas) &&
		   (affreldinfeas/parameters.atytol <bestmeas))
		 {
		   lastimprove=iter;
		   
		   bestmeas=affrelgap/parameters.objtol;
		   if (affrelpinfeas/parameters.axtol > bestmeas)
		     bestmeas=affrelpinfeas/parameters.axtol;
		   if (affreldinfeas/parameters.atytol > bestmeas)
		     bestmeas=affreldinfeas/parameters.atytol;
		   
		   store_packed(work1,bestx);
		   store_packed(work2,bestz);
		   for (i=1; i<=k; i++)
		     besty[i]=y[i]+alphad1*dy[i];
		   if (printlevel >= 3)
		     printf("Affine step: New best solution, %e \n",bestmeas);
		 };

	       if ((ispfeasprob==1) &&
		   (affrelpinfeas/parameters.axtol < bestmeas))
		 {
		   lastimprove=iter;
		   
		   bestmeas=affrelpinfeas/parameters.axtol;
		   
		   store_packed(work1,bestx);
		   for (i=1; i<=k; i++)
		     besty[i]=0.0;
		   zero_mat(work3);
		   addscaledmat(work3,1.0e-50,work1,work3);
		   store_packed(work3,bestz);

		   if (printlevel >= 3)
		     printf("Affine step: New best solution, %e \n",bestmeas);
		 };
	       
	       if ((isdfeasprob==1) &&
		   (affreldinfeas/parameters.atytol <bestmeas))
		 {
		   lastimprove=iter;
		   
		   bestmeas=affreldinfeas/parameters.atytol;
		   
		   zero_mat(work3);
		   make_i(work1);
		   addscaledmat(work3,1.0e-40,work1,work3);
		   store_packed(work3,bestx);
		   store_packed(work2,bestz);
		   for (i=1; i<=k; i++)
		     besty[i]=workvec1[i];
		   if (printlevel >= 3)
		     printf("Affine step: New best solution, %e \n",bestmeas);
		 };

	       if (bestmeas < 1.0)
		 {
		   if (printlevel >= 3)
		     printf("Finishing with a final affine step.\n");
		   iter=iter+1;
		   if (printlevel >= 1)
		     printf("Iter: %2d Ap: %.2e Pobj: % .7e Ad: %.2e Dobj: % .7e \n",iter,alphap1,affpobj,alphad1,affdobj);
		   if (printlevel >= 2)
		     printf("Total Iterations: %d \n",iter);

		   store_unpacked(bestx,X);
		   store_unpacked(bestz,Z);
		   for (i=1; i<=k; i++)
		     y[i]=besty[i];
		   *pobj=calc_pobj(C,X,constant_offset);
		   *dobj=calc_dobj(k,a,y,constant_offset);
		   return(0);
		 };
	     };

	   /*
	    * Compute muplus and prepare for the corrector step.
	    */

	   muplus=trace_prod(work1,work2)/(n);
	   muk=trace_prod(X,Z)/(n);

	   if (muk < 0.0)
	     muk=fabs(muk);
	   if (muplus < 0.0)
	     muplus=muk/2;

	   gamma=(muplus/muk);

	   /*
	    * Pick the new mu as follows:
	    *
	    * If we have been making good progress (alphap > 0.5) and
	    * (alphad>0.5) then use mu=muk*gamma*gamma*min(gamma,0.5);
	    *
	    * If we haven't been making good progress, then just do 
	    * mu=muplus. 
	    *
	    * Also, make sure that mu is no larger than the old mu.
	    */
	   
	   if ((relpinfeas < 0.1*parameters.axtol) && (alphad>0.2) &&
	       (reldinfeas < 0.1*parameters.atytol) && (alphap>0.2) &&
	       (mu > 1.0e-6) && (gamma < 1.0) && (alphap+alphad > 1.0))
	     {
	       mu=muk*pow(gamma,alphap+alphad);
	     }
	   else
	     {
	       if (muplus < 0.9*muk)
		 mu=muplus;
	       else
		 mu=muk*0.9;
	     };
	   
	   /*
	    * If we have primal and dual infeasibility in hand, then 
	    * make sure that mu is <=muk/2.
	    */
	   
	   if ((relpinfeas < 0.9*parameters.axtol) &&
	       (reldinfeas < 0.9*parameters.atytol) &&
	       (mu > muk/2))
	     mu=muk/2;
	     
	   /*
	    * If we want a primal-dual affine step, then set mu=0.0. 
	    */

	   if (parameters.affine==1)
	     {
	       mu=0.0;
	       if (printlevel >= 3)
		 printf("Taking an affine step because parameters.affine=1\n"); 
	     };


 	   /*
	    * Printout some info on mu.
	    */

	   if (printlevel >= 2)
	     {
	       printf("muk is %e \n",muk);
	       printf("muplus is %e \n",muplus);
	       printf("New mu is %e \n",mu);
	       printf("mu*n = target duality gap is %e \n",mu*n);
	       fflush(stdout);
	     };

	   /*
	    * Take a moment to figure out how well we're doing on feasibility.
	    */

	   addscaledmat(X,1.0,dX,work1);
	   op_a(k,constraints,work1,workvec1);
	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]-a[i];
	   relerr1=norm2(k,workvec1+1)/(1.0+norma);	   
	   if (printlevel >= 3)
	     {
	       printf("refinement: Relative error in A(X+dX)=a (Fphat) is %e \n",
		      relerr1);
	     };

	   /*
	     Now, compute the corrector step.
	   */

	   /*
	     rhs for the corrector step.
	   */

	   /*
	     Update Fphat=a-A(X+dX)
	    */

	   addscaledmat(dX,1.0,X,work1);
	   op_a(k,constraints,work1,Fp);
	   for (i=1; i<=k; i++)
	     Fp[i]=a[i]-Fp[i];

	   /*
	     The RHS is now A(Zi*Fdhat*X)+A(Zi*(-dZ*dX+mu*I))-Fphat

                 = A(Zi*(Fdhat*X-dZ*dX+mu*I))-Fphat

	    */

	   make_i(work1);
	   scale1=0.0;
	   scale2=mu;
	   mat_mult(scale1,scale2,work2,work2,work1);

	   scale1=-1.0;
	   scale2=1.0;
	   mat_multspa(scale1,scale2,dZ,dX,work1,fill); 

	   scale1=1.0;
	   scale2=0.0;
	   mat_multspc(scale1,scale2,Zi,work1,work2,fill); 
	   


	   /*
	     Next, compute op_a of work2, and put the result in rhs.
	   */

	   op_a(k,constraints,work2,rhs);

	   /*
	    * Finally, subtract off Fphat.
	    */

	   for (i=1; i<=k; i++)
	     rhs[i]=rhs[i]-Fp[i];

	   for (i=1; i<=k; i++)
	     workvec1[i]=rhs[i];

	   /*
	     Solve for dy1.
	   */

	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]*workvec8[i];

	   info=solvesys(k,ldam,O,workvec1);

	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]*workvec8[i];

	   if (info != 0)
	     {
	       if (printlevel >= 1)
		 printf("Solving for dy1 failed! \n");
	       retcode=8;
	       goto RETURNBEST;
	     };

	   /*
	    * Do iterative refinement.
	    */

	   if ((iter>1) && (relerr2 > 0.01*parameters.axtol) &&
	       (parameters.fastmode==0))
	     {
	       op_at(k,workvec1,constraints,work1);
	       mat_multspa(1.0,0.0,work1,X,work2,fill);
	       mat_multspc(1.0,0.0,Zi,work2,work3,fill);
	       op_a(k,constraints,work3,workvec2);
	       for (i=1; i<=k; i++)
		 workvec2[i]=rhs[i]-workvec2[i];
	       
	       relerr=norm2(k,workvec2+1)/(1.0+norm2(k,rhs+1));
	       besterr=relerr;
	       for (i=1; i<=k; i++)
		 workvec4[i]=workvec1[i];
	       
	       if (printlevel >= 3)
		 {
		   printf("refinement: Before relative error in Odyhat=r is %e \n",besterr);
		 };
	       
	       refinements=0;
	       lastimproverefinement=0;
	       
	       while ((refinements < 20) && (refinements-lastimproverefinement < 3) 
		      && (besterr > 1.0e-14))
		 {
		   refinements++;
		   
		   for (i=1; i<=k; i++)
		     workvec3[i]=workvec2[i]*workvec8[i];
		   
		   info=solvesys(k,ldam,O,workvec3);
		   
		   for (i=1; i<=k; i++)
		     workvec3[i]=workvec3[i]*workvec8[i];
		   
		   for (i=1; i<=k; i++)
		     workvec1[i]=workvec1[i]+workvec3[i];
		   
		   op_at(k,workvec1,constraints,work1);
		   mat_multspa(1.0,0.0,work1,X,work2,fill);
		   mat_multspc(1.0,0.0,Zi,work2,work3,fill);
		   op_a(k,constraints,work3,workvec2);
		   for (i=1; i<=k; i++)
		     workvec2[i]=rhs[i]-workvec2[i];
		   
		   relerr=norm2(k,workvec2+1)/(1.0+norm2(k,rhs+1));
		   if (relerr < besterr)
		     {
		       lastimproverefinement=refinements;
		       besterr=relerr;
		       for (i=1; i<=k; i++)
			 workvec4[i]=workvec1[i];
		     };
		   if (printlevel >= 4)
		     {
		       printf("refinement: During relative error in Ody=r is %e \n",besterr);
		       fflush(stdout);
		     };
		   
		 };
	       
	       if (printlevel >= 3)
		 {
		   printf("refinement: After relative error in Odyhat=r is %e,%d \n",besterr,lastimproverefinement);
		 };

	       for (i=1; i<=k; i++)
		 workvec1[i]=workvec4[i];
	     };

	   /*
	    * retrieve dy1.
	    */

	   for (i=1; i<=k; i++)
	     dy1[i]=workvec1[i];

	   /*
	     Compute dZ1=A'(dy1).

	     dZ1 is stored in work3.
	   */

	   op_at(k,dy1,constraints,work3);


	   /*
	     Compute dX1=-Zi*dZ1*X-Zi*dZ*dX+mu*zi;
	     dX1=Zi*(-dZ1*X-dZ*dX+mu*I)

	     for storage sake, dX1 is stored in work2.  
	   */

	   make_i(work1);
	   scale1=-1.0;
	   scale2=mu;
	   mat_multspa(scale1,scale2,dZ,dX,work1,fill); 
	   scale1=-1.0;
	   scale2=1.0;
	   mat_multspa(scale1,scale2,work3,X,work1,fill); 
	   scale1=1.0;
	   scale2=0.0;
	   mat_mult(scale1,scale2,Zi,work1,work2);
	   sym_mat(work2);

	   addscaledmat(X,1.0,dX,work1);
	   op_a(k,constraints,work1,workvec1);
	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]-a[i];
	   relerr2=norm2(k,workvec1+1)/(1.0+norma);
	   
	   if (printlevel >= 3)
	     {
	       printf("refinement: Before dX+dX1 Relative error in A(X+dX)=a is %e \n",relerr2);
	       if (relerr1 < relerr2)
		 printf("refinement: worse\n");
	       else
		 printf("refinement: better\n");
	     };
	   /*
	     Update the predictor step.
	   */

	   if (printlevel >= 3)
	     {
	       printf("Fnorm of dX1 is %e\n",Fnorm(work2));
	       printf("Fnorm of dZ1 is %e\n",Fnorm(work3));
	     };
	   add_mat(work2,dX);
	   add_mat(work3,dZ);
	   for (i=1; i<=k; i++)
	     dy[i]=dy1[i]+dy[i];



	   /*
	    * Check A(X+dX)=a.
	    */
	   addscaledmat(X,1.0,dX,work1);
	   op_a(k,constraints,work1,workvec1);
	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]-a[i];
	   relerr2=norm2(k,workvec1+1)/(1.0+norma);
	   
	   if (printlevel >= 3)
	     {
	       printf("refinement: Before adjust dX Relative error in A(X+dX)=a is %e \n",relerr2);
	       if (relerr1 < relerr2)
		 printf("refinement: worse\n");
	       else
		 printf("refinement: better\n");
	     };

	   if (printlevel >= 3)
	     {
	       printf("Fnorm of dX is %e \n",Fnorm(dX));
	       printf("Fnorm of dZ is %e \n",Fnorm(dZ));
	     };

	   /*
	    * Take a moment to figure out how well we're doing on feasibility.
	    */

	   addscaledmat(X,1.0,dX,work1);
	   op_a(k,constraints,work1,workvec1);
	   for (i=1; i<=k; i++)
	     workvec1[i]=workvec1[i]-a[i];
	   relerr2=norm2(k,workvec1+1)/(1.0+norma);
	   
	   if (printlevel >= 3)
	     {
	       printf("refinement: After adjust error in A(X+dX)=a is %e \n",relerr2);
	       if (relerr1 < relerr2)
		 printf("refinement: worse\n");
	       else
		 printf("refinement: better\n");
	     };


	   /* 
	      Now, we've got the individual steps.
	      Find maximum possible step sizes.
	   */

	   alphap=linesearch(n,dX,work1,work2,work3,cholxinv,workvec4,
			     workvec5,workvec6,mystepfrac,1.0,
			     printlevel);


	   /*
	    * Compute the objective value of the new solution.
	    */

	   newpobj=calc_pobj(C,work1,constant_offset);

	   alphad=linesearch(n,dZ,work1,work2,work3,cholzinv,workvec4,
			     workvec5,workvec6,mystepfrac,1.0,
			     printlevel);

	   if (printlevel >= 3)
	     {
	       printf("After linesearches, alphap=%e alphad=%e \n",alphap,alphad);
	     };

	   for (i=1; i<=k; i++)
	     workvec1[i]=y[i]+alphad*dy[i];

	   /*
	    * Calculate the prospective new dual objective.
	    */

	   newdobj=calc_dobj(k,a,workvec1,constant_offset);

	   /*
	    * Check on the feasibility of the new solutions.  If they're
	    * worse, and the old solution was feasible, then don't take the
	    * step.
	    */

	   addscaledmat(X,alphap,dX,work1);
	   newrelpinfeas=pinfeas(k,constraints,work1,a,workvec1);

	   /*
	    * For the primal infeasibility, check the relative gap and the
	    * current primal infeasibility to establish a limit on the 
	    * new infeasibility.
	    */
	   
	   limrelpinfeas=bestrelpinfeas*100;

	   if ((limrelpinfeas >relgap) && (relgap > 0))
	     limrelpinfeas=relgap;

	   /*
	    * In the early stages, don't worry about limrelpinfeas.
	    */

	   if ((iter < 10) || (relpinfeas > 0.01)  ||
	       (dinfeasmeas > 1.0e-3*parameters.dinftol))
	     limrelpinfeas=relpinfeas*1000;

	   /*
	    * Don't ever ask for more than the required tolerance.
	    */

	   if (parameters.axtol > limrelpinfeas)
	     limrelpinfeas=parameters.axtol;

	   /*
	    * If we're trying to prove dual infeasibility than don't limit
	    * the step.
	    */

	   if ((probdfeas==0) && (dinfeasmeas>1.0e4))
	     limrelpinfeas=1.0e30;

	   /*
	    * Now, make sure that the step keeps us feasible enough.
	    */

	   if (printlevel >= 3)
	     {
	       printf("newrelpinfeas is %e\n",newrelpinfeas);
	       printf("limrelpinfeas is %e\n",limrelpinfeas);
	     };

	   i=1;
	   while (newrelpinfeas > limrelpinfeas)
	     {
	       alphap=0.80*alphap;
	       addscaledmat(X,alphap,dX,work1);
	       newrelpinfeas=pinfeas(k,constraints,work1,a,workvec1);
	       newpobj=calc_pobj(C,work1,constant_offset);
	       i=i+1;

	       if (i>20)
		 {
		   if (printlevel >=3)
		     printf("Stuck at edge of primal feasibility.\n");
		   
		   if (retries < 15)
		     {
		       if (retries == 0)
			 diagfact=0.1;
		       retries=retries+1;
		       diagfact=diagfact*10.0;
		       for (i=1; i<=k-1; i++)
			 for (j=i; j<=k; j++)
			   O[ijtok(i,j,ldam)]=O[ijtok(j,i,ldam)];
		       for (i=1; i<=k; i++)
			 O[ijtok(i,i,ldam)]=diagO[i];
		       goto RETRYFACTOR;
		     }
		   else
		     {
		       if (printlevel >= 1) 
			 printf("Stuck at edge of primal feasibility, giving up. \n");
		       
		       /*
			* Tighten up the solution as much as possible.
			*/
		       
		       retcode=5;
		       goto RETURNBEST;

		     };


		 };
	     };

	   /*
	     Now make sure that we aren't stepping outside of dual feasibility.
	    */
	   

	   addscaledmat(Z,alphad,dZ,work1);

	   for (i=1; i<=k; i++)
	     workvec1[i]=y[i]+alphad*dy[i];

	   newreldinfeas=dinfeas(k,C,constraints,workvec1,work1,work2);

	   limreldinfeas=bestreldinfeas*100;

	   if ((limreldinfeas > relgap) && (relgap > 0))
	     limreldinfeas=relgap;

	   /*
	    * In the early stages, don't worry about limreldinfeas.
	    * Also don't worry if we're trying to establish primal 
	    * infeasibility.
	    */

	   if ((iter < 10) || (reldinfeas > 0.01) || 
	       (pinfeasmeas > 1.0e-3*parameters.pinftol))
	     limreldinfeas=reldinfeas*1000;

	   /*
	    * Don't ever ask for more than the required tolerance.
	    */

	   if (parameters.atytol > limreldinfeas)
	     limreldinfeas=parameters.atytol;

	   if (printlevel >= 3)
	     {
	       printf("newreldinfeas is %e\n",newreldinfeas);
	       printf("limreldinfeas is %e\n",limreldinfeas);
	     };

	   i=1;
	   while (newreldinfeas > limreldinfeas)
	     {
	       alphad=0.80*alphad;
	       addscaledmat(Z,alphad,dZ,work1);
	       
	       for (j=1; j<=k; j++)
		 workvec1[j]=y[j]+alphad*dy[j];
	       
	       newreldinfeas=dinfeas(k,C,constraints,workvec1,work1,
				     work2);
	       newdobj=calc_dobj(k,a,workvec1,constant_offset);
	       i=i+1;
	       if (i>15)
		 {
		   if (printlevel >=3)
		     printf("Stuck at edge of dual feasibility.\n");

		   if (retries < 15)
		     {
		       if (retries == 0)
			 diagfact=0.1;
		       retries=retries+1;
		       diagfact=diagfact*10.0;
		       for (i=1; i<=k-1; i++)
			 for (j=i; j<=k; j++)
			   O[ijtok(i,j,ldam)]=O[ijtok(j,i,ldam)];
		       for (i=1; i<=k; i++)
			 O[ijtok(i,i,ldam)]=diagO[i];
		       goto RETRYFACTOR;
		     }
		   else
		     {
		       if (printlevel >= 1) 
			 printf("Stuck at edge of dual feasibility, giving up. \n");
		       
		       /*
			* Tighten up the solution as much as possible.
			*/
		       
		       retcode=6;
		       goto RETURNBEST;

		     };

		 };
	     };

	   if (printlevel >= 3)
	     {
	       printf("After feas check, alphap=%e alphad=%e \n",alphap,alphad);
	     };

	   /*
	    * Give up if step lengths are way too small.
	    */

	   if ((alphap<=parameters.minstepp) || 
	       (alphad<=parameters.minstepd))
	     {
	       if (printlevel >= 2)
		 printf("line search failure in corrector step.\n");

	       if (retries < 15)
		 {
		   if (retries == 0)
		     diagfact=0.1;
		   retries=retries+1;
		   diagfact=diagfact*10.0;
		   for (i=1; i<=k-1; i++)
		     for (j=i; j<=k; j++)
		       O[ijtok(i,j,ldam)]=O[ijtok(j,i,ldam)];
		   for (i=1; i<=k; i++)
		     O[ijtok(i,i,ldam)]=diagO[i];
		   goto RETRYFACTOR;
		 }
	       else
		 {
		   if (printlevel >= 1) 
		     printf("Too many line search failures, giving up. \n");
		   
		   /*
		    * Tighten up the solution as much as possible.
		    */
		   
		   retcode=9;
		   goto RETURNBEST;
		   
		 };
	     };

	   /*
	    * In case alphap changed, recompute these.
	    */

	   addscaledmat(X,alphap,dX,work1);
	   newpobj=calc_pobj(C,work1,constant_offset);
	   newrelpinfeas=pinfeas(k,constraints,work1,a,workvec1);

	   addscaledmat(Z,alphad,dZ,work1);
	   for (i=1; i<=k; i++)
	     workvec1[i]=y[i]+alphad*dy[i];
	   newdobj=calc_dobj(k,a,workvec1,constant_offset);
	   
	   newreldinfeas=dinfeas(k,C,constraints,workvec1,work1,work2);

	   /*
	    * Update cholzinv.
	    */

	   copy_mat(work1,work2);
	   ret=chol(work2);

	   while (ret != 0)
	     {
	       alphad=alphad*0.90;
	       addscaledmat(Z,alphad,dZ,work1);
	       for (i=1; i<=k; i++)
		 workvec1[i]=y[i]+alphad*dy[i];
	       copy_mat(work1,work2);
	       ret=chol(work2);
	       if (printlevel >=2)
		 printf("eigsearch missed! Adjusting alphad\n");
	     };

	   chol_inv(work2,work3);
	   store_packed(work3,cholzinv);

	   /*
	     Compute Zi.
	   */

	   copy_mat(work3,work1);
	   trans(work1);
	   scale1=1.0;
	   scale2=0.0;
	   mat_mult(scale1,scale2,work3,work1,Zi);
	   if (printlevel >= 4)
	     printf("Fnorm of Zi is %e \n",Fnorm(Zi));
	   
	   /*
	    * Confirm that X is in fact factorable.  If not, reduce
	    * alpha until it is.
 	    */

	   addscaledmat(X,alphap,dX,work1);
	   copy_mat(work1,work2);
	   ret=chol(work2);

	   while (ret != 0)
	     {
	       if (printlevel >=2)
		 printf("eigsearch missed! Adjusting alphap\n");
	       alphap=alphap*0.90;
	       addscaledmat(X,alphap,dX,work1);
	       copy_mat(work1,work2);
	       ret=chol(work2);
	     };
	   
	   chol_inv(work2,work3);
	   store_packed(work3,cholxinv);

	   /*
	    * do a line search for feasibility.
	    */

	   if (relpinfeas > 1.0)
	     {
	       newalphap=alphap;
	       for (i=1; i<=9; i++)
		 {
		   addscaledmat(X,(i*1.0)*alphap/10.0,dX,work1);
		   if (pinfeas(k,constraints,work1,a,workvec1) < newrelpinfeas)
		     {
		       newalphap=i*1.0*alphap/10.0;
		       newrelpinfeas=pinfeas(k,constraints,work1,a,workvec1);
		     };
		 };
	       if (newalphap < alphap)
		 {
		   if (printlevel >= 2)
		     printf("Feasibility Adjusting alphap to %e \n",newalphap);
		   alphap=newalphap;
		 };
	     };

	   /*
	    *Take the step.
	    */

	   addscaledmat(X,alphap,dX,X);
	   addscaledmat(Z,alphad,dZ,Z);

	   for (i=1; i<=k; i++)
	     y[i]=y[i]+alphad*dy[i];

	   /*
	    *  Update the objectives.
	    */

	   newdobj=calc_dobj(k,a,y,constant_offset);
	   newpobj=calc_pobj(C,X,constant_offset);

	   /*
	     Recompute the objective function values.
	   */

	   *pobj=calc_pobj(C,X,constant_offset);
	   *dobj=calc_dobj(k,a,y,constant_offset);
	   if (parameters.usexzgap==0)
	     {
	       gap=*dobj-*pobj;
	       if (gap < 0.0)
		 gap=0.0;
	       relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
	     }
	   else
	     {
	       gap=trace_prod(X,Z);
	       if (gap < 0.0)
		 gap=0.0;
	       relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
	     };

	   if (printlevel >= 2)
	     {
	       printf("pobj is %e \n",*pobj);
	       printf("dobj is %e \n",*dobj);
	       printf("gap is %e \n",gap);
	       printf("relgap is %e \n",relgap);
	     };

	   relpinfeas=pinfeas(k,constraints,X,a,workvec1);
	   reldinfeas=dinfeas(k,C,constraints,y,Z,work2);
	   
	   if (relpinfeas < parameters.axtol )
	     probpfeas=1;
	   
	   if (reldinfeas < parameters.atytol)
	     probdfeas=1;

	   oldmaxrelinfeas=maxrelinfeas;
	   if (relpinfeas > reldinfeas)
	     {
	       maxrelinfeas=relpinfeas;
	     }
	   else
	     {
	       maxrelinfeas=reldinfeas;
	     };

	   /*
	    * Make sure that the objective value hasn't gone crazy.
	    *
	    * This was a test with isnan, but isnan isn't ANSI C, so
	    * we use an equivalent that typically works.
	    *
	    */

	   if ((gap) != (gap))
	     {
	       retcode=12;
	       goto RETURNBEST;
	     };

	   /*
	    *  This was a test with isinf, but isinf isn't ANSI C, so
	    *  we just test for extremely large values.
	    */

	   if ((gap > 1.0e100) || (gap < -1.0e100))
	     {
	       retcode=12;
	       goto RETURNBEST;
	     };

	   /*
	    *  if this solution is better than the previous best, then 
	    *  update our best solution.
	    */

	   if ((relgap/parameters.objtol < bestmeas) &&
	       (relpinfeas/parameters.axtol <bestmeas) &&
	       (ispfeasprob==0) && (isdfeasprob==0) &&
	       (reldinfeas/parameters.atytol <bestmeas))
	     {
	       lastimprove=iter;

	       bestmeas=relgap/parameters.objtol;
	       if (relpinfeas/parameters.axtol > bestmeas)
		 bestmeas=relpinfeas/parameters.axtol;
	       if (reldinfeas/parameters.atytol > bestmeas)
		 bestmeas=reldinfeas/parameters.atytol;

	       store_packed(X,bestx);
	       store_packed(Z,bestz);
	       for (i=1; i<=k; i++)
		 besty[i]=y[i];
	       if (printlevel >= 3)
		 printf("New best solution, %e \n",bestmeas);
	     };

	   if ((ispfeasprob==1) &&
	       (relpinfeas/parameters.axtol <bestmeas))
	     {
	       lastimprove=iter;

	       bestmeas=relpinfeas/parameters.axtol;

	       store_packed(X,bestx);
	       for (i=1; i<=k; i++)
		 besty[i]=0.0;
	       make_i(work2);
	       zero_mat(work3);
	       addscaledmat(work3,0.005*parameters.objtol/trace_prod(X,work2),work2,work3);
	       store_packed(work3,bestz);
	       if (printlevel >= 3)
		 printf("New best solution, %e \n",bestmeas);
	     };

	   if ((isdfeasprob==1) &&
	       (reldinfeas/parameters.atytol <bestmeas))
	     {
	       lastimprove=iter;

	       bestmeas=reldinfeas/parameters.atytol;

	       zero_mat(work3);
	       make_i(work1);
	       addscaledmat(work3,1.0e-40,work1,work3);
	       store_packed(work3,bestx);
	       store_packed(Z,bestz);
	       for (i=1; i<=k; i++)
		 besty[i]=y[i];
	       if (printlevel >= 3)
		 printf("New best solution, %e \n",bestmeas);
	     };

	   /*
	    *  If we haven't improved the gap much in 20 iterations, then
	    *  give up, because we're not making progress.
	    */

	   if ((iter > 60) && (bestmeas > 0.5*bestarray[((iter-20) % BASIZE)]))
	     {
	       if (printlevel >= 1)
		 printf("Lack of progress.  Giving up!\n");
	       retcode=7;
	       goto RETURNBEST;
	     };

	   /*
	     Update counters and display status.
	   */

	   iter++;
	   if (printlevel >= 1)
	     {
	       printf("Iter: %2d Ap: %.2e Pobj: % .7e Ad: %.2e Dobj: % .7e \n",iter,alphap,*pobj,alphad,*dobj);
	       fflush(stdout);
	     };
	   
	   /*
	    *  If iter gets above maxiter, then exit.
	    */
	   
	   if (iter >= parameters.maxiter)
	     {
	       if (printlevel >= 1)
		 printf("Maximum iterations reached. \n");
	       retcode=4;
	       goto RETURNBEST;
	     };

	 };

  /*
   *  Return success.
   */

  retcode=0;

 RETURNBEST:

  store_unpacked(bestx,X);
  store_unpacked(bestz,Z);
  for (i=1; i<=k; i++)
    y[i]=besty[i];

  if (parameters.usexzgap==0)
    {
      *pobj=calc_pobj(C,X,constant_offset);
      *dobj=calc_dobj(k,a,y,constant_offset);
      gap=*dobj-*pobj;
    }
  else
    {
      *pobj=calc_pobj(C,X,constant_offset);
      *dobj=calc_dobj(k,a,y,constant_offset);
      gap=trace_prod(X,Z);
    };


  if ((gap < 0.0) && (parameters.usexzgap==0) && (parameters.tweakgap==1)
      && (ispfeasprob==0) && (isdfeasprob==0))
    {
      tweakgap(n,k,a,constraints,gap,Z,dZ,y,dy,work1,work2,work3,dX,
	       workvec1,workvec2,workvec3,workvec4,printlevel);
    };

  /*
    Recompute the objective function values.
  */

  *pobj=calc_pobj(C,X,constant_offset);
  *dobj=calc_dobj(k,a,y,constant_offset);

  if (parameters.usexzgap==0)
    {
      gap=*dobj-*pobj;
      if (gap < 0.0)
	gap=0.0;
      relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
    }
  else
    {
      gap=trace_prod(X,Z);
      if (gap < 0.0)
	gap=0.0;
      relgap=gap/(1.0+fabs(*dobj)+fabs(*pobj));
    };

  /*
   * Recheck the primal and dual infeasibilities and gap.
   */

  relpinfeas=pinfeas(k,constraints,X,a,workvec1);
  reldinfeas=dinfeas(k,C,constraints,y,Z,work1);

  if (relpinfeas < parameters.axtol )
    probpfeas=1;

  if (reldinfeas < parameters.atytol)
    probdfeas=1;

  if ((relgap < parameters.objtol) &&
      (relpinfeas <parameters.axtol ) &&
      (reldinfeas <parameters.atytol))
    {
      /*
       * Our best solution actually did satisfy the conditions, so we've
       * succeeded in solving the problem!
       *
       */
      retcode=0;
    };

  /*
   * Check for a good solution with slightly reduced accuracy.
   */

  if ((ispfeasprob==0) && (isdfeasprob==0))
    {
      bestmeas=relgap/parameters.objtol;
      if ((relpinfeas/parameters.axtol) > bestmeas)
	bestmeas=relpinfeas/parameters.axtol;
      if ((reldinfeas/parameters.atytol) > bestmeas)
	bestmeas=reldinfeas/parameters.atytol;

      if ((bestmeas > 1.0) && (bestmeas < 1000.0))
	retcode=3;
    };

  if (ispfeasprob==1)
    {
      bestmeas=relpinfeas/parameters.axtol;
 
      if ((bestmeas > 1.0) && (bestmeas < 1000.0))
	retcode=3;
    };

  if (isdfeasprob==1)
    {
      bestmeas=reldinfeas/parameters.atytol;
 
      if ((bestmeas > 1.0) && (bestmeas < 1000.0))
	retcode=3;
    };

  /*
   * Come here if we have an infeasible problem and are returning the
   * certificate of infeasibility.
   */

RETURNCERT:

  /*
   * Print out the total number of iterations.
   */
  
  if (printlevel >= 2)
    printf("Total Iterations: %d \n",iter);
  /*
   * Now, go ahead and return.
   */

  return(retcode);

}


