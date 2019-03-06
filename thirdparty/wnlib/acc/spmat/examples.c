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
#include <stdio.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmem.h"
#include "wnrnd.h"
#include "wnrndd.h"
#include "wnvect.h"
#include "wnspmat.h"
#include "wntrn.h"
#include "wnsp.h"
#include "wnlp.h"
#include "wncp.h"
#include "wnmst.h"
#include "wnflow.h"



#if 0
#define SIZE  20000


local void test1()
{
  wn_sparse_matrix mat;
  wn_sparse_matrix_entry entry;
  wn_sll result,el;
  int code;
  int i,j,count;
  double len;

  wn_make_sparse_matrix(&mat,SIZE,SIZE);

  for(count=0;count<10*SIZE;++count)
  {
    do
    {
      i = (int)wn_random_mod_int(SIZE);
      j = (int)wn_random_mod_int(SIZE);
    }
    while(!(i<j));

    wn_insert_sparse_matrix_value(mat,wn_random_double_between(0.0,1.0),
				  i,j);
  }
  /*
  wn_enter_sparse_matrix(&mat);
  */
  /*
  wn_random_symmetric_sparse_matrix(&mat,10,10,0.0,1.0,0.1);
  wn_random_sparse_matrix(&mat,40000,40000,0.0,1.0,3.0/40000.0);
  */
  printf("matrix made\n");
  /*
  wn_print_sparse_matrix(mat);
  */

  wn_critical_path(&code,&len,&result,mat,0,SIZE-1);
  /*
  wn_longest_path(&code,&len,&result,mat,0,SIZE-1);
  wn_shortest_path(&code,&len,&result,mat,0,SIZE-1);
  */

  printf("code = %d\n",code);
  printf("len = %lf\n",len);

  /*
  for(el=result;el!=NULL;el=el->next)
  {
    entry = el->contents;

    wn_print_sparse_matrix_entry(entry);
  }
  */
}
#endif


#if 0
local void test1(void)
{
  wn_sparse_matrix mat,result;
  int code;
  double len;


  /*
  wn_enter_sparse_matrix(&mat);
  */
  wn_random_symmetric_sparse_matrix(&mat,1000,0.0,1.0,1.0);
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
#endif



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



#if 1
local void make_random_monotonic_matrix
(
  wn_sparse_matrix *pmat,
  int size,
  double density
)
{
  double value;
  int i,j;

  wn_assert(density > 0.0);
  wn_assert(density <= 1.0);

  wn_make_sparse_matrix(pmat,size,size);

  i = j = 0;

  while(i<size)
  {
    if(density >= 1.0)
    {
      ++j;
    }
    else
    {
      j += (1 + wn_poisson_distribution(1.0/density-1.0));
    }
    while(j >= size)
    {
      ++i;
      if(i >= size)
      {
        goto finish;
      }

      j -= size;
      j += (i+1);
    }

    value = 100.0*wn_flat_distribution();

    wn_assert(i < j);
    wn_assert(j < size);
    wn_assert(i >= 0);
    wn_assert(i < size);

    wn_insert_sparse_matrix_value(*pmat,value,i,j);
  }

finish:
  wn_sort_sparse_matrix(*pmat);
}


local void make_random_cost_matrix
(
  wn_sparse_matrix *pmat,
  wn_sparse_matrix capacity_mat
)
{
  double value;
  int i,j,size;
  wn_sll el;
  wn_sparse_matrix_entry entry;

  size = capacity_mat->len_i;
  wn_assert(size == capacity_mat->len_j);

  wn_make_sparse_matrix(pmat,size,size);

  for(i=0;i<size;++i)
  {
    for(el=(capacity_mat->i_lists)[i];el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);
      
      value = wn_normal_distribution();

      wn_insert_sparse_matrix_value(*pmat,value,i,entry->j);
    }
  }

  wn_sort_sparse_matrix(*pmat);
}


local void make_random_assignment_problem
(
  wn_sparse_matrix *pcapacity_mat,
  wn_sparse_matrix *pcost_mat,
  int size
)
{
  int i,j;
  double cost,flow;

  wn_make_sparse_matrix(pcapacity_mat,2*size+2,2*size+2);
  wn_make_sparse_matrix(pcost_mat,2*size+2,2*size+2);

  for(i=0;i<size;++i)
  for(j=0;j<size;++j)
  {
    cost = wn_random_double_between(0.0,1.0);
    flow = 1.0;
    /*
    flow = wn_random_double_between(0.0,1.0);
    */

    wn_insert_sparse_matrix_value(*pcapacity_mat,flow,i,size+j);
    wn_insert_sparse_matrix_value(*pcost_mat,cost,i,size+j);
  }

  for(i=0;i<size;++i)
  {
    flow = 1.0;
    /*
    flow = wn_random_double_between(0.0,1.0);
    */

    wn_insert_sparse_matrix_value(*pcapacity_mat,flow,2*size,i);
    wn_insert_sparse_matrix_value(*pcost_mat,0.0,2*size,i);
    /*
    */
  }
  for(j=0;j<size;++j)
  {
    flow = 1.0;
    /*
    flow = wn_random_double_between(0.0,1.0);
    */

    wn_insert_sparse_matrix_value(*pcapacity_mat,flow,size+j,2*size+1);
    wn_insert_sparse_matrix_value(*pcost_mat,0.0,size+j,2*size+1);
    /*
    */
  }

  wn_sort_sparse_matrix(*pcapacity_mat);
  wn_sort_sparse_matrix(*pcost_mat);
}


#define SIZE  1600

local void test3(void)
{
  wn_sparse_matrix mat,cost_mat;
  wn_sparse_matrix_entry entry;
  int code;
  double objective;
  double flow,cost;
  wn_sparse_matrix result;
  wn_sll el;
  int i;

  /*
  make_random_monotonic_matrix(&mat,10,0.5);
  */
  make_random_assignment_problem(&mat,&cost_mat,SIZE);
  /*
  wn_random_sparse_matrix(&mat,SIZE,SIZE,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,SIZE,SIZE,0.0,1.0,1.0);
  wn_random_sparse_matrix(&mat,SIZE,SIZE,0.0,1.0,50.0/SIZE);
  wn_print_sparse_matrix(mat);
  wn_enter_sparse_matrix(&mat);
  wn_random_sparse_matrix(&mat,200,200,0.0,1.0,1.0);
  wn_print_sparse_matrix(mat);
  */
  /*
  wn_shortest_path(&code,&objective,&result,mat,0,99);
  wn_critical_path(&code,&objective,&result,mat,0,99);
  wn_print_sparse_matrix(mat);
  */

  /*
  wn_print_sparse_matrix(mat);
  make_random_cost_matrix(&cost_mat,mat);
  wn_print_sparse_matrix(cost_mat);
  */
  /*
  */

  wn_seed_random_number_generator(1);
  printf("start....\n");
  /*
  wn_max_flow(&flow,&cost,&result,mat,cost_mat,0,SIZE-1);
  */
  wn_max_flow(&flow,&cost,&result,mat,cost_mat,2*SIZE,2*SIZE+1);

  printf("flow=%20.20lg,cost=%20.20lg\n",
	 flow,cost);
  /*
  wn_print_sparse_matrix(result);
  */
  /*
  printf("code = %d,objective = %f\n",code,(float)objective);

  wn_print_sparse_matrix(mat);
  */
  /*
  if(code == WN_SUCCESS)
  */
  {
    for(i=0;i<(mat->len_i);++i)
    for(el=(mat->i_lists)[i];el!=NULL;el=el->next)
    {
      entry = (wn_sparse_matrix_entry)(el->contents);

      if(entry->value == 0.0)
      {
        printf("entry->i=%d,entry->j=%d,value=%lg\n",
               entry->i,entry->j,entry->value);
      }
    }
  }
}
#endif



#undef SIZE
#define SIZE 200


local void test4(void)
{
  wn_sparse_matrix mat,result;
  double objective,delta,*i_capacities,*j_capacities;
  int code;
  int i;

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

  for(i=0;i<mat->len_i;++i)
  {
    i_capacities[i] = 1.0;
  }
  for(i=0;i<mat->len_j;++i)
  {
    j_capacities[i] = 1.0;
  }

  wn_trans_problem(&code,&objective,&result,
		   mat,i_capacities,j_capacities);
  printf("code=%d,objective = %lg\n",
         code,objective);
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

  wn_print_sparse_matrix(result);
}



local void test_nimish(void)
{
  wn_sparse_matrix mat,result;
  double objective,delta,*i_capacities,*j_capacities;
  int code;
  int i;
  int size;
  double total_area;

  wn_seed_random_number_generator(1);

  size = 20;

  wn_random_sparse_matrix(&mat,size,4,0.0,1.0,1.00);
  /*
  wn_print_sparse_matrix(mat);
  */
  printf("matrix made\n");

  wn_make_vect(&i_capacities,mat->len_i);
  wn_make_vect(&j_capacities,mat->len_j);

  total_area = 0.0;

  wn_assert(mat->len_i == size);
  for(i=0;i<mat->len_i;++i)
  {
    i_capacities[i] = wn_random_double_between(1,4);
    printf("i_capacities[%d]=%lg\n",i,i_capacities[i]);
    total_area += i_capacities[i];
  }

  printf("total_area = %lg\n",total_area);

  wn_assert(mat->len_j == 4);
  for(i=0;i<mat->len_j;++i)
  {
    j_capacities[i] = (1.0/4.0)*total_area;
    printf("j_capacities[%d]=%lg\n",i,j_capacities[i]);
  }

  wn_trans_problem(&code,&objective,&result,
		   mat,i_capacities,j_capacities);
  printf("code=%d,objective = %lg\n",
         code,objective);
  wn_print_sparse_matrix(result);

  /*
  for(i=0;i<result->len_i;++i)
  {
    wn_sll el;

    if(wn_sllcount((result->i_lists)[i]) > 1)
    {
      for(el=(result->i_lists)[i];el!=NULL;el=el->next)
      {
	wn_sparse_matrix_entry entry;

	entry = el->contents;

	wn_print_sparse_matrix_entry(entry);
      }
    }
  }
  */
}



#if 0
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
#endif



#if 0
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
#endif



#if 0
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
#endif


int main(void)
{
  /*
  wn_gp_fill();
  wn_gp_pad();
  wn_stack_fill();
  */

  wn_gpmake("no_free");

  /*
  test1();
  test_nimish();
  test4();
  */
  test3();

  wn_gpfree();

  return(0);
}


