/*************************************************************************

  void wn_do_nothing()
  void wn_trap()
  int wn_return_0()
  int wn_return_1()
  int wn_return_neg_1()
  bool wn_return_FALSE()
  bool wn_return_TRUE()

*************************************************************************/
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
#include "wnnop.h"



void wn_do_nothing(void)
{
}


void wn_trap(void)
{
}


int wn_return_0(void)
{
  return(0);
}


int wn_return_1(void)
{
  return(1);
}


int wn_return_neg_1(void)
{
  return(-1);
}


bool wn_return_FALSE(void)
{
  return(FALSE);
}


bool wn_return_TRUE(void)
{
  return(TRUE);
}


