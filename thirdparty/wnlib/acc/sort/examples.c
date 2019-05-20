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
#include <string.h>

#include "wnlib.h"
#include "wnmem.h"
#include "wnsll.h"
#include "wncmp.h"
#include "wnrnd.h"
#include "wnrndd.h"

#include "wnsort.h"
#include "wnrsrl.h"



#if 1
local char string_index(char string[],int index)
{
  return(string[index]);
}


int lo_test_sort(void)
{
  char string_index();
  char *string;
  wn_sll list,el;

  wn_gpmake("no_free");
  wn_gplabel("main group");

  list = NULL;
  
  wn_sllins(&list, (char *) "foo");
  wn_sllins(&list, (char *) "bar");
  wn_sllins(&list, (char *) "tim");
  wn_sllins(&list, (char *) "mouse");
  wn_sllins(&list, (char *) "ant");
  wn_sllins(&list, (char *) "turkey");

  wn_sort_sll(&list,(int (*)(ptr,ptr))(strcmp));
  /*
  wn_radix_sort_sll(&list,(string_index),(strlen));
  */

  printf("Old example sort\n");
  for(el=list;el!=NULL;el=el->next)
  {
    string = (char *)(el->contents);

    printf("%s\n",string);
  }
  printf("\n");

  return(0);
}
#endif



#if 0
/* Warning: this example DOES not work.  Ignore it. */

local char int_index(int *pint,int index)
{
  return(((char *)pint)[index]);
}


local int int_len(int *pint)
{
  return(sizeof(int));
}


local int pintcmp(int *pi1,int *pi2)
{
  return(wn_intcmp(*pi1,*pi2));
}


void main(void)
{
  int i,r,*r_copy;
  wn_sll list,el;

  wn_gpmake("no_free");
  wn_gplabel("main group");

  list = NULL;
  
  printf("start\n");
  for(i=0;i<5000000;i++)
  {
    r = wn_random_int_between(0,1000000);
   
    r_copy = wn_alloc(sizeof(int));
    *r_copy = r;
    wn_sllins(&list,(ptr)r_copy);
  }

  printf("start sort\n");
  /*
  wn_radix_sort_sll(&list,(int_index),(int_len));
  */
  wn_sort_sll(&list,(int (*)(ptr,ptr))(pintcmp));
  printf("finish sort\n");

  /*
  for(el=list;el!=NULL;el=el->next)
  {
    r_copy = el->contents;

    printf("%d\n",*r_copy);
  }
  */
}
#endif



#define LENGTH(a)	(sizeof(a)/sizeof((a)[0]))

typedef struct lo_person_record_struct		*lo_person_record;

struct lo_person_record_struct {
  const char *first_name;
  const char *last_name;
  int  score;
};


local char lo_first_name_index_char(ptr contents, int index)
{
  return wn_radix_sort_string_index_char(
  /**/			((lo_person_record) contents)->first_name, index);
}


local char lo_last_name_index_char(ptr contents, int index)
{
  return wn_radix_sort_string_index_char(
  /**/			((lo_person_record) contents)->last_name, index);
}


local char lo_score_index_char(ptr contents, int index)
{
  return wn_radix_sort_int_index_char(
  /**/			((lo_person_record) contents)->score, index);
}


local int lo_first_name_string_length(ptr contents)
{
  return wn_radix_sort_string_length(
  /**/			(ptr) ((lo_person_record) contents)->first_name);
}


local int lo_last_name_string_length(ptr contents)
{
  return wn_radix_sort_string_length(
  /**/			(ptr) ((lo_person_record) contents)->last_name);
}


local void lo_test_sorting_person_records(void)
{
  struct lo_person_record_struct records[] = {
  /**/		{ "Frank", "Meyer", -1000000000 },
  /**/		{ "Mike", "Fernandez", 4 },
  /**/		{ "Chris", "Cotterel", 2 },
  /**/		{ "Howard", "Chang", -333333333 },
  /**/		{ "Keith", "Warfield", -2111111111 },
  /**/		{ "Pat", "Walp", 1111111111 } };
  wn_sll list, el;
  int i;

  list = NULL;
  for (i = 0;  i < LENGTH(records);  ++i)
  {
    wn_sllins(&list,(ptr) &records[i]);
  }

  /* sort by first names */
  wn_radix_sort_sll(&list, (char (*)(ptr,int))(lo_first_name_index_char),
  /**/			   (int (*)(ptr))(lo_first_name_string_length));
  (void) printf("Sorted by first names\n");
  for (el = list;  el;  el = el->next)
  {
    (void) printf("  %s %s %11d\n",
    /**/		((lo_person_record) el->contents)->first_name,
    /**/		((lo_person_record) el->contents)->last_name,
    /**/		((lo_person_record) el->contents)->score);
  }
  (void) printf("\n");

  wn_radix_sort_sll(&list, (char (*)(ptr,int))(lo_last_name_index_char),
  /**/			   (int (*)(ptr))(lo_last_name_string_length));
  (void) printf("Sorted by last names\n");
  for (el = list;  el;  el = el->next)
  {
    (void) printf("  %-8s %s %11d\n",
    /**/		((lo_person_record) el->contents)->first_name,
    /**/		((lo_person_record) el->contents)->last_name,
    /**/		((lo_person_record) el->contents)->score);
  }
  (void) printf("\n");

  wn_radix_sort_sll(&list, (char (*)(ptr,int))(lo_score_index_char),
  /**/			   (int (*)(ptr))(wn_radix_sort_int_length));
  (void) printf("Sorted by scores, highest first\n");
  wn_sllrev(&list);	/* highest scores first */
  for (el = list;  el;  el = el->next)
  {
    (void) printf("  %-8s %-9s %11d\n",
    /**/		((lo_person_record) el->contents)->first_name,
    /**/		((lo_person_record) el->contents)->last_name,
    /**/		((lo_person_record) el->contents)->score);
  }
  (void) printf("\n");
} /* lo_test_sorting_person_records */


local void lo_test_quicksort()     /* array sort */
{
  long int array[1000];
  FILE *fp;
  int high_word_minus_one_count = 0;
  int shift_by = 32;	/*     if we shift by a this constant the compiler
  **			** gets upset on 32 bit platforms. */
  int i;

  for (i = 0;  i < LENGTH(array);  ++i)
  {
    array[i] = wn_random_int_between(0,3000) - 1500;	/* choose a number low
    **		enough that there will be some duplicates, make sure there are
    **		some -ve numbers */
  }

  wn_sort_ptrarray((ptr *)array, LENGTH(array), (int(*)(ptr,ptr))(wn_longcmp));

  fp = fopen("tmp.txt","w");
  for(i=0;i<LENGTH(array);i++)
  {
    fprintf(fp, "%ld\n", array[i]);
  }
  printf("\nResults of quicksort example are in ./tmp.txt\n");

  for (i = 0;  i < LENGTH(array);  ++i)
  {
    array[i] = wn_random_int();
    if (sizeof(array[i])  >  sizeof(int))
    {
      array[i] ^= (long int) wn_random_int() << shift_by;
    }
  }

  wn_sort_ptrarray((ptr *)array, LENGTH(array), (int(*)(ptr,ptr))(wn_longcmp));

  fp = fopen("tmp2.txt","w");
  for(i=0;i<LENGTH(array);i++)
  {
    fprintf(fp, "%ld == 0x%lx\n", array[i], array[i]);
  }
  printf("\nResults of large numbers quicksort example are in ./tmp2.txt\n");
  printf("high_word_minus_one_count == %d\n", high_word_minus_one_count);
} /* lo_test_quicksort */


int main(void)
{
  (void) fprintf(stderr, "Sort Examples\n");
  lo_test_sort();
  lo_test_sorting_person_records();
  lo_test_quicksort();

  return 0;
} /* main */
