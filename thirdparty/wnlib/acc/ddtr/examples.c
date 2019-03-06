/****************************************************************************

/****************************************************************************
#include <stdlib.h>

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
#include <stdlib.h>

#include "wnlib.h"
#include "wnargp.h"
#include "wnddtr.h"
#include "wnddtl.h"

#define LENGTH(array)  (sizeof(array)/sizeof((array)[0]))

void action(wn_ddhandle ddhandle)
{
  printf("(%d, %d), ", ((int *) ddhandle->key)[0], ((int *) ddhandle->key)[1]);
}


int dd_example_int()
{
  wn_ddtree ddtree;
  int points[][2] = { { 2,  2},  { 1,  3},  { 2,  3},  { 2, -1},
  /**/		      { 5,  4},  {11, 10},  {12,  3},  { 9,  9},
  /**/		      { 7, 15},  {23,  9},  { 5,  7},  {-3, -5},
  /**/		      {-8,  5},  {17, 14},  { 0,  4},  { 7,  0} };
  int ll_point[2] = { 0,  0};
  int ur_point[2] = {10, 10};
  wn_ddhandle ddhandle;
  int inc_compare[] = { WN_DD_INCLUSIVE, WN_DD_INCLUSIVE };
  int exc_compare[] = { WN_DD_EXCLUSIVE, WN_DD_EXCLUSIVE };
  int unb_compare[] = { WN_DD_UNBOUNDED, WN_DD_UNBOUNDED };
  int i;

  printf("array contents...\n");

  for (i = 0;  i < sizeof(points) / sizeof(points[0]);  ++i)
  {
    printf("(%d, %d)\n", points[i][0], points[i][1]);
  }

  printf("\n");

  printf("ddtr example...\n");

  wn_mkintddtree(&ddtree, 2);

  /* put points into datastructure */
  for (i = 0;  i < sizeof(points) / sizeof(points[0]);  ++i)
  {
    wn_ddins(&ddhandle, ddtree, (ptr *) &points[i]);
  }

  printf("\n");

  printf("Inclusive points:\n");

  wn_ddact(ddtree, &action, (ptr *) &ll_point, inc_compare,
  /**/			    (ptr *) &ur_point, inc_compare);

  printf("\nExclusive points:\n");

  wn_ddact(ddtree, &action, (ptr *) &ll_point, exc_compare,
  /**/			    (ptr *) &ur_point, exc_compare);

  printf("\nUnbounded points:\n");

  wn_ddact(ddtree, &action, (ptr *) &ll_point, unb_compare,
  /**/			    (ptr *) &ur_point, unb_compare);

  wn_freeddtree(ddtree);

  return(0);
} /* dd_example_int */


void daction(wn_ddhandle ddhandle)
{
  double x, y;

  x = *(((double **) ddhandle->key)[0]);
  y = *(((double **) ddhandle->key)[1]);

  printf("(%.1f, %.1f), ", x, y);
} /* daction */


void local dd_example_double()
{
  wn_ddtree ddtree;
  double ll_point_raw[2] = { 0,  0};
  double ur_point_raw[2] = {10, 10};
  double *ll_point[2], *ur_point[2];
  wn_ddhandle ddhandle;
  double dpoints[][2] =
  /**/		    { { 2,  2},  { 1,  3},  { 2,  3},  { 2, -1},
  /**/		      { 5,  4},  {11, 10},  {12,  3},  { 9,  9},
  /**/		      { 7, 15},  {23,  9},  { 5,  7},  {-3, -5},
  /**/		      {-8,  5},  {17, 14},  { 0,  4},  { 7,  0} };
  int inc_compare[] = { WN_DD_INCLUSIVE, WN_DD_INCLUSIVE };
  int exc_compare[] = { WN_DD_EXCLUSIVE, WN_DD_EXCLUSIVE };
  int unb_compare[] = { WN_DD_UNBOUNDED, WN_DD_UNBOUNDED };
  int x_inc_compare[] =
  /**/		      { WN_DD_INCLUSIVE, WN_DD_EXCLUSIVE };
  double *dptrs[2];
  int i;

  printf("double dd trees example...\n");

  wn_mkdoubleddtree(&ddtree, 2);

  /* put points into datastructure */
  for (i = 0;  i < LENGTH(dpoints);  ++i)
  {
    dptrs[0] = &dpoints[i][0];
    dptrs[1] = &dpoints[i][1];

    wn_ddins(&ddhandle, ddtree, (ptr *) dptrs);
  }

  ll_point[0] = &ll_point_raw[0];
  ll_point[1] = &ll_point_raw[1];
  ur_point[0] = &ur_point_raw[0];
  ur_point[1] = &ur_point_raw[1];

  printf("Inclusive points\n");

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, inc_compare,
  /**/			     (ptr *) &ur_point, inc_compare);

  printf("\nExclusive points\n");

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, exc_compare,
  /**/			     (ptr *) &ur_point, exc_compare);

  printf("\nX inclusive points\n");

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, x_inc_compare,
  /**/			     (ptr *) &ur_point, x_inc_compare);

  printf("\nUnbounded points\n");

  wn_ddact(ddtree, &daction, (ptr *) &ll_point, unb_compare,
  /**/			     (ptr *) &ur_point, unb_compare);

  printf("\n\nExample done.");

  wn_freeddtree(ddtree);
} /* dd_example_double */


int main(int argc,char *argv[])
{
  static bool no_double;
  static wn_arg_format arg_format_array[] =
  {
    WN_KEY_BOOL(&no_double,"n"),
    WN_ARG_TERMINATE
  };

  wn_parse_args(argc,argv,arg_format_array);

  dd_example_int();

  printf("\n\n");

  if (!no_double)
  {
    dd_example_double();
  }

  return(0);
} /* main */
