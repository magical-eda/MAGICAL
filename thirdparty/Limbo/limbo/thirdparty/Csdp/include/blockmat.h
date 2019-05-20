/*
  This file contains definitions for the block matrix data structures used
  in CSDP 3.0.  Note that there are an additional set of definitions used
  for sparse constraint matrices.
*/


/*
  Each block is a diagonal block or a matrix
*/

enum blockcat {DIAG, MATRIX, PACKEDMATRIX};

/*
  A block data record contains a pointer to the actual data for the block.
  Note that matrices are stored in Fortran form, while vectors are stored
  using indices 1, 2, ..., n.
*/

union blockdatarec {
  double *vec;
  double *mat;
};

/*
  A block record describes an individual block within a matrix. 
*/

struct blockrec {
  union blockdatarec data;
  enum blockcat blockcategory;
#ifndef NOSHORTS
  unsigned short blocksize;
#else
  int blocksize;
#endif
};

/*
  A block matrix contains an entire matrix in block diagonal form.
 */

struct blockmatrix {
  int nblocks;
  struct blockrec *blocks;
};

/*
   Definition for constraint matrices.
 */

/*
 * There's one of these for each nonzero block in each constraint matrix.
 */
struct sparseblock {
  struct sparseblock *next;
  struct sparseblock *nextbyblock;
  double *entries;
#ifndef NOSHORTS
  unsigned short *iindices;
  unsigned short *jindices;
  int numentries;
  unsigned short blocknum;
  unsigned short blocksize;
  unsigned short constraintnum;
  unsigned short issparse;
#else
  int *iindices;
  int *jindices;
  int numentries;
  int blocknum;
  int blocksize;
  int constraintnum;
  int issparse;
#endif
};


/*
 * A constraint matrix is just an array of pointers to linked lists of
 * the constraint blocks.  
 */

struct constraintmatrix {
  struct sparseblock *blocks;
};




