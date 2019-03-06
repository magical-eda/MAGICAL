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

void wn_minimize_1d(pval_min,px_min,x_start,pfunction)

********************************************************************/
#include <stdio.h>
#include <math.h>

#include "wnlib.h"
#include "wnsqr.h"
#include "wnabs.h"
#include "wnasrt.h"
#include "wnswap.h"
#include "wnmax.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wntol.h"

#include "wnconj.h"

#define DEBUG                 FALSE

#define GOLDEN_RATIO          1.618034
#define GOLDEN_SECTION        0.3819660
#define MAX_ITERATIONS        500
#define MAX_MAGNIFICATION     (20.0)
#define EXPANSION_RATIO       MAX_MAGNIFICATION

#ifdef INFINITY
# undef INFINITY
#endif
#define INFINITY              WN_FHUGE

#define BUF_RADIUS   5
#define BUF_SIZE (2*BUF_RADIUS-1)
#define BUF_MID_INDEX (BUF_RADIUS-1)


/* function data known so far */
local double xs_buf[BUF_SIZE];
local double fs_buf[BUF_SIZE];
local double low_x_max_offshifted_f,high_x_max_offshifted_f;
local double low_x,high_x;

local double *xs = xs_buf+BUF_MID_INDEX;
local double *fs = fs_buf+BUF_MID_INDEX;
local int num_low_data,num_high_data;

local double f_goal;
local double (*pfunction)(double x);

/* it turns out C++ already defines 'terminate' - bc 040826 */
local bool lo_terminate;

local double sqrt_tolerance;


double wn_clip_f(double f)
{
  if(!(f < INFINITY))
  {
    return(INFINITY);
  }
  else
  {
    return(f);
  }
}


local bool is_valid_number(double x)
{
  return((-WN_FHUGE < x)&&(x < WN_FHUGE));
}


#if 0 /* unused, commenting - bchapman 041111 */
local void init_minmax(double *pmin,double *pmax)
{
  *pmin = INFINITY;
  *pmax = -INFINITY;
}


local void add_x_to_minmax(double *pmin,double *pmax,double x)
{
  if(x < *pmin)
  {
    *pmin = x;
  }
  if(x > *pmax)
  {
    *pmax = x;
  }
}
#endif /* 0 */


local void clear_data(void)
{
  num_low_data = num_high_data = 0;
}


#if 0 /* unused, commenting - bchapman 041111 */
local void print_data(void)
{
  int i;

  printf("max_offshifted_fs = (%lg - %lg)\n",
	 low_x_max_offshifted_f,high_x_max_offshifted_f);
  printf("low_xy = (%lg - %lg)\n",low_x,high_x);

  printf("[ ");

  for(i=-num_low_data+1;i<=num_high_data-1;++i)
  {
    if(i==0)
    {
      printf(" <(%20.20lg %20.20lg)>",xs[i],fs[i]);
    }
    else
    {
      printf(" (%20.20lg %20.20lg)",xs[i],fs[i]);
    }
  }

  printf("]\n");
}
#endif /* 0 */


local void check_data(void)
{
  int i;

  wn_assert(num_low_data >= 0);
  wn_assert(num_high_data >= 0);
  wn_assert(num_low_data <= BUF_RADIUS);
  wn_assert(num_high_data <= BUF_RADIUS);

  for(i=-num_low_data+1;i<=num_high_data-1-1;++i)
  {
    wn_assert(xs[i] < xs[i+1]);
  }
  for(i=-num_low_data+1;i<=num_high_data-1;++i)
  {
    wn_assert(fs[0] <= fs[i]);
  }
}


local void shift_data(int start,int fin,int offset)
{
  int from_i,to_i;

  if(start > fin)
  {
    return;
  }

  wn_assert(start <= fin);
  wn_assert(-BUF_RADIUS+1 <= start);
  wn_assert(fin <= BUF_RADIUS-1);

  if(offset > 0)
  {
    for(from_i=fin;from_i>=start;--from_i)
    {
      to_i = from_i+offset;

      if(to_i <= BUF_RADIUS-1)
      {
        xs[to_i] = xs[from_i];
        fs[to_i] = fs[from_i];
      }
      else
      {
	if(fs[from_i] > high_x_max_offshifted_f)
	{
	  high_x_max_offshifted_f = fs[from_i];
	  high_x = xs[from_i];
	}
      }
    }
  }
  else if(offset < 0)
  {
    for(from_i=start;from_i<=fin;++from_i)
    {
      to_i = from_i+offset;

      if(to_i >= -BUF_RADIUS+1)
      {
        xs[to_i] = xs[from_i];
        fs[to_i] = fs[from_i];
      }
      else
      {
	if(fs[from_i] > low_x_max_offshifted_f)
	{
	  low_x_max_offshifted_f = fs[from_i];
	  low_x = xs[from_i];
	}
      }
    }
  }
}


#if 0 /* unused, commenting - bchapman 041111 */
local void recenter_data(void)
{
  double f,f_min;
  int i,best_i;

  f_min = INFINITY;
  best_i = 0;

  for(i=-num_low_data+1;i<=num_high_data-1;++i)
  {
    f = fs[i];

    if(f < f_min)
    {
      f_min = f;
      best_i = i;
    }
  }

  if(best_i != 0)
  {
    shift_data(-num_low_data+1,num_high_data-1,-best_i);

    num_low_data += best_i;
    num_high_data -= best_i;

    if(num_low_data > BUF_RADIUS-1)
    {
      num_low_data = BUF_RADIUS-1;
    }
    if(num_high_data > BUF_RADIUS-1)
    {
      num_high_data = BUF_RADIUS-1;
    }

    wn_assert(num_low_data > 0);
    wn_assert(num_high_data > 0);
  }
}
#endif /* 0 */


local int get_x_index(double x)
{
  int i;

  for(i=-num_low_data+1;i<=num_high_data-1;++i)
  {
#if DEBUG
    printf(" | Component %d of present structure is %20.20lg\n", i, xs[i]);
#endif
    wn_assert(x != xs[i]);

    if(x < xs[i])
    {
      return(i);
    }
  }

  return(i);
}


local void insert_new_point(double xnew,double fnew)
{

  if((num_low_data == 0)&&(num_high_data == 0))
  {
    fs[0] = fnew;
    xs[0] = xnew;
    num_low_data = num_high_data = 1;
  }
  else
  {
    int x_index;

    x_index = get_x_index(xnew);

    if(fnew < fs[0])
    {
      int neg_shift,pos_shift;

      neg_shift = -x_index;
      pos_shift = -x_index+1;

      shift_data(-num_low_data+1,x_index-1,neg_shift);
      shift_data(x_index,num_high_data-1,pos_shift);

      fs[0] = fnew;
      xs[0] = xnew;

      num_low_data -= neg_shift;
      if(num_low_data > BUF_RADIUS)
      {
	num_low_data = BUF_RADIUS;
      }
      else if(num_low_data < 1)
      {
	num_low_data = 1;
      }

      num_high_data += pos_shift;
      if(num_high_data > BUF_RADIUS)
      {
	num_high_data = BUF_RADIUS;
      }
      else if(num_high_data < 1)
      {
	num_high_data = 1;
      }
    }
    else /* fnew >= fs[0] */
    {
      if(x_index > 0)
      {
	if(x_index <= BUF_RADIUS-1)
	{
	  shift_data(x_index,num_high_data-1,1);
          fs[x_index] = fnew;
          xs[x_index] = xnew;
	  if(num_high_data < BUF_RADIUS)
	  {
	    ++num_high_data;
          }
	}
	else
	{
	  wn_assert(x_index == BUF_RADIUS);

	  if(
	      (fnew > high_x_max_offshifted_f)
		||
	      ((fnew == high_x_max_offshifted_f)&&(xnew > high_x))
            )
	  {
	    high_x_max_offshifted_f = fnew;
	    high_x = xnew;
	  }
	}
      }
      else /* x_index <= 0 */
      {
	if(x_index > -BUF_RADIUS+1)
	{
	  shift_data(-num_low_data+1,x_index-1,-1);
          fs[x_index-1] = fnew;
          xs[x_index-1] = xnew;
	  if(num_low_data < BUF_RADIUS)
	  {
	    ++num_low_data;
          }
	}
	else
	{
	  wn_assert(x_index == -BUF_RADIUS+1);

	  if(
	      (fnew > low_x_max_offshifted_f)
		||
	      ((fnew == low_x_max_offshifted_f)&&(xnew < low_x))
            )
	  {
	    low_x_max_offshifted_f = fnew;
	    low_x = xnew;
	  }
	}
      }
    }
  }

  /*
  print_data();
  */
  check_data();
}


local bool x0_is_in_legal_range(double x0)
{
  if(num_low_data > 1)
  {
    if(!(xs[-1] < x0))
    {
      return(FALSE);
    }
  }

  if(num_high_data > 1)
  {
    if(!(x0 < xs[1]))
    {
      return(FALSE);
    }
  }

  return(TRUE);
}


local void gen_quadratic_probe(bool *psuccess,double *pxnew)
{
  int i;
  double a,x0[3],b,worst_f[3];
  /* double test_x0; ** unused - bchapman 041111 */
  int code;
  bool success[3];
  double best_worst_f;

  /* fit to center points */
  if(
      (num_low_data > 1)&&(num_high_data > 1)
	&&
      is_valid_number(fs[-1]) && is_valid_number(fs[1])
    )
  {
    wn_fit_parabola_3p(&code,&a,&(x0[0]),&b,
		       xs[-1],fs[-1],xs[0],fs[0],xs[1],fs[1]);
    if(
	(code == WN_SUCCESS)
	  &&
	(a > 0.0) && (a < INFINITY)
	  &&
	(xs[-1] < x0[0])&&(x0[0] < xs[1])
      )
    {
      success[0] = TRUE;
      worst_f[0] = wn_max(fs[-1],fs[1]);
    }
    else
    {
      success[0] = FALSE;
    }
  }
  else
  {
    success[0] = FALSE;
  }

  /* fit to left points */
  if(
      (num_low_data > 2)&&(num_high_data > 0)
	&&
      is_valid_number(fs[-2]) && is_valid_number(fs[-1])
    )
  {
    wn_fit_parabola_3p(&code,&a,&(x0[1]),&b,
		       xs[-2],fs[-2],xs[-1],fs[-1],xs[0],fs[0]);
    if(
	(code == WN_SUCCESS)
	  &&
	(a > 0.0) && (a < INFINITY)
      )
    {
      success[1] = TRUE;
      worst_f[1] = fs[-2];
    }
    else
    {
      success[1] = FALSE;
    }
  }
  else
  {
    success[1] = FALSE;
  }

  /* fit to right points */
  if(
      (num_low_data > 0)&&(num_high_data > 2)
	&&
      is_valid_number(fs[1]) && is_valid_number(fs[2])
    )
  {
    wn_fit_parabola_3p(&code,&a,&(x0[2]),&b,
		       xs[0],fs[0],xs[1],fs[1],xs[2],fs[2]);
    if(
	(code == WN_SUCCESS)
	  &&
	(a > 0.0) && (a < INFINITY)
      )
    {
      success[2] = TRUE;
      worst_f[2] = fs[2];
    }
    else
    {
      success[2] = FALSE;
    }
  }
  else
  {
    success[2] = FALSE;
  }

  if(!(success[0]||success[1]||success[2]))
  {
    *psuccess = FALSE;
    return;
  }

  /* find best fit points */
  *psuccess = FALSE;
  best_worst_f = INFINITY;

  for(i=0;i<3;++i)
  {
    if(success[i]&&x0_is_in_legal_range(x0[i])&&
       (worst_f[i] < best_worst_f))
    {
      best_worst_f = worst_f[i];
      *pxnew = x0[i];
      *psuccess = TRUE;
    }
  }

  if(!(*psuccess))
  {
    return;
  }

  if(*pxnew == xs[0])
  {
    double fract;

    fract = 1.0/MAX_MAGNIFICATION;

    if(num_low_data < 2)
    {
      do
      {
        *pxnew += -fract*(xs[1]-xs[0]);
	fract = 2.0*fract;
      } while(*pxnew == xs[0]);
    }
    else if(num_high_data < 2)
    {
      do
      {
        *pxnew += fract*(xs[0]-xs[-1]);
	fract = 2.0*fract;
      } while(*pxnew == xs[0]);
    }
    else if((xs[1]-xs[0])>(xs[0]-xs[-1]))
    {
      do
      {
        *pxnew += fract*(xs[1]-xs[-1]);
	if(*pxnew >= xs[1])
	{
#if DEBUG
	printf("Terminating minimization because parabola probe does not bring any news.\n");
#endif
	  lo_terminate = TRUE;
	  return;
	}
	fract = 2.0*fract;
      } while(*pxnew == xs[0]);
    }
    else /* ((xs[1]-xs[0])<=(xs[0]-xs[-1])) */
    {
      do
      {
        *pxnew += -fract*(xs[1]-xs[-1]);
	if(*pxnew <= xs[-1])
	{
#if DEBUG
	printf("Terminating minimization because parabola probe does not bring any news.\n");
#endif
	  lo_terminate = TRUE;
	  return;
	}
	fract = 2.0*fract;
      } while(*pxnew == xs[0]);
    }
  }

  if(!(wn_abs(*pxnew) < INFINITY))
  {
    *psuccess = FALSE;
  }
}


local bool pos_greater_found(void)
{
  int i;

  if(high_x_max_offshifted_f > fs[0])
  {
    return(TRUE);
  }

  for(i=1;i<=num_high_data-1;++i)
  {
    if(fs[i] > fs[0])
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


local bool neg_greater_found(void)
{
  int i;

  if(low_x_max_offshifted_f > fs[0])
  {
    return(TRUE);
  }

  for(i=-1;i>=-num_low_data+1;--i)
  {
    if(fs[i] > fs[0])
    {
      return(TRUE);
    }
  }

  return(FALSE);
}


local double get_x_min(void)
{
  double x_min;

  if(low_x_max_offshifted_f > -INFINITY)
  {
    x_min = wn_min(low_x,xs[-num_low_data+1]);
  }
  else
  {
    x_min = xs[-num_low_data+1];
  }

  return(x_min);
}


local double get_x_max(void)
{
  double x_max;

  if(high_x_max_offshifted_f > -INFINITY)
  {
    x_max = wn_max(high_x,xs[num_high_data-1]);
  }
  else
  {
    x_max = xs[num_high_data-1];
  }

  return(x_max);
}


local double search_width(void)
{
  double x_min,x_max;

  x_min = get_x_min();
  x_max = get_x_max();

  wn_assert(x_max >= x_min);

  return(x_max-x_min);
}


local void gen_golden_probe(double *pxnew)
{
  double width;

  if(pos_greater_found())
  {
    if(neg_greater_found())
    {
      double dpos,dneg;

      dpos = xs[1]-xs[0];
      dneg = xs[0]-xs[-1];

      if(dpos > dneg)
      {
	*pxnew = xs[0] + GOLDEN_SECTION*dpos;
#if DEBUG
	printf("Golden probe at %lg, in between xs[1] and xs[0], interval = %lg\n", *pxnew, dpos);
#endif
      }
      else
      {
	*pxnew = xs[0] - GOLDEN_SECTION*dneg;
#if DEBUG
	printf("Golden probe at %lg, in between xs[0] and xs[-1], interval = %lg\n", *pxnew, dneg);
#endif
      }

      if(*pxnew == xs[0])
      {
	lo_terminate = TRUE;
#if DEBUG
	printf("Terminating minimization because golden probe does not bring any news.\n");
#endif
      }
    }
    else
    {
      width = search_width();
      width *= EXPANSION_RATIO;
      *pxnew = get_x_min() - width;
    }
  }
  else
  {
    if(neg_greater_found())
    {
      width = search_width();
      width *= EXPANSION_RATIO;
      *pxnew = get_x_max() + width;
    }
    else
    {  /* totally flat plane, expand equally in both directions */
      width = search_width();
      width *= EXPANSION_RATIO;
      if((get_x_max()-xs[0])>(xs[0]-get_x_min()))
      {
	*pxnew = get_x_min() - width;
      }
      else
      {
	*pxnew = get_x_max() + width;
      }
    }
  }
}


local double fract_diff(double n1,double n2)
{
  n1 = wn_abs(n1);
  n2 = wn_abs(n2);

  if(n1 > n2)
  {
    return(1.0-n2/n1);
  }
  else if(n2 > n1)
  {
    return(1.0-n1/n2);
  }
  else if(n1 == n2)
  {
    return(0.0);
  }
  else
  {
    return(1.0);
  }
  /*
  if(n1 > n2)
  {
    return(n1/n2-1.0);
  }
  else
  {
    return(n2/n1-1.0);
  }
  */
}


local bool optimum_found(void)
{
  if((num_low_data < 2)||(num_high_data < 2))
  {
#if DEBUG
    printf("Optimum not found - not enough data.\n");
#endif
    return(FALSE);
  }

  if((!(fs[0]<=fs[-1]))||(!(fs[0]<=fs[1])))
  {
#if DEBUG
    printf("Optimum not found - badly ordered data\n");
#endif
    return(FALSE);
  }

  if(!(neg_greater_found()&&pos_greater_found()))
  {
#if DEBUG
    printf("Optimum not found - have not yet explored both sides.\n");
#endif
    return(FALSE);
  }

  if(xs[0] != 0.0)
  {
    if(fract_diff(xs[1],xs[0]) > sqrt_tolerance)
    {
#if DEBUG
    printf("Optimum not found - fs[1] not close enough.\n");
#endif
      return(FALSE);
    }
    if(fract_diff(xs[-1],xs[0]) > sqrt_tolerance)
    {
#if DEBUG
    printf("Optimum not found - fs[-1] not close enough.\n");
#endif
      return(FALSE);
    }
  }
  else
  {
    if(wn_abs(xs[1]-xs[0]) > sqrt_tolerance)
    {
#if DEBUG
    printf("Optimum not found - fs[1] not close enough.\n");
#endif
      return(FALSE);
    }
    if(wn_abs(xs[-1]-xs[0]) > sqrt_tolerance)
    {
#if DEBUG
    printf("Optimum not found - fs[-1] not close enough.\n");
#endif
      return(FALSE);
    }
  }

#if DEBUG
    printf("Optimum found!\n");
#endif
  return(TRUE);
}


local void check_termination_conditions(double xnew,double fnew)
{
  if(wn_abs(xnew) >= INFINITY)
  {
#if DEBUG
    printf("Terminating minimization because absolute value of xnew is too large.\n");
#endif
    lo_terminate = TRUE;
    return;
  }
  if(fnew <= -INFINITY)
  {
#if DEBUG
    printf("Terminating minimization because fnew is too small.\n");
#endif
    lo_terminate = TRUE;
    return;
  }
  if(optimum_found())
  {
#if DEBUG
    printf("Terminating minimization because optimum has been found.\n");
#endif
    lo_terminate = TRUE;
    return;
  }
}


local void extract_solution
(
  int *pcode,
  double *pf0,
  double *pf1,
  double *pf2,
  double *px0,
  double *px1,
  double *px2,
  double xnew,
  double fnew
)
{
  *pf1 = fs[0];
  *px1 = xs[0];

  if(num_low_data > 1)
  {
    *pf0 = fs[-1];
    *px0 = xs[-1];
  }
  else
  {
    *pf0 = fs[0];
    *px0 = xs[0];
  }
  if(num_high_data > 1)
  {
    *pf2 = fs[1];
    *px2 = xs[1];
  }
  else
  {
    *pf2 = fs[0];
    *px2 = xs[0];
  }

  if(wn_abs(xnew) >= INFINITY)
  {
    if(fnew == fs[0])
    {
      *pcode = WN_SUCCESS;
    }
    else
    {
      *pcode = WN_UNBOUNDED;
    }
    return;
  }
  if(fnew <= -INFINITY)
  {
    *pcode = WN_UNBOUNDED;
    return;
  }

  if(optimum_found())
  {
    *pcode = WN_SUCCESS; 
  }
  else
  {
    *pcode = WN_SUBOPTIMAL; 
  }
}


/* assume *pf0,*pf1,*pf2 already computed */
EXTERN void wn_minimize_1d_raw
(
  int *pcode,
  double *pf0,
  double *pf1,
  double *pf2,
  double *px0,
  double *px1,
  double *px2,
  double passed_f_goal,
  double (*passed_pfunction)(double x),
  int max_improve_iterations,
  int max_total_iterations
)
{
  int improve_iteration_count,total_iteration_count;
  bool success;
  double xnew,fnew = 0;
  int parabola_fit_count;

  sqrt_tolerance = sqrt(wn_machine_tolerance());
  clear_data();

  f_goal = wn_clip_f(passed_f_goal);
  pfunction = passed_pfunction;

#if DEBUG
  printf("Inserting x1 = %lg, f1 = %lg into structure.\n", *px1, wn_clip_f(*pf1));
#endif
  insert_new_point(*px1,wn_clip_f(*pf1));
#if DEBUG
  printf("Inserting x0 = %lg, f0 = %lg into structure.\n", *px0, wn_clip_f(*pf0));
#endif
  insert_new_point(*px0,wn_clip_f(*pf0));
#if DEBUG
  printf("Inserting x2 = %lg, f2 = %lg into structure.\n", *px2, wn_clip_f(*pf2));
#endif
  insert_new_point(*px2,wn_clip_f(*pf2));

  low_x_max_offshifted_f = high_x_max_offshifted_f = -INFINITY;
  lo_terminate = FALSE;

  total_iteration_count = 0;
  improve_iteration_count = 0;

  if((fs[-1] <= f_goal)||(fs[1] <= f_goal))
  {
    wn_assert(fs[0] <= f_goal);

    ++improve_iteration_count;
  }

  parabola_fit_count = 0;

  while(
	 !lo_terminate  
	   && 
	 !(
	    (max_total_iterations < WN_IHUGE)
	      &&
	    (total_iteration_count >= max_total_iterations)
	  )
       )
  {
    /*
    print_data();
    */

    if(parabola_fit_count < 2)
    {
      gen_quadratic_probe(&success,&xnew);
    }
    else
    {
      success = FALSE;
    }

    if(lo_terminate) 
    {
      break;
    }

    if(success)
    {
      ++parabola_fit_count;
    }
    else
    {
      /*
      printf("golden probe.\n");
      */
      gen_golden_probe(&xnew);
      parabola_fit_count = 0;
    }

    fnew = wn_clip_f((*pfunction)(xnew));
    ++total_iteration_count;

    check_termination_conditions(xnew,fnew);
    if(lo_terminate) 
    {
      break;
    }

#if DEBUG
  printf("Inserting xnew = %lg, fnew = %lg into structure.\n", xnew, fnew);
#endif
    insert_new_point(xnew,fnew);

    if((fnew <= f_goal)&&(fs[0] <= f_goal))
    {
      ++improve_iteration_count;
      if(improve_iteration_count >= max_improve_iterations)
      {
#if DEBUG
  printf("Terminating minimization because improving target has been achieved.\n");
#endif
	break;
      }
    }
    else
    {
      improve_iteration_count = 0;
    }

    check_termination_conditions(xnew,fnew);
  }

  /*
  print_data();
  */
#if DEBUG
  printf("One-d minimization finished with (%lg, %lg)\n", xnew, fnew);
  printf("Final data #0: (%lg, %lg)\n", *px0, *pf0);
  printf("Final data #1: (%lg, %lg)\n", *px1, *pf1);
  printf("Final data #2: (%lg, %lg)\n", *px2, *pf2);
#endif
  extract_solution(pcode,pf0,pf1,pf2,px0,px1,px2,xnew,fnew);
#if DEBUG
  printf("Returned code = ");
  if(*pcode == WN_SUCCESS)
  {
    printf("SUCCESS\n");
  }
  else if(*pcode == WN_UNBOUNDED)
  {
    printf("UNBOUNDED\n");
  }
  else if(*pcode == WN_SUBOPTIMAL)
  {
    printf("SUBOPTIMAL\n");
  }
  else
  {
    wn_assert_notreached();
  }
#endif
}


EXTERN void wn_minimize_1d
(
  int *pcode,
  double *pval_min,
  double *px_min,
  double (*pfunction)(double x),
  int max_improve_iterations
)
{
  double x0,x1,x2,f0,f1,f2;

  x1 = *px_min;
  x0 = x1-1.0;
  x2 = x1+1.0;

  f0 = wn_clip_f((*pfunction)(x0));
  f1 = wn_clip_f((*pfunction)(x1));
  f2 = wn_clip_f((*pfunction)(x2));

  wn_minimize_1d_raw(pcode,&f0,&f1,&f2,&x0,&x1,&x2,f1,
		     pfunction,
		     max_improve_iterations,WN_IHUGE);

  *pval_min = f1;
  *px_min = x1;
}
