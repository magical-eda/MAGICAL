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
#include "wnsqr.h"
#include "wnswap.h"
#include "wnrndd.h"

#include "wnvect.h"
#include "wnpoly.h"
#include "wnmat.h"

#include "wnconj.h"
#include "wncnjdn2.h"



#define MAX_EXPAND   (20.0)
#define MIN_CONTRACT (1.0/10.0)

#define GOLDEN_RATIO          1.618034
#define GOLDEN_SECTION        0.3819660


#define WN_CDN_DBG_NONE        0
#define WN_CDN_DBG_PASSES      1
#define WN_CDN_DBG_LINESEARCH  2
#define WN_CDN_DBG_ALL         3

local int wn_cdn_debug = WN_CDN_DBG_NONE;



void wn_cdn_compute_func_call_vect
(
  wn_cdn_context_type c,
  double *search_direction,
  double x
)
{
  wn_copy_vect(c->func_call_vect,c->current_vect,c->num_vars);
  wn_add_scaled_vect(c->func_call_vect,search_direction,x,c->num_vars);
}


local void default_line_eval
(
  wn_cdn_context_type c,
  double f[],double x[],int n,
  double *search_direction
)
{
  int i;

  for(i=0;i<n;++i)
  {
    wn_cdn_compute_func_call_vect(c,search_direction,x[i]);

    f[i] = 
	(*(c->pfunction))(c->func_call_vect,c->num_func_calls + i);
  }
}


local void reset_search_direction
(
  wn_cdn_context_type c,
  wn_cdn_srchdir_type search_direction
)
{
  if(search_direction->dir_vect != NULL)
  {
    wn_zero_vect(search_direction->dir_vect,c->num_vars);
  }

  search_direction->x_min = -WN_FHUGE;
  search_direction->x_max = WN_FHUGE;

  search_direction->x0 = 0.0;  
  search_direction->curvature = 0.0; 
  search_direction->x_width = 0.0;  
  search_direction->max_x_width = WN_FHUGE;
}


local void make_search_direction
(
  wn_cdn_context_type c,
  wn_cdn_srchdir_type *psearch_direction,
  bool is_coord_direction,
  int coord
)
{
  *psearch_direction = (wn_cdn_srchdir_type)wn_zalloc(
                           sizeof(struct wn_cdn_srchdir_type_struct));

  (*psearch_direction)->is_coord_direction = is_coord_direction;
  if(is_coord_direction)
  {
    (*psearch_direction)->coord = coord;
    (*psearch_direction)->dir_vect = NULL;
  }
  else
  {
    (*psearch_direction)->coord = -1;
    wn_make_vect(&((*psearch_direction)->dir_vect),c->num_vars);
  }

  reset_search_direction(c,*psearch_direction);
}


void wn_cdn_make_context
(
  wn_cdn_context_type *pc,
  double (*pfunction)(double vect[],int sequence_number),
  int num_vars
)
{
  int i;

  *pc = (wn_cdn_context_type)wn_zalloc(
               sizeof(struct wn_cdn_context_type_struct));

  (*pc)->num_vars = num_vars;
  (*pc)->pfunction = pfunction;
  (*pc)->pline_eval = &default_line_eval;

  (*pc)->num_search_directions = 0;
  (*pc)->max_num_search_directions = (*pc)->num_vars;
  (*pc)->coord_search_direction_array = 
      (wn_cdn_srchdir_type *)wn_zalloc(
           num_vars*sizeof(wn_cdn_srchdir_type));
  for(i=0;i<num_vars;++i)
  {
    make_search_direction((*pc),&(((*pc)->coord_search_direction_array)[i]),
                          TRUE,i);
  }
  (*pc)->search_direction_array = 
      (wn_cdn_srchdir_type *)wn_zalloc(
           ((*pc)->max_num_search_directions)*sizeof(wn_cdn_srchdir_type));
  for(i=0;i<((*pc)->max_num_search_directions);++i)
  {
    ((*pc)->search_direction_array)[i] = NULL;
  }

  wn_make_vect(&((*pc)->current_vect),num_vars);
  wn_make_vect(&((*pc)->func_call_vect),num_vars);
  wn_make_vect(&((*pc)->old_vect),num_vars);
  wn_make_vect(&((*pc)->coord_direction),num_vars);

  (*pc)->min_num_line_samples = 10;
  (*pc)->num_line_samples = (*pc)->min_num_line_samples;
  wn_make_vect(&((*pc)->xa),(*pc)->num_line_samples);
  wn_make_vect(&((*pc)->fa),(*pc)->num_line_samples);

  (*pc)->ob = WN_FHUGE;
  (*pc)->code = WN_CDN_NOT_STARTED;

  (*pc)->num_func_calls = 0;
  (*pc)->max_num_func_calls = 0;

  (*pc)->force_optimize_stop_flag = FALSE;

  (*pc)->current_group = wn_curgp();
}


local void free_search_direction(wn_cdn_srchdir_type search_direction)
{
  if(!(search_direction->is_coord_direction))
  {
    wn_free(search_direction->dir_vect);
  }

  wn_free(search_direction);
}


void wn_cdn_free_context(wn_cdn_context_type c)
{
  int i;

  wn_gppush(c->current_group);

  for(i=0;i<c->num_vars;++i)
  {
    free_search_direction((c->coord_search_direction_array)[i]);
  }
  for(i=0;i<c->num_search_directions;++i)
  {
    free_search_direction((c->search_direction_array)[i]);
  }

  wn_free(c->current_vect);
  wn_free(c->func_call_vect);
  wn_free(c->old_vect);
  wn_free(c->coord_direction);

  wn_gppop();
}


void wn_cdn_set_solution
(
  wn_cdn_context_type c,
  double vect[]
)
{
  wn_copy_vect(c->current_vect,vect,c->num_vars);
}


void wn_cdn_get_solution
(
  wn_cdn_context_type c,
  int *pcode,
  double *pob,
  double vect[]
)
{
  *pcode = c->code;
  *pob = c->ob;
  wn_copy_vect(vect,c->current_vect,c->num_vars);
}


void wn_cdn_set_line_eval
(
  wn_cdn_context_type c,
  void (*pline_eval)(wn_cdn_context_type c,
                     double f[],double x[],int n,
                     double *search_direction)
)
{
  c->pline_eval = pline_eval;
}


void wn_cdn_set_coord_x_min
(
  wn_cdn_context_type c,
  int coord,
  double x_min
)
{
  wn_assert(coord >= 0);
  wn_assert(coord < c->num_vars);

  (c->coord_search_direction_array)[coord]->x_min = x_min;
}


void wn_cdn_set_coord_x_max
(
  wn_cdn_context_type c,
  int coord,
  double x_max
)
{
  wn_assert(coord >= 0);
  wn_assert(coord < c->num_vars);

  (c->coord_search_direction_array)[coord]->x_max = x_max;
}


void wn_cdn_set_coord_x0
(
  wn_cdn_context_type c,
  int coord,
  double x0
)
{
  wn_assert(coord >= 0);
  wn_assert(coord < c->num_vars);

  (c->coord_search_direction_array)[coord]->x0 = x0;
}


void wn_cdn_set_coord_x_width
(
  wn_cdn_context_type c,
  int coord,
  double x_width
)
{
  wn_assert(coord >= 0);
  wn_assert(coord < c->num_vars);

  (c->coord_search_direction_array)[coord]->x_width = x_width;
}


void wn_cdn_set_coord_max_x_width
(
  wn_cdn_context_type c,
  int coord,
  double max_x_width
)
{
  wn_assert(coord >= 0);
  wn_assert(coord < c->num_vars);

  (c->coord_search_direction_array)[coord]->max_x_width = max_x_width;
}


void wn_cdn_force_stop(wn_cdn_context_type c)
{
  c->force_optimize_stop_flag = TRUE;
}


local void fit_parabola_2pa(int *pcode,double *px0,double *pb,
			    double a,
			    double x1,double y1,
			    double x2,double y2)
{
  double b,c;

  if(a <= 0.0)
  {
    *pcode = WN_SINGULAR;
    return;
  }

  wn_fit_traditional_parabola_2pa(pcode,&b,&c,a,x1,y1,x2,y2);

  if(*pcode != WN_SUCCESS)
  {
    return;
  }

  wn_convert_parabola_traditional_to_centered(pcode,px0,pb,a,b,c);
}


local void line_eval
(
  wn_cdn_context_type c,
  double f[],double x[],int n,
  double *search_direction
)
{
  int i;

  (*(c->pline_eval))(c,f,x,n,search_direction);

  for(i=0;i<n;++i)
  {
    f[i] = wn_clip_f(f[i]);

    if(wn_cdn_debug >= WN_CDN_DBG_ALL)
    {
      wn_cdn_compute_func_call_vect(c,search_direction,x[i]);

      printf("function call %d at value\n",c->num_func_calls + i);
      wn_print_vect(c->func_call_vect,c->num_vars);
      printf("function at x = %lg value is %lg\n",x[i],f[i]);
      fflush(stdout);
    }
  }

  c->num_func_calls += n;
}


local void compute_search_direction_vect
(
  wn_cdn_context_type c,
  double **psearch_direction_vect,
  wn_cdn_srchdir_type search_direction
)
{
  if(search_direction->is_coord_direction)
  {
    (c->coord_direction)[search_direction->coord] = 1.0;
    *psearch_direction_vect = c->coord_direction;
  }
  else
  {
    *psearch_direction_vect = search_direction->dir_vect;
  }
}


local void retire_search_direction_vect
(
  wn_cdn_context_type c,
  double **psearch_direction_vect,
  wn_cdn_srchdir_type search_direction
)
{
  if(search_direction->is_coord_direction)
  {
    (c->coord_direction)[search_direction->coord] = 0.0;
  }

  *psearch_direction_vect = NULL;
}


local void generate_x_sample_values
(
  double xa[],
  double x_width,
  int num_line_samples
)
{
  int i;

  for(i=0;i<num_line_samples;++i)
  {
    xa[i] = -x_width + 2.0*x_width*((double)i)/(num_line_samples-1);
  }
}


local void add_scalar_to_vect(double vect[],double x,int n)
{
  int i;

  for(i=0;i<n;++i)
  {
    vect[i] += x;
  }
}


local void ms_accum(int *pn,double *ps1,double *ps2,double x)
{
  ++(*pn);
  (*ps1) += x;
  (*ps2) += x*x;
}


local void ms_finish(double *pmean,double *pvar,int n,double s1,double s2)
{
  wn_assert(n > 1);

  s1 /= n;
  s2 /= n;

  *pmean = s1;
  *pvar = s2 - s1*s1;
}


local void compute_noise2
(
  double *pnoise2,
  double **m,
  double poly[],
  double f_vect[],
  int n,
  int degree
)
{
  int i;
  double sum,diff;

  sum = 0.0;

  for(i=0;i<n;++i)
  {
    diff = wn_dot_vects(m[i],poly,degree) - f_vect[i];
    sum += diff*diff;
  }

  *pnoise2 = sum/(n-degree);
}


local void ls_invert_mat(int *pcode,double **mlsinv,
			 double **m,int len_i,int len_j)
{
  double **mt,**mtm,**mtmi;
  int i,j;

  wn_gpmake("no_free");

  wn_assert(len_i >= len_j);

  wn_make_mat(&mt,len_j,len_i);
  wn_make_mat(&mtm,len_j,len_j);
  wn_make_mat(&mtmi,len_j,len_j);

  /*
  printf("m =\n");
  wn_print_mat(m,len_i,len_j);
  */
  wn_transpose_mat(mt,m,len_i,len_j);
  /*
  printf("mt =\n");
  wn_print_mat(mt,len_j,len_i);
  */
  wn_mult_mats(mtm,mt,m,len_j,len_i,len_j);
  /*
  printf("mtm =\n");
  wn_print_mat(mtm,len_j,len_j);
  */
  wn_copy_mat(mtmi,mtm,len_j,len_j);
  wn_invert_mat(pcode,mtmi,len_j);

  if(*pcode == WN_SUCCESS)
  {
    /*
    printf("mtmi =\n");
    wn_print_mat(mtmi,len_j,len_j);
    */
    wn_mult_mats(mlsinv,mtmi,mt,len_j,len_j,len_i);
    /*
    printf("mlsinv =\n");
    wn_print_mat(mlsinv,len_j,len_i);
    */
  }

  wn_gpfree();
}


local void fit_experiments_to_poly2
(
  double poly[3],
  double poly_total_noise[3],
  double poly_random_noise[3],
  double poly_systematic_noise[3],
  double *precommend_widen,
  double x_vect[],
  double f_vect[],
  int n
)
{
  int i,j,code;
  double **m,**mlsinv,**m3lsinv;
  double poly4[4],poly4_random_noise[4];
  double total4_noise2;
  double poly3_2;
  double *x3_vect;

  wn_assert(n > 4);

  wn_gpmake("no_free");

  wn_make_mat(&m,n,4);

  for(i=0;i<n;++i)
  for(j=0;j<4;++j)
  {
    m[i][j] = pow(x_vect[i],(double)j);
  }

  wn_make_vect(&x3_vect,n);

  for(i=0;i<n;++i)
  {
    x3_vect[i] = m[i][3];
  }

  wn_make_mat(&m3lsinv,3,n);
  ls_invert_mat(&code,m3lsinv,m,n,3);
  wn_assert(code == WN_SUCCESS);

  wn_make_mat(&mlsinv,4,n);
  ls_invert_mat(&code,mlsinv,m,n,4);
  wn_assert(code == WN_SUCCESS);

  wn_mult_mat_by_vect(poly,m3lsinv,f_vect,
		      3,n);
  /*
  printf("poly = \n");
  wn_print_vect(poly,3);
  */

  wn_mult_mat_by_vect(poly4,mlsinv,f_vect,
		      4,n);
  compute_noise2(&total4_noise2,m,poly4,f_vect,n,4);
  /*
  printf("total4_noise2 = %lg\n",total4_noise2);
  printf("poly4 = \n");
  wn_print_vect(poly4,4);
  */

  for(i=0;i<4;++i)
  {
    poly4_random_noise[i] = sqrt(total4_noise2*wn_norm2_vect(mlsinv[i],n));
  }
  for(i=0;i<3;++i)
  {
    /* use total4_noise2 because it is the most accurate
       estimate of function noise that we have */
    poly_random_noise[i] = sqrt(total4_noise2*wn_norm2_vect(m3lsinv[i],n));
  }

  poly3_2 = wn_square(poly4[3]) - wn_square(poly4_random_noise[3]);
  /*
  printf("poly3_2(%lg) = wn_square(poly4[3])(%lg) - wn_square(poly4_random_noise[3])(%lg)\n",
         poly3_2,wn_square(poly4[3]),wn_square(poly4_random_noise[3]));
  */
  if(poly3_2 < 0.0)
  {
    poly3_2 = 0.0;
  }

  wn_mult_mat_by_vect(poly_systematic_noise,m3lsinv,x3_vect,
		      3,n);
  for(i=0;i<3;++i)
  {
    poly_systematic_noise[i] = sqrt(poly3_2)*wn_abs(poly_systematic_noise[i]);
  }

  for(i=0;i<3;++i)
  {
    poly_total_noise[i] = 
                sqrt(wn_square(poly_random_noise[i])+
	             wn_square(poly_systematic_noise[i]));
  }

  if(poly_systematic_noise[1] == 0.0)
  {
    *precommend_widen = WN_FHUGE;
  }
  else
  {
    *precommend_widen = pow(0.5*wn_square(poly_random_noise[1])/
			    wn_square(poly_systematic_noise[1]),1.0/6.0);
  }

  wn_gpfree();
}


void clamp_number(double *px,double lo_clamp,double hi_clamp)
{
  if(*px > hi_clamp)
  {
    *px = hi_clamp;
  }
  else if(*px < lo_clamp)
  {
    *px = lo_clamp;
  }
  /*
  printf("clamped x = %lg\n",*px);
  */
}


/*
  Find 2nd degree approximating Taylor polynomial about x, using
  c->num_line_samples function samples.
  Return Taylor coefs in poly[..] and 
  total noise estimate in poly_total_noise[..]
*/
local void find_taylor_poly
(
  wn_cdn_context_type c,
  double poly[3],
  double poly_total_noise[3],
  wn_cdn_srchdir_type search_direction,
  double x
)
{
  double *search_direction_vect;
  double poly_random_noise[3],poly_systematic_noise[3];
  double recommend_widen;

  printf("find_taylor_poly: x = %lg, x_width = %lg\n",
	 x,search_direction->x_width);
  printf("current_vect = \n");
  wn_print_vect(c->current_vect,c->num_vars);

  compute_search_direction_vect(c,&search_direction_vect,search_direction);
  printf("search_direction_vect = \n");
  wn_print_vect(search_direction_vect,c->num_vars);

  generate_x_sample_values(c->xa,search_direction->x_width,c->num_line_samples);
  add_scalar_to_vect(c->xa,x,c->num_line_samples);

  line_eval(c,c->fa,c->xa,c->num_line_samples,search_direction_vect);

  add_scalar_to_vect(c->xa,-x,c->num_line_samples);

  fit_experiments_to_poly2(poly,
                           poly_total_noise,
                           poly_random_noise,poly_systematic_noise,
			   &recommend_widen,
                           c->xa,c->fa,c->num_line_samples);

  printf("recommend_widen = %lg\n",recommend_widen);
  clamp_number(&recommend_widen,0.5,2.0);
  printf("clamp recommend_widen = %lg\n",recommend_widen);

  search_direction->x_width *= recommend_widen;
  if(search_direction->x_width > search_direction->max_x_width)
  {
    search_direction->x_width = search_direction->max_x_width;
  }

  retire_search_direction_vect(c,&search_direction_vect,search_direction);
}


local void line_minimize
(
  wn_cdn_context_type c,
  wn_cdn_srchdir_type search_direction,
  double *px_width_sum
)
{
  double *search_direction_vect;
  double poly[3],poly_total_noise[3];
  double xopt,fopt,dfopt,fopt_noise,dfopt_noise;
  double x_width;
  double x0,x1,x2,f0,f1,f2,df0,df1,df2,
         f0_noise,f1_noise,f2_noise,df0_noise,df1_noise,df2_noise;
  int code;
  double a;
  double diff_noise;

  printf("line minimize: coord = %d\n",
         search_direction->coord);
  printf("  x_width = %lg\n",search_direction->x_width);
  x_width = search_direction->x_width;

  find_taylor_poly(c,poly,poly_total_noise,search_direction,0.0);

  if(poly_total_noise[1] == 0.0)
  {
    search_direction->ratio_df1_noise = WN_FHUGE;
  }
  else
  {
    search_direction->ratio_df1_noise = wn_abs(poly[1])/poly_total_noise[1];
  }

  printf("  x_width = %lg\n",search_direction->x_width);

  printf("poly=\n");
  wn_print_vect(poly,3);
  printf("poly_total_noise=\n");
  wn_print_vect(poly_total_noise,3);

/* try to find min using slope and curvature - this should work 
   most of the time */
  if(poly[2] >= 2.0*poly_total_noise[2])
  {
    xopt = -0.5*poly[1]/poly[2];

    printf("  xopt = %lg\n",xopt);

    if((-x_width < xopt)&&(xopt < x_width))
    {
      printf("slope-curvature method successful!!!!!!\n");
      fopt = wn_eval_poly(xopt,poly,3);

      goto finish;
    }
    else
    {
      printf("slope-curvature method failed because xopt too far out\n");
      x0 = xopt;
    }
  }
  else
  {
    printf("slope-curvature method failed because non-positive curvature\n");
    x0 = search_direction->x0;
    if(x0 == 0.0)
    {
      x0 = search_direction->x_width;
    }

    if(poly[1] < 0.0)
    {
      x0 = wn_abs(x0);
    }
    else if(poly[1] > 0.0)
    {
      x0 = -wn_abs(x0);
    }
  }
  
  if((-search_direction->x_width < x0)&&(x0 <  search_direction->x_width))
  {
    if(x0 < 0.0)
    {
      x0 = -search_direction->x_width;
    }
    else
    {
      wn_assert(x0 >= 0.0);

      x0 = search_direction->x_width;
    }
  }

/* try secant method using slopes */
secant: ;
  printf("trying secant method....\n");
  x1 = 0.0;
  f1 = poly[0];
  df1 = poly[1];
  f1_noise = poly_total_noise[0];
  df1_noise = poly_total_noise[1];

  find_taylor_poly(c,poly,poly_total_noise,search_direction,x0);
  f0 = poly[0];
  df0 = poly[1];
  f0_noise = poly_total_noise[0];
  df0_noise = poly_total_noise[1];

  printf("x0 = %lg\n",x0);
  printf("poly=\n");
  wn_print_vect(poly,3);
  printf("poly_total_noise=\n");
  wn_print_vect(poly_total_noise,3);

  wn_fit_parabola_2d(&code,&a,&xopt,x0,df0,x1,df1);

  printf("code = %d,a = %lg, xopt = %lg\n",code,a,xopt);

  if((code != WN_SUCCESS)||(!(a > 0.0))||
     (!(wn_abs(xopt)<MAX_EXPAND*wn_abs(x0))))
  {
    printf("secant method failed because parabola fit failed.\n");
    goto full_linesearch;
  }   

  find_taylor_poly(c,poly,poly_total_noise,search_direction,xopt);
  fopt = poly[0];
  dfopt = poly[1];
  fopt_noise = poly_total_noise[0];
  dfopt_noise = poly_total_noise[1];

  printf("xopt = %lg\n",xopt);
  printf("poly=\n");
  wn_print_vect(poly,3);
  printf("poly_total_noise=\n");
  wn_print_vect(poly_total_noise,3);

  diff_noise = sqrt(wn_square(f1_noise)+wn_square(fopt_noise));
  if((fopt-f1) > 2.0*diff_noise)
  {
    printf("secant method failed because f increased too much\n");
    goto full_linesearch;
  }

  if(!(wn_abs(dfopt) <= wn_abs(df1)))
  {
    printf("secant method failed because df increased\n");
    goto full_linesearch;
  }

  printf("secant method successful!!!!!!\n");
  goto finish;

/* full linesearch:  search out, or in by 2x steps*/
full_linesearch: ;
  if(wn_abs(df1) <= 2.0*df1_noise)
  {
    xopt = x1;
    fopt = f1;
    dfopt = df1;
    printf("full linesearch failed because slope indistinguishable from 0\n");
    goto finish;
  }
  
  if(df1 < 0.0)
  {
    wn_assert(x0 > 0.0);
  }
  else if(df1 > 0.0)
  {
    wn_assert(x0 < 0.0);
  }
  else
  {
    wn_assert_notreached();
  }

  printf("df1 = %lg,df0 = %lg\n",df1,df0);
  if(wn_sign(df1) == wn_sign(df0))
  {
    printf("searching outward....\n");

    /* search outward until slopes differ */
    while(wn_sign(df1) == wn_sign(df0))
    { 
      xopt = x0;
      fopt = f0;
      dfopt = df0;

      x0 *= 2.0;

      find_taylor_poly(c,poly,poly_total_noise,search_direction,x0);
      f0 = poly[0];
      df0 = poly[1];

      printf("xopt=%lg,fopt=%lg,dfopt=%lg,x0=%lg,f0=%lg,df0=%lg,x1=%lg,f1=%lg,df1=%lg\n",
             xopt,fopt,dfopt,x0,f0,df0,x1,f1,df1);
    }
  }
  else if(wn_sign(df1) == -wn_sign(df0))
  {
    xopt = x0;
    fopt = f0;
    dfopt = df0;

    printf("searching inward....\n");

    while(wn_sign(dfopt) == -wn_sign(df1))
    {
      xopt *= 0.5;

      find_taylor_poly(c,poly,poly_total_noise,search_direction,xopt);
      fopt = poly[0];
      dfopt = poly[1];

      printf("xopt=%lg,fopt=%lg,dfopt=%lg,x1=%lg,f1=%lg,df1=%lg\n",
             xopt,fopt,dfopt,x1,f1,df1);
    }
  }
  else
  {
    wn_assert_notreached();
  }

finish: ;
  compute_search_direction_vect(c,&search_direction_vect,search_direction);
  wn_cdn_compute_func_call_vect(c,search_direction_vect,xopt);
  wn_copy_vect(c->current_vect,c->func_call_vect,c->num_vars);
  retire_search_direction_vect(c,&search_direction_vect,search_direction);

  if(xopt != 0.0)
  {
    (*px_width_sum) += wn_abs(xopt)/search_direction->x_width;
  }

  c->ob = fopt;

  printf("finish:  xopt = %lg, fopt = %lg\n",xopt,fopt);
}


local void compute_average_ratio_df1_noise
(
  wn_cdn_context_type c,
  double *paverage_ratio_df1_noise
)
{
  int i;
  int count;
  double sum;

  count = 0;
  sum = 0.0;

  for(i=0;i<c->num_search_directions;++i)
  {
    sum += (c->search_direction_array)[i]->ratio_df1_noise;
    ++count;
  }
  for(i=0;i<c->num_vars;++i)
  {
    sum += (c->coord_search_direction_array)[i]->ratio_df1_noise;
    ++count;
  }

  wn_assert(count > 0);
  *paverage_ratio_df1_noise = sum/count;
}


local void compute_fract_too_small_ratio_df1_noise
(
  wn_cdn_context_type c,
  double *pfract_too_small,
  double target
)
{
  int i;
  int total_count,too_small_count;
  double sum;

  too_small_count = 0;
  total_count = 0;

  for(i=0;i<c->num_search_directions;++i)
  {
    if((c->search_direction_array)[i]->ratio_df1_noise < target)
    {
      ++too_small_count;
    }
    ++total_count;
  }
  for(i=0;i<c->num_vars;++i)
  {
    if((c->coord_search_direction_array)[i]->ratio_df1_noise < target)
    {
      ++too_small_count;
    }
    ++total_count;
  }

  wn_assert(total_count > 0);
  *pfract_too_small = ((double)too_small_count)/total_count;
}


local void recompute_num_line_samples(wn_cdn_context_type c)
{
# define TARGET_RATIO_DF1_NOISE  (3.0)
  double average_ratio_df1_noise;
  double adjustment; 
  double fract_too_small;

  printf("old num_line_samples = %d\n",c->num_line_samples);

#if 0
  compute_average_ratio_df1_noise(c,&average_ratio_df1_noise);

  printf("average_ratio_df1_noise = %lg\n",average_ratio_df1_noise);

  clamp_number(&average_ratio_df1_noise,0.0,20.0);

  adjustment = pow(average_ratio_df1_noise/TARGET_RATIO_DF1_NOISE,-3.0);

  clamp_number(&adjustment,1.0/1.3,1.3);
#endif

  compute_fract_too_small_ratio_df1_noise(c,&fract_too_small,
					  TARGET_RATIO_DF1_NOISE);
  printf("fract_too_small = %lg\n",fract_too_small);

  if(fract_too_small > 0.3)
  {
    adjustment = 1.3;
  }
  else
  {
    adjustment = 1.0/1.3;
  }

  c->num_line_samples = (int)(adjustment*c->num_line_samples);
  if(c->num_line_samples < c->min_num_line_samples)
  {
    c->num_line_samples = c->min_num_line_samples;
  }
  if(c->num_line_samples > c->num_line_samples_mem)
  {
    wn_gppush(c->current_group);
    c->num_line_samples_mem = c->num_line_samples;
    wn_free(c->xa);
    wn_free(c->fa);
    wn_make_vect(&(c->xa),c->num_line_samples_mem);
    wn_make_vect(&(c->fa),c->num_line_samples_mem);
    wn_gppop();
  }

  printf("new num_line_samples = %d\n",c->num_line_samples);
}


local bool vects_equal(double v1[],double v2[],int n)
{
  int i;

  for(i=0;i<n;++i)
  {
    if(v1[i] != v2[i])
    {
      return(FALSE);
    }
  }

  return(TRUE);
}


void wn_cdn_optimize
(
  wn_cdn_context_type c,
  int num_func_calls
)
{
  int i,j,iteration;
  double *old_vect,*coord_direction;
  wn_cdn_srchdir_type new_search_direction;
  double old_ob;
  double x_width_sum;

  wn_gppush(c->current_group);

  c->force_optimize_stop_flag = FALSE;

  if(num_func_calls >= WN_IHUGE)
  {
    c->max_num_func_calls = WN_IHUGE;
  }
  else
  {
    c->max_num_func_calls = c->num_func_calls + num_func_calls;
  }

  if(c->code == WN_CDN_NOT_STARTED)
  {
    c->code = WN_SUBOPTIMAL;
    c->ob = wn_clip_f((*(c->pfunction))(c->current_vect,c->num_func_calls));
    ++(c->num_func_calls);
  }

  for(iteration=0;;++iteration)
  {
    /*
    if(wn_cdn_debug >= WN_CDN_DBG_PASSES)
    */
    {
      printf("iteration = %d ********************************\n",iteration);
      printf("num_func_calls = %d\n",c->num_func_calls);
      printf("ob = %lg\n",c->ob);
      printf("vect = \n");
      wn_print_vect(c->current_vect,c->num_vars);
      fflush(stdout);
    }
    /*
    (void)getchar();
    */

    old_ob = c->ob;
    wn_copy_vect(c->old_vect,c->current_vect,
                 c->num_vars);
    x_width_sum = 0.0;

    /* minimize along acceleration search directions */
    if(wn_cdn_debug >= WN_CDN_DBG_LINESEARCH)
    {
      printf("start acceleration line minimizations ------------------\n");
    }
    for(i=0;i<c->num_search_directions;++i)
    {
      if(wn_cdn_debug >= WN_CDN_DBG_LINESEARCH)
      {
        printf("acceleration line search %d\n",i);
      }

      line_minimize(c,(c->search_direction_array)[i],&x_width_sum);

      if(
	  (
            (c->max_num_func_calls < WN_IHUGE)
              &&
            (c->num_func_calls > c->max_num_func_calls)
          )
	    ||
          c->force_optimize_stop_flag
	)
      {
        c->code = WN_SUBOPTIMAL;
	goto finish;
      }
    }

    /* minimize along coordinate directions */
    if(wn_cdn_debug >= WN_CDN_DBG_LINESEARCH) 
    {
      printf("start coordinate line minimizations ------------------\n");
    }
    for(i=0;i<c->num_vars;++i)
    {
      if(wn_cdn_debug >= WN_CDN_DBG_LINESEARCH)
      {
        printf("coord line search %d\n",i);
      }

      line_minimize(c,(c->coord_search_direction_array)[i],&x_width_sum);

      if(
	  (
            (c->max_num_func_calls < WN_IHUGE)
              &&
            (c->num_func_calls > c->max_num_func_calls)
          )
	    ||
          c->force_optimize_stop_flag
	)
      {
        c->code = WN_SUBOPTIMAL;
	goto finish;
      }
    }

    recompute_num_line_samples(c);

    if(!vects_equal(c->current_vect,c->old_vect,c->num_vars))
    {
      /* compute new acceleration search direction */
      if(c->num_search_directions < c->max_num_search_directions)
      {
        make_search_direction(c,&new_search_direction,FALSE,-1);
        for(i=c->num_search_directions;i>0;--i)
        {
          (c->search_direction_array)[i] = 
              (c->search_direction_array)[i-1];
        }
        (c->search_direction_array)[0] = new_search_direction;

        ++(c->num_search_directions);
      }
      else
      {
        new_search_direction = 
          (c->search_direction_array)[(c->max_num_search_directions) - 1];
        reset_search_direction(c,new_search_direction);
        for(i=c->max_num_search_directions-1;i>0;--i)
        {
          (c->search_direction_array)[i] = 
              (c->search_direction_array)[i-1];
        }
        (c->search_direction_array)[0] = new_search_direction;
      }

      for(j=0;j<c->num_vars;++j)
      {
        (new_search_direction->dir_vect)[j] = 
	    (c->current_vect)[j] - (c->old_vect)[j];
      }

      new_search_direction->x0 = 1.0;
      new_search_direction->x_width = 1.0/x_width_sum;
      new_search_direction->max_x_width = 100.0*new_search_direction->x_width;
    }
  }

finish: ;

  c->force_optimize_stop_flag = FALSE;

  wn_gppop();
}


