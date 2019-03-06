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



#if 0
main()
{
  extern bool wn_streqnc();
  char s1[100],s2[100],s3[100],*s3_copy;
  wn_htab string_table;   /* hash table */

  wn_gpmake("no_free");   /* memory group for all operations below */
  wn_gplabel("main group");

  wn_mkstrhtab(&string_table);  /* make string hash table, using current
                                   memory group */

  while(TRUE)
  {
    printf("command > ");
    scanf("%s",s1);

    if(wn_streqnc(s1,"enter"))   /* enter value */
    {
      printf("enter key: ");
      scanf("%s",s2);
      printf("enter value: ");
      scanf("%s",s3);

      wn_stracpy(&s3_copy,s3);  /* make allocated memory copy of s3 */

      wn_hfins((ptr)s3_copy,string_table,(ptr)s2); /* insert into 
                                                      hash table */ 
    }
    else if(wn_streqnc(s1,"print"))  /* print value */
    {
      printf("enter key: ");
      scanf("%s",s2);

      if(wn_hget((ptr *)&s3_copy,string_table,(ptr)s2))
      {
        printf("value = %s\n",s3_copy);
      }
      else
      {
        printf("key <%s> not found.\n",s2);
      }
    }
    else if(wn_streqnc(s1,"exit"))  /* exit */
    {
      printf("exiting.\n");

      break;
    }
    else
    {
      printf("illegal command.\n");
    }
  }

  wn_gpfree();            /* release all memory */
}
#endif



#if 1
#include "wnbtr.h"

main()
{
  void print_handle();
  wn_btree tree;
  wn_bhandle handle;
  int i,num,count,index,index2;
  
  wn_gpmake("general_free");

  wn_mkintbtree(&tree);
  wn_bverify(tree);

  for(i=0;i<1000;i++)
  {
    /*
    num = wn_random_int_between(0,1000000);
    num = 1;
    */
    num = i;
    wn_bins(&handle,tree,(ptr)num);
  }

  wn_bverify(tree);
  printf("inserts finished\n");

  wn_bact(tree,(print_handle),(ptr)1,WN_B_EQ,(ptr)10,WN_B_GT);
  printf("wnbact finished\n");

  for(i=0;i<1000;i++)
  {
    count = wn_bcount(tree);
    wn_assert(count > 0);

    index = wn_random_int_between(0,count);
    wn_bget_handle_of_index(&handle,tree,index);
    wn_bget_index_of_handle(&index2,tree,handle);
    wn_assert(index == index2);
    wn_bdel(handle,tree);
  }

  wn_bverify(tree);

  count = wn_bcount(tree);
  wn_assert(count == 0);

  wn_bverify(tree);
  printf("deletes finished\n");

  wn_gpfree();
}



local print_handle(handle)

wn_bhandle handle;

{
  printf("handle->key=%d\n",handle->key);
}
#endif



#if 0

#define SIZE  1000000

#include "wnbtr.h"


static int count;

local int count_intcmp(i1,i2)

int i1,i2;

{
  ++count;

  return(wn_intcmp(i1,i2));
}


main()
{
  extern void wn_intcpy();
  extern void wn_do_nothing();
  wn_btree tree;
  wn_bhandle handle;
  int i,num,count_ins,count_del;
  
  wn_gpmake("general_free");

  wn_mkbtree(&tree,(count_intcmp),(wn_intcpy),(wn_do_nothing));

  count = 0;

  wn_seed_random_number_generator(0);

  for(i=0;i<SIZE;++i)
  {
    num = wn_random_int();
    /*
    num = i;
    */
    wn_bins(&handle,tree,(ptr)num);
  }

  printf("inserts finished. compare count = %d\n",count);

#if 0
  count = 0;

  wn_seed_random_number_generator(0);

  for(i=0;i<SIZE;i++)
  {
    num = wn_random_int();
    /*
    num = i;
    */
    wn_bget(&handle,tree,num,WN_B_EQ);
  }

  printf("searches finished. compare count = %d\n",count);
#endif

  count_ins = count_del = 0;

  wn_seed_random_number_generator(1);

  for(i=0;i<SIZE;i++)
  {
    num = wn_random_int();

    count = 0;
    wn_bins(&handle,tree,num);
    count_ins += count;

    count = 0;
    wn_bdel(handle,tree);
    count_del += count;
  }

  printf("trials finished. ins count = %d,delete count = %d\n",
	 count_ins,count_del);

  wn_gpfree();
}

#endif

#if 0

#define SIZE  1000000

void main()
{
  int i,sum,remain,this;

  remain = SIZE;
  sum = 0;

  for(i=0;;++i)
  {
    this = 1<<i;

    if(this >= remain)
    {
      this = remain;
      sum += (i+1)*this;
      printf("sum = %d\n",sum);

      return;
    }
    else
    {
      sum += (i+1)*this;
      remain -= this;
    }
  }
}

#endif


#if 0
main()
{
  extern int wn_strlen();
  char string_index();
  char *string;
  wn_sll list,el;

  wn_gpmake("no_free");
  wn_gplabel("main group");

  list = NULL;
  
  wn_sllins(&list,(ptr)"foo");
  wn_sllins(&list,(ptr)"bar");
  wn_sllins(&list,(ptr)"tim");
  wn_sllins(&list,(ptr)"mouse");
  wn_sllins(&list,(ptr)"ant");
  wn_sllins(&list,(ptr)"turkey");

  wn_radix_sort_sll(&list,(string_index),(wn_strlen));

  for(el=list,wn_loopinit();wn_sllloop(&el,&string);)
  {
    printf("%s\n",string);
  }
}



local char string_index(string,index)

char string[];
int index;

{
  return(string[index]);
}
#endif



#if 0
main()
{
  extern int wn_random_int_between(),pintcmp();
  char int_index();
  int int_len();
  int i,r,*r_copy;
  wn_sll list,el;

  wn_gpmake("no_free");
  wn_gplabel("main group");

  list = NULL;
  
  printf("start\n");
  for(i=0;i<1000000;i++)
  {
    r = wn_random_int_between(0,1000000);
   
    wn_blkacpy((ptr *)&r_copy,(ptr)&r,wn_sizeof(int));
    wn_sllins(&list,(ptr)r_copy);
  }

  printf("start sort\n");
  wn_radix_sort_sll(&list,(int_index),(int_len));
  /*
  wn_sort_sll(&list,(pintcmp));
  */
  printf("finish sort\n");

  /*
  for(el=list,wn_loopinit();wn_sllloop(&el,&r_copy);)
  {
    printf("%d\n",*r_copy);
  }
  */
}



local char int_index(pint,index)

int *pint;

{
  return(((char *)pint)[index]);
}



local int int_len(pint)

int *pint;

{
  return(wn_sizeof(int));
}



local int pintcmp(pi1,pi2)

int *pi1,*pi2;

{
  return(wn_intcmp(*pi1,*pi2));
}



#endif



#if 0
main()
{
  wn_sll list,el;
  int i;

  wn_gpmake("no_free");
  wn_gplabel("main group");

  list = NULL;
  
  printf("start\n");
  for(i=0;i<100000;i++)
  {
    wn_sllins(&list,(ptr)i);
  }

  printf("start randomize sll\n");
  wn_randomize_sll(&list);
  printf("finish randomize\n");

  /*
  for(el=list,wn_loopinit();wn_sllloop(&el,&i);)
  {
    printf("%d\n",i);
  }
  */
}
#endif



#if 0
#include "wnbsr.h"

local float search_number;

main()
{
  int find_direction();
  int code,result;

  printf("enter number:");
  scanf("%f",&search_number);
  
  wn_binary_search_int(&code,&result,-100,100,(find_direction));
  
  switch(code)
  {
    case(WN_EXACT_MATCH):
    {
      printf("code = WN_EXACT_MATCH,result=%d\n",result);
    } break;
    case(WN_NO_EXACT_MATCH):
    {
      printf("code = WN_NO_EXACT_MATCH,result=%d\n",result);
    } break;
    case(WN_OUT_OF_RANGE):
    {
      printf("code = WN_OUT_OF_RANGE,result=%d\n",result);
    } break;
    default:
      wn_crash();
  }        
}
  
local int find_direction(num)
int num;
{
  extern int wndoublecmp();

  return(wn_doublecmp((double)search_number,(double)num));
}
#endif



#if 0
main()
{
  int size,seed,*perm1,*perm2,*perm3;
   
  wn_gpmake("no_free");

  printf("enter size, seed: ");
  scanf("%d,%d",&size,&seed);

  wn_seed_random_number_generator(seed);

  perm1 = (int *)wn_zalloc(size*wn_sizeof(int));
  perm2 = (int *)wn_zalloc(size*wn_sizeof(int));
  perm3 = (int *)wn_zalloc(size*wn_sizeof(int));

  printf("start:\n");
  wn_random_permutation(perm1,size);

  printf("verify:\n");
  wn_assert(wn_is_valid_permutation(perm1,size));

  printf("base:\n");
  print_permutation(perm1,size);

  wn_invert_permutation(perm2,perm1,size);

  printf("inverse:\n");
  print_permutation(perm2,size);

  wn_permute_permutation(perm3,perm2,perm1,size);

  printf("product:\n");
  print_permutation(perm3,size);

  wn_gpfree();
}



local print_permutation(permutation,size)

int permutation[],size;

{
  int i;

  printf("[ ");

  for(i=0;i<size;i++)
  {
    printf("%d ",permutation[i]);
  }

  printf("]\n");
}
#endif



#if 0

#include "wnary.h"
#include "wninr.h"

main()
{
  extern double wn_normal_distribution(),wn_floor();
  int i;
  int array[10];
  double num;

  wn_seed_random_number_generator(0);

  wn_arrayfill(array,10,0,int);

  for(i=0;i<10000;i++)
  {
    num = wn_normal_distribution();

    if(wn_inrange(0.0,<,num,<,.01))
    {
      array[(int)wn_floor(num/.001)]++; 
    }
  }

  for(i=0;i<10;i++)
  {
    printf("array[%d]=%d\n",i,array[i]);
  }
}

#endif 



#if 0
main()
{
  extern double wn_normal_distribution();
  int i,num;

  for(i=0;i<1000000;++i)
  {
    /*
    num = wn_random_int_between(0,200);
    num = wn_random_n_bits(1);
    num = wn_random_mod_int(200);
    num = wn_random_bit();
    num = random()%200;
    num = random()^1;
    */

    num = wn_random_bit();

    /*

    printf("%d\n",num);
    */
  }
}
#endif



#if 0
main()
{
  wn_htab table;
  int i,j,num;
  bool success;

  /*
  for(i=0;i<2000000;++i)
  {
    num = wn_inthash(i);
  }

  return;
  */

  wn_gpmake("no_free");

  wn_mkinthtab(&table);

  for(i=0;i<10000;++i)
  {
    success = wn_hins(i+1,table,i);
    wn_assert(success);
  }

  wn_hverify(table);

  /*
  for(j=0;j<50;++j)
  */
  for(i=0;i<10000;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(success);
  }

  wn_hverify(table);

  for(i=0;i<5000;++i)
  {
    success = wn_hdel(table,i);
    wn_assert(success);
  }

  wn_hverify(table);

  for(i=0;i<5000;++i)
  {
    success = wn_hdel(table,i);
    wn_assert(not(success));
  }

  wn_hverify(table);

  for(i=0;i<5000;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(not(success));
  }

  for(i=5000;i<10000;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(success);
    wn_assert(num == i+1);
  }

  for(i=2500;i<7500;++i)
  {
    success = wn_hfins(i,table,i);
    wn_assert(success);
  }

  for(i=2500;i<7500;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(success);
    wn_assert(num == i);
  }

  for(i=0;i<2500;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(not(success));
  }

  for(i=7500;i<10000;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(success);
    wn_assert(num == i+1);
  }

  wn_hverify(table);

  for(i=2500;i<10000;++i)
  {
    success = wn_hdel(table,i);
    wn_assert(success);
  }

  wn_hverify(table);

  for(i=0;i<10000;++i)
  {
    success = wn_hget(&num,table,i);
    wn_assert(not(success));
  }

  printf("success!!!\n");
}
#endif



#if 0
main()
{
  wn_listen(50);

  while(TRUE) 
    ;
}
#endif


#if 0
/* chi-square test random number generator */

#define SIZE  (1<<10)
#define MULT  1000 

main()
{
  int counts[SIZE];
  int i,j,n,seed;
  double sum,diff;

  /*
  */
  for(seed=0;seed<10;++seed)
  {
  wn_seed_random_number_generator(seed);

  wn_blkzero(counts,SIZE*sizeof(int));
  
  for(i=0;i<SIZE;++i)
  for(j=0;j<MULT;++j)
  {
    n = wn_random_int();
    /*
    n = i;
    */

    ++(counts[n&(SIZE-1)]);
  }

  sum = 0.0;

  for(i=0;i<SIZE;++i)
  {
    diff = (double)(counts[i]-MULT);
    sum += diff*diff;
  }

  sum = sum/(((double)(SIZE))*((double)MULT));

  printf("sum = %lf\n",sum);
  }
}
#endif

#if 0
main()
{
  printf("%d\n",wn_imod(1,69));
}
#endif

#if 0
main()
{
  int i,n;

  wn_seed_random_number_generator(0);

  for(i=0;i<1000000;++i)
  {
    n = wn_random_int();
    /*
    n = random();
    */
    printf("%x\n",n);
    /*
    */
  }
}
#endif


#if 0
main()
{
  extern double wn_fmod();
  double n1,n2,n3;

  n1 = -10.67;
  n2 = -0.5;

  n3 = wn_fmod(n1,n2);

  printf("%lf mod %lf = %lf\n",n1,n2,n3);
}
#endif


#if 0
main()
{
  int n1,n2,n3;

  n1 = -70008;
  n2 = -5;

  n3 = wn_imod(n1,n2);

  printf("%d mod %d = %d\n",n1,n2,n3);
}
#endif
