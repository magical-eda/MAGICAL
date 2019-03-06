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



main()
{
  /*
  test_inverse();
  test_ls_inverse();
  test_simplex_method();
  */
  test_fft();
}



local test_inverse()
{
  wn_matrix mat,mati,prod;
  int code;

  wn_gpmake("no_free");    /* temp memory group */
  wn_gplabel("main group");

  create_matrix1(&mat);
  printf("matrix made\n");

  wn_print_matrix(mat);
  wn_invert_matrix(&code,&mati,mat);

  printf("code = %d\n",code);
  wn_print_matrix(mati);

  wn_matrix_multiply_matrix(&prod,mat,mati);
  printf("product\n");
  wn_print_matrix(prod);

  wn_gpfree();
}



local create_matrix1(pmat)

wn_matrix *pmat;

{
  wn_enter_matrix(pmat);
  /*
  random_matrix(pmat,4,4,-10.0,10.0);
  random_matrix(pmat,200,200,-10.0,10.0);
  random_matrix(pmat,100,100,-10.0,10.0);
  random_matrix(pmat,400,400,-10.0,10.0);
  */
}



local test_ls_inverse()
{
  wn_matrix mat,mati,prod;
  wn_vector vect,coefs;
  int code;

  wn_gpmake("no_free");    /* temp memory group */
  wn_gplabel("main group");

  printf("mat:\n");
  wn_enter_matrix(&mat);
  printf("vect:\n");
  wn_enter_vector(&vect);

  printf("mat:\n");
  wn_print_matrix(mat);
  printf("vect:\n");
  wn_print_vector(vect);

  wn_ls_invert_matrix(&code,&mati,mat);

  printf("code = %d\n",code);
  wn_print_matrix(mati);

  wn_matrix_multiply_matrix(&prod,mati,mat);
  printf("product\n");
  wn_print_matrix(prod);

  wn_matrix_multiply_vector(&coefs,mati,vect);
  printf("coefs\n");
  wn_print_vector(coefs);

  wn_gpfree();
}



local test_simplex_method()
{
  double objective;
  wn_vector objective_vect,right_side,solution,shadow_prices;
  wn_matrix mat;
  int code;

  wn_gpmake("no_free");    /* temp memory group */
  wn_gplabel("main group");

  create_problem(&objective_vect,&right_side,&mat);

  printf("objective_vect:\n");
  wn_print_vector(objective_vect);
  printf("matrix:\n");
  wn_print_matrix(mat);
  printf("right_side:\n");
  wn_print_vector(right_side);

  wn_simplex_method(&code,&objective,&shadow_prices,&solution,
                    objective_vect,mat,right_side);

  printf("code = %d\n",code);

  printf("objective = %f\n",objective);

  printf("solution:\n");
  wn_print_vector(solution);

  printf("shadow_prices:\n");
  wn_print_vector(shadow_prices);

  wn_gpfree();
}



local create_problem(pobjective_vect,pright_side,pmat)

wn_vector *pobjective_vect,*pright_side;
wn_matrix *pmat;

{
  random_simplex_problem(pobjective_vect,pright_side,pmat,8,17);
  /*
  enter_problem(pobjective_vect,pright_side,pmat);
  random_simplex_problem(pobjective_vect,pright_side,pmat,50,100);
  random_simplex_problem(pobjective_vect,pright_side,pmat,50,100);
  random_simplex_problem(pobjective_vect,pright_side,pmat,100,200);
  create_assignment_problem(pobjective_vect,pright_side,pmat);
  */
}



local enter_problem(pobjective_vect,pright_side,pmat)

wn_vector *pobjective_vect,*pright_side;
wn_matrix *pmat;

{
  printf("objective:\n");
  wn_enter_vector(pobjective_vect);
  printf("matrix:\n");
  wn_enter_matrix(pmat);
  printf("right_side:\n");
  wn_enter_vector(pright_side);
}



local create_assignment_problem(pobjective_vect,pright_side,pmat)

wn_vector *pobjective_vect,*pright_side;
wn_matrix *pmat;

{
  wn_matrix assignment_mat;

  wn_enter_matrix(&assignment_mat);
  /*
  random_matrix(&assignment_mat,10,10,0.0,10.0);
  random_matrix(&assignment_mat,4,4,0.0,10.0);
  */

  printf("assignment_mat:\n");
  wn_print_matrix(assignment_mat);

  make_assignment_problem(pobjective_vect,pright_side,pmat,assignment_mat);
}



local test_fft()
{
  extern double wn_flat_distribution();
  wn_cplx *vector;
  int i,len_i;

  wn_gpmake("no_free");    /* temp memory group */

  len_i = 1<<18;
  /*
  len_i = 1<<3;
  */

  wn_cplx_make_raw_vector(&vector,len_i);

  for(i=0;i<len_i;++i)
  {
    /*
    printf("i=%d\n",i);
    wn_cplx_enter(vector[i]);
    vector[i]->real = (i < 5);
    */
    vector[i]->real = wn_flat_distribution();
    vector[i]->imag = 0.0;
  }

  /*
  */
  wn_fft_raw_vector(vector,len_i);

  /*
  cplx_print_vector(vector,len_i);

  wn_fft_raw_vector(vector,len_i);

#if 0
  cplx_print_vector(vector,len_i);
#endif

  wn_inverse_sft_raw_vector(vector,len_i);

  cplx_print_vector(vector,len_i);
  */
}



local cplx_print_vector(vector,len_i)

wn_cplx vector[];
int len_i;

{
  int i;

  for(i=0;i<len_i;++i)
  {
    printf("vector[%d] = ",i);
    wn_cplx_print(vector[i]);
    printf("\n");
  }
}


