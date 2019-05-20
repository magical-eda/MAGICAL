/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/


#include "wnlib.h"
#include "wnmem.h"

/*
#include "wnpary.h"
#include "wnaryp.h"
*/
#include "wnary.h"


#if 0
int main(void)
{
  int i,*pi,*pj,sum;
  int *iarray;
  int *nextptr,*finptr;

  wn_gpmake("no_free");

  iarray = NULL;
  nextptr = NULL;
  finptr = NULL;

  /*
  for(i=0;i<20000;++i)
  */
  for(i=0;i<10;++i)
  {
    wn_arraypins_into_group(iarray,nextptr,finptr,i*i,wn_defaultgp(),int);
  }

  sum = 0;

  /*
  for(j=0;j<10000;++j)
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    sum += i;
  } wn_end_for_all_array_entries
  */

  /*
  for(j=0;j<10000;++j)
  for(i=0;i<usedsize;++i)
  {
    sum += iarray[i];
  } 
  */

  /*
  printf("sum = %d\n",sum);
  */

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,2*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,0*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,7*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,6*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,4*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,0*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_arraypdelentry(iarray,nextptr,1*sizeof(int),int);

  printf("array:");
  for(pi=iarray;pi!=nextptr;++pi)
  {
    printf(" %d",*pi);
  } 
  printf("\n");

  wn_gpfree();

  return(0);
}
#endif


#if 1
int main(void)
{
  int i,j,sum;
  int *iarray;
  int usedsize,memsize;

  wn_gpmake("no_free");

  usedsize = 0;
  memsize = 0;

  /*
  for(i=0;i<20000;++i)
  */
  for(i=0;i<10;++i)
  {
    wn_arrayins_into_group(iarray,usedsize,memsize,i*i,wn_defaultgp(),int);
  }

  sum = 0;

  /*
  for(j=0;j<10000;++j)
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    sum += i;
  } wn_end_for_all_array_entries
  */

  /*
  for(j=0;j<10000;++j)
  for(i=0;i<usedsize;++i)
  {
    sum += iarray[i];
  } 
  */

  /*
  printf("sum = %d\n",sum);
  */

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,2);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,0);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,7);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,6);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,4);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,0);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_arraydelindex(iarray,usedsize,1);

  printf("array:");
  wn_for_all_array_entries(iarray,usedsize,i,int)
  {
    printf(" %d",i);
  } wn_end_for_all_array_entries
  printf("\n");

  wn_gpfree();

  return(0);
}
#endif

