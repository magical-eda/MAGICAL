/**************************************************************************

wn_sort_ptrarray(array,size,pcompare_func)
ptr array[];
int size;
int (*pcompare_func)(p1,p2);   * ptr p1,p2; *

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
#include "wnswap.h"
#include "wnrnd.h"

#include "wnsort.h"



local void sort_2(ptr array[2],int (*pcompare_func)(ptr p1,ptr p2))
{
  if((*pcompare_func)(array[0],array[1]) > 0)
  {
    wn_swap(array[0],array[1],ptr);
  }
}


local void sort_3(ptr array[3],int (*pcompare_func)(ptr p1,ptr p2))
{
  if((*pcompare_func)(array[0],array[1]) > 0)
  {
    wn_swap(array[0],array[1],ptr);
  }

  if((*pcompare_func)(array[1],array[2]) > 0)
  {
    wn_swap(array[1],array[2],ptr);

    if((*pcompare_func)(array[0],array[1]) > 0)
    {
      wn_swap(array[0],array[1],ptr);
    }
  }
}


local void choose_discriminator
(
  ptr *pdiscriminator,
  ptr array[],
  int size,
  int (*pcompare_func)(ptr p1,ptr p2)
)
{
  ptr median_array[3];

  median_array[0] = array[0];
  median_array[2] = array[size-1];
  median_array[1] = array[size/2];

  sort_3(median_array,pcompare_func);

  *pdiscriminator = median_array[1];  /* choose median */
}


local void sort_big(ptr array[],int size,int (*pcompare_func)(ptr p1,ptr p2))
{
  int small_index,big_index,comparison;
  ptr discriminator,pivot;

  choose_discriminator(&discriminator,array,size,pcompare_func);

  small_index = 0;
  big_index = size-1;

  pivot = array[big_index];

  while(small_index <= big_index)
  {
    comparison = (*pcompare_func)(pivot,discriminator);

    if(comparison == 0)
    {
      comparison = wn_random_bit();
    }

    if(comparison <= 0)
    {
      wn_swap(array[small_index],pivot,ptr);

      small_index++;
    }
    else   /* > */
    {
      array[big_index] = pivot;

      big_index--;

      pivot = array[big_index];
    }
  }      

  wn_sort_ptrarray(array,small_index,pcompare_func);
  wn_sort_ptrarray(array+small_index,size-small_index,pcompare_func);
}

EXTERN
void wn_sort_ptrarray(ptr array[],int size,int (*pcompare_func)(ptr p1,ptr p2))
{
  switch(size)
  {
    case(0):  
    case(1):  
    {
      return;
    } /*break;*/
    case(2):  
    {
      sort_2(array,pcompare_func);
    } break;
    case(3):  
    {
      sort_3(array,pcompare_func);
    } break;
    default:
    {
      sort_big(array,size,pcompare_func);
    } break;
  }
}
