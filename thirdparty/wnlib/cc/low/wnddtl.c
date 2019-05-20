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
#include "wnary.h"

#include "wnddtr.h"



typedef int (*intfunc)();
typedef void (*voidfunc)();



wn_mkintddtree(ptree,num_dimensions)

wn_ddtree *ptree;
int num_dimensions;

{
  extern int wn_intcmp();
  extern void wn_intcpy();
  extern void wn_do_nothing();
  int (*(*compare_keys_func))(/*key1,key2*/);      /* ptr key1,key2; */
  void (*(*alloc_copy_key_func))(/*&out_key,in_key*/);
						   /* ptr out_key,in_key */
  void (*(*free_key_func))(/*key*/);               /* ptr key; */

  compare_keys_func = (intfunc *)wn_zalloc(num_dimensions*wn_sizeof(intfunc));
  alloc_copy_key_func = (voidfunc *)
			    wn_zalloc(num_dimensions*wn_sizeof(voidfunc));
  free_key_func = (voidfunc *)wn_zalloc(num_dimensions*wn_sizeof(voidfunc));

  wn_arrayfill(compare_keys_func,num_dimensions,(wn_intcmp),
	       intfunc);
  wn_arrayfill(alloc_copy_key_func,num_dimensions,(wn_intcpy),
	       voidfunc);
  wn_arrayfill(free_key_func,num_dimensions,(wn_do_nothing),
	       voidfunc);

  wn_mkddtree(ptree,num_dimensions,
	      compare_keys_func,alloc_copy_key_func,free_key_func);

  wn_free((ptr)compare_keys_func);
  wn_free((ptr)alloc_copy_key_func);
  wn_free((ptr)free_key_func);
}



wn_mkdoubleddtree(ptree,num_dimensions)

wn_ddtree *ptree;
int num_dimensions;

{
  extern int wn_pdoublecmp();
  extern void wn_pdoublecpy();
  extern void wn_free();
  int (*(*compare_keys_func))(/*key1,key2*/);      /* ptr key1,key2; */
  void (*(*alloc_copy_key_func))(/*&out_key,in_key*/);
						   /* ptr out_key,in_key */
  void (*(*free_key_func))(/*key*/);               /* ptr key; */

  compare_keys_func = (intfunc *)wn_zalloc(num_dimensions*wn_sizeof(intfunc));
  alloc_copy_key_func = (voidfunc *)
			    wn_zalloc(num_dimensions*wn_sizeof(voidfunc));
  free_key_func = (voidfunc *)wn_zalloc(num_dimensions*wn_sizeof(voidfunc));

  wn_arrayfill(compare_keys_func,num_dimensions,(wn_pdoublecmp),
	       intfunc);
  wn_arrayfill(alloc_copy_key_func,num_dimensions,(wn_pdoublecpy),
	       voidfunc);
  wn_arrayfill(free_key_func,num_dimensions,(wn_free),
	       voidfunc);

  wn_mkddtree(ptree,num_dimensions,
	      compare_keys_func,alloc_copy_key_func,free_key_func);

  wn_free((ptr)compare_keys_func);
  wn_free((ptr)alloc_copy_key_func);
  wn_free((ptr)free_key_func);
}



