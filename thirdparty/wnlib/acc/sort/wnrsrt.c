/**************************************************************************

wn_radix_sort_sll(&list,pkeyindex_func,pkeylen_func)
wn_sll list;
char (*pkeyindex_func)(key,index);   * ptr key; int index; *
int (*pkeylen_func)(key);            * ptr key; *

**************************************************************************/
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
#include "wnmemb.h"
#include "wnsll.h"

#include "wnsort.h"



#define CHAR_BITS          8
#define NUM_CHAR_VALS      (1<<CHAR_BITS) 

#define MAX_KEY_LEN        1000



local void find_lengths_of_entries
(
  wn_sll list_by_length[MAX_KEY_LEN],
  wn_sll list_end_by_length[MAX_KEY_LEN],
  wn_sll list,
  int (*pkeylen_func)(ptr key)
)
{
  register int length;
  register wn_sll el,next,*ploc;

  wn_memzero(list_by_length,MAX_KEY_LEN*sizeof(wn_sll));
  wn_memzero(list_end_by_length,MAX_KEY_LEN*sizeof(wn_sll));

  for(el=list;el!=NULL;el=next)
  {
    next = el->next;

    length = (*pkeylen_func)(el->contents);

    ploc = &(list_by_length[length]);
    if(*ploc == NULL)
    {  
      list_end_by_length[length] = el;
    }
    el->next = *ploc;
    *ploc = el;
  }
}


local void put_entries_in_list_by_char
(
  wn_sll list_by_char[NUM_CHAR_VALS],
  wn_sll list_end_by_char[NUM_CHAR_VALS],
  wn_sll list_to_sort,
  int sort_by_char,
  char (*pkeyindex_func)(ptr key,int index)
)
{
  register wn_sll *ploc,el,next;
  register unsigned int char_val;

  wn_memzero(list_by_char,NUM_CHAR_VALS*sizeof(wn_sll));
  wn_memzero(list_end_by_char,NUM_CHAR_VALS*sizeof(wn_sll));

  for(el=list_to_sort;el!=NULL;el=next)  /* this is the inner loop */
  {
    next = el->next;

    char_val = (unsigned int)((*pkeyindex_func)(el->contents,sort_by_char));
    char_val &= (NUM_CHAR_VALS-1);
    
    ploc = &(list_end_by_char[char_val]);
    if(*ploc == NULL)
    {
      list_by_char[char_val] = el;
    }
    else
    {
      (*ploc)->next = el;
    }
    *ploc = el;
  }
}


local void compute_start_index
(
  int *pstart_index,
  wn_sll list_by_char[NUM_CHAR_VALS]
)
{
  int index;

  for(index=0;index<NUM_CHAR_VALS;index++)
  {
    if(list_by_char[index] != NULL)
    {
      *pstart_index = index;

      return;
    }
  }

  *pstart_index = 0;
}


local void compute_end_index
(
  int *pend_index,
  wn_sll list_by_char[NUM_CHAR_VALS]
)
{
  int index;

  for(index=NUM_CHAR_VALS-1;index>=0;index--)
  {
    if(list_by_char[index] != NULL)
    {
      *pend_index = index;

      return;
    }
  }

  *pend_index = 0;
}


local void put_list_by_char_entries_into_sorted_list
(
  wn_sll *psorted_list_begin,
  wn_sll list_by_char[NUM_CHAR_VALS],
  wn_sll list_end_by_char[NUM_CHAR_VALS]
)
{
  wn_sll last_list_end;
  int start_index,end_index,index;

  compute_start_index(&start_index,list_by_char);
  compute_end_index(&end_index,list_by_char);

  *psorted_list_begin = list_by_char[start_index];

  last_list_end = list_end_by_char[start_index];

  for(index=start_index+1;index<=end_index;index++)
  {
    if(list_by_char[index] != NULL)
    {
      last_list_end->next = list_by_char[index];

      last_list_end = list_end_by_char[index];
    }
  }

  last_list_end->next = (wn_sll)NULL;  /* terminate list */
}


local void sort_list_by_nth_char
(
  wn_sll *psorted_list_begin,
  wn_sll list_to_sort,
  int sort_by_char,
  char (*pkeyindex_func)(ptr key,int index)
)
{
  wn_sll list_by_char[NUM_CHAR_VALS],list_end_by_char[NUM_CHAR_VALS];

  put_entries_in_list_by_char(list_by_char,list_end_by_char,
                              list_to_sort,sort_by_char,
                              pkeyindex_func);
  put_list_by_char_entries_into_sorted_list(psorted_list_begin,
                                            list_by_char,
                                            list_end_by_char);
}

EXTERN
void wn_radix_sort_sll
(
  wn_sll *plist,
  char (*pkeyindex_func)(ptr key,int index),
  int (*pkeylen_func)(ptr key)
)
{
  int length;
  wn_sll list_by_length[MAX_KEY_LEN],list_end_by_length[MAX_KEY_LEN],
         current_list_begin;

  find_lengths_of_entries(list_by_length,list_end_by_length,*plist,
                          pkeylen_func);

  current_list_begin = (wn_sll)NULL;

  for(length=MAX_KEY_LEN-1;length>=0;length--)
  {
    if(list_by_length[length] != NULL)
    {
      list_end_by_length[length]->next = current_list_begin;
      current_list_begin = list_by_length[length];
    }

    if(length == 0)
    {
      break;
    }

    if(current_list_begin != NULL)
    {
      sort_list_by_nth_char(&current_list_begin,
                            current_list_begin,length-1,pkeyindex_func);
    }   
  }

  *plist = current_list_begin;
}



