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
#include "wnsmat.h"
#include "wnary.h"
#include "wncons.h"



main()
{
  /*
  wn_gp_fill();
  wn_gp_pad();
  wn_stack_fill();
  */

  wn_gpmake("no_free");

  test1();

  wn_gpfree();
}



local test1()
{
  wn_sparse_matrix mat,result;
  int code;
  double len;


  /*
  wn_enter_sparse_matrix(&mat);
  */
  wn_random_symmetric_sparse_matrix(&mat,500,500,0.0,1.0,1.0);
  /*
  wn_random_sparse_matrix(&mat,200,200,0.0,1.0,1.0);
  wn_print_sparse_matrix(mat);
  */
  printf("matrix made\n");

  wn_min_spanning_tree(&code,&len,&result,mat);

  printf("code = %d,len = %f\n",code,(float)len);

  /*
  wn_print_sparse_matrix(result);
  */
}



#if 0
local test3()
{
  wn_sparse_matrix mat;
  wn_sparse_matrix_entry entry;
  int code;
  double objective;
  wn_sll result,el;

  /*
  wn_enter_sparse_matrix(&mat);
  */
  wn_random_sparse_matrix(&mat,16,16,0.0,1.0,0.3);
  /*
  wn_random_sparse_matrix(&mat,200,200,0.0,1.0,1.0);
  wn_print_sparse_matrix(mat);
  */
  printf("matrix made\n");

  mst_estimate(&code,&objective,mat);
  printf("code = %d,estimate objective = %f\n",code,(float)objective);

  /*
  wn_shortest_path(&code,&objective,&result,mat,0,1);
  */
  wn_longest_path(&code,&objective,&result,mat,0,1);

  printf("code = %d,objective = %f\n",code,(float)objective);

  if(code == WN_SUCCESS)
  {
    for(wn_loopinit(),el=result;wn_sllloop(&el,&entry);)
    {
      printf("entry->i=%d,entry->j=%d\n",entry->i,entry->j);
    }
  }
}
#endif



local test3()
{
  wn_sparse_matrix mat;
  wn_sparse_matrix_entry entry;
  int code;
  double objective;
  wn_sll result,el;

  /*
  */
  make_random_monotonic_matrix(&mat,1000,100000);
  /*
  wn_print_sparse_matrix(mat);
  wn_enter_sparse_matrix(&mat);
  wn_random_sparse_matrix(&mat,16,16,0.0,1.0,0.3);
  wn_random_sparse_matrix(&mat,200,200,0.0,1.0,1.0);
  wn_print_sparse_matrix(mat);
  */
  /*
  wn_critical_path(&code,&objective,&result,mat,0,99);
  */
  wn_shortest_path(&code,&objective,&result,mat,0,99);

  printf("code = %d,objective = %f\n",code,(float)objective);

  /*
  if(code == WN_SUCCESS)
  {
    for(wn_loopinit(),el=result;wn_sllloop(&el,&entry);)
    {
      printf("entry->i=%d,entry->j=%d\n",entry->i,entry->j);
    }
  }
  */
}



local make_random_monotonic_matrix(pmat,size,entry_count)

wn_sparse_matrix *pmat;
int size,entry_count;

{
  extern double wn_flat_distribution();
  double value;
  int i,j,c;

  wn_make_sparse_matrix(pmat,size,size);

  for(c=0;c<entry_count;++c)
  {
    i = wn_random_mod_int(size);
    j = wn_random_mod_int(size);

    value = wn_flat_distribution();

    if(i<j)
    {
      wn_insert_sparse_matrix_value(*pmat,value,i,j);
    }
    else if(i>j)
    {
      wn_insert_sparse_matrix_value(*pmat,value,j,i);
    }
    else  /* i == j */
    {
      ;  
    }
  }

  wn_sort_sparse_matrix(*pmat);
}



#define SIZE 100


local test4()
{
  wn_sparse_matrix mat,result;
  double objective,delta,*i_capacities,*j_capacities;
  int code;

  /*
  wn_enter_sparse_matrix(&mat);
  */
  wn_random_sparse_matrix(&mat,SIZE,SIZE,0.0,1.0,1.00);
  /*
  wn_random_sparse_matrix(&mat,10000,100,0.0,1.0,0.1);
  wn_random_sparse_matrix(&mat,1000,100,0.0,1.0,0.1);
  wn_random_sparse_matrix(&mat,1000,10,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,50,50,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,1000,1000,0.0,1.0,0.01);
  wn_random_sparse_matrix(&mat,100,100,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,50,50,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,5,5,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,12,3,0.0,1.0,1.0);
  slant_sparse_matrix(mat);
  wn_print_sparse_matrix(mat);
  */
  printf("matrix made\n");

  wn_make_vect(&i_capacities,mat->len_i);
  wn_make_vect(&j_capacities,mat->len_j);

  wn_arrayfill(i_capacities,mat->len_i,1.0,double);
  wn_arrayfill(j_capacities,mat->len_j,1.0,double);

  wn_trans_problem(&code,&objective,&result,
		   mat,i_capacities,j_capacities);
  printf("code=%d,objective = %f\n",
         code,(float)objective);
  /*
  */

  /*
  wn_trans_problem_feasible(&code,&result,mat,i_capacities,j_capacities);
  printf("code=%d\n",code);

  wn_trans_problem_swap_improve(&code,&objective,&delta,&result,
                                result,
                                mat,
                                WN_IHUGE);
  printf("swap code=%d,objective = %lf,delta = %lf\n",
         code,objective,delta);

  wn_trans_problem_simplex_improve(&code,&objective,&delta,&result,
                                   result,
                                   mat,
                                   WN_IHUGE);
  printf("code=%d,objective = %lf,delta = %lf\n",
         code,objective,delta);
  */

  /*
  wn_trans_problem_swap_improve(&code,&objective,&delta,&result,
                                result,
                                mat,
                                4*wn_sparse_matrix_entry_count(mat));
  printf("swap code=%d,objective = %f,delta = %f\n",
         code,(float)objective,(float)delta);

  wn_trans_problem_simplex_improve(&code,&objective,&delta,&result,
                                   result,
                                   mat,
                                   4*wn_sparse_matrix_entry_count(mat));
  printf("code=%d,objective = %f,delta = %f\n",
         code,(float)objective,(float)delta);

  */
  /*
  wn_trans_problem_swap_improve(&code,&objective,&delta,&result,
                                result,
                                mat,
                                WN_IHUGE);
  printf("swap code=%d,objective = %f,delta = %f\n",
         code,(float)objective,(float)delta);

  wn_trans_problem_simplex_improve(&code,&objective,&delta,&result,
                                   result,
                                   mat,
                                   WN_IHUGE);
  printf("code=%d,objective = %f,delta = %f\n",
         code,(float)objective,(float)delta);
  */

  /*
  wn_print_sparse_matrix(result);
  */
}



local make_identity_matrix(presult,len)

wn_sparse_matrix *presult;
int len;

{
  int i;

  wn_make_sparse_matrix(presult,len,len);

  for(i=0;i<len;++i)
  {
    wn_insert_sparse_matrix_value(*presult,1.0,i,i);
  }

  wn_sort_sparse_matrix(*presult);
}



local slant_sparse_matrix(mat)

wn_sparse_matrix mat;

{
  int j,len_j;
  wn_sll el,list;
  double slant;
  wn_sparse_matrix_entry entry;

  len_j = mat->len_j;

  slant = 0.0;

  for(j=0;j<len_j;j++)
  {
    list = (mat->j_lists)[j];

    for(wn_loopinit(),el=list;wn_sllloop(&el,&entry);)
    {
      entry->value += slant;
    }

    slant += 0.1;
  }
}



local test5()
{
  extern void wn_resistance_series_reduce(),
              wn_resistance_parallel_reduce(),
              wn_resistance_y_to_delta(),
              wn_resistance_delta_to_y(),
              wn_path_series_reduce(),
              wn_max_path_parallel_reduce(),
              wn_min_path_parallel_reduce(),
              wn_path_y_to_delta(),
              wn_path_delta_to_y();
  wn_sparse_matrix mat;
  wn_set significant_nodes;
  int code;
  wn_sll el,result;
  wn_sparse_matrix_entry entry;
  double objective;

  create_circuit_mat(&significant_nodes,&mat);

  wn_print_symmetric_sparse_matrix(mat);

  /*
  wn_truncate_tributary_nodes(mat,significant_nodes);
  wn_y_delta_reduce(&mat,mat,significant_nodes,
		    (wn_resistance_series_reduce),
		    (wn_resistance_parallel_reduce),
		    (wn_resistance_y_to_delta),
		    (wn_resistance_delta_to_y));
  wn_y_delta_reduce(&mat,mat,significant_nodes,
		    (wn_path_series_reduce),
		    (wn_max_path_parallel_reduce),
		    (void (*)())NULL,
		    (void (*)())NULL);
  */

  wn_print_symmetric_sparse_matrix(mat);

  mst_estimate(&code,&objective,mat);
  printf("code = %d,estimate objective = %f\n",code,(float)objective);

  /*
  wn_longest_path(&code,&objective,&result,mat,0,1);
  wn_longest_path(&code,&objective,&result,mat,1,4);
  wn_shortest_path(&code,&objective,&result,mat,1,4);
  */
  wn_longest_path(&code,&objective,&result,mat,0,1);

  printf("code = %d,objective = %f\n",code,(float)objective);

  /*
  if(code == WN_SUCCESS)
  {
    for(wn_loopinit(),el=result;wn_sllloop(&el,&entry);)
    {
      printf("entry->i=%d,entry->j=%d\n",entry->i,entry->j);
    }
  }
  */
}



local mst_estimate(pcode,pobjective,mat)

int *pcode;
double *pobjective;
wn_sparse_matrix mat;

{
  int i;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  wn_copy_sparse_matrix(&mat,mat);

  for(i=0;i<(mat->len_i);++i)
  {
    for(wn_loopinit(),el=(mat->i_lists)[i];wn_sllloop(&el,&entry);)
    {
      entry->value = -(entry->value);
    }
  }

  wn_min_spanning_tree(pcode,pobjective,&mat,mat);

  *pobjective = -(*pobjective);
}



#ifdef SIZE
#undef SIZE
#endif
#define SIZE 25

local create_circuit_mat(psignificant_nodes,pmat)

wn_set *psignificant_nodes;
wn_sparse_matrix *pmat;

{
  /*
  wn_enter_sparse_matrix(pmat);
  create_circuit_mat1(psignificant_nodes,pmat);
  create_circuit_mat1_5(psignificant_nodes,pmat);
  create_circuit_mat2(psignificant_nodes,pmat);
  */
  wn_random_symmetric_sparse_matrix(pmat,SIZE,SIZE,0.0,1.0,0.10);

  wn_mkset(psignificant_nodes);

  wn_setins(*psignificant_nodes,0);
  wn_setins(*psignificant_nodes,1);
}



local create_circuit_mat1(psignificant_nodes,pmat)

wn_set *psignificant_nodes;
wn_sparse_matrix *pmat;

{
  wn_make_sparse_matrix(pmat,4,4);

  wn_insert_sparse_matrix_value(*pmat,1.0,0,2);
  wn_insert_sparse_matrix_value(*pmat,1.0,2,1);
  wn_insert_sparse_matrix_value(*pmat,3.0,2,3);
  wn_insert_sparse_matrix_value(*pmat,2.0,0,3);
  wn_insert_sparse_matrix_value(*pmat,2.0,3,1);

  wn_symmetrize_sparse_matrix(*pmat); 


  wn_mkset(psignificant_nodes);

  wn_setins(*psignificant_nodes,0);
  wn_setins(*psignificant_nodes,1);
}



local create_circuit_mat1_5(psignificant_nodes,pmat)

wn_set *psignificant_nodes;
wn_sparse_matrix *pmat;

{
  wn_make_sparse_matrix(pmat,7,7);

  wn_insert_sparse_matrix_value(*pmat,1.0,0,2);
  wn_insert_sparse_matrix_value(*pmat,1.0,2,1);
  wn_insert_sparse_matrix_value(*pmat,3.0,2,3);
  wn_insert_sparse_matrix_value(*pmat,2.0,0,3);
  wn_insert_sparse_matrix_value(*pmat,2.0,3,1);

  wn_insert_sparse_matrix_value(*pmat,2.0,3,4);
  wn_insert_sparse_matrix_value(*pmat,2.0,3,5);
  wn_insert_sparse_matrix_value(*pmat,2.0,3,6);
  wn_insert_sparse_matrix_value(*pmat,2.0,4,5);
  wn_insert_sparse_matrix_value(*pmat,2.0,5,6);
  wn_insert_sparse_matrix_value(*pmat,2.0,4,6);

  wn_symmetrize_sparse_matrix(*pmat); 


  wn_mkset(psignificant_nodes);

  wn_setins(*psignificant_nodes,0);
  wn_setins(*psignificant_nodes,1);
}



local create_circuit_mat2(psignificant_nodes,pmat)

wn_set *psignificant_nodes;
wn_sparse_matrix *pmat;

{
  wn_make_sparse_matrix(pmat,9,9);

  wn_insert_sparse_matrix_value(*pmat,1.0,1,2);
  wn_insert_sparse_matrix_value(*pmat,1.0,1,7);
  wn_insert_sparse_matrix_value(*pmat,1.0,1,6);
  wn_insert_sparse_matrix_value(*pmat,1.0,2,3);
  wn_insert_sparse_matrix_value(*pmat,1.0,2,8);
  wn_insert_sparse_matrix_value(*pmat,1.0,2,5);
  wn_insert_sparse_matrix_value(*pmat,1.0,7,8);
  wn_insert_sparse_matrix_value(*pmat,1.0,7,0);
  wn_insert_sparse_matrix_value(*pmat,1.0,7,6);
  wn_insert_sparse_matrix_value(*pmat,1.0,6,5);
  wn_insert_sparse_matrix_value(*pmat,1.0,6,3);
  wn_insert_sparse_matrix_value(*pmat,1.0,3,4);
  wn_insert_sparse_matrix_value(*pmat,1.0,8,3);
  wn_insert_sparse_matrix_value(*pmat,1.0,8,0);
  wn_insert_sparse_matrix_value(*pmat,1.0,0,4);
  wn_insert_sparse_matrix_value(*pmat,1.0,0,5);
  wn_insert_sparse_matrix_value(*pmat,1.0,4,5);

  wn_symmetrize_sparse_matrix(*pmat); 


  wn_mkset(psignificant_nodes);

  wn_setins(*psignificant_nodes,1);
  wn_setins(*psignificant_nodes,4);
}



#ifdef SIZE
#undef SIZE
#endif
#define SIZE 10

local test7()
{
  wn_sparse_matrix mat;
  wn_set significant_nodes;

  wn_random_symmetric_sparse_matrix(&mat,SIZE,SIZE,0.0,1.0,0.25);
  wn_print_symmetric_sparse_matrix(mat);

  wn_mkset(&significant_nodes);
  wn_setins(significant_nodes,0);
  wn_setins(significant_nodes,1);

  wn_truncate_tributary_nodes(mat,significant_nodes);

  wn_print_symmetric_sparse_matrix(mat);
}



