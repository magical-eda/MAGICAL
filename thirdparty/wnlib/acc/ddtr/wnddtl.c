/**************************************************************************

wn_mkintddtree(&tree,num_dimensions)
wn_mkdoubleddtree(&tree,num_dimensions)

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
#include "wnasrt.h"
#include "wnmem.h"
#include "wnnop.h"
#include "wncmp.h"
#include "wncpy.h"

#include "wnddtr.h"
#include "wnddtl.h"



void wn_mkintddtree
(
  wn_ddtree *ptree,
  int num_dimensions
)
{
  compare_keys_func_type *compare_keys_func;
  alloc_copy_key_func_type *alloc_copy_key_func;
  free_key_func_type *free_key_func;
  int i;

  /* this code is deprecated and will not work on 64 bit platforms */
  wn_assert(sizeof(int) == sizeof(ptr));

  compare_keys_func = (compare_keys_func_type *)
              wn_zalloc(num_dimensions*sizeof(compare_keys_func_type));
  alloc_copy_key_func = (alloc_copy_key_func_type *)
	    wn_zalloc(num_dimensions*sizeof(alloc_copy_key_func_type));
  free_key_func = (free_key_func_type *) 
            wn_zalloc(num_dimensions*sizeof(free_key_func_type));

  for(i=0;i<num_dimensions;++i)
  {
    compare_keys_func[i] = (compare_keys_func_type)&wn_intcmp;
    alloc_copy_key_func[i] = (alloc_copy_key_func_type)&wn_intcpy;
    free_key_func[i] = (free_key_func_type)&wn_do_nothing;
  }

  wn_mkddtree(ptree,num_dimensions,
	      compare_keys_func,alloc_copy_key_func,free_key_func);

  wn_free((ptr)compare_keys_func);
  wn_free((ptr)alloc_copy_key_func);
  wn_free((ptr)free_key_func);
}



void wn_mkdoubleddtree
(
  wn_ddtree *ptree,
  int num_dimensions
)
{
  compare_keys_func_type *compare_keys_func;
  alloc_copy_key_func_type *alloc_copy_key_func;
  free_key_func_type *free_key_func;
  int i;

  compare_keys_func = (compare_keys_func_type *)
              wn_zalloc(num_dimensions*sizeof(compare_keys_func_type));
  alloc_copy_key_func = (alloc_copy_key_func_type *)
	    wn_zalloc(num_dimensions*sizeof(alloc_copy_key_func_type));
  free_key_func = (free_key_func_type *) 
            wn_zalloc(num_dimensions*sizeof(free_key_func_type));

  for(i=0;i<num_dimensions;++i)
  {
    compare_keys_func[i] = (compare_keys_func_type)&wn_pdoublecmp;
    alloc_copy_key_func[i] = (alloc_copy_key_func_type)&wn_pdoublecpy;
    free_key_func[i] = (free_key_func_type)&wn_free;
  }

  wn_mkddtree(ptree,num_dimensions,
	      compare_keys_func,alloc_copy_key_func,free_key_func);

  wn_free((ptr)compare_keys_func);
  wn_free((ptr)alloc_copy_key_func);
  wn_free((ptr)free_key_func);
}

