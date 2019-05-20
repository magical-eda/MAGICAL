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
#include "wnsll.h"
#include "wnstr.h"

#include "wnhtbl.h"



#if 0
void main(void)
{
  bool success;
  wn_htab table;
  int i,j;
  ptr dummy;

  wn_gpmake("no_free");

  wn_mkinthtab(&table);

  for(i=0;i<25000;++i)
  {
    success = wn_hins((ptr)NULL,table,(ptr)i);
  }

  wn_hanalyze(table);

  for(j=0;j<1000;++j)
  {
    for(i=0;i<25000;++i)
    {
      success = wn_hget(&dummy,table,(ptr)&i);
    }
  }

  wn_gpfree();
}
#endif


#if 1
int main(void)
{
  char string[100];
  char *string_copy;
  bool success;
  wn_htab table;
  wn_sll list,el;
  int i;
  ptr dummy;

  wn_gpmake("no_free");

#if 0
  for(i=0;i<50000000;++i)
  {
    wn_memhash((ptr)&i,sizeof(int));
  }
#endif

#if 1
  list = NULL;
  wn_mkstrhtab(&table);

  while(!feof(stdin))
  {
    scanf("%s",string);
    /*
    printf("%s\n",string);
    */
    wn_stracpy(&string_copy,string);
    wn_sllins(&list,string_copy);
    success = wn_hins((ptr)NULL,table,string);
    /*
    */
  }

  wn_hanalyze(table);

  for(i=0;i<500;++i)
  /*
  for(i=0;i<10000;++i)
  */
  {
    for(el=list;el!=NULL;el=el->next)
    {
      string_copy = (char *)(el->contents);
      success = wn_hget(&dummy,table,string_copy);
      /*
      (void)strlen(string_copy);
      (void)wn_strhash(string_copy);
      */
    }
  }
#endif

  wn_gpfree();

  return(0);
}
#endif


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


#if 0
void main(void)
{
  int hash;
  int i;

  hash = 1;

  for(i=0;;++i)
  {
    wn_cummodhash(&hash,0,1);
    
    /*
    printf("i=%d,hash=%d\n",i,hash);
    */
    if(hash == 1)
    {
      printf("i=%d,hash=%d\n",i,hash);
      printf("cycle.\n");
      break;
    }
  }
}
#endif


#if 0
main()
{
  wn_htab table;
  int i,j,num;
  bool success;
  ptr tmp;  /* mfu 05/16/01.  64-bit porting change. */

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
    success = wn_hins((ptr)(i+1),table,(ptr)i);
    wn_assert(success);
  }

  wn_hverify(table);

  /*
  for(j=0;j<50;++j)
  */
  for(i=0;i<10000;++i)
  {
    success = wn_hget(&tmp,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(success);
  }

  wn_hverify(table);

  for(i=0;i<5000;++i)
  {
    success = wn_hdel(table,(ptr)i);
    wn_assert(success);
  }

  wn_hverify(table);

  for(i=0;i<5000;++i)
  {
    success = wn_hdel(table,(ptr)i);
    wn_assert(!(success));
  }

  wn_hverify(table);

  for(i=0;i<5000;++i)
  {
    success = wn_hget(&tmp,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(!(success));
  }

  for(i=5000;i<10000;++i)
  {
    success = wn_hget(&tmp,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(success);
    wn_assert(num == i+1);
  }

  for(i=2500;i<7500;++i)
  {
    success = wn_hfins((ptr)i,table,(ptr)i);
    wn_assert(success);
  }

  for(i=2500;i<7500;++i)
  {
    success = wn_hget(&ptr,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(success);
    wn_assert(num == i);
  }

  for(i=0;i<2500;++i)
  {
    success = wn_hget(&tmp,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(!(success));
  }

  for(i=7500;i<10000;++i)
  {
    success = wn_hget(&tmp,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(success);
    wn_assert(num == i+1);
  }

  wn_hverify(table);

  for(i=2500;i<10000;++i)
  {
    success = wn_hdel(table,(ptr)i);
    wn_assert(success);
  }

  wn_hverify(table);

  for(i=0;i<10000;++i)
  {
    success = wn_hget(&tmp,table,(ptr)i);
    num = (int)((long)tmp);
    wn_assert(!(success));
  }

  printf("success!!!\n");
}
#endif


