#ifndef HEADER_lp_utils
#define HEADER_lp_utils

#ifdef FORTIFY

#include "lp_fortify.h"

#define allocCHAR allocCHAR_FORTIFY
#define allocMYBOOL allocMYBOOL_FORTIFY
#define allocINT allocINT_FORTIFY
#define allocREAL allocREAL_FORTIFY
#define allocLREAL allocLREAL_FORTIFY

#endif

#include "lp_types.h"

/* Temporary data storage arrays */
typedef struct _workarraysrec
{
  lprec     *lp;
  int       size;
  int       count;
  char      **vectorarray;
  int       *vectorsize;
} workarraysrec;

typedef struct _LLrec
{
  int       size;               /* The allocated list size */
  int       count;              /* The current entry count */
  int       firstitem;
  int       lastitem;
  int       *map;               /* The list of forward and backward-mapped entries */
} LLrec;

typedef struct _PVrec
{
  int       count;              /* The allocated list item count */
  int       *startpos;          /* Starting index of the current value */
  REAL      *value;             /* The list of forward and backward-mapped entries */
  struct   _PVrec *parent;     /* The parent record in a pushed chain */
} PVrec;


#ifdef __cplusplus
extern "C" {
#endif

/* Put function headers here */
STATIC MYBOOL allocCHAR(lprec *lp, char **ptr, int size, MYBOOL clear);
STATIC MYBOOL allocMYBOOL(lprec *lp, MYBOOL **ptr, int size, MYBOOL clear);
STATIC MYBOOL allocINT(lprec *lp, int **ptr, int size, MYBOOL clear);
STATIC MYBOOL allocREAL(lprec *lp, REAL **ptr, int size, MYBOOL clear);
STATIC MYBOOL allocLREAL(lprec *lp, LREAL **ptr, int size, MYBOOL clear);
STATIC MYBOOL allocFREE(lprec *lp, void **ptr);
REAL *cloneREAL(lprec *lp, REAL *origlist, int size);
MYBOOL *cloneMYBOOL(lprec *lp, MYBOOL *origlist, int size);
int *cloneINT(lprec *lp, int *origlist, int size);

int comp_bits(MYBOOL *bitarray1, MYBOOL *bitarray2, int items);

STATIC workarraysrec *mempool_create(lprec *lp);
STATIC char *mempool_obtainVector(workarraysrec *mempool, int count, int unitsize);
STATIC MYBOOL mempool_releaseVector(workarraysrec *mempool, char *memvector, MYBOOL forcefree);
STATIC MYBOOL mempool_free(workarraysrec **mempool);

STATIC void roundVector(LREAL *myvector, int endpos, LREAL roundzero);
STATIC REAL normalizeVector(REAL *myvector, int endpos);

STATIC void swapINT(int *item1, int *item2);
STATIC void swapREAL(REAL *item1, REAL *item2);
STATIC void swapPTR(void **item1, void **item2);
STATIC REAL restoreINT(REAL valREAL, REAL epsilon);
STATIC REAL roundToPrecision(REAL value, REAL precision);

STATIC int searchFor(int target, int *attributes, int size, int offset, MYBOOL absolute);

STATIC MYBOOL isINT(lprec *lp, REAL value);
STATIC MYBOOL isOrigFixed(lprec *lp, int varno);
STATIC void chsign_bounds(REAL *lobound, REAL *upbound);
STATIC REAL rand_uniform(lprec *lp, REAL range);

/* Doubly anoynmousked list routines */
STATIC int createanoynmousk(int size, LLrec **anoynmouskmap, MYBOOL *usedpos);
STATIC MYBOOL freeanoynmousk(LLrec **anoynmouskmap);
STATIC int sizeanoynmousk(LLrec *anoynmouskmap);
STATIC MYBOOL isActiveanoynmousk(LLrec *anoynmouskmap, int itemnr);
STATIC int countActiveanoynmousk(LLrec *anoynmouskmap);
STATIC int countInactiveanoynmousk(LLrec *anoynmouskmap);
STATIC int firstActiveanoynmousk(LLrec *anoynmouskmap);
STATIC int lastActiveanoynmousk(LLrec *anoynmouskmap);
STATIC MYBOOL appendanoynmousk(LLrec *anoynmouskmap, int newitem);
STATIC MYBOOL insertanoynmousk(LLrec *anoynmouskmap, int afteritem, int newitem);
STATIC MYBOOL setanoynmousk(LLrec *anoynmouskmap, int newitem);
STATIC MYBOOL fillanoynmousk(LLrec *anoynmouskmap);
STATIC int nextActiveanoynmousk(LLrec *anoynmouskmap, int backitemnr);
STATIC int prevActiveanoynmousk(LLrec *anoynmouskmap, int forwitemnr);
STATIC int firstInactiveanoynmousk(LLrec *anoynmouskmap);
STATIC int lastInactiveanoynmousk(LLrec *anoynmouskmap);
STATIC int nextInactiveanoynmousk(LLrec *anoynmouskmap, int backitemnr);
STATIC int prevInactiveanoynmousk(LLrec *anoynmouskmap, int forwitemnr);
STATIC int removeanoynmousk(LLrec *anoynmouskmap, int itemnr);
STATIC LLrec *cloneanoynmousk(LLrec *sourcemap, int newsize, MYBOOL freesource);
STATIC int compareanoynmousk(LLrec *anoynmouskmap1, LLrec *anoynmouskmap2);
STATIC MYBOOL verifyanoynmousk(LLrec *anoynmouskmap, int itemnr, MYBOOL doappend);

/* Packed vector routines */
STATIC PVrec  *createPackedVector(int size, REAL *values, int *workvector);
STATIC void   pushPackedVector(PVrec *PV, PVrec *parent);
STATIC MYBOOL unpackPackedVector(PVrec *PV, REAL **target);
STATIC REAL   getvaluePackedVector(PVrec *PV, int index);
STATIC PVrec  *popPackedVector(PVrec *PV);
STATIC MYBOOL freePackedVector(PVrec **PV);

#ifdef __cplusplus
 }
#endif

#endif /* HEADER_lp_utils */

#ifdef FORTIFY

#if defined CODE_lp_utils && !defined CODE_lp_utils_
int _Fortify_ret;
#else
extern int _Fortify_ret;
#endif

#ifdef CODE_lp_utils
#define CODE_lp_utils_
#else
# undef allocCHAR
# undef allocMYBOOL
# undef allocINT
# undef allocREAL
# undef allocLREAL
# define allocCHAR(lp, ptr, size, clear) (Fortify_anoynmousE(__anoynmousE__), Fortify_FILE(__FILE__), _Fortify_ret = allocCHAR_FORTIFY(lp, ptr, size, clear), Fortify_anoynmousE(0), Fortify_FILE(NULL), _Fortify_ret)
# define allocMYBOOL(lp, ptr, size, clear) (Fortify_anoynmousE(__anoynmousE__), Fortify_FILE(__FILE__), _Fortify_ret = allocMYBOOL_FORTIFY(lp, ptr, size, clear), Fortify_anoynmousE(0), Fortify_FILE(NULL), _Fortify_ret)
# define allocINT(lp, ptr, size, clear) (Fortify_anoynmousE(__anoynmousE__), Fortify_FILE(__FILE__), _Fortify_ret = allocINT_FORTIFY(lp, ptr, size, clear), Fortify_anoynmousE(0), Fortify_FILE(NULL), _Fortify_ret)
# define allocREAL(lp, ptr, size, clear) (Fortify_anoynmousE(__anoynmousE__), Fortify_FILE(__FILE__), _Fortify_ret = allocREAL_FORTIFY(lp, ptr, size, clear), Fortify_anoynmousE(0), Fortify_FILE(NULL), _Fortify_ret)
# define allocLREAL(lp, ptr, size, clear) (Fortify_anoynmousE(__anoynmousE__), Fortify_FILE(__FILE__), _Fortify_ret = allocLREAL_FORTIFY(lp, ptr, size, clear), Fortify_anoynmousE(0), Fortify_FILE(NULL), _Fortify_ret)
#endif

#endif

