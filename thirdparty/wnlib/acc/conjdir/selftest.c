/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

AUTHOR:

  Bill Chapman

****************************************************************************/

/*   The idea of this selftest is you have an N x N matrix of nodes to be
** placed.  The optimal placement is an N x N grid, with each node being
** connected to its 4 neighbors.  Nodes on the edge are connected to points
** on the walls.
**
**				** y ==  3.0 **
**
**	0	1	2	** y ==  2.0 **
**
**	3	4	5	** y ==  1.0 **
**
**	6	7	8	** y ==  0.0 **
**
**				** y == -1.0 **
**
**    X values are stored from 0 .. (N**2-1) in the array of doubles, Y
**  values are stored from N .. (2*(N**2)-1).
**    The energy in a connection between 2 nodes is determined by 1
**  dimensional hooke's law.
**    N defaults to 10.  I am showing the 3 x 3 case here for simplicity. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnsqr.h"
#include "wnabs.h"
#include "wnmem.h"
#include "wnrndd.h"
#include "wnconj.h"

#define LO_ALMOST_EQUAL(_a, _b)  wn_assert(wn_abs((_a)-(_b)) < 0.0001)

local int lo_dim;	/* there are lo_dim * lo_dim nodes */
local int lo_dim_square;

local double lo_left_wall_x,  lo_bottom_wall_y;
local double lo_right_wall_x, lo_top_wall_y;

/* this should really be in wnlib/low */
local bool lo_getenv_bool(char *str) {
  char *getenv_str = getenv(str);

  /* note the variable not being set at all is considered FALSE */

  if (!getenv_str) return FALSE;
  if (!strcmp(getenv_str, "t")) return TRUE;
  if (!strcmp(getenv_str, "f")) return FALSE;
  if (!strcmp(getenv_str, "T")) return TRUE;
  if (!strcmp(getenv_str, "F")) return FALSE;
  if (!strcmp(getenv_str, "true")) return TRUE;
  if (!strcmp(getenv_str, "false")) return FALSE;
  if (!strcmp(getenv_str, "TRUE")) return TRUE;
  if (!strcmp(getenv_str, "FALSE")) return FALSE;
  if (!strcmp(getenv_str, "1")) return TRUE;
  if (!strcmp(getenv_str, "0")) return FALSE;

  /* note set to "" is considered ambiguous */

  fprintf(stderr, "lo_getenv_bool: value specified for boolean environment\n");
  fprintf(stderr, "    variable %s was \"%s\": ambiguous,\n",
  /**/							str, getenv_str);
  fprintf(stderr, "    try setting it to \"t\" or \"f\".\n");

  exit(1);

  return FALSE;
} /* lo_getenv_bool */


local long lo_round(double d) {
  return (long) (d + 0.5);
} /* lo_round */


local double lo_node_cost(int node_number, double *coords) {
  double *x_coords = coords, *y_coords = coords + lo_dim_square;
  double ret = 0;

  wn_assert((unsigned) lo_dim_square > (unsigned) node_number);

  /*     we will just add hook's law spring energy from the nodes to the
  ** left and the right, except if the node is next to the right or bottom
  ** walls, in which case we also add the distance to those walls */

  /* distance to the neighbor to our left */
  if (!(node_number % lo_dim)) {
    /* we are connected to the left wall */
    ret += wn_square(x_coords[node_number]    - lo_left_wall_x);
  } else {
    ret += wn_square(x_coords[node_number]    - x_coords[node_number-1]);
  }

  /* distance to the neighbor to our right - only counted if it's the wall */
  if (!((node_number+1) % lo_dim)) {
    ret += wn_square(lo_right_wall_x          - x_coords[node_number]);
  }

  /* distance to the neighbor above us */
  if (node_number < lo_dim) {
    ret += wn_square(lo_top_wall_y                - y_coords[node_number]);
  } else {
    ret += wn_square(y_coords[node_number-lo_dim] - y_coords[node_number]);
  }

  /* distance to the neighbor below us - only counted if it's the wall */
  if (node_number >= (lo_dim-1)*lo_dim) {
    ret += wn_square(y_coords[node_number]    - lo_bottom_wall_y);
  }

  return ret;
} /* lo_node_cost */


/*     this is really local, but EXTERN to shut up the compiler when we
** pass the ptr */
EXTERN double lo_conjdir_selftest_vector_cost(double *coords) {
  double ret = 0;
  int i;

  for (i = lo_dim_square - 1;  i >= 0;  --i) {
    ret += lo_node_cost(i, coords);
  }

  return ret;
} /* lo_conjdir_selftest_vector_cost */


local double lo_node_x_gradient(int node_number, double *coords) {
  double *x_coords = coords;
  /* double *y_coords = coords + lo_dim_square; */
  double ret = 0;

  wn_assert((unsigned) node_number < (unsigned) lo_dim_square);

  /*     we will just add hook's law spring energy from the nodes to the
  ** left and the right, except if the node is next to the right or bottom
  ** walls, in which case we also add the distance to those walls */

  /* distance to the neighbor to our left */
  if (!(node_number % lo_dim)) {
    /* we are connected to the left wall */
    ret +=  2 * (x_coords[node_number]   - lo_left_wall_x);
  } else {
    ret +=  2 * (x_coords[node_number]   - x_coords[node_number-1]);
  }

  /* distance to the neighbor to our right */
  if (!((node_number+1) % lo_dim)) {
    ret += -2 * (lo_right_wall_x            - x_coords[node_number]);
  } else {
    ret += -2 * (x_coords[node_number+1] - x_coords[node_number]);
  }

  return ret;
} /* lo_node_x_gradient */


local double lo_node_y_gradient(int node_number, double *coords) {
  double ret = 0;
  /* double *x_coords = coords; */
  double *y_coords = coords + lo_dim_square;

  wn_assert((unsigned) node_number < (unsigned) lo_dim_square);

  /* distance to the neighbor above us */
  if (node_number < lo_dim) {
    ret += -2 * (lo_top_wall_y                - y_coords[node_number]);
  } else {
    ret += -2 * (y_coords[node_number-lo_dim] - y_coords[node_number]);
  }

  /* distance to the neighbor below us */
  if (node_number >= (lo_dim-1)*lo_dim) {
    ret +=  2 * (y_coords[node_number] - lo_bottom_wall_y);
  } else {
    ret +=  2 * (y_coords[node_number] - y_coords[node_number+lo_dim]);
  }

  return ret;
} /* lo_node_y_gradient */


EXTERN void lo_conjdir_selftest_vect_gradient(double *gradient,
/**/							double *coords) {
  int i;
  double *x_gradient = gradient, *y_gradient = &gradient[lo_dim_square];

  for (i = lo_dim_square-1;  i >= 0;  --i) {
    x_gradient[i] = lo_node_x_gradient(i, coords);
    y_gradient[i] = lo_node_y_gradient(i, coords);
  }

  return;
} /* lo_conjdir_selftest_vect_gradient */


void lo_check_result(double *coords) {
  double *x_coords = coords, *y_coords = coords + lo_dim_square;
  int row, column, k;

  for (row = 0;  row < lo_dim;  ++row) {
    for (column = 0;  column < lo_dim;  ++column) {
      k = row*lo_dim + column;

      LO_ALMOST_EQUAL(column,       x_coords[k]);
      LO_ALMOST_EQUAL(lo_dim-row-1, y_coords[k]);
    }
  }
} /* lo_check_result */


local void lo_display_coords(double *coords) {
  double *x_coords = coords, *y_coords = coords + lo_dim_square;
  int row, col, k;

  if (lo_dim <= 10) {
    for (row = 0;  row < lo_dim;  ++row) {
      for (col = 0;  col < lo_dim;  ++col) {
	k = row * lo_dim  +  col;

	printf("(%ld,%ld)%s", lo_round(x_coords[k]), lo_round(y_coords[k]),
	/**/				col+1 == lo_dim ? "\n" : "");
      }
    }
  }

  printf("\n");

  for (row = 0;  row < lo_dim;  ++row) {
    for (col = 0;  col < lo_dim;  ++col) {
      k = row*lo_dim + col;

      printf("node %d: (%23.20f, %23.20f)\n", k,
      /**/				x_coords[k], y_coords[k]);
    }
  }
} /* lo_display_coords */


local void lo_randomize_coords(double *coords) {
  int i;

  for (i = 2*lo_dim_square-1;  i >= 0;  --i) {
    coords[i] = wn_flat_distribution() * (lo_dim-1);
  }
} /* lo_randomize_coords */


int main(int argc, char **argv) {
  double *coords, *x_coords, *y_coords;
  int code;
  double val_min;
  bool traces	      =  lo_getenv_bool("WN_CONJDIR_SELFTEST_TRACES");
  bool skip_direction =  lo_getenv_bool("WN_CONJDIR_SELFTEST_SKIP_DIRECTION");
  bool randomize      = !lo_getenv_bool("WN_CONJDIR_SELFTEST_DONT_RANDOMIZE");
  int row, col, k, sts;

  fprintf(stderr, "testing conjdir...\n");

  if (1 < argc) {
    wn_assert(2 == argc);

    sts = sscanf(argv[1], "%d", &lo_dim);
    wn_assert(1 == sts);
    wn_assert(1 <= lo_dim  &&  10000 >= lo_dim);
  } else {
    lo_dim = 10;
  }
  lo_dim_square = wn_square(lo_dim);

  lo_left_wall_x  = lo_bottom_wall_y = -1;
  lo_right_wall_x = lo_top_wall_y = lo_dim;

  coords = (double *) wn_zalloc(lo_dim_square * 2 * sizeof(double));
  x_coords = coords;  y_coords = coords + lo_dim_square;

  if (!skip_direction) {
    if (randomize) {
      lo_randomize_coords(coords);
    }

    wn_conj_direction_method(&code, &val_min, coords, NULL,
    /**/	lo_dim_square*2, &lo_conjdir_selftest_vector_cost, WN_IHUGE);

    if (traces) {
      lo_display_coords(coords);
      printf("Return code = %d, val_min = %g\n", code, val_min);
    }
    wn_assert(WN_SUCCESS == code);
    lo_check_result(coords);
  }

  if (traces) {
    printf("\nNow the gradient method\n\n");
  }
  if (randomize) {
    lo_randomize_coords(coords);
  } else {
    for (row = 0;  row < lo_dim;  ++row) {
      for (col = 0;  col < lo_dim;  ++col) {
	k = row*lo_dim + col;

	x_coords[k] =
	y_coords[k] = 0.0;
      }
    }
  }
  code = 10;  val_min = -48;
  wn_conj_gradient_method(&code, &val_min, coords,
  /**/			lo_dim_square*2, &lo_conjdir_selftest_vector_cost,
  /**/				&lo_conjdir_selftest_vect_gradient, WN_IHUGE);

  if (traces) {
    lo_display_coords(coords);

    printf("Return code = %d, val_min = %g\n", code, val_min);
  }

  wn_assert(WN_SUCCESS == code);
  lo_check_result(coords);

  fprintf(stderr, "  ok!!!!!!\n");

  return 0;
} /* main */
