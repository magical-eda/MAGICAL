/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/********************************************************************


********************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnabs.h"
#include "wnmax.h"
#include "wnsqr.h"
#include "wncpy.h"
#include "wnswap.h"
#include "wnrnd.h"
#include "wnrndd.h"
#include "wnsll.h"
#include "wnsort.h"
#include "wncmp.h"

#include "wnvect.h"
#include "wnmat.h"

#include "wnconj.h"
#include "wnconjd.h"
#include "wncnjdn.h"



#define MAX_EXPAND   (20.0)
/*
#define MIN_CONTRACT (1.0/10.0)
*/
#define MIN_CONTRACT (0.75)

#define FRACT_CONSTRAINTS_VIOLATE   (0.20)

#define GOLDEN_RATIO          1.618034
#define GOLDEN_SECTION        0.3819660



int wn_conj_direction_noise_debug = WN_CONJ_DIR_DBG_NONE;


local int num_vars;

local double **search_directions;
local int num_search_directions,max_num_search_directions;

local double *coord_search_radiuses,*search_dir_search_radiuses;

local double *coord_mins,*coord_maxes;

local int num_func_calls;

local bool last_line_function_x_valid;
local double last_line_function_x,
	     last_line_function_mean,last_line_function_sdev;

local int num_evals;

local double save_sdev_for_1_eval;



local double *buffer_vect,*save_vect,*save_direction;
local void (*prepeat_function)(double *pmean,double *psdev,
			       double vect[],int num_evals);


local bool force_optimize_stop_flag=FALSE;



void wn_force_conj_direction_noise_stop(void)
{
  force_optimize_stop_flag = TRUE;
}


local void snap_num_into_allowed_range(double *pnum,double min,double max)
{
  wn_assert(min <= max);

  if(*pnum < min)
  {
    *pnum = min;
  }
  else if(*pnum > max)
  {
    *pnum = max;
  }
}


local void snap_vect_into_allowed_values(double vect[])
{
  int i;

  for(i=0;i<num_vars;++i)
  {
    snap_num_into_allowed_range(&(vect[i]),coord_mins[i],coord_maxes[i]);
  }
}


local void function_stats(double *pmean,double *psdev,double vect[],
			  int num_evals)
{
  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_ALL)
  {
    printf("function call %d at value\n",num_func_calls);
    wn_print_vect(vect,num_vars);
    printf("num_evals = %d\n",num_evals);
    fflush(stdout);
  }

  (*prepeat_function)(pmean,psdev,vect,num_evals);

  wn_assert(num_evals >= 1);
  num_func_calls += num_evals;

  *pmean = wn_clip_f(*pmean);
  if(num_evals > 1)
  {
    *psdev /= sqrt(num_evals-1.0);
  }
  else
  {
    *psdev = save_sdev_for_1_eval;
  }

  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
  {
    printf("function value is %lg, sdev of mean = %lg\n",*pmean,*psdev);
    fflush(stdout);
  }
}


local void line_function_stats(double *pmean,double *psdev,double x)
{
  if(last_line_function_x_valid)
  {
    if(x == last_line_function_x)
    {
      *pmean = last_line_function_mean;
      *psdev = last_line_function_sdev;
    }
  }

  wn_copy_vect(save_vect,buffer_vect,num_vars);
  wn_add_scaled_vect(save_vect,save_direction,x,num_vars);
  snap_vect_into_allowed_values(save_vect);

  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
  {
    printf("testing function value at x = %lg\n",x);
    fflush(stdout);
  }

  function_stats(pmean,psdev,save_vect,num_evals);

  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
  {
    printf("function value at x = %lg is %lg, sdev of mean = %lg\n",
	   x,*pmean,*psdev);
    fflush(stdout);
  }

  last_line_function_x_valid = TRUE;
  last_line_function_x = x;
  last_line_function_mean = *pmean;
  last_line_function_sdev = *psdev;
}


static double save_min,save_max;

local double line_function(double x)
{
  double mean,sdev;

  if((x < save_min)||(x > save_max))
  {
    return(WN_FHUGE);
  }

  line_function_stats(&mean,&sdev,x);

  return(mean);
}


local bool significantly_less_than(double f1,double s1,
				   double f2,double s2,
				   double num_sdevs)
{
  double sdev;

  sdev = sqrt(wn_square(s1)+wn_square(s2));

  return(f2-f1 > num_sdevs*sdev);
}


#if 0
local double compute_noise_error
(
  double ax,double fa,double sa,
  double bx,double fb,double sb,
  double cx,double fc,double sc
)
{
  double a,x0,b;
  int i;
  int code;
  double fan,fbn,fcn;  /* with noise */
  double sum_x0,sum2_x0;

  if(num_evals <= 2)
  {
    sa = sb = sc = (1.0/3.0)*(sa+sb+sc);
  }

  sum_x0 = 0.0;
  sum2_x0 = 0.0;

  for(i=0;i<100;++i)
  {
    fan = fa + sa*wn_normal_distribution();
    fbn = fb + sb*wn_normal_distribution();
    fcn = fc + sc*wn_normal_distribution();

    wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fan,bx,fbn,cx,fcn);

    if((code != WN_SUCCESS)||(!(a > 0.0)))
    {
      return(WN_FHUGE);
    }

    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }

  sum_x0 /= 100.0;
  sum2_x0 /= 100.0;

  sum2_x0 -= wn_square(sum_x0);

  sum2_x0 *= (100.0/99.0);

  return(sqrt(sum2_x0));
}
#endif


local double compute_curvature_f_weight(double xthis,double x1,double x2)
{
  return(1.0/((xthis-x1)*(xthis-x2)));
}


local void compute_curvature
(
  double *pa,double *psa,
  double x0,double f0,double s0,
  double x1,double f1,double s1,
  double x2,double f2,double s2
)
{
  double w0,w1,w2;

  wn_assert(x0 != x1);
  wn_assert(x0 != x2);
  wn_assert(x1 != x2);

  w0 = compute_curvature_f_weight(x0,x1,x2);
  w1 = compute_curvature_f_weight(x1,x0,x2);
  w2 = compute_curvature_f_weight(x2,x0,x1);

  *pa = w0*f0 + w1*f1 + w2*f2;

  *psa = sqrt(wn_square(w0*s0) + wn_square(w1*s1) + wn_square(w2*s2));
}


local void compute_delta_curvature
(
  double *pda,double *psda,
  double x0,double f0,double s0,
  double x1,double f1,double s1,
  double x2,double f2,double s2,
  double x3,double f3,double s3
)
{
  double wl0,wl1,wl2,wr1,wr2,wr3;
  double w0,w1,w2,w3;

  wn_assert(x0 < x1);
  wn_assert(x1 < x2);
  wn_assert(x2 < x3);

  wl0 = compute_curvature_f_weight(x0,x1,x2);
  wl1 = compute_curvature_f_weight(x1,x0,x2);
  wl2 = compute_curvature_f_weight(x2,x0,x1);

  wr1 = compute_curvature_f_weight(x1,x2,x3);
  wr2 = compute_curvature_f_weight(x2,x1,x3);
  wr3 = compute_curvature_f_weight(x3,x1,x2);

  w0 = -wl0;
  w1 = wr1 - wl1;
  w2 = wr2 - wl2;
  w3 = wr3;

  *pda = w0*f0 + w1*f1 + w2*f2 + w3*f3;

  *psda = sqrt(wn_square(w0*s0) + wn_square(w1*s1) +
	       wn_square(w2*s2) + wn_square(w3*s3));
}


local double compute_noise_error
(
  double ax,double fa,double sa,
  double bx,double fb,double sb,
  double cx,double fc,double sc
)
{
  double curvature,scurvature;

  if(num_evals <= 2)
  {
    sa = sb = sc = (1.0/3.0)*(sa+sb+sc);
  }

  compute_curvature(&curvature,&scurvature,
		    ax,fa,sa,
		    bx,fb,sb,
		    cx,fc,sc);

  return(scurvature);
}


local void strict_order_data2
(
  double *px0,double *pf0,double *ps0,
  double *px1,double *pf1,double *ps1
)
{
  if(*px0 > *px1)
  {
    wn_swap(*px0,*px1,double);
    wn_swap(*pf0,*pf1,double);
    wn_swap(*ps0,*ps1,double);
  }
}


local void strict_order_data3
(
  double *px0,double *pf0,double *ps0,
  double *px1,double *pf1,double *ps1,
  double *px2,double *pf2,double *ps2
)
{
  strict_order_data2(px0,pf0,ps0,
		     px1,pf1,ps1);
  strict_order_data2(px0,pf0,ps0,
		     px2,pf2,ps2);
  strict_order_data2(px1,pf1,ps1,
		     px2,pf2,ps2);
}


local void strict_order_data4
(
  double *px0,double *pf0,double *ps0,
  double *px1,double *pf1,double *ps1,
  double *px2,double *pf2,double *ps2,
  double *px3,double *pf3,double *ps3
)
{
  strict_order_data2(px0,pf0,ps0,
		     px1,pf1,ps1);
  strict_order_data2(px0,pf0,ps0,
		     px2,pf2,ps2);
  strict_order_data2(px0,pf0,ps0,
		     px3,pf3,ps3);
  strict_order_data2(px1,pf1,ps1,
		     px2,pf2,ps2);
  strict_order_data2(px1,pf1,ps1,
		     px3,pf3,ps3);
  strict_order_data2(px2,pf2,ps2,
		     px3,pf3,ps3);
}


local void compute_cubic_error
(
  double *pcubic_error,double *ps_cubic_error,
  double testx,double ftest,double stest,
  double ax,double fa,double sa,
  double bx,double fb,double sb,
  double cx,double fc,double sc
)
{
  double da,sda,da2;

  strict_order_data4(&testx,&ftest,&stest,
		     &ax,&fa,&sa,
		     &bx,&fb,&sb,
		     &cx,&fc,&sc);

  if(num_evals <= 2)
  {
    stest = sa = sb = sc = (1.0/4.0)*(stest+sa+sb+sc);
  }

  compute_delta_curvature(&da,&sda,
			  testx,ftest,stest,
			  ax,fa,sa,
			  bx,fb,sb,
			  cx,fc,sc);

  printf("da = %lg,sda = %lg\n",da,sda);

  da2 = wn_square(da) - wn_square(sda);
  if(da2 < 0.0)
  {
    da2 = 0.0;
  }

  *pcubic_error = sqrt(da2);
  *ps_cubic_error = sda;
}


#if 0
local void compute_metric_for_error
(
  double *pmetric,
  double ax,double fa,
  double bx,double fb,
  double cx,double fc
)
{
  double a,x0,b,c;
  int code;

  /*
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,bx,fb,cx,fc);

  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    *pmetric = WN_FHUGE;
  }
  else
  {
    *pmetric = x0;
  }
  */

  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,bx,fb,cx,fc);
  wn_assert(code == WN_SUCCESS);
  *pmetric = a;
}


local double compute_noise_error
(
  double ax,double fa,double sa,
  double bx,double fb,double sb,
  double cx,double fc,double sc
)
{
  double a,x0,b,c;
  int i;
  int code;
  int ca,cb,cc;
  double fan,fbn,fcn;  /* with noise */
  double sum_x0,sum2_x0;

  if(num_evals <= 2)
  {
    sa = sb = sc = (1.0/3.0)*(sa+sb+sc);
  }

  sum_x0 = 0.0;
  sum2_x0 = 0.0;

  for(ca = -1;ca <= 1;ca += 2)
  for(cb = -1;cb <= 1;cb += 2)
  for(cc = -1;cc <= 1;cc += 2)
  {
    fan = fa + sa*ca;
    fbn = fb + sb*cb;
    fcn = fc + sc*cc;

    /*
    */
    compute_metric_for_error(&x0,ax,fan,bx,fbn,cx,fcn);
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
#if 0
    wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fan,bx,fbn,cx,fcn);

    if((code != WN_SUCCESS)||(!(a > 0.0)))
    {
      sum2_x0 += wn_square(WN_FHUGE);
    }
    else
    {
      sum_x0 += x0;
      sum2_x0 += wn_square(x0);
    }
    wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fan,bx,fbn,cx,fcn);
    wn_assert(code == WN_SUCCESS);
    sum_x0 += b;
    sum2_x0 += wn_square(b);
#endif
  }

  sum_x0 /= 8.0;
  sum2_x0 /= 8.0;

  sum2_x0 -= wn_square(sum_x0);

  return(sqrt(sum2_x0));
}


local double compute_cubic_error
(
  double testx,double ftest,
  double ax,double fa,
  double bx,double fb,
  double cx,double fc
)
{
  double a,x0,b,c;
  int code;
  double sum_x0,sum2_x0;

  sum_x0 = 0.0;
  sum2_x0 = 0.0;

  /*
  */
  compute_metric_for_error(&x0,ax,fa,bx,fb,cx,fc);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
#if 0
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,bx,fb,cx,fc);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,bx,fb,cx,fc);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += a;
  sum2_x0 += wn_square(a);
#endif

  /*
  */
  compute_metric_for_error(&x0,testx,ftest,bx,fb,cx,fc);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
#if 0
  wn_fit_parabola_3p(&code,&a,&x0,&b,testx,ftest,bx,fb,cx,fc);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,testx,ftest,bx,fb,cx,fc);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += b;
  sum2_x0 += wn_square(b);
#endif

  /*
  */
  compute_metric_for_error(&x0,ax,fa,testx,ftest,cx,fc);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
#if 0
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,testx,ftest,cx,fc);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,testx,ftest,cx,fc);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += b;
  sum2_x0 += wn_square(b);
#endif

  /*
  */
  compute_metric_for_error(&x0,ax,fa,bx,fb,testx,ftest);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
#if 0
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,bx,fb,testx,ftest);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,bx,fb,testx,ftest);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += b;
  sum2_x0 += wn_square(b);
#endif

  /* compute sdev */
  sum_x0 /= 4.0;
  sum2_x0 /= 4.0;

  sum2_x0 -= wn_square(sum_x0);

  /*
  return((1.0/3.0)*sqrt(sum2_x0));
  */
  return((1.0/8.0)*sqrt(sum2_x0));
}
#endif


#if 0
local double compute_noise_error
(
  double ax,double fa,double sa,
  double bx,double fb,double sb,
  double cx,double fc,double sc
)
{
  double a,x0,b,c;
  int i;
  int code;
  int ca,cb,cc;
  double fan,fbn,fcn;  /* with noise */
  double sum_x0,sum2_x0;

  if(num_evals <= 2)
  {
    sa = sb = sc = (1.0/3.0)*(sa+sb+sc);
  }

  sum_x0 = 0.0;
  sum2_x0 = 0.0;

  for(ca = -1;ca <= 1;ca += 2)
  for(cb = -1;cb <= 1;cb += 2)
  for(cc = -1;cc <= 1;cc += 2)
  {
    fan = fa + sa*ca;
    fbn = fb + sb*cb;
    fcn = fc + sc*cc;

    /*
    compute_metric_for_error(&x0,ax,fan,bx,fbn,cx,fcn);
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
    */
    wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fan,bx,fbn,cx,fcn);

    if((code != WN_SUCCESS)||(!(a > 0.0)))
    {
      sum2_x0 += wn_square(WN_FHUGE);
    }
    else
    {
      sum_x0 += x0;
      sum2_x0 += wn_square(x0);
    }
#if 0
    wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fan,bx,fbn,cx,fcn);
    wn_assert(code == WN_SUCCESS);
    sum_x0 += b;
    sum2_x0 += wn_square(b);
#endif
  }

  sum_x0 /= 8.0;
  sum2_x0 /= 8.0;

  sum2_x0 -= wn_square(sum_x0);

  return(sqrt(sum2_x0));
}


local double compute_cubic_error
(
  double testx,double ftest,
  double ax,double fa,
  double bx,double fb,
  double cx,double fc
)
{
  double a,x0,b,c;
  int code;
  double sum_x0,sum2_x0;

  sum_x0 = 0.0;
  sum2_x0 = 0.0;

  /*
  compute_metric_for_error(&x0,ax,fa,bx,fb,cx,fc);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
  */
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,bx,fb,cx,fc);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
#if 0
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,bx,fb,cx,fc);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += a;
  sum2_x0 += wn_square(a);
#endif

  /*
  compute_metric_for_error(&x0,testx,ftest,bx,fb,cx,fc);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
  */
  wn_fit_parabola_3p(&code,&a,&x0,&b,testx,ftest,bx,fb,cx,fc);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
#if 0
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,testx,ftest,bx,fb,cx,fc);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += b;
  sum2_x0 += wn_square(b);
#endif

  /*
  compute_metric_for_error(&x0,ax,fa,testx,ftest,cx,fc);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
  */
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,testx,ftest,cx,fc);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
#if 0
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,testx,ftest,cx,fc);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += b;
  sum2_x0 += wn_square(b);
#endif

  /*
  compute_metric_for_error(&x0,ax,fa,bx,fb,testx,ftest);
  sum_x0 += x0;
  sum2_x0 += wn_square(x0);
  */
  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,bx,fb,testx,ftest);
  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    sum2_x0 += wn_square(WN_FHUGE);
  }
  else
  {
    sum_x0 += x0;
    sum2_x0 += wn_square(x0);
  }
#if 0
  wn_fit_traditional_parabola_3p(&code,&a,&b,&c,ax,fa,bx,fb,testx,ftest);
  wn_assert(code == WN_SUCCESS);
  sum_x0 += b;
  sum2_x0 += wn_square(b);
#endif

  /* compute sdev */
  sum_x0 /= 4.0;
  sum2_x0 /= 4.0;

  sum2_x0 -= wn_square(sum_x0);

  return((1.0/3.0)*sqrt(sum2_x0));
  /*
  return((1.0/8.0)*sqrt(sum2_x0));
  */
}
#endif


local void sort_data2
(
  double *pax,double *pfa,double *psa,
  double *pbx,double *pfb,double *psb
)
{
  if(*pax > *pbx)
  {
    wn_swap(*pax,*pbx,double);
    wn_swap(*pfa,*pfb,double);
    wn_swap(*psa,*psb,double);
  }
}


local void sort_data3
(
  double *pax,double *pfa,double *psa,
  double *pbx,double *pfb,double *psb,
  double *pcx,double *pfc,double *psc
)
{
  sort_data2(pax,pfa,psa,
	     pbx,pfb,psb);
  sort_data2(pbx,pfb,psb,
	     pcx,pfc,psc);
  sort_data2(pax,pfa,psa,
	     pbx,pfb,psb);
}


local void sort_data4
(
  double *pax,double *pfa,double *psa,
  double *pbx,double *pfb,double *psb,
  double *pcx,double *pfc,double *psc,
  double *ptestx,double *pftest,double *pstest
)
{
  sort_data3(pax,pfa,psa,
	     pbx,pfb,psb,
	     pcx,pfc,psc);

  if(*ptestx < *pax)
  {
    if(((*pfa) < (*pfb))&&((*pfa) < (*pfc)))
    {
      wn_swap(*pax,*ptestx,double);
      wn_swap(*pfa,*pftest,double);
      wn_swap(*psa,*pstest,double);

      wn_swap(*pbx,*ptestx,double);
      wn_swap(*pfb,*pftest,double);
      wn_swap(*psb,*pstest,double);

      wn_swap(*pcx,*ptestx,double);
      wn_swap(*pfc,*pftest,double);
      wn_swap(*psc,*pstest,double);
    }
  }
  else if(*ptestx > *pcx)
  {
    if(((*pfc) < (*pfb))&&((*pfc) < (*pfa)))
    {
      wn_swap(*pcx,*ptestx,double);
      wn_swap(*pfc,*pftest,double);
      wn_swap(*psc,*pstest,double);

      wn_swap(*pbx,*ptestx,double);
      wn_swap(*pfb,*pftest,double);
      wn_swap(*psb,*pstest,double);

      wn_swap(*pax,*ptestx,double);
      wn_swap(*pfa,*pftest,double);
      wn_swap(*psa,*pstest,double);
    }
  }
}


local void gen_shrink_testx
(
  double *ptestx,
  double ax,double fa,
  double bx,double fb,
  double cx,double fc
)
{
  double dummy;

  sort_data3(&ax,&fa,&dummy,
	     &bx,&fb,&dummy,
	     &cx,&fc,&dummy);

  if((fb <= fa)&&(fb <= fc))
  {
    /* force wn_abs(ax-bx) >= wn_abs(cx-bx) */
    if(
	(wn_abs(ax-bx) < wn_abs(cx-bx))
	  ||
        (
	  (wn_abs(ax-bx) == wn_abs(cx-bx))
	    &&
	  (fa > fc)
        )
      )
    {
      wn_swap(ax,cx,double);
      wn_swap(fa,fc,double);
    }

    *ptestx = GOLDEN_SECTION*ax + (1.0-GOLDEN_SECTION)*bx;
  }
  else /* !((fb <= fa)&&(fb <= fc)) */
  {
    /* force fa < fc */
    if(fa > fc)
    {
      wn_swap(ax,cx,double);
      wn_swap(fa,fc,double);
    }

    *ptestx = (1.0-GOLDEN_SECTION)*ax + GOLDEN_SECTION*bx;
  }
}


local void encorporate_test_into_shrunk_range
(
  double *pax,double *pfa,double *psa,
  double *pbx,double *pfb,double *psb,
  double *pcx,double *pfc,double *psc,
  double *ptestx,double *pftest,double *pstest
)
{
  sort_data3(pax,pfa,psa,
	     pbx,pfb,psb,
	     pcx,pfc,psc);

  wn_assert(((*pax) < (*ptestx))&&((*ptestx) < (*pcx)));
  wn_assert((*ptestx) != (*pbx));

  /* force testx to be between ax and bx */
  if((*ptestx) > (*pbx))
  {
    wn_swap(*pax,*pcx,double);
    wn_swap(*pfa,*pfc,double);
    wn_swap(*psa,*psc,double);
  }

  if((*pfb) <= (*pfa))
  {
    wn_assert((*pfb) <= (*pfc));

    if(*pftest >= *pfb)
    {
      wn_swap(*pax,*ptestx,double);
      wn_swap(*pfa,*pftest,double);
      wn_swap(*psa,*pstest,double);
    }
    else /* ftest < *pfb */
    {
      wn_swap(*pbx,*ptestx,double);
      wn_swap(*pfb,*pftest,double);
      wn_swap(*psb,*pstest,double);

      wn_swap(*pcx,*ptestx,double);
      wn_swap(*pfc,*pftest,double);
      wn_swap(*psc,*pstest,double);
    }
  }
  else /* (fa < fb) */
  {
    wn_swap(*pbx,*ptestx,double);
    wn_swap(*pfb,*pftest,double);
    wn_swap(*psb,*pstest,double);

    wn_swap(*pcx,*ptestx,double);
    wn_swap(*pfc,*pftest,double);
    wn_swap(*psc,*pstest,double);
  }

  sort_data3(pax,pfa,psa,
             pbx,pfb,psb,
	     pcx,pfc,psc);
}


local void gen_expand_testx
(
  double *ptestx,
  int preferred_sign,
  double range_lo,double range_hi,
  double dist,
  double min,double max
)
{
  int original_preferred_sign;

  original_preferred_sign = preferred_sign;

  if(range_hi < range_lo)
  {
    wn_swap(range_hi,range_lo,double);
  }
  if(max < min)
  {
    wn_swap(max,min,double);
  }
  if(range_hi > max)
  {
    range_hi = max;
  }
  if(range_lo < min)
  {
    range_lo = min;
  }
  dist = wn_abs(dist);

  if(preferred_sign == 0)
  {
    preferred_sign = wn_random_bit()?(1):(-1);
  }

  if(preferred_sign > 0)
  {
    *ptestx = range_hi + dist;
    if(*ptestx <= max)
    {
      return;
    }

    if(original_preferred_sign > 0)
    {
      if((max-range_hi) >= 0.5*dist)
      {
	*ptestx = max;
	return;
      }
    }

    if(max-range_hi >= range_lo-min)
    {
      *ptestx = max;
    }
    else /* max-range_hi < range_lo-min */
    {
      *ptestx = wn_max(range_lo-dist,min);
    }
  }
  else
  {
    wn_assert(preferred_sign < 0);

    *ptestx = range_lo - dist;
    if(*ptestx >= min)
    {
      return;
    }

    if(original_preferred_sign < 0)
    {
      if((range_lo-min) >= 0.5*dist)
      {
	*ptestx = min;
	return;
      }
    }

    if(range_lo-min >= max-range_hi)
    {
      *ptestx = min;
    }
    else /* range_lo-min < max-range_hi */
    {
      *ptestx = wn_min(range_hi+dist,max);
    }
  }
}


local void setup_initial_range
(
  bool *pexpansion_is_possible,
  double *pax,double *pfa,double *psa,
  double *pbx,double *pfb,double *psb,
  double *pcx,double *pfc,double *psc,
  double *ptestx,double *pftest,double *pstest,
  double search_radius,
  double min,double max
)
{
  double separation,x,dist,closest_dist;
  int i,i_closest_to_b;
  int preferred_sign;

  wn_assert(min <= *pbx);
  wn_assert(*pbx <= max);

  separation = (max-min)/3.0;
  if(separation <= search_radius)
  {
    closest_dist = WN_FHUGE;

    for(i=0;i<=3;++i)
    {
      x = ((3-i)*min + i*max)/3.0;
      dist = wn_abs(x - *pbx);
      if(dist < closest_dist)
      {
	closest_dist = dist;
	i_closest_to_b = i;
      }
    }

    switch(i_closest_to_b)
    {
      default:
	wn_assert_notreached();
	break;
      case(0):
	*pax = *pbx;
	*pfa = *pfb;
	*psa = *psb;

        i = 1;
        *pbx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfb,psb,*pbx);

        i = 2;
        *pcx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfc,psc,*pcx);

        i = 3;
        *ptestx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pftest,pstest,*ptestx);

	break;
      case(1):
        i = 0;
        *pax = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfa,psa,*pax);

        i = 2;
        *pcx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfc,psc,*pcx);

        i = 3;
        *ptestx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pftest,pstest,*ptestx);

	break;
      case(2):
        i = 3;
        *pax = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfa,psa,*pax);

        i = 1;
        *pcx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfc,psc,*pcx);

        i = 0;
        *ptestx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pftest,pstest,*ptestx);

	break;
      case(3):
	*pax = *pbx;
	*pfa = *pfb;
	*psa = *psb;

        i = 2;
        *pbx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfb,psb,*pbx);

        i = 1;
        *pcx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pfc,psc,*pcx);

        i = 0;
        *ptestx = ((3-i)*min + i*max)/3.0;
        line_function_stats(pftest,pstest,*ptestx);

	break;
    }

    sort_data3(pax,pfa,psa,
	       pbx,pfb,psb,
	       pcx,pfc,psc);

    *pexpansion_is_possible = FALSE;
  }
  else /* separation > search_radius */
  {
    separation = search_radius;

    gen_expand_testx(pax,0,*pbx,*pbx,separation,min,max);
    line_function_stats(pfa,psa,*pax);

    sort_data2(pax,pfa,psa,
	       pbx,pfb,psb);

    if(*pfa < *pfb)
    {
      preferred_sign = -1;
    }
    else if(*pfa > *pfb)
    {
      preferred_sign = 1;
    }
    else
    {
      preferred_sign = 0;
    }

    gen_expand_testx(pcx,preferred_sign,*pax,*pbx,separation,min,max);
    line_function_stats(pfc,psc,*pcx);

    sort_data3(pax,pfa,psa,
	       pbx,pfb,psb,
	       pcx,pfc,psc);

    if(*pfa < *pfc)
    {
      preferred_sign = -1;
    }
    else if(*pfa > *pfc)
    {
      preferred_sign = 1;
    }
    else
    {
      preferred_sign = 0;
    }

    gen_expand_testx(ptestx,preferred_sign,*pax,*pcx,2*separation,min,max);
    line_function_stats(pftest,pstest,*ptestx);

    *pexpansion_is_possible = !(((*ptestx) == min)||((*ptestx) == max));
  }
}


local void compute_new_search_radius
(
  double *pax,double *pfa,double *psa,
  double *pbx,double *pfb,double *psb,
  double *pcx,double *pfc,double *psc,
  double *psearch_radius,
  double min,double max,
  int max_iterations
)
{
  double testx,ftest,stest;
  double cubic_error,s_cubic_error,noise_error,total_error;
  double best_ax,best_fa,best_sa,
         best_bx,best_fb,best_sb,
         best_cx,best_fc,best_sc,
	 best_total_error;
  bool expansion_is_possible;
  int iterations;
  int preferred_sign;

  printf("entering compute_new_search_radius.\n");

  if(*psearch_radius == 0.0)
  {
    *psearch_radius = 1.0;
  }

  setup_initial_range(&expansion_is_possible,
                      pax,pfa,psa,
                      pbx,pfb,psb,
                      pcx,pfc,psc,
		      &testx,&ftest,&stest,
                      *psearch_radius,
                      min,max);

  sort_data4(pax,pfa,psa,
             pbx,pfb,psb,
             pcx,pfc,psc,
             &testx,&ftest,&stest);

  printf("ax=%lg,fa=%lg,sa=%lg\n",*pax,*pfa,*psa);
  printf("bx=%lg,fb=%lg,sb=%lg\n",*pbx,*pfb,*psb);
  printf("cx=%lg,fc=%lg,sc=%lg\n",*pcx,*pfc,*psc);
  printf("testx=%lg,ftest=%lg,stest=%lg\n",testx,ftest,stest);

  noise_error = 
      compute_noise_error(*pax,*pfa,*psa,*pbx,*pfb,*psb,*pcx,*pfc,*psc);
  compute_cubic_error(&cubic_error,&s_cubic_error,
		      testx,ftest,stest,
		      *pax,*pfa,*psa,*pbx,*pfb,*psb,*pcx,*pfc,*psc);
  total_error = sqrt(wn_square(noise_error) + wn_square(cubic_error));
  printf("total_error = %lg,cubic_error = %lg,noise_error = %lg\n",
	 total_error,cubic_error,noise_error);
  best_total_error = total_error;
  best_ax = *pax; best_fa = *pfa; best_sa = *psa;
  best_bx = *pbx; best_fb = *pfb; best_sb = *psb;
  best_cx = *pcx; best_fc = *pfc; best_sc = *psc;

  iterations = 0;

  if(cubic_error > noise_error+3.0*s_cubic_error)
  { /* shrink */
    do
    {
      if(!(iterations < max_iterations))
      {
	break;
      }

      printf("compute_new_search_radius: shrinking search_radius=%lg\n",
	     *psearch_radius);

      gen_shrink_testx(&testx,*pax,*pfa,*pbx,*pfb,*pcx,*pfc);

      line_function_stats(&ftest,&stest,testx);

      encorporate_test_into_shrunk_range(pax,pfa,psa,
					 pbx,pfb,psb,
					 pcx,pfc,psc,
					 &testx,&ftest,&stest);

      (*psearch_radius) = 0.5*wn_abs(*pax - *pcx);

      printf("ax=%lg,fa=%lg,sa=%lg\n",*pax,*pfa,*psa);
      printf("bx=%lg,fb=%lg,sb=%lg\n",*pbx,*pfb,*psb);
      printf("cx=%lg,fc=%lg,sc=%lg\n",*pcx,*pfc,*psc);
      printf("testx=%lg,ftest=%lg,stest=%lg\n",testx,ftest,stest);

      noise_error = 
          compute_noise_error(*pax,*pfa,*psa,*pbx,*pfb,*psb,*pcx,*pfc,*psc);
      compute_cubic_error(&cubic_error,&s_cubic_error,
			  testx,ftest,stest,
			  *pax,*pfa,*psa,*pbx,*pfb,*psb,*pcx,*pfc,*psc);
      total_error = sqrt(wn_square(noise_error) + wn_square(cubic_error));
      printf("total_error = %lg,cubic_error = %lg,noise_error = %lg\n",
	     total_error,cubic_error,noise_error);
      if(total_error < best_total_error)
      {
        best_total_error = total_error;
        best_ax = *pax; best_fa = *pfa; best_sa = *psa;
        best_bx = *pbx; best_fb = *pfb; best_sb = *psb;
        best_cx = *pcx; best_fc = *pfc; best_sc = *psc;
      }

      ++iterations;
    }
    while(cubic_error > noise_error+3.0*s_cubic_error);
  }
  else /* !(cubic_error > noise_error+3.0*s_cubic_error) */
  { /* grow */
    do
    {
      if((!(iterations < max_iterations))||(!expansion_is_possible))
      {
	break;
      }

      printf("compute_new_search_radius: growing search_radius=%lg\n",
	     *psearch_radius);

      if(testx < *pbx)
      {
	if((*pfa <= *pfb)||(*pfc <= *pfb))  /* fb is not minimum */
	{
          *pbx = *pax;
          *pfb = *pfa;
          *psb = *psa;
        }

        *pax = testx;
        *pfa = ftest;
        *psa = stest;
      }
      else 
      {
	wn_assert(testx > *pbx);

	if((*pfa <= *pfb)||(*pfc <= *pfb))  /* fb is not minimum */
	{
          *pbx = *pcx;
          *pfb = *pfc;
          *psb = *psc;
        }

        *pcx = testx;
        *pfc = ftest;
        *psc = stest;
      }

      if(*pfa < *pfc)
      {
        preferred_sign = -1;
      }
      else if(*pfa > *pfc)
      {
        preferred_sign = 1;
      }
      else
      {
        preferred_sign = 0;
      }

      gen_expand_testx(&testx,preferred_sign,*pax,*pcx,(*pcx)-(*pax),min,max);

      line_function_stats(&ftest,&stest,testx);

      expansion_is_possible = !((testx == min)||(testx == max));

      sort_data4(pax,pfa,psa,
                 pbx,pfb,psb,
                 pcx,pfc,psc,
                 &testx,&ftest,&stest);

      (*psearch_radius) = 0.5*wn_abs(*pax - *pcx);

      printf("ax=%lg,fa=%lg,sa=%lg\n",*pax,*pfa,*psa);
      printf("bx=%lg,fb=%lg,sb=%lg\n",*pbx,*pfb,*psb);
      printf("cx=%lg,fc=%lg,sc=%lg\n",*pcx,*pfc,*psc);
      printf("testx=%lg,ftest=%lg,stest=%lg\n",testx,ftest,stest);

      noise_error = 
          compute_noise_error(*pax,*pfa,*psa,*pbx,*pfb,*psb,*pcx,*pfc,*psc);
      compute_cubic_error(&cubic_error,&s_cubic_error,
			  testx,ftest,stest,
			  *pax,*pfa,*psa,*pbx,*pfb,*psb,*pcx,*pfc,*psc);
      total_error = sqrt(wn_square(noise_error) + wn_square(cubic_error));
      printf("total_error = %lg,cubic_error = %lg,noise_error = %lg\n",
	     total_error,cubic_error,noise_error);
      if(total_error < best_total_error)
      {
        best_total_error = total_error;
        best_ax = *pax; best_fa = *pfa; best_sa = *psa;
        best_bx = *pbx; best_fb = *pfb; best_sb = *psb;
        best_cx = *pcx; best_fc = *pfc; best_sc = *psc;
      }

      ++iterations;
    }
    while(
	   (cubic_error <= noise_error+3.0*s_cubic_error)
	     &&
           !(
	     (wn_abs(*pax - *pbx) < 0.2*wn_abs(*pcx - *pbx))
	       ||
	     (wn_abs(*pcx - *pbx) < 0.2*wn_abs(*pax - *pbx))
	   )
	 );
  }

  *pax = best_ax; *pfa = best_fa; *psa = best_sa;
  *pbx = best_bx; *pfb = best_fb; *psb = best_sb;
  *pcx = best_cx; *pfc = best_fc; *psc = best_sc;
  (*psearch_radius) = 0.5*wn_abs(*pax - *pcx);

  printf("done compute_new_search_radius: search_radius=%lg\n",
	 *psearch_radius);
  printf("ax=%lg,fa=%lg,sa=%lg\n",*pax,*pfa,*psa);
  printf("bx=%lg,fb=%lg,sb=%lg\n",*pbx,*pfb,*psb);
  printf("cx=%lg,fc=%lg,sc=%lg\n",*pcx,*pfc,*psc);
  printf("testx=%lg,ftest=%lg,stest=%lg\n",testx,ftest,stest);
}


local void line_minimize
(
  double vect[],
  double direction[],
  double *pval_min,
  double *psdev,
  double *psave_search_radius,
  double min,double max,
  bool find_new_search_radius
)
{
  double ax,bx,cx,x0,xtest,fa,fb,fc,fx0,ftest,sa,sb,sc,sx0,stest;
  double a,b;
  double old_search_radius;
  int code;
  bool search_radius_expanded;

  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
  {
    printf("start line minimize.\n");
  }

  last_line_function_x_valid = FALSE;

  wn_copy_vect(buffer_vect,vect,num_vars);
  save_vect = vect;
  save_direction = direction;

  old_search_radius = *psave_search_radius;

  bx = 0.0;
  fb = *pval_min;
  sb = *psdev;

  if(find_new_search_radius)
  {
    compute_new_search_radius(&ax,&fa,&sa,&bx,&fb,&sb,&cx,&fc,&sc,
			      psave_search_radius,
			      min,max,
			      10);
  }
  else
  {
    compute_new_search_radius(&ax,&fa,&sa,&bx,&fb,&sb,&cx,&fc,&sc,
			      psave_search_radius,
			      min,max,
			      3);
  }

  sort_data3(&ax,&fa,&sa,
	     &bx,&fb,&sb,
	     &cx,&fc,&sc);

  wn_fit_parabola_3p(&code,&a,&x0,&b,ax,fa,bx,fb,cx,fc);

  snap_num_into_allowed_range(&x0,min,max);

  if((code != WN_SUCCESS)||(!(a > 0.0)))
  {
    if(fa < fc)
    {
      if(min > -WN_FHUGE)
      {
	x0 = min;
      }
      else
      {
        goto full_linesearch;
      }
    }
    else if(fa > fc)
    {
      if(max < WN_FHUGE)
      {
	x0 = max;
      }
      else
      {
        goto full_linesearch;
      }
    }
    else
    {
      goto full_linesearch;
    }
  }   

  if(ax < cx)
  {
    if(x0 < ax - 0.5*wn_abs(*psave_search_radius))
    {
      goto full_linesearch;
    }
    if(x0 > cx + 0.5*wn_abs(*psave_search_radius))
    {
      goto full_linesearch;
    }
  }
  else if(ax > cx)
  {
    if(x0 > ax + 0.5*wn_abs(*psave_search_radius))
    {
      goto full_linesearch;
    }
    if(x0 < cx - 0.5*wn_abs(*psave_search_radius))
    {
      goto full_linesearch;
    }
  }

  if(x0 == ax)
  {
    fx0 = fa;
    sx0 = sa;
  }
  else if(x0 == bx)
  {
    fx0 = fb;
    sx0 = sb;
  }
  else if(x0 == cx)
  {
    fx0 = fc;
    sx0 = sc;
  }
  else
  {
    line_function_stats(&fx0,&sx0,x0);
  }

  if(significantly_less_than(*pval_min,*psdev,fx0,sx0,4.0))
  {
    printf("garbage result: reseting.\n");
    /*
    x0 = 0.0;
    fx0 = *pval_min;
    sx0 = *psdev;
    */
    goto full_linesearch;
  }

  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
  {
    printf("new search_radius = %lg, old search_radius = %lg\n",
	   *psave_search_radius,old_search_radius);
  }

  bx = x0;
  fb = fx0;
  sb = sx0;

  goto finish;

full_linesearch: ;

  printf("doing full linesearch....\n");

  save_min = min;
  save_max = max;
  wn_minimize_1d_raw(&code,&fa,&fb,&fc,&ax,&bx,&cx,*pval_min,
		     &line_function,1,10);

finish: ;

  wn_copy_vect(vect,buffer_vect,num_vars);
  wn_add_scaled_vect(vect,direction,bx,num_vars);
  snap_vect_into_allowed_values(vect);

  *pval_min = fb;
  *psdev = sb;

  if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
  {
    printf("****** finish line minimize.\n");
    fflush(stdout);
  }
}


local void initialize_coord_mins_and_maxes(void)
{
  int i;

  if(coord_mins == NULL)
  {
    wn_make_vect(&coord_mins,num_vars);

    for(i=0;i<num_vars;++i)
    {
      coord_mins[i] = -WN_FHUGE;
    }
  }

  if(coord_maxes == NULL)
  {
    wn_make_vect(&coord_maxes,num_vars);

    for(i=0;i<num_vars;++i)
    {
      coord_maxes[i] = WN_FHUGE;
    }
  }
}


#if 0
local void compute_1dsearch_limits
(
  double *pmin,double *pmax,
  double vect[],
  double search_direction[]
)
{
  int i;
  double coef,limit;

  *pmin = -WN_FHUGE;
  *pmax = WN_FHUGE;

  for(i=0;i<num_vars;++i)
  {
    coef = search_direction[i];

    if(coef > 0.0)
    {
      if(coord_maxes[i] < WN_FHUGE)
      {
        limit = (coord_maxes[i]-vect[i])/coef;
        if(limit < *pmax)
        {
	  *pmax = limit;
        }
      }

      if(coord_mins[i] > -WN_FHUGE)
      {
        limit = (coord_mins[i]-vect[i])/coef;
        if(limit > *pmin)
        {
	  *pmin = limit;
        }
      }
    }
    else if(coef < 0.0)
    {
      if(coord_maxes[i] < WN_FHUGE)
      {
        limit = (coord_maxes[i]-vect[i])/coef;
        if(limit > *pmin)
        {
	  *pmin = limit;
        }
      }

      if(coord_mins[i] > -WN_FHUGE)
      {
        limit = (coord_mins[i]-vect[i])/coef;
        if(limit < *pmax)
        {
	  *pmax = limit;
        }
      }
    }
  }
}
#endif


local void compute_1dsearch_limits
(
  double *pmin,double *pmax,
  double vect[],
  double search_direction[]
)
{
  int i;
  double coef,limit;
  double *plimit_copy;
  wn_sll min_limit_list,max_limit_list;
  int index;

  wn_gpmake("no_free");

  min_limit_list = NULL;
  max_limit_list = NULL;

  for(i=0;i<num_vars;++i)
  {
    coef = search_direction[i];

    if(coef > 0.0)
    {
      if(coord_maxes[i] < WN_FHUGE)
      {
        limit = (coord_maxes[i]-vect[i])/coef;
      }
      else
      {
	limit = WN_FHUGE;
      }
      wn_memacpy((ptr *)&plimit_copy,(ptr)&limit,sizeof(double));
      wn_sllins(&max_limit_list,plimit_copy);

      if(coord_mins[i] > -WN_FHUGE)
      {
        limit = (coord_mins[i]-vect[i])/coef;
      }
      else
      {
	limit = -WN_FHUGE;
      }
      wn_memacpy((ptr *)&plimit_copy,(ptr)&limit,sizeof(double));
      wn_sllins(&min_limit_list,plimit_copy);
    }
    else if(coef < 0.0)
    {
      if(coord_maxes[i] < WN_FHUGE)
      {
        limit = (coord_maxes[i]-vect[i])/coef;
      }
      else
      {
	limit = -WN_FHUGE;
      }
      wn_memacpy((ptr *)&plimit_copy,(ptr)&limit,sizeof(double));
      wn_sllins(&min_limit_list,plimit_copy);

      if(coord_mins[i] > -WN_FHUGE)
      {
        limit = (coord_mins[i]-vect[i])/coef;
      }
      else
      {
	limit = WN_FHUGE;
      }
      wn_memacpy((ptr *)&plimit_copy,(ptr)&limit,sizeof(double));
      wn_sllins(&max_limit_list,plimit_copy);
    }
    else
    {
      limit = -WN_FHUGE;
      wn_memacpy((ptr *)&plimit_copy,(ptr)&limit,sizeof(double));
      wn_sllins(&min_limit_list,plimit_copy);

      limit = WN_FHUGE;
      wn_memacpy((ptr *)&plimit_copy,(ptr)&limit,sizeof(double));
      wn_sllins(&max_limit_list,plimit_copy);
    }
  }

  wn_sort_sll(&min_limit_list,(int (*)(ptr,ptr))&wn_pdoublecmp);
  wn_sllrev(&min_limit_list);
  wn_sort_sll(&max_limit_list,(int (*)(ptr,ptr))&wn_pdoublecmp);

  index = (int)(FRACT_CONSTRAINTS_VIOLATE*wn_sllcount(min_limit_list));
  wn_sllindex((ptr *)&plimit_copy,min_limit_list,index);
  *pmin = *plimit_copy;

  index = (int)(FRACT_CONSTRAINTS_VIOLATE*wn_sllcount(max_limit_list));
  wn_sllindex((ptr *)&plimit_copy,max_limit_list,index);
  *pmax = *plimit_copy;

  wn_gpfree();
}


local bool is_zero_vect(double vect[],int len)
{
  int i;

  for(i=0;i<len;++i)
  {
    if(vect[i] != 0.0)
    {
      return(FALSE);
    }
  }

  return(TRUE);
}


void wn_conj_direction_noise_method
(
  int *pcode,
  double *pval_min,
  double vect[],
  double passed_coord_mins[],
  double passed_coord_maxes[],
  double initial_coord_search_radiuses[],
  int initial_offset,
  int passed_num_vars,
  void (*passed_prepeat_function)(double *pmean,double *psdev,
				  double vect[],int num_evals),
  int start_num_evals,
  int max_func_calls
)
{
  int i,j,iteration,count;
  int coord_index;
  double *old_vect,*coord_direction;
  double *new_search_direction;
  double old_val_min,sdev,mean_val_min,old_mean_val_min;
  double sdev_val_min,old_sdev_val_min;
  double min,max;

  wn_gpmake("no_free");

  force_optimize_stop_flag = FALSE;

  num_vars = passed_num_vars;
  max_num_search_directions = num_vars;
  prepeat_function = passed_prepeat_function;

  coord_mins = passed_coord_mins;
  coord_maxes = passed_coord_maxes;

  wn_make_vect(&buffer_vect,num_vars);
  search_directions = (double **)wn_zalloc(
		  max_num_search_directions*sizeof(double *));
  wn_make_vect(&old_vect,num_vars);
  wn_make_vect(&coord_direction,num_vars);
  wn_make_vect(&coord_search_radiuses,num_vars);
  wn_make_vect(&search_dir_search_radiuses,max_num_search_directions);

  if(initial_coord_search_radiuses != NULL)
  {
    wn_copy_vect(coord_search_radiuses,initial_coord_search_radiuses,num_vars);
  }

  initialize_coord_mins_and_maxes();

  snap_vect_into_allowed_values(vect);

  num_search_directions = 0;

  num_func_calls = 0;

  mean_val_min = WN_FHUGE;
  sdev_val_min = 0.0;

  num_evals = wn_max(1,start_num_evals);

  function_stats(pval_min,&sdev,vect,num_evals);

  for(iteration=0;;++iteration)
  {
    if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_PASSES)
    {
      printf("iteration = %d ********************************\n",iteration);
      printf("ob = %lg\n",*pval_min);
      printf("total num_func_calls = %d\n",num_func_calls);
      printf("experiment num_evals to %d\n",num_evals);
      fflush(stdout);
    }

    old_val_min = *pval_min;
    wn_copy_vect(old_vect,vect,num_vars);
    old_mean_val_min = mean_val_min;
    old_sdev_val_min = sdev_val_min;

    mean_val_min = 0.0;
    sdev_val_min = 0.0;
    count = 0;

    /* save sdev for num_evals == 1 */
    if(num_evals == 1)
    {
      int num_sdev_evals;

      num_sdev_evals = 10;

      function_stats(pval_min,&sdev,vect,num_sdev_evals);
      save_sdev_for_1_eval = sdev*sqrt((double) num_sdev_evals);

      if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
      {
        printf("num_evals == 1 : save_sdev_for_1_eval = %lg\n",
	       save_sdev_for_1_eval);
      }
    }

    /* minimize along acceleration search directions */
    if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
    {
      printf("start acceleration line minimizations ------------------\n");
    }
    for(i=0;i<num_search_directions;++i)
    {
      if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
      {
        printf("acceleration line search %d\n",i);
      }

      if(is_zero_vect(search_directions[i],num_vars))
      {
	continue;
      }

      compute_1dsearch_limits(&min,&max,vect,search_directions[i]);
      printf("min=%lg,max=%lg\n",min,max);

      if(min >= max)
      {
	continue;
      }

      line_minimize(vect,search_directions[i],pval_min,&sdev,
		    &(search_dir_search_radiuses[i]),
		    min,max,(i == 0));

      mean_val_min += (*pval_min);
      sdev_val_min += wn_square(sdev);
      ++count;

      if(
	  ((max_func_calls < WN_IHUGE)&&(num_func_calls > max_func_calls))
	    ||
          force_optimize_stop_flag
	)
      {
        *pcode = WN_SUBOPTIMAL;
	goto finish;
      }
    }

    /* minimize along coordinate directions */
    if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH) 
    {
      printf("start coordinate line minimizations ------------------\n");
    }
    for(i=0;i<num_vars;++i)
    {
      coord_index = (i+initial_offset)%num_vars;

      if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_LINESEARCH)
      {
        printf("coord line search %d,coord_index = %d\n",
	       i,coord_index);
      }

      coord_direction[coord_index] = 1.0;

      line_minimize(vect,coord_direction,pval_min,&sdev,
		    &(coord_search_radiuses[coord_index]),
		    coord_mins[coord_index]-vect[coord_index],
		    coord_maxes[coord_index]-vect[coord_index],
		    (iteration == 0));

      coord_direction[coord_index] = 0.0;

      mean_val_min += (*pval_min);
      sdev_val_min += wn_square(sdev);
      ++count;

      if(
	  ((max_func_calls < WN_IHUGE)&&(num_func_calls > max_func_calls))
	    ||
          force_optimize_stop_flag
	)
      {
        *pcode = WN_SUBOPTIMAL;
	goto finish;
      }
    }

    mean_val_min /= count;
    sdev_val_min = sqrt(sdev_val_min)/count; 

    if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_PASSES) 
    {
      printf("mean_val_min = %lg,old_mean_val_min = %lg\n",
	     mean_val_min,old_mean_val_min);
      printf("sdev_val_min = %lg,old_sdev_val_min = %lg\n",
	     sdev_val_min,old_sdev_val_min);
    }
    /*
    if(mean_val_min >= old_mean_val_min)
    */
    if(significantly_less_than(old_mean_val_min,old_sdev_val_min,
			       mean_val_min,sdev_val_min,
			       1.0))
    {
      num_evals *= 2;

      if(wn_conj_direction_noise_debug >= WN_CONJ_DIR_DBG_PASSES) 
      {
	printf("incrementing num_evals to %d\n",num_evals);
      }
    }

    /* compute new acceleration search direction */
    if(num_search_directions < max_num_search_directions)
    {
      wn_make_vect(&new_search_direction,num_vars);
      for(i=num_search_directions;i>0;--i)
      {
        search_directions[i] = search_directions[i-1];
	search_dir_search_radiuses[i] = search_dir_search_radiuses[i-1];
      }
      search_directions[0] = new_search_direction;
      search_dir_search_radiuses[0] = 1.0;

      ++num_search_directions;
    }
    else
    {
      new_search_direction = search_directions[max_num_search_directions-1];
      for(i=max_num_search_directions-1;i>0;--i)
      {
        search_directions[i] = search_directions[i-1];
	search_dir_search_radiuses[i] = search_dir_search_radiuses[i-1];
      }
      search_directions[0] = new_search_direction;
      search_dir_search_radiuses[0] = 1.0;
    }

    for(j=0;j<num_vars;++j)
    {
      new_search_direction[j] = vect[j] - old_vect[j];
    }

    initial_offset = 0;
  }

finish: ;

  force_optimize_stop_flag = FALSE;
  last_line_function_x_valid = FALSE;

  wn_gpfree();
}



