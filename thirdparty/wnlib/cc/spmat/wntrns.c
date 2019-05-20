/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/**********************************************************************

wn_trans_problem_swap_improve(&code,&objective,&delta,&new_result,
                              result,cost_mat,
                              max_time)

**********************************************************************/

#include "wnlib.h"
#include "wnmax.h"

#include "wnsmat.h"



typedef struct perif_data_struct *perif_data;
typedef struct entry_data_struct *entry_data;


local int basic_entry_queue_size,basic_entry_queue_top;
local entry_data *basic_entry_queue;

local wn_htab entry_table;
local wn_set entry_set;
local int len_i,len_j,time_spent;


struct perif_data_struct
{
  wn_sll entry_list,*pentry_list_end,basic_list;
};

struct entry_data_struct
{
  double flow,cost;

  int basic_entry_queue_index;
  
  int i,j;
};

local wn_memgp top_group;
local perif_data *i_perif_array,*j_perif_array;
local double old_objective;
local bool transpose;



wn_trans_problem_swap_improve(pcode,pobjective,pdelta,pnew_result,
                              result,cost_mat,
                              max_time)

int *pcode;
double *pobjective,*pdelta;
wn_sparse_matrix *pnew_result,result,cost_mat;
int max_time;

{
  initialize(result,cost_mat);

  compute_approximate_solution(max_time);

  compute_result(pobjective,pnew_result);
  *pdelta = *pobjective - old_objective;

  if(transpose)
  {
    wn_transpose_sparse_matrix(*pnew_result);
    wn_transpose_sparse_matrix(result);
    wn_transpose_sparse_matrix(cost_mat);
  }

  finish();

  *pcode = WN_SUBOPTIMAL;
}



local initialize(result,cost_mat)

wn_sparse_matrix result,cost_mat;

{
  perif_data perif;
  int i,j;

  top_group = wn_curgp();
  wn_gpmake("no_free");

  wn_assertmsg(result->len_i == cost_mat->len_i,
       "wn_trans_problem_swap_improve:  result and cost_mat len_i differ.");
  wn_assertmsg(result->len_j == cost_mat->len_j,
       "wn_trans_problem_swap_improve:  result and cost_mat len_j differ.");

  transpose = (result->len_i < result->len_j);
  if(transpose)
  {
    wn_transpose_sparse_matrix(result);
    wn_transpose_sparse_matrix(cost_mat);
  }

  len_i = cost_mat->len_i;
  len_j = cost_mat->len_j;

  time_spent = 0;

  i_perif_array = (perif_data *)wn_zalloc(len_i*wn_sizeof(perif_data));
  j_perif_array = (perif_data *)wn_zalloc(len_j*wn_sizeof(perif_data));

  for(i=0;i<len_i;i++)
  {
    perif = wn_znew(perif_data);
    
    i_perif_array[i] = perif;
  }
  for(j=0;j<len_j;j++)
  {
    perif = wn_znew(perif_data);
    
    j_perif_array[j] = perif;
  }

  make_entries(result,cost_mat);

  compute_old_objective();
}



local make_entries(result,cost_mat)

wn_sparse_matrix result,cost_mat;

{
  int i,j;
  wn_sparse_matrix_entry matrix_entry;
  entry_data entry;
  perif_data perif;
  wn_sll el;
  bool success;

  make_basic_entry_queue(cost_mat->len_i + cost_mat->len_j + 2);
  make_entry_table();
  wn_mkset(&entry_set);

  for(j=0;j<len_j;++j)
  {
    perif = j_perif_array[j];

    for(wn_loopinit(),el=(cost_mat->j_lists)[j];wn_sllloop(&el,&matrix_entry);)
    {
      entry = wn_znew(entry_data);

      entry->flow = 0.0;
      entry->cost = matrix_entry->value;

      entry->basic_entry_queue_index = -1;

      entry->i = matrix_entry->i;
      entry->j = matrix_entry->j;

      wn_setins(entry_set,entry);

      insert_into_entry_table(entry);

      wn_sllins(&(perif->entry_list),entry);
    }

    wn_randomize_sll(&(perif->entry_list));
    wn_sllend(&(perif->pentry_list_end),&(perif->entry_list));
  }

  for(i=0;i<len_i;++i)
  {
    perif = i_perif_array[i];

    for(wn_loopinit(),el=(result->i_lists)[i];wn_sllloop(&el,&matrix_entry);)
    {
      if(matrix_entry->value > 0.0)
      {
        success = get_from_entry_table(&entry,matrix_entry->i,matrix_entry->j);
        wn_assert(success);

        entry->flow = matrix_entry->value;

        wn_sllins(&(perif->basic_list),entry);

        insert_into_basic_entry_queue(entry);
      }
    }
  }
}



local make_basic_entry_queue(size)

int size;

{
  basic_entry_queue_size = size;

  basic_entry_queue = (entry_data *)wn_zalloc(size*wn_sizeof(entry_data));
  basic_entry_queue_top = 0;
}



local insert_into_basic_entry_queue(entry)

entry_data entry;

{
  wn_assert(entry->basic_entry_queue_index == -1);

  basic_entry_queue[basic_entry_queue_top] = entry;
  entry->basic_entry_queue_index = basic_entry_queue_top;

  ++basic_entry_queue_top;

  wn_assert(basic_entry_queue_top <= basic_entry_queue_size);
}



local delete_from_basic_entry_queue(entry)

entry_data entry;

{
  int basic_entry_queue_index;
  entry_data moved_entry;

  --basic_entry_queue_top;
  wn_assert(basic_entry_queue_top >= 0);

  basic_entry_queue_index = entry->basic_entry_queue_index;
  entry->basic_entry_queue_index = -1;

  wn_assert(basic_entry_queue_index >= 0);

  if(basic_entry_queue_index < basic_entry_queue_top)
  {
    moved_entry = basic_entry_queue[basic_entry_queue_top];
    basic_entry_queue[basic_entry_queue_top] = NULL;

    basic_entry_queue[basic_entry_queue_index] = moved_entry;
    moved_entry->basic_entry_queue_index = basic_entry_queue_index;
  }
  else
  {
    basic_entry_queue[basic_entry_queue_top] = NULL;
  }
}



local get_random_basic_entry(pentry)

entry_data *pentry;

{
  int index;

  if(basic_entry_queue_top == 0)
  {
    *pentry = NULL;
  }
  else
  {
    index = wn_random_mod_int(basic_entry_queue_top);

    *pentry = basic_entry_queue[index];
  }
}



struct index_struct
{
  int i,j;
};

local make_entry_table()
{
  int hash_index_struct();
  bool index_structs_equal();
  void copy_index_struct();
  extern void wn_do_nothing();

  wn_mkhtab(&entry_table,
            (hash_index_struct),(index_structs_equal),
            (copy_index_struct),(wn_do_nothing)); 
}



local int hash_index_struct(s)

struct index_struct *s;

{
  int hash;

  hash = 0;

  wn_cumhasho(&hash,wn_inthash(s->i));
  wn_cumhasho(&hash,wn_inthash(s->j));

  return(hash);
}



local bool index_structs_equal(s1,s2)

struct index_struct *s1,*s2;

{
  return(
          (s1->i == s2->i)
            &&
          (s1->j == s2->j)
        );
}



local void copy_index_struct(ps,s)

struct index_struct **ps,*s;

{
  wn_blkacpy((ptr **)ps,(ptr)s,wn_sizeof(struct index_struct));
}



local insert_into_entry_table(entry)

entry_data entry;

{
  static struct index_struct key_struct;
  struct index_struct *key = &key_struct;
  bool success;

  key->i = entry->i;
  key->j = entry->j;

  success = wn_hins((ptr)entry,entry_table,(ptr)key);
  wn_assert(success);
}



local bool get_from_entry_table(pentry,i,j)

entry_data *pentry;
int i,j;

{
  static struct index_struct key_struct;
  struct index_struct *key = &key_struct;

  key->i = i;
  key->j = j;

  return(wn_hget((ptr *)pentry,entry_table,(ptr)key));
}



local compute_old_objective()
{
  int j;
  perif_data perif;
  register wn_sll el;
  register entry_data entry;
  double flow;

  old_objective = 0.0;

  for(j=0;j<len_j;++j)
  {
    perif = j_perif_array[j];

    for(el=perif->entry_list;el != NULL;el=el->next)
    {
      entry = (entry_data)(el->contents);

      flow = entry->flow;

      wn_assert(flow >= 0.0);

      if(flow > 0.0)
      {
        old_objective += flow * entry->cost;
      }
    }
  }
}



local finish()
{
  wn_gpfree();
}



local compute_result(pobjective,presult)

double *pobjective;
wn_sparse_matrix *presult;

{
  double objective;
  entry_data entry;

  wn_gppush(top_group);

  objective = 0.0;
  wn_make_sparse_matrix(presult,len_i,len_j);

  for(wn_loopinit();wn_setloop(entry_set,&entry);)
  {
    if(entry->flow > 0.0)
    {
       objective += (entry->flow * entry->cost);
       wn_insert_sparse_matrix_value(*presult,
                                     entry->flow,entry->i,entry->j);
    }
  }

  wn_sort_sparse_matrix(*presult);
  *pobjective = objective;

  wn_gppop();
}



local compute_approximate_solution(max_time)

int max_time;

{
  entry_data basic_entry;

  while(time_spent < max_time)
  {
    get_random_basic_entry(&basic_entry);

    if(basic_entry == NULL)
    {
      break;
    }

    try_to_swap_basic_entry(basic_entry);
  }
}



local try_to_swap_basic_entry(out2)

entry_data out2;

{
  bool success;
  entry_data in1,in2,out1;

  find_good_swap_involving_basic_entry(&success,&in1,&in2,&out1,out2);

  if(success)
  {
    perform_swap(in1,in2,out1,out2);
  }
  else
  {
    delete_from_basic_entry_queue(out2);
  }
}



local find_good_swap_involving_basic_entry(psuccess,pin1,pin2,pout2,out1)

bool *psuccess;
entry_data *pin1,*pin2,*pout2,out1;

{
  double partial_score1,partial_score2,score,best_score;
  entry_data in1,in2,out2,best_in2,best_out2;
  perif_data i_perif,j_perif;
  wn_sll i_el,j_el,last_j_el;

  *psuccess = FALSE;

  best_score = 0.0;
  best_in2 = NULL;
  best_out2 = NULL;

  partial_score1 = -out1->cost;
  j_perif = j_perif_array[out1->j]; 
  last_j_el = NULL; 

  for(j_el=j_perif->entry_list;j_el!=NULL;j_el=j_el->next)
  {
    in1 = (entry_data)(j_el->contents);
    partial_score2 = partial_score1 + in1->cost;
    i_perif = i_perif_array[in1->i];

    for(i_el=i_perif->basic_list;i_el!=NULL;i_el=i_el->next)
    {
      ++time_spent;

      out2 = (entry_data)(i_el->contents);

      if(get_from_entry_table(&in2,out1->i,out2->j))  /* swap possible */
      {
        score = partial_score2 - out2->cost + in2->cost;
  
        if(score < best_score)  /* improvement */
        {
	  best_in2 = in2;
	  best_out2 = out2;
	  best_score = score;
        } 
      }
    }

    if(best_in2 != NULL)
    {
      *psuccess = TRUE;

      break;
    }

    last_j_el = j_el;
  }

  if(*psuccess)
  {
    *pin1 = in1;
    *pin2 = best_in2;
    *pout2 = best_out2;

    if(last_j_el != NULL)
    {
      *(j_perif->pentry_list_end) = j_perif->entry_list;
      j_perif->entry_list = last_j_el->next;
      last_j_el->next = NULL;
      j_perif->pentry_list_end = &(last_j_el->next);
    }
  }
}



local perform_swap(in1,in2,out1,out2)

entry_data in1,in2,out1,out2;

{
  double flow;

  flow = wn_min(out1->flow,out2->flow);

  wn_assert(flow >= 0.0);

  insert_basic_entry_if_necessary(in1);
  insert_basic_entry_if_necessary(in2);

  in1->flow += flow;
  in2->flow += flow;

  out1->flow -= flow;
  out2->flow -= flow;

  delete_basic_entry_if_necessary(out1);
  delete_basic_entry_if_necessary(out2);

  /*
  keep_score(flow,in1,in2,out1,out2);
  */
}



local keep_score(flow,in1,in2,out1,out2)

double flow;
entry_data in1,in2,out1,out2;

{
  double score;
  static double total = 0.0;
  static int count = 0;

  score = in1->cost + in2->cost - out1->cost - out2->cost;
  total += flow*score;
  ++count;

  /*
  if(count%10 == 0)
  */
  {
    printf("%d: score=%f\n",count,(float)(old_objective + total));
  }
}



local insert_basic_entry_if_necessary(entry)

entry_data entry;

{
  if(entry->flow == 0.0)
  {
    insert_into_basic_entry_queue(entry);

    wn_sllins(&(i_perif_array[entry->i]->basic_list),entry);
  }
}



local delete_basic_entry_if_necessary(entry)

entry_data entry;

{
  if(entry->flow == 0.0)
  {
    if(entry->basic_entry_queue_index != -1)   /* in queue now */
    {
      delete_from_basic_entry_queue(entry);
    }

    wn_slldel(&(i_perif_array[entry->i]->basic_list),entry);
  }
}




