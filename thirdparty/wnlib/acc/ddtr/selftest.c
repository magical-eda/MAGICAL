/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/


#include <stdlib.h>
#include <stdio.h>

#include "wnlib.h"
#include "wnargp.h"
#include "wnasrt.h"
#include "wnddtr.h"
#include "wnddtl.h"

#define LENGTH(array)  ((int) ((sizeof(array)/sizeof((array)[0]))))

local int points[][2] = 
/**/		    { { 2,  2},  { 1,  3},  { 2,  3},  { 2, -1},
/**/		      { 5,  4},  { 4, 10},  {12,  3},  { 9,  9},
/**/		      { 7, 15},  {23,  9},  { 5,  7},  {-3, -5},
/**/		      {-8,  5},  {17, 14},  { 0,  4},  { 7,  0},
/**/		      {10,  7},  {10, -4},  {17,  3},  {10, 15} };

local bool found[LENGTH(points)];	/* was point i found in the
**					** current call to wn_ddact */

local bool expected_inclusive[] =
/**/		    {     TRUE,      TRUE,      TRUE,     FALSE,
/**/		          TRUE,      TRUE,     FALSE,      TRUE,
/**/		         FALSE,     FALSE,      TRUE,     FALSE,
/**/		         FALSE,     FALSE,      TRUE,      TRUE,
/**/			  TRUE,     FALSE,     FALSE,     FALSE};

local bool expected_exclusive[] =
/**/		    {     TRUE,      TRUE,      TRUE,     FALSE,
/**/		          TRUE,     FALSE,     FALSE,      TRUE,
/**/		         FALSE,     FALSE,      TRUE,     FALSE,
/**/		         FALSE,     FALSE,     FALSE,     FALSE,
/**/			 FALSE,     FALSE,     FALSE,     FALSE};

local bool expected_x_inclusive[] =
/**/		    {     TRUE,      TRUE,      TRUE,     FALSE,
/**/		          TRUE,     FALSE,     FALSE,      TRUE,
/**/		         FALSE,     FALSE,      TRUE,     FALSE,
/**/		         FALSE,     FALSE,      TRUE,     FALSE,
/**/			  TRUE,     FALSE,     FALSE,     FALSE};

local double dpoints[LENGTH(points)][2];

#if 0
/* int ddtrees are deprecated */
void action(wn_ddhandle ddhandle)
{
  int x, y;
  int i;

  x = ((int *) ddhandle->key)[0];
  y = ((int *) ddhandle->key)[1];

  /* find the point in points */
  for (i = 0;  i < LENGTH(points);  ++i)
  {
    if (x == points[i][0]  &&  y == points[i][1]) {
      wn_assert(!found[i]);	/* no duplicates in this test case */
      found[i] = TRUE;
      return;
  } }

  wn_assert_notreached();	/* point not in points */
}


void local dd_test_int()
{
  wn_ddtree ddtree;

  int ll_point[2] = { 0,  0};
  int ur_point[2] = {10, 10};
  wn_ddhandle ddhandle;
  int inc_compare[] = { WN_DD_INCLUSIVE, WN_DD_INCLUSIVE };
  int exc_compare[] = { WN_DD_EXCLUSIVE, WN_DD_EXCLUSIVE };
  int unb_compare[] = { WN_DD_UNBOUNDED, WN_DD_UNBOUNDED };
  int x_inc_compare[] =
  /**/		      { WN_DD_INCLUSIVE, WN_DD_EXCLUSIVE };
  int i;

  fprintf(stderr,"testing int dd trees...\n");

  wn_mkintddtree(&ddtree, 2);

  /* put points into datastructure */
  for (i = 0;  i < LENGTH(points);  ++i)
  {
    wn_ddins(&ddhandle, ddtree, (ptr *) &points[i]);
  }

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  /* Inclusive points */

  wn_ddact(ddtree, &action, (ptr *) &ll_point, inc_compare,
  /**/			    (ptr *) &ur_point, inc_compare);

  /* check found is now identical with expected_inclusive */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i] == expected_inclusive[i]);
  }

  /* Exclusive points */

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  wn_ddact(ddtree, &action, (ptr *) &ll_point, exc_compare,
  /**/			    (ptr *) &ur_point, exc_compare);

  /* check found is now identical with expected_exclusive */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i] == expected_exclusive[i]);
  }

  /* X inclusive points */

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  wn_ddact(ddtree, &action, (ptr *) &ll_point, x_inc_compare,
  /**/			    (ptr *) &ur_point, x_inc_compare);

  /* check found is now identical with expected_x_inclusive */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i] == expected_x_inclusive[i]);
  }

  /* Unbounded points */

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  wn_ddact(ddtree, &action, (ptr *) &ll_point, unb_compare,
  /**/			    (ptr *) &ur_point, unb_compare);

  /* check found is all set */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i]);
  }

  wn_freeddtree(ddtree);

  fprintf(stderr,"  ok!!!!!!\n");
} /* dd_test_int */
#endif /* 0 */


/***************************************************************
**	double test case */

void daction(wn_ddhandle ddhandle)
{
  double x, y;
  int i;

  x = *(((double **) ddhandle->key)[0]);
  y = *(((double **) ddhandle->key)[1]);

  /* find the point in points */
  for (i = 0;  i < LENGTH(points);  ++i)
  {
    if (x == dpoints[i][0]  &&  y == dpoints[i][1]) {
      wn_assert(!found[i]);	/* no duplicates in this test case */
      found[i] = TRUE;
      return;
  } }

  wn_assert_notreached();
}


void local dd_test_double()
{
  wn_ddtree ddtree;
  double ll_point_raw[2] = { 0,  0};
  double ur_point_raw[2] = {10, 10};
  double *ll_point[2], *ur_point[2];
  wn_ddhandle ddhandle;
  int inc_compare[] = { WN_DD_INCLUSIVE, WN_DD_INCLUSIVE };
  int exc_compare[] = { WN_DD_EXCLUSIVE, WN_DD_EXCLUSIVE };
  int unb_compare[] = { WN_DD_UNBOUNDED, WN_DD_UNBOUNDED };
  int x_inc_compare[] =
  /**/		      { WN_DD_INCLUSIVE, WN_DD_EXCLUSIVE };
  double *dptrs[2];
  int i;

  fprintf(stderr,"testing double dd trees...\n");

  /* set up dpoints to be used just like points was */
  for (i = 0;  i < LENGTH(points);  ++i)
  {
    dpoints[i][0] = points[i][0];
    dpoints[i][1] = points[i][1];
  }

  wn_mkdoubleddtree(&ddtree, 2);

  /* put points into datastructure */
  for (i = 0;  i < LENGTH(points);  ++i)
  {
#   if 1
      double x, y;	/* this local copy is to test that wn_ddins really
			  ** does make its own copy of the doubles, it's not
			  ** just relying on the dpoints storage. */

      x = dpoints[i][0];
      y = dpoints[i][1];
      dptrs[0] = &x;
      dptrs[1] = &y;
#   else
      /* former version, also works */

      dptrs[0] = &dpoints[i][0];
      dptrs[1] = &dpoints[i][1];
#   endif

    wn_ddins(&ddhandle, ddtree, (ptr *) dptrs);
  }

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  ll_point[0] = &ll_point_raw[0];
  ll_point[1] = &ll_point_raw[1];
  ur_point[0] = &ur_point_raw[0];
  ur_point[1] = &ur_point_raw[1];

  /* Inclusive points */

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, inc_compare,
  /**/			     (ptr *) &ur_point, inc_compare);

  /* check found is now identical with expected_inclusive */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i] == expected_inclusive[i]);
  }

  /* Exclusive points */

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, exc_compare,
  /**/			    (ptr *) &ur_point, exc_compare);

  /* check found is now identical with expected_inclusive */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i] == expected_exclusive[i]);
  }

  /* X inclusive points */

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, x_inc_compare,
  /**/			    (ptr *) &ur_point, x_inc_compare);

  /* check found is now identical with expected_inclusive */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i] == expected_x_inclusive[i]);
  }

  /* Unbounded points */

  for (i = 0;  i < LENGTH(found);  ++i)
  {
    found[i] = FALSE;
  }

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, unb_compare,
  /**/			    (ptr *) &ur_point, unb_compare);

  /* check found is all set */
  for (i = 0;  i < LENGTH(found);  ++i)
  {
    wn_assert(found[i]);
  }

  wn_freeddtree(ddtree);

  fprintf(stderr,"  ok!!!!!!\n");
} /* dd_test_double */


int main(int argc,char *argv[])
{
  static bool no_double;
  static wn_arg_format arg_format_array[] =
  {
    WN_KEY_BOOL(&no_double,"d"),
    WN_ARG_TERMINATE
  };

  wn_parse_args(argc,argv,arg_format_array);

#if 0
  /* int ddtrees are deprecated */

  if (sizeof(int) != sizeof(ptr))
  {
    fprintf(stderr,"not testing int dd trees, don't work on 64 bit...\n");
  }
  else
  {
    dd_test_int();
  }
#endif

  if (!no_double)
  {
    dd_test_double();
  }

  return(0);
}
