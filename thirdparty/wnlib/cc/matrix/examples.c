/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Will Naylor

****************************************************************************/
#include "wnlib.h"
#include "wnmat.h"
#include "wncplx.h"



#if 1
main()
{
  double **mat;
  int len,code;

  wn_gpmake("no_free");    /* temp memory group */

  len = 500;

  wn_make_mat(&mat,len,len);

  wn_random_mat(mat,len,len);
  /*
  wn_enter_mat(mat,len,len);
  */

  /*
  wn_print_mat(mat,len,len);
  */

  printf("started.\n");
  wn_gramm_schmidt(&code,mat,len,len);

  printf("code = %d\n",code);
  /*
  wn_print_mat(mat,len,len);
  */
}
#endif


#if 0
main()
{
  extern double wn_dist();
  double **m1,**m2,**m3,*v1,*v2,*v3;
  int len,code;

  wn_gpmake("no_free");    /* temp memory group */

  len = 100;

  wn_make_mat(&m1,len,len);
  wn_make_mat(&m2,len,len);
  wn_make_mat(&m3,len,len);

  wn_make_vect(&v1,len);
  wn_make_vect(&v2,len);
  wn_make_vect(&v3,len);

  while(TRUE)
  {
    wn_random_mat(m1,len,len);

    wn_copy_mat(m2,m1,len,len);
    wn_invert_mat(&code,m2,len);
    printf("code = %d\n",code);

    wn_mult_mats(m3,m1,m2,len,len,len);
    wn_print_mat(m3,len,len);
    wn_mult_mats(m3,m2,m1,len,len,len);
    wn_print_mat(m3,len,len);
  }

  wn_random_vect(v1,len);
  /*
  wn_print_mat(m1,len,len);
  */
  wn_mult_mat_by_vect(v2,m1,v1,len,len);

  wn_copy_mat(m2,m1,len,len);
  wn_invert_mat(&code,m2,len);
  printf("code = %d\n",code);

  wn_solve_system(v3,v2,m2,m1,len,5);

  printf("error = %lg\n",wn_dist(v1,v3,len));
}
#endif


#if 0
main()
{
  extern double wn_normal_distribution();
  double val,objective,*solution_vect,*objective_vect,*right_side,**mat;
  int vars,eqs;
  int len_i,len_j,code,i,j;

  wn_gpmake("no_free");    /* temp memory group */

  vars = 200;
  eqs = 200;

  len_i = eqs;
  len_j = vars+eqs;

  wn_make_mat(&mat,len_i,len_j);

  wn_make_vect(&solution_vect,len_j);
  wn_make_vect(&objective_vect,len_j);
  wn_make_vect(&right_side,len_i);

  for(j=0;j<len_j;++j)
  {
    val = wn_normal_distribution();
    /*
    if(val < 0.0)
    {
      val = -val;
    }
    */

    objective_vect[j] = val; 
    /*
    objective_vect[j] = 1.0;
    */
  }

  for(i=0;i<len_i;++i)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    right_side[i] = val; 
  }

  for(i=0;i<len_i;++i)
  {
  wn_zero_vect(mat[i],len_j);

  for(j=0;j<vars;++j)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    mat[i][j] = val;
  }

  mat[i][vars+i] = 1;
  }

  /*
  printf("mat =\n");
  wn_print_mat(mat,len_i,len_j);
  printf("rhs =\n");
  wn_print_vect(right_side,len_i);
  printf("ob =\n");
  wn_print_vect(objective_vect,len_j);
  */

  printf("starting....\n");
  wn_simplex_method(&code,&objective,NULL,solution_vect,
		    objective_vect,mat,right_side,
		    len_i,len_j);

  printf("code = %d\n",code);
  printf("objective = %lf\n",objective);
  /*
  wn_print_vect(solution_vect,len_j);
  */
}
#endif


#if 0
main()
{
  extern double wn_normal_distribution();
  double val,objective,*solution_vect,*objective_vect,*right_side,**mat;
  int vars,eqs;
  int len_i,len_j,code,i,j;

  wn_gpmake("no_free");    /* temp memory group */

  len_i = 10;
  len_j = 2*len_i;

  wn_make_mat(&mat,len_i,len_j);

  wn_make_vect(&solution_vect,len_j);
  wn_make_vect(&objective_vect,len_j);
  wn_make_vect(&right_side,len_i);

  for(j=0;j<len_j;++j)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    objective_vect[j] = val; 
  }

  for(i=0;i<len_i;++i)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    right_side[i] = val; 
  }

  for(i=0;i<len_i;++i)
  for(j=0;j<len_j;++j)
  {
    val = wn_normal_distribution();
    if(val < 0.0)
    {
      val = -val;
    }

    mat[i][j] = val;
  }
  }

  /*
  printf("mat =\n");
  wn_print_mat(mat,len_i,len_j);
  printf("rhs =\n");
  wn_print_vect(right_side,len_i);
  printf("ob =\n");
  wn_print_vect(objective_vect,len_j);
  */

  wn_simplex_loop(&code,mat,right_side,non_zero_vars,zero_vars,
		  len_i,len_j);

  printf("code = %d\n",code);
  /*
  wn_print_vect(solution_vect,len_j);
  */
}
#endif

