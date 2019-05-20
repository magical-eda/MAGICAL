
	.section	".text",#alloc,#execinstr
	.file	"wnhtab.c"

	.section	".data1",#alloc,#write
	.align	4
.L179:
	.ascii	"wnhtab.c\0"
	.align	4
.L187:
	.ascii	"wnhtab.c\0"
	.align	4
.L198:
	.ascii	"wnhtab.c\0"
	.align	4
.L201:
	.ascii	"wnhtab.c\0"
	.align	4
.L204:
	.ascii	"wnhtab.c\0"
	.align	4
.L207:
	.ascii	"wnhtab.c\0"
	.align	4
.L210:
	.ascii	"wnhtab.c\0"

	.section	".rodata1",#alloc
	.align	4
.L372:
	.ascii	"num_entries = %d,avg depth=%lf,merit=%lf,#collisions=%d\n\0"
	.align	4
.L374:
	.ascii	"num_entries = %d,avg depth=--,merit=--,#collisions=--\n\0"

	.section	".bss",#alloc,#write
	.local	count
	.common	count,4,4

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
/* 000000	     */		.skip	16
!
! SUBROUTINE wn_mkhtab
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_mkhtab
                       wn_mkhtab:
/* 000000	     */		save	%sp,-96,%sp
! FILE wnhtab.c

!    1		      !/**************************************************************************
!    3		      !wn_htab table;
!    5		      !wn_mkhtab(&table,phash_func,pkeys_eq_func,palloc_copy_func,pfree_func)
!    6		      !wn_freehtab(table)
!    8		      !bool wn_hget(&data,table,key)
!    9		      !bool wn_hins(data,table,key)
!   10		      !bool wn_hfins(data,table,key)
!   11		      !bool wn_hdel(table,key)
!   13		      !int wn_hcount(table)
!   14		      !wn_hact(table,paction)      void (*paction)(data,key);
!   16		      !wn_hverify(table)
!   17		      !wn_hdump(table)
!   18		      !wn_hanalyze(table)
!   20		      !**************************************************************************/
!   21		      !/****************************************************************************
!   23		      !COPYRIGHT NOTICE:
!   25		      !  The source code in this file is provided free of charge
!   26		      !  to the author's consulting clients.  It is in the
!   27		      !  public domain and therefore may be used by anybody for
!   28		      !  any purpose.
!   30		      !AUTHOR:
!   32		      !  Will Naylor
!   34		      !****************************************************************************/
!   35		      !#include "wnlib.h"
!   36		      !#include "wnasrt.h"
!   38		      !#include "wnhtab.h"
!   42		      !void wn_mkhtab
!   43		      !(
!   44		      !  wn_htab *ptable,
!   45		      !  int (*phash_func)(ptr key),
!   46		      !  bool (*pkeys_eq_func)(ptr key1,ptr key2),
!   47		      !  void (*palloc_copy_func)(ptr *pkey,ptr key),
!   48		      !  void (*pfree_func)(ptr key)
!   49		      !)
!   50		      !{
!   51		      !  *ptable = (wn_htab)wn_alloc(sizeof(struct wn_htab_struct));

/* 0x0004	  51 */		call	wn_alloc,1	! Result = %o0
/* 0x0008	     */		or	%g0,24,%o0
/* 0x000c	     */		st	%o0,[%i0] ! volatile

!   53		      !  (*ptable)->binary_tree = NULL;

/* 0x0010	  53 */		ld	[%i0],%o1 ! volatile

!   55		      !  (*ptable)->group = wn_curgp();

/* 0x0014	  55 */		call	wn_curgp,0	! Result = %o0
/* 0x0018	     */		st	%g0,[%o1] ! volatile
/* 0x001c	     */		ld	[%i0],%o1 ! volatile
/* 0x0020	     */		st	%o0,[%o1+20] ! volatile

!   57		      !  (*ptable)->phash_func = phash_func;

/* 0x0024	  57 */		ld	[%i0],%o1 ! volatile
/* 0x0028	     */		st	%i1,[%o1+4] ! volatile

!   58		      !  (*ptable)->pkeys_eq_func = pkeys_eq_func;

/* 0x002c	  58 */		ld	[%i0],%o1 ! volatile
/* 0x0030	     */		st	%i2,[%o1+8] ! volatile

!   59		      !  (*ptable)->palloc_copy_func = palloc_copy_func;

/* 0x0034	  59 */		ld	[%i0],%o1 ! volatile
/* 0x0038	     */		st	%i3,[%o1+12] ! volatile

!   60		      !  (*ptable)->pfree_func = pfree_func;

/* 0x003c	  60 */		ld	[%i0],%o1 ! volatile
/* 0x0040	     */		st	%i4,[%o1+16] ! volatile
/* 0x0044	     */		ret
/* 0x0048	     */		restore	%g0,%g0,%g0
/* 0x004c	   0 */		.type	wn_mkhtab,2
/* 0x004c	     */		.size	wn_mkhtab,(.-wn_mkhtab)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE free_binary_tree
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       free_binary_tree:
/* 000000	     */		save	%sp,-96,%sp

!   61		      !}
!   64		      !local void free_binary_tree
!   65		      !(
!   66		      !  wn_binary_tree_data binary_tree,
!   67		      !  void (*pfree_func)(ptr key)
!   68		      !)
!   69		      !{
!   70		      !  if(binary_tree != NULL)

/* 0x0004	  70 */		cmp	%i0,0
/* 0x0008	     */		be	.L77000007
/* 0x000c	     */		or	%g0,%i1,%i2
                       .L77000005:

!   71		      !  {
!   72		      !    free_binary_tree((binary_tree->children)[0],pfree_func);

/* 0x0010	  72 */		ld	[%i0],%o0 ! volatile
/* 0x0014	     */		call	free_binary_tree,2	! Result = %g0
/* 0x0018	     */		or	%g0,%i1,%o1

!   73		      !    free_binary_tree((binary_tree->children)[1],pfree_func);

/* 0x001c	  73 */		ld	[%i0+4],%o0 ! volatile
/* 0x0020	     */		call	free_binary_tree,2	! Result = %g0
/* 0x0024	     */		or	%g0,%i1,%o1

!   75		      !    (*pfree_func)(binary_tree->key);

/* 0x0028	  75 */		jmpl	%i2,%o7
/* 0x002c	     */		ld	[%i0+16],%o0 ! volatile

!   76		      !    wn_free(binary_tree);

/* 0x0030	  76 */		call	wn_free,1	! Result = %g0	! (tail call)
/* 0x0034	     */		restore	%g0,%g0,%g0
                       .L77000007:
/* 0x0038	     */		ret
/* 0x003c	     */		restore	%g0,%g0,%g0
/* 0x0040	   0 */		.type	free_binary_tree,2
/* 0x0040	     */		.size	free_binary_tree,(.-free_binary_tree)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
/* 000000	     */		.skip	16
!
! SUBROUTINE wn_freehtab
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_freehtab
                       wn_freehtab:
/* 000000	     */		save	%sp,-96,%sp

!   77		      !  }
!   78		      !}
!   81		      !void wn_freehtab(wn_htab table)
!   82		      !{
!   83		      !  wn_gppush(table->group);

/* 0x0004	  83 */		call	wn_gppush,1	! Result = %g0
/* 0x0008	     */		ld	[%i0+20],%o0 ! volatile

!   85		      !  free_binary_tree(table->binary_tree,table->pfree_func);

/* 0x000c	  85 */		ld	[%i0],%o0 ! volatile
/* 0x0010	     */		call	free_binary_tree,2	! Result = %g0
/* 0x0014	     */		ld	[%i0+16],%o1 ! volatile

!   86		      !  wn_free(table);

/* 0x0018	  86 */		call	wn_free,1	! Result = %g0
/* 0x001c	     */		or	%g0,%i0,%o0

!   88		      !  wn_gppop();

/* 0x0020	  88 */		call	wn_gppop,0	! Result = %g0	! (tail call)
/* 0x0024	     */		restore	%g0,%g0,%g0
                       .L77000010:
/* 0x0028	     */		ret
/* 0x002c	     */		restore	%g0,%g0,%g0
/* 0x0030	   0 */		.type	wn_freehtab,2
/* 0x0030	     */		.size	wn_freehtab,(.-wn_freehtab)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hins
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hins
                       wn_hins:
/* 000000	     */		save	%sp,-96,%sp

!   89		      !}
!   92		      !bool wn_hins(ptr data,wn_htab table,ptr key)
!   93		      !{
!   94		      !  register wn_binary_tree_data tree,*ptree;
!   95		      !  register int hashed_key,shifted_hashed_key;
!   97		      !  ptree = &(table->binary_tree);
!   98		      !  tree = *ptree;

/* 0x0004	  98 */		ld	[%i1],%i4 ! volatile
/* 0x0008	  93 */		or	%g0,%i1,%l2
/* 0x000c	     */		or	%g0,%i2,%l1

!  100		      !  hashed_key = (*(table->phash_func))(key);

/* 0x0010	 100 */		ld	[%i1+4],%l0 ! volatile
/* 0x0014	  93 */		or	%g0,%i0,%i5
/* 0x0018	 100 */		jmpl	%l0,%o7
/* 0x001c	     */		or	%g0,%i2,%o0

!  102		      !  if(tree == NULL)  /* table is empty */

/* 0x0020	 102 */		cmp	%i4,0
/* 0x0024	     */		bne	.L77000014
/* 0x0028	     */		or	%g0,%o0,%i3
                       .L77000013:

!  103		      !  {
!  104		      !    wn_gppush(table->group);

/* 0x002c	 104 */		call	wn_gppush,1	! Result = %g0
/* 0x0030	     */		ld	[%i1+20],%o0 ! volatile

!  106		      !    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_da
!  106		      >ta_struct));

/* 0x0034	 106 */		call	wn_alloc,1	! Result = %o0
/* 0x0038	     */		or	%g0,20,%o0
/* 0x003c	     */		st	%o0,[%i1] ! volatile

!  108		      !    (tree->children)[0] = (tree->children)[1] = NULL;

/* 0x0040	 108 */		st	%g0,[%o0+4] ! volatile
/* 0x0044	     */		st	%g0,[%o0] ! volatile

!  109		      !    tree->hashed_key = hashed_key;

/* 0x0048	 109 */		st	%i3,[%o0+8] ! volatile

!  110		      !    tree->data = data;

/* 0x004c	 110 */		st	%i0,[%o0+12] ! volatile

!  111		      !    (*(table->palloc_copy_func))(&(tree->key),key);

/* 0x0050	 111 */		add	%o0,16,%o0
/* 0x0054	     */		ld	[%i1+12],%l0 ! volatile
/* 0x0058	     */		jmpl	%l0,%o7
/* 0x005c	     */		or	%g0,%i2,%o1

!  113		      !    wn_gppop();

/* 0x0060	 113 */		call	wn_gppop,0	! Result = %g0
/* 0x0064	     */		nop
/* 0x0068	     */		ret
/* 0x006c	     */		restore	%g0,1,%o0
                       .L77000014:

!  115		      !    return(TRUE);
!  116		      !  }
!  117		      !  else
!  118		      !  {
!  119		      !    shifted_hashed_key = hashed_key;

/* 0x0070	 119 */		or	%g0,%o0,%i1

!  121		      !    do
!  122		      !    {
!  123		      !      if(hashed_key == tree->hashed_key)

/* 0x0074	 123 */		ld	[%i4+8],%o0 ! volatile
                       .L900000405:
/* 0x0078	 123 */		cmp	%i3,%o0
/* 0x007c	     */		bne	.L900000408
/* 0x0080	     */		and	%i1,1,%o0
                       .L77000016:

!  124		      !      {
!  125		      !        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */

/* 0x0084	 125 */		ld	[%i4+16],%o1 ! volatile
/* 0x0088	     */		ld	[%l2+8],%l0 ! volatile
/* 0x008c	     */		jmpl	%l0,%o7
/* 0x0090	     */		or	%g0,%l1,%o0
/* 0x0094	     */		cmp	%o0,0
/* 0x0098	     */		be	.L900000407
/* 0x009c	     */		and	%i1,1,%o0
                       .L77000017:
/* 0x00a0	     */		ret
/* 0x00a4	     */		restore	%g0,0,%o0
                       .L900000407:

!  126		      !        {
!  127		      !	  return(FALSE);
!  128		      !        }
!  129		      !        else
!  130		      !        {
!  131		      !          ptree = &((tree->children)[shifted_hashed_key & 1]);

/* 0x00a8	 131 */		sll	%o0,2,%o0
/* 0x00ac	     */		add	%i4,%o0,%i0

!  133		      !          shifted_hashed_key >>= 1;

/* 0x00b0	 133 */		sra	%i1,1,%i1
/* 0x00b4	     */		ba	.L900000406
/* 0x00b8	     */		ld	[%i0],%i4 ! volatile
                       .L900000408:

!  134		      !        }
!  135		      !      }
!  136		      !      else
!  137		      !      {
!  138		      !        ptree = &((tree->children)[shifted_hashed_key & 1]);

/* 0x00bc	 138 */		sll	%o0,2,%o0
/* 0x00c0	     */		add	%i4,%o0,%i0

!  140		      !        shifted_hashed_key >>= 1;

/* 0x00c4	 140 */		sra	%i1,1,%i1

!  141		      !      }
!  143		      !      tree = *ptree;

/* 0x00c8	 143 */		ld	[%i0],%i4 ! volatile
                       .L900000406:

!  144		      !    }
!  145		      !    while(tree != NULL);

/* 0x00cc	 145 */		cmp	%i4,0
/* 0x00d0	     */		bne,a	.L900000405
/* 0x00d4	     */		ld	[%i4+8],%o0 ! volatile
                       .L77000021:

!  147		      !    wn_gppush(table->group);

/* 0x00d8	 147 */		call	wn_gppush,1	! Result = %g0
/* 0x00dc	     */		ld	[%l2+20],%o0 ! volatile

!  149		      !    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_da
!  149		      >ta_struct));

/* 0x00e0	 149 */		call	wn_alloc,1	! Result = %o0
/* 0x00e4	     */		or	%g0,20,%o0
/* 0x00e8	     */		st	%o0,[%i0] ! volatile

!  151		      !    (tree->children)[0] = (tree->children)[1] = NULL;

/* 0x00ec	 151 */		st	%g0,[%o0+4] ! volatile
/* 0x00f0	     */		st	%g0,[%o0] ! volatile

!  152		      !    tree->hashed_key = hashed_key;

/* 0x00f4	 152 */		st	%i3,[%o0+8] ! volatile

!  153		      !    tree->data = data;

/* 0x00f8	 153 */		st	%i5,[%o0+12] ! volatile

!  154		      !    (*(table->palloc_copy_func))(&(tree->key),key);

/* 0x00fc	 154 */		add	%o0,16,%o0
/* 0x0100	     */		ld	[%l2+12],%l0 ! volatile
/* 0x0104	     */		jmpl	%l0,%o7
/* 0x0108	     */		or	%g0,%l1,%o1

!  156		      !    wn_gppop();

/* 0x010c	 156 */		call	wn_gppop,0	! Result = %g0
/* 0x0110	     */		nop
/* 0x0114	     */		ret
/* 0x0118	     */		restore	%g0,1,%o0
/* 0x011c	   0 */		.type	wn_hins,2
/* 0x011c	     */		.size	wn_hins,(.-wn_hins)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hfins
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hfins
                       wn_hfins:
/* 000000	     */		save	%sp,-96,%sp

!  158		      !    return(TRUE);
!  159		      !  } 
!  160		      !}
!  163		      !bool wn_hfins(ptr data,wn_htab table,ptr key)
!  164		      !{
!  165		      !  register wn_binary_tree_data tree,*ptree;
!  166		      !  register int hashed_key,shifted_hashed_key;
!  168		      !  ptree = &(table->binary_tree);
!  169		      !  tree = *ptree;

/* 0x0004	 169 */		ld	[%i1],%i4 ! volatile
/* 0x0008	 164 */		or	%g0,%i1,%l2
/* 0x000c	     */		or	%g0,%i2,%l1

!  171		      !  hashed_key = (*(table->phash_func))(key);

/* 0x0010	 171 */		ld	[%i1+4],%l0 ! volatile
/* 0x0014	 164 */		or	%g0,%i0,%i5
/* 0x0018	 171 */		jmpl	%l0,%o7
/* 0x001c	     */		or	%g0,%i2,%o0

!  173		      !  if(tree == NULL)  /* table is empty */

/* 0x0020	 173 */		cmp	%i4,0
/* 0x0024	     */		bne	.L77000028
/* 0x0028	     */		or	%g0,%o0,%i3
                       .L77000027:

!  174		      !  {
!  175		      !    wn_gppush(table->group);

/* 0x002c	 175 */		call	wn_gppush,1	! Result = %g0
/* 0x0030	     */		ld	[%i1+20],%o0 ! volatile

!  177		      !    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_da
!  177		      >ta_struct));

/* 0x0034	 177 */		call	wn_alloc,1	! Result = %o0
/* 0x0038	     */		or	%g0,20,%o0
/* 0x003c	     */		st	%o0,[%i1] ! volatile

!  179		      !    (tree->children)[0] = (tree->children)[1] = NULL;

/* 0x0040	 179 */		st	%g0,[%o0+4] ! volatile
/* 0x0044	     */		st	%g0,[%o0] ! volatile

!  180		      !    tree->hashed_key = hashed_key;

/* 0x0048	 180 */		st	%i3,[%o0+8] ! volatile

!  181		      !    tree->data = data;

/* 0x004c	 181 */		st	%i0,[%o0+12] ! volatile

!  182		      !    (*(table->palloc_copy_func))(&(tree->key),key);

/* 0x0050	 182 */		add	%o0,16,%o0
/* 0x0054	     */		ld	[%i1+12],%l0 ! volatile
/* 0x0058	     */		jmpl	%l0,%o7
/* 0x005c	     */		or	%g0,%i2,%o1

!  184		      !    wn_gppop();

/* 0x0060	 184 */		call	wn_gppop,0	! Result = %g0
/* 0x0064	     */		nop
/* 0x0068	     */		ret
/* 0x006c	     */		restore	%g0,1,%o0
                       .L77000028:

!  186		      !    return(TRUE);
!  187		      !  }
!  188		      !  else
!  189		      !  {
!  190		      !    shifted_hashed_key = hashed_key;

/* 0x0070	 190 */		or	%g0,%o0,%i1

!  192		      !    do
!  193		      !    {
!  194		      !      if(hashed_key == tree->hashed_key)

/* 0x0074	 194 */		ld	[%i4+8],%o0 ! volatile
                       .L900000505:
/* 0x0078	 194 */		cmp	%i3,%o0
/* 0x007c	     */		bne	.L900000508
/* 0x0080	     */		and	%i1,1,%o0
                       .L77000030:

!  195		      !      {
!  196		      !        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */

/* 0x0084	 196 */		ld	[%i4+16],%o1 ! volatile
/* 0x0088	     */		ld	[%l2+8],%l0 ! volatile
/* 0x008c	     */		jmpl	%l0,%o7
/* 0x0090	     */		or	%g0,%l1,%o0
/* 0x0094	     */		cmp	%o0,0
/* 0x0098	     */		be	.L900000507
/* 0x009c	     */		and	%i1,1,%o0
                       .L77000031:

!  197		      !        {
!  198		      !          tree->data = data;   /* overwrite existing data */

/* 0x00a0	 198 */		st	%i5,[%i4+12] ! volatile
/* 0x00a4	     */		ret
/* 0x00a8	     */		restore	%g0,1,%o0
                       .L900000507:

!  200		      !	  return(TRUE);
!  201		      !        }
!  202		      !        else
!  203		      !        {
!  204		      !          ptree = &((tree->children)[shifted_hashed_key & 1]);

/* 0x00ac	 204 */		sll	%o0,2,%o0
/* 0x00b0	     */		add	%i4,%o0,%i0

!  206		      !          shifted_hashed_key >>= 1;

/* 0x00b4	 206 */		sra	%i1,1,%i1
/* 0x00b8	     */		ba	.L900000506
/* 0x00bc	     */		ld	[%i0],%i4 ! volatile
                       .L900000508:

!  207		      !        }
!  208		      !      }
!  209		      !      else
!  210		      !      {
!  211		      !        ptree = &((tree->children)[shifted_hashed_key & 1]);

/* 0x00c0	 211 */		sll	%o0,2,%o0
/* 0x00c4	     */		add	%i4,%o0,%i0

!  213		      !        shifted_hashed_key >>= 1;

/* 0x00c8	 213 */		sra	%i1,1,%i1

!  214		      !      }
!  216		      !      tree = *ptree;

/* 0x00cc	 216 */		ld	[%i0],%i4 ! volatile
                       .L900000506:

!  217		      !    }
!  218		      !    while(tree != NULL);

/* 0x00d0	 218 */		cmp	%i4,0
/* 0x00d4	     */		bne,a	.L900000505
/* 0x00d8	     */		ld	[%i4+8],%o0 ! volatile
                       .L77000035:

!  220		      !    wn_gppush(table->group);

/* 0x00dc	 220 */		call	wn_gppush,1	! Result = %g0
/* 0x00e0	     */		ld	[%l2+20],%o0 ! volatile

!  222		      !    *ptree = tree = (wn_binary_tree_data)wn_alloc(sizeof(struct wn_binary_tree_da
!  222		      >ta_struct));

/* 0x00e4	 222 */		call	wn_alloc,1	! Result = %o0
/* 0x00e8	     */		or	%g0,20,%o0
/* 0x00ec	     */		st	%o0,[%i0] ! volatile

!  224		      !    (tree->children)[0] = (tree->children)[1] = NULL;

/* 0x00f0	 224 */		st	%g0,[%o0+4] ! volatile
/* 0x00f4	     */		st	%g0,[%o0] ! volatile

!  225		      !    tree->hashed_key = hashed_key;

/* 0x00f8	 225 */		st	%i3,[%o0+8] ! volatile

!  226		      !    tree->data = data;

/* 0x00fc	 226 */		st	%i5,[%o0+12] ! volatile

!  227		      !    (*(table->palloc_copy_func))(&(tree->key),key);

/* 0x0100	 227 */		add	%o0,16,%o0
/* 0x0104	     */		ld	[%l2+12],%l0 ! volatile
/* 0x0108	     */		jmpl	%l0,%o7
/* 0x010c	     */		or	%g0,%l1,%o1

!  229		      !    wn_gppop();

/* 0x0110	 229 */		call	wn_gppop,0	! Result = %g0
/* 0x0114	     */		nop
/* 0x0118	     */		ret
/* 0x011c	     */		restore	%g0,1,%o0
/* 0x0120	   0 */		.type	wn_hfins,2
/* 0x0120	     */		.size	wn_hfins,(.-wn_hfins)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hget
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hget
                       wn_hget:
/* 000000	     */		save	%sp,-96,%sp

!  231		      !    return(TRUE);
!  232		      !  } 
!  233		      !}
!  236		      !#if 1
!  237		      !bool wn_hget(ptr *pdata,wn_htab table,ptr key)
!  238		      !{
!  239		      !  wn_binary_tree_data tree;
!  240		      !  int hashed_key,shifted_hashed_key;
!  241		      !  int masked_bit;
!  243		      !  tree = table->binary_tree;

/* 0x0004	 243 */		ld	[%i1],%i4 ! volatile
/* 0x0008	 238 */		or	%g0,%i1,%l1

!  244		      !  
!  245		      !  if(tree == NULL)  /* table is empty */

/* 0x000c	 245 */		cmp	%i4,0
/* 0x0010	     */		be	.L77000052
/* 0x0014	     */		or	%g0,%i2,%i5
                       .L77000042:

!  246		      !  {
!  247		      !    goto fail;
!  248		      !  }
!  250		      !  hashed_key = (*(table->phash_func))(key);

/* 0x0018	 250 */		ld	[%i1+4],%l0 ! volatile
/* 0x001c	     */		jmpl	%l0,%o7
/* 0x0020	     */		or	%g0,%i2,%o0
/* 0x0024	     */		or	%g0,%o0,%i1

!  251		      !  shifted_hashed_key = hashed_key;

/* 0x0028	 251 */		or	%g0,%o0,%i3

!  253		      !  for(;;)
!  254		      !  {
!  255		      !    if(hashed_key != tree->hashed_key)

/* 0x002c	 255 */		ld	[%i4+8],%o0 ! volatile
                       .L900000605:
/* 0x0030	 255 */		cmp	%i1,%o0
/* 0x0034	     */		be,a	.L900000608
/* 0x0038	     */		ld	[%i4+16],%o1 ! volatile
                       .L77000044:

!  256		      !    {
!  257		      !      masked_bit = (shifted_hashed_key&1);

/* 0x003c	 257 */		and	%i3,1,%i2

!  258		      !      shifted_hashed_key >>= 1;

/* 0x0040	 258 */		sra	%i3,1,%i3

!  260		      !      do  /* the main inner loop */
!  261		      !      {
!  262		      !        tree = (tree->children)[masked_bit];

/* 0x0044	 262 */		sll	%i2,2,%o0
                       .L900000607:
/* 0x0048	 262 */		ld	[%i4+%o0],%i4 ! volatile

!  263		      !        masked_bit = (shifted_hashed_key&1);

/* 0x004c	 263 */		and	%i3,1,%i2

!  264		      !        shifted_hashed_key >>= 1;

/* 0x0050	 264 */		sra	%i3,1,%i3

!  266		      !        if(tree == NULL)

/* 0x0054	 266 */		cmp	%i4,0
/* 0x0058	     */		be	.L77000052
/* 0x005c	     */		nop
                       .L77000047:

!  267		      !	{
!  268		      !	  goto fail;
!  269		      !        }
!  270		      !      }
!  271		      !      while(hashed_key != tree->hashed_key);

/* 0x0060	 271 */		ld	[%i4+8],%o0 ! volatile
/* 0x0064	     */		cmp	%i1,%o0
/* 0x0068	     */		bne	.L900000607
/* 0x006c	     */		sll	%i2,2,%o0
                       .L77000049:

!  272		      !    }
!  273		      ! 
!  274		      !    if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */

/* 0x0070	 274 */		ld	[%i4+16],%o1 ! volatile
                       .L900000608:
/* 0x0074	 274 */		ld	[%l1+8],%l0 ! volatile
/* 0x0078	     */		jmpl	%l0,%o7
/* 0x007c	     */		or	%g0,%i5,%o0
/* 0x0080	     */		cmp	%o0,0
/* 0x0084	     */		be	.L900000606
/* 0x0088	     */		sll	%i2,2,%o0
                       .L77000050:

!  275		      !    {
!  276		      !      *pdata = tree->data;

/* 0x008c	 276 */		ld	[%i4+12],%o0 ! volatile
/* 0x0090	     */		st	%o0,[%i0] ! volatile
/* 0x0094	     */		ret
/* 0x0098	     */		restore	%g0,1,%o0
                       .L900000606:

!  277		      ! 
!  278		      !      return(TRUE);
!  279		      !    }
!  281		      !    tree = (tree->children)[masked_bit];

/* 0x009c	 281 */		ld	[%i4+%o0],%i4 ! volatile
/* 0x00a0	     */		ba	.L900000605
/* 0x00a4	     */		ld	[%i4+8],%o0 ! volatile
                       .L77000052:

!  282		      !  }
!  284		      !fail:
!  285		      !  *pdata = NULL;

/* 0x00a8	 285 */		st	%g0,[%i0] ! volatile
/* 0x00ac	     */		ret
/* 0x00b0	     */		restore	%g0,0,%o0
/* 0x00b4	   0 */		.type	wn_hget,2
/* 0x00b4	     */		.size	wn_hget,(.-wn_hget)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
/* 000000	     */		.skip	16
!
! SUBROUTINE find_leaf_tree_node_pointer
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       find_leaf_tree_node_pointer:

!  287		      !  return(FALSE);
!  288		      !}
!  289		      !#endif
!  290		      !#if 0
!  291		      !bool wn_hget(ptr *pdata,wn_htab table,ptr key)
!  292		      !{
!  293		      !  register wn_binary_tree_data tree;
!  294		      !  register int hashed_key,shifted_hashed_key;
!  296		      !  tree = table->binary_tree;
!  298		      !  if(tree == NULL)  /* table is empty */
!  299		      !  {
!  300		      !    *pdata = NULL;
!  302		      !    return(FALSE);
!  303		      !  }
!  304		      !  else
!  305		      !  {
!  306		      !    hashed_key = (*(table->phash_func))(key);
!  307		      !    shifted_hashed_key = hashed_key;
!  309		      !    do
!  310		      !    {
!  311		      !      if(hashed_key == tree->hashed_key)
!  312		      !      {
!  313		      !        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */
!  314		      !        {
!  315		      !          *pdata = tree->data;
!  316		      ! 
!  317		      !          return(TRUE);
!  318		      !        }
!  319		      !        else
!  320		      !        {
!  321		      !          tree = (tree->children)[shifted_hashed_key & 1];
!  323		      !          shifted_hashed_key >>= 1;
!  324		      !        }
!  325		      !      }
!  326		      !      else
!  327		      !      {
!  328		      !        tree = (tree->children)[shifted_hashed_key & 1];
!  330		      !        shifted_hashed_key >>= 1;
!  331		      !      }
!  332		      !    }
!  333		      !    while(tree != NULL);
!  335		      !    *pdata = NULL;
!  337		      !    return(FALSE);
!  338		      !  } 
!  339		      !}
!  340		      !#endif
!  341		      !#if 0
!  342		      !bool wn_hget(ptr *pdata,wn_htab table,ptr key)
!  343		      !{
!  344		      !  register wn_binary_tree_data tree;
!  345		      !  register int hashed_key,shifted_hashed_key;
!  347		      !  tree = table->binary_tree;
!  349		      !  if(tree == NULL)  /* table is empty */
!  350		      !  {
!  351		      !    *pdata = NULL;
!  353		      !    return(FALSE);
!  354		      !  }
!  355		      !  else
!  356		      !  {
!  357		      !    hashed_key = (*(table->phash_func))(key);
!  358		      !    shifted_hashed_key = hashed_key;
!  360		      !    do
!  361		      !    {
!  362		      !      if(
!  363		      !	  (hashed_key == tree->hashed_key)
!  364		      !	    &&
!  365		      !          (*(table->pkeys_eq_func))(key,tree->key)  /* if == */
!  366		      !	)
!  367		      !      {
!  368		      !        *pdata = tree->data;
!  369		      ! 
!  370		      !        return(TRUE);
!  371		      !      }
!  373		      !      tree = (tree->children)[shifted_hashed_key & 1];
!  375		      !      shifted_hashed_key >>= 1;
!  376		      !    }
!  377		      !    while(tree != NULL);
!  379		      !    *pdata = NULL;
!  381		      !    return(FALSE);
!  382		      !  } 
!  383		      !}
!  384		      !#endif
!  387		      !local void find_leaf_tree_node_pointer
!  388		      !(
!  389		      !  register wn_binary_tree_data **ppleaf_node,
!  390		      !  register wn_binary_tree_data tree_node,
!  391		      !  register int shifted_hashed_key
!  392		      !)
!  393		      !{
!  394		      !  register wn_binary_tree_data *pchild_node,child_node;
!  395		      !  register int index;
!  397		      !  pchild_node = NULL;

/* 000000	 397 */		or	%g0,0,%g3

!  399		      !loop:
!  400		      !    *ppleaf_node = pchild_node;

/* 0x0004	 400 */		st	%g3,[%o0] ! volatile
                       .L900000705:

!  402		      !    index = (shifted_hashed_key & 1);

/* 0x0008	 402 */		and	%o2,1,%g1

!  403		      !    pchild_node = &((tree_node->children)[index]);

/* 0x000c	 403 */		sll	%g1,2,%g2
/* 0x0010	     */		add	%o1,%g2,%g3

!  404		      !    child_node = *pchild_node;

/* 0x0014	 404 */		ld	[%g3],%g2 ! volatile

!  406		      !    shifted_hashed_key >>= 1;
!  408		      !    if(child_node != NULL)

/* 0x0018	 408 */		cmp	%g2,0
/* 0x001c	     */		be	.L77000062
/* 0x0020	     */		sra	%o2,1,%o2
                       .L77000061:

!  409		      !    {
!  410		      !      tree_node = child_node;

/* 0x0024	 410 */		or	%g0,%g2,%o1

!  412		      !      goto loop;

/* 0x0028	 412 */		ba	.L900000705
/* 0x002c	     */		st	%g3,[%o0] ! volatile
                       .L77000062:

!  413		      !    }
!  415		      !    index ^= 1;
!  416		      !    pchild_node = &((tree_node->children)[index]);

/* 0x0030	 416 */		xor	%g1,1,%g1
/* 0x0034	     */		sll	%g1,2,%g1
/* 0x0038	     */		add	%o1,%g1,%g3

!  417		      !    child_node = *pchild_node;

/* 0x003c	 417 */		ld	[%g3],%o1 ! volatile

!  419		      !    if(child_node != NULL)

/* 0x0040	 419 */		cmp	%o1,0
/* 0x0044	     */		be	.L77000065
/* 0x0048	     */		nop

!  420		      !    {
!  421		      !      tree_node = child_node;
!  423		      !      goto loop;

/* 0x004c	 423 */		ba	.L900000705
/* 0x0050	     */		st	%g3,[%o0] ! volatile
                       .L77000065:
/* 0x0054	     */		retl
/* 0x0058	     */		nop
/* 0x005c	   0 */		.type	find_leaf_tree_node_pointer,2
/* 0x005c	     */		.size	find_leaf_tree_node_pointer,(.-find_leaf_tree_node_pointer)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE remove_tree_node
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       remove_tree_node:
/* 000000	     */		save	%sp,-104,%sp

!  424		      !    }
!  425		      !}
!  428		      !local void remove_tree_node
!  429		      !(
!  430		      !  wn_binary_tree_data *ptree_node,
!  431		      !  int shifted_hashed_key
!  432		      !)
!  433		      !{
!  434		      !  wn_binary_tree_data tree_node;
!  435		      !  wn_binary_tree_data *pleaf_node,leaf_node;
!  437		      !  tree_node = *ptree_node;

/* 0x0004	 437 */		ld	[%i0],%i2 ! volatile

!  439		      !  find_leaf_tree_node_pointer(&pleaf_node,tree_node,shifted_hashed_key);

/* 0x0008	 439 */		add	%fp,-4,%o0
/* 0x000c	     */		or	%g0,%i2,%o1
/* 0x0010	     */		call	find_leaf_tree_node_pointer,3	! Result = %g0
/* 0x0014	     */		or	%g0,%i1,%o2

!  441		      !  if(pleaf_node == NULL)

/* 0x0018	 441 */		ld	[%fp-4],%o1
/* 0x001c	     */		cmp	%o1,0
/* 0x0020	     */		bne,a	.L900000805
/* 0x0024	     */		ld	[%o1],%o0 ! volatile
                       .L77000070:

!  442		      !  {
!  443		      !    *ptree_node = NULL;

/* 0x0028	 443 */		st	%g0,[%i0] ! volatile
/* 0x002c	     */		ret
/* 0x0030	     */		restore	%g0,%g0,%g0
                       .L900000805:

!  444		      !  }
!  445		      !  else
!  446		      !  {
!  447		      !    leaf_node = *pleaf_node;
!  448		      !    *pleaf_node = NULL;

/* 0x0034	 448 */		st	%g0,[%o1] ! volatile

!  450		      !    (leaf_node->children)[0] = (tree_node->children)[0];

/* 0x0038	 450 */		ld	[%i2],%o1 ! volatile
/* 0x003c	     */		st	%o1,[%o0] ! volatile

!  451		      !    (leaf_node->children)[1] = (tree_node->children)[1];

/* 0x0040	 451 */		ld	[%i2+4],%o1 ! volatile
/* 0x0044	     */		st	%o1,[%o0+4] ! volatile

!  453		      !    *ptree_node = leaf_node;

/* 0x0048	 453 */		st	%o0,[%i0] ! volatile
/* 0x004c	     */		ret
/* 0x0050	     */		restore	%g0,%g0,%g0
/* 0x0054	   0 */		.type	remove_tree_node,2
/* 0x0054	     */		.size	remove_tree_node,(.-remove_tree_node)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hdel
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hdel
                       wn_hdel:
/* 000000	     */		save	%sp,-96,%sp

!  454		      !  }
!  455		      !}
!  458		      !bool wn_hdel(wn_htab table,ptr key)
!  459		      !{
!  460		      !  register wn_binary_tree_data tree,*ptree;
!  461		      !  register int hashed_key,shifted_hashed_key;
!  463		      !  ptree = &(table->binary_tree);
!  464		      !  tree = *ptree;

/* 0x0004	 464 */		ld	[%i0],%i3 ! volatile
/* 0x0008	 459 */		or	%g0,%i0,%i5
/* 0x000c	     */		or	%g0,%i1,%i4

!  466		      !  if(tree == NULL)  /* table is empty */

/* 0x0010	 466 */		cmp	%i3,0
/* 0x0014	     */		bne	.L77000077
/* 0x0018	     */		or	%g0,%i0,%i2
                       .L77000076:
/* 0x001c	     */		ret
/* 0x0020	     */		restore	%g0,0,%o0
                       .L77000077:

!  467		      !  {
!  468		      !    return(FALSE);
!  469		      !  }
!  470		      !  else
!  471		      !  {
!  472		      !    hashed_key = (*(table->phash_func))(key);

/* 0x0024	 472 */		ld	[%i0+4],%l0 ! volatile
/* 0x0028	     */		jmpl	%l0,%o7
/* 0x002c	     */		or	%g0,%i1,%o0
/* 0x0030	     */		or	%g0,%o0,%i0

!  473		      !    shifted_hashed_key = hashed_key;

/* 0x0034	 473 */		or	%g0,%o0,%i1

!  475		      !    do
!  476		      !    {
!  477		      !      if(hashed_key == tree->hashed_key)

/* 0x0038	 477 */		ld	[%i3+8],%o0 ! volatile
                       .L900000905:
/* 0x003c	 477 */		cmp	%i0,%o0
/* 0x0040	     */		bne	.L900000908
/* 0x0044	     */		and	%i1,1,%o0
                       .L77000079:

!  478		      !      {
!  479		      !        if((*(table->pkeys_eq_func))(key,tree->key))  /* if == */

/* 0x0048	 479 */		ld	[%i3+16],%o1 ! volatile
/* 0x004c	     */		ld	[%i5+8],%l0 ! volatile
/* 0x0050	     */		jmpl	%l0,%o7
/* 0x0054	     */		or	%g0,%i4,%o0
/* 0x0058	     */		cmp	%o0,0
/* 0x005c	     */		be	.L900000907
/* 0x0060	     */		and	%i1,1,%o0
                       .L77000080:

!  480		      !        {
!  481		      !          wn_gppush(table->group);  /* only push group if found */

/* 0x0064	 481 */		call	wn_gppush,1	! Result = %g0
/* 0x0068	     */		ld	[%i5+20],%o0 ! volatile

!  483		      !          (*(table->pfree_func))(tree->key);

/* 0x006c	 483 */		ld	[%i3+16],%o0 ! volatile
/* 0x0070	     */		ld	[%i5+16],%l0 ! volatile
/* 0x0074	     */		jmpl	%l0,%o7
/* 0x0078	     */		nop

!  484		      !          remove_tree_node(ptree,shifted_hashed_key); 

/* 0x007c	 484 */		or	%g0,%i2,%o0
/* 0x0080	     */		call	remove_tree_node,2	! Result = %g0
/* 0x0084	     */		or	%g0,%i1,%o1

!  485		      !          wn_free(tree);

/* 0x0088	 485 */		call	wn_free,1	! Result = %g0
/* 0x008c	     */		or	%g0,%i3,%o0

!  487		      !          wn_gppop();

/* 0x0090	 487 */		call	wn_gppop,0	! Result = %g0
/* 0x0094	     */		nop
/* 0x0098	     */		ret
/* 0x009c	     */		restore	%g0,1,%o0
                       .L900000907:

!  489		      !	  return(TRUE);
!  490		      !        }
!  491		      !        else
!  492		      !        {
!  493		      !          ptree = &((tree->children)[shifted_hashed_key & 1]);

/* 0x00a0	 493 */		sll	%o0,2,%o0
/* 0x00a4	     */		add	%i3,%o0,%i2

!  495		      !          shifted_hashed_key >>= 1;

/* 0x00a8	 495 */		sra	%i1,1,%i1
/* 0x00ac	     */		ba	.L900000906
/* 0x00b0	     */		ld	[%i2],%i3 ! volatile
                       .L900000908:

!  496		      !        }
!  497		      !      }
!  498		      !      else
!  499		      !      {
!  500		      !        ptree = &((tree->children)[shifted_hashed_key & 1]);

/* 0x00b4	 500 */		sll	%o0,2,%o0
/* 0x00b8	     */		add	%i3,%o0,%i2

!  502		      !        shifted_hashed_key >>= 1;

/* 0x00bc	 502 */		sra	%i1,1,%i1

!  503		      !      }
!  505		      !      tree = *ptree;

/* 0x00c0	 505 */		ld	[%i2],%i3 ! volatile
                       .L900000906:

!  506		      !    }
!  507		      !    while(tree != NULL);

/* 0x00c4	 507 */		cmp	%i3,0
/* 0x00c8	     */		bne,a	.L900000905
/* 0x00cc	     */		ld	[%i3+8],%o0 ! volatile
                       .L77000084:
/* 0x00d0	     */		ret
/* 0x00d4	     */		restore	%g0,0,%o0
/* 0x00d8	   0 */		.type	wn_hdel,2
/* 0x00d8	     */		.size	wn_hdel,(.-wn_hdel)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE count_entries
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       count_entries:

!  509		      !    return(FALSE);
!  510		      !  } 
!  511		      !}
!  514		      !local int count;
!  516		      !local /*ARGSUSED*/ void count_entries(ptr data,ptr key)
!  517		      !{
!  518		      !  ++count;

/* 000000	 518 */		sethi	%hi(count),%g1
/* 0x0004	     */		ld	[%g1+%lo(count)],%g2 ! volatile
/* 0x0008	     */		add	%g2,1,%g2
/* 0x000c	     */		retl
/* 0x0010	     */		st	%g2,[%g1+%lo(count)] ! volatile
/* 0x0014	   0 */		.type	count_entries,2
/* 0x0014	     */		.size	count_entries,(.-count_entries)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hcount
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hcount
                       wn_hcount:
/* 000000	     */		save	%sp,-96,%sp

!  519		      !}
!  522		      !int wn_hcount(wn_htab table)
!  523		      !{
!  524		      !  count = 0;

/* 0x0004	 524 */		sethi	%hi(count),%l0

!  526		      !  wn_hact(table,(count_entries));

/* 0x0008	 526 */		sethi	%hi(count_entries),%o0
/* 0x000c	     */		add	%o0,%lo(count_entries),%o1
/* 0x0010	 524 */		st	%g0,[%l0+%lo(count)] ! volatile
/* 0x0014	 526 */		call	wn_hact,2	! Result = %g0
/* 0x0018	     */		or	%g0,%i0,%o0

!  528		      !  return(count);

/* 0x001c	 528 */		ld	[%l0+%lo(count)],%i0 ! volatile
/* 0x0020	     */		ret
/* 0x0024	     */		restore	%g0,%g0,%g0
/* 0x0028	   0 */		.type	wn_hcount,2
/* 0x0028	     */		.size	wn_hcount,(.-wn_hcount)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE act_on_binary_tree
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       act_on_binary_tree:
/* 000000	     */		save	%sp,-96,%sp

!  529		      !}
!  532		      !local void act_on_binary_tree
!  533		      !(
!  534		      !  wn_binary_tree_data binary_tree,
!  535		      !  void (*paction)(ptr data,ptr key)
!  536		      !)
!  537		      !{
!  538		      !  if(binary_tree != NULL)

/* 0x0004	 538 */		cmp	%i0,0
/* 0x0008	     */		be	.L77000098
/* 0x000c	     */		or	%g0,%i0,%i2
                       .L77000096:

!  539		      !  {
!  540		      !    (*paction)(binary_tree->data,binary_tree->key);

/* 0x0010	 540 */		ld	[%i2+12],%o0 ! volatile
                       .L900001205:
/* 0x0014	 540 */		jmpl	%i1,%o7
/* 0x0018	     */		ld	[%i2+16],%o1 ! volatile

!  542		      !    act_on_binary_tree((binary_tree->children)[0],paction);

/* 0x001c	 542 */		ld	[%i2],%o0 ! volatile
/* 0x0020	     */		call	act_on_binary_tree,2	! Result = %g0
/* 0x0024	     */		or	%g0,%i1,%o1

!  543		      !    act_on_binary_tree((binary_tree->children)[1],paction);

/* 0x0028	 543 */		ld	[%i2+4],%i2 ! volatile
/* 0x002c	     */		cmp	%i2,0
/* 0x0030	     */		bne,a	.L900001205
/* 0x0034	     */		ld	[%i2+12],%o0 ! volatile
                       .L77000098:
/* 0x0038	     */		ret
/* 0x003c	     */		restore	%g0,%g0,%g0
/* 0x0040	   0 */		.type	act_on_binary_tree,2
/* 0x0040	     */		.size	act_on_binary_tree,(.-act_on_binary_tree)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hact
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hact
                       wn_hact:

!  544		      !  }
!  545		      !}
!  548		      !void wn_hact(wn_htab table,void (*paction)(ptr data,ptr key))
!  549		      !{
!  550		      !  act_on_binary_tree(table->binary_tree,paction);

/* 000000	 550 */		ld	[%o0],%o0 ! volatile
/* 0x0004	     */		or	%g0,%o7,%g1
/* 0x0008	     */		call	act_on_binary_tree,2	! Result = %g0	! (tail call)
/* 0x000c	     */		or	%g0,%g1,%o7
                       .L77000105:
/* 0x0010	     */		retl
/* 0x0014	     */		nop
/* 0x0018	   0 */		.type	wn_hact,2
/* 0x0018	     */		.size	wn_hact,(.-wn_hact)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE verify_child_polarity
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       verify_child_polarity:

!  551		      !}
!  554		      !local void verify_child_polarity
!  555		      !(
!  556		      !  wn_binary_tree_data child,
!  557		      !  int polarity,
!  558		      !  int shift_count
!  559		      !)
!  560		      !{
!  561		      !  wn_assert(((child->hashed_key>>shift_count) & 1) == polarity);

/* 000000	 561 */		ld	[%o0+8],%g1 ! volatile
/* 0x0004	     */		sra	%g1,%o2,%g1
/* 0x0008	     */		and	%g1,1,%g1
/* 0x000c	     */		cmp	%g1,%o1
/* 0x0010	     */		be	.L77000110
/* 0x0014	     */		nop
                       .L77000108:
/* 0x0018	     */		sethi	%hi(.L179),%g1
/* 0x001c	     */		or	%g0,561,%o1
/* 0x0020	     */		add	%g1,%lo(.L179),%o0
/* 0x0024	     */		or	%g0,%o7,%g1
/* 0x0028	     */		call	wn_assert_routine,2	! Result = %g0	! (tail call)
/* 0x002c	     */		or	%g0,%g1,%o7
                       .L77000110:
/* 0x0030	     */		retl
/* 0x0034	     */		nop
/* 0x0038	   0 */		.type	verify_child_polarity,2
/* 0x0038	     */		.size	verify_child_polarity,(.-verify_child_polarity)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
/* 000000	     */		.skip	16
!
! SUBROUTINE verify_binary_tree
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       verify_binary_tree:
/* 000000	     */		save	%sp,-96,%sp

!  562		      !}
!  565		      !local void verify_binary_tree
!  566		      !(
!  567		      !  wn_htab table,
!  568		      !  wn_binary_tree_data binary_tree,
!  569		      !  int shift_count
!  570		      !)
!  571		      !{
!  572		      !  wn_binary_tree_data child0,child1;
!  573		      !  int new_shift_count;
!  575		      !  wn_assert(binary_tree->hashed_key == 

/* 0x0004	 575 */		ld	[%i1+16],%o0 ! volatile
                       .L900001506:
/* 0x0008	 575 */		ld	[%i0+4],%l0 ! volatile
/* 0x000c	     */		jmpl	%l0,%o7
/* 0x0010	     */		nop
/* 0x0014	     */		ld	[%i1+8],%o1 ! volatile
/* 0x0018	     */		cmp	%o1,%o0
/* 0x001c	     */		be	.L77000114
/* 0x0020	     */		add	%i2,1,%i3
                       .L77000113:
/* 0x0024	     */		sethi	%hi(.L187),%o0
/* 0x0028	     */		add	%o0,%lo(.L187),%o0
/* 0x002c	     */		call	wn_assert_routine,2	! Result = %g0
/* 0x0030	     */		or	%g0,576,%o1
                       .L77000114:

!  576		      !	    ((*(table->phash_func))(binary_tree->key)));
!  578		      !  new_shift_count = shift_count+1;
!  580		      !  child0 = (binary_tree->children)[0];

/* 0x0034	 580 */		ld	[%i1],%i4 ! volatile

!  581		      !  child1 = (binary_tree->children)[1];

/* 0x0038	 581 */		ld	[%i1+4],%i1 ! volatile

!  583		      !  if(child0 != NULL)

/* 0x003c	 583 */		cmp	%i4,0
/* 0x0040	     */		be	.L900001507
/* 0x0044	     */		cmp	%i1,0
                       .L77000115:

!  584		      !  {
!  585		      !    verify_child_polarity(child0,0,shift_count);

/* 0x0048	 585 */		or	%g0,%i4,%o0
/* 0x004c	     */		or	%g0,0,%o1
/* 0x0050	     */		call	verify_child_polarity,3	! Result = %g0
/* 0x0054	     */		or	%g0,%i2,%o2

!  586		      !    verify_binary_tree(table,child0,new_shift_count);

/* 0x0058	 586 */		or	%g0,%i0,%o0
/* 0x005c	     */		or	%g0,%i4,%o1
/* 0x0060	     */		call	verify_binary_tree,3	! Result = %g0
/* 0x0064	     */		or	%g0,%i3,%o2

!  587		      !  }
!  588		      !  if(child1 != NULL)

/* 0x0068	 588 */		cmp	%i1,0
                       .L900001507:
/* 0x006c	 588 */		be	.L77000119
/* 0x0070	     */		or	%g0,%i1,%o0
                       .L77000117:

!  589		      !  {
!  590		      !    verify_child_polarity(child1,1,shift_count);

/* 0x0074	 590 */		or	%g0,1,%o1
/* 0x0078	     */		call	verify_child_polarity,3	! Result = %g0
/* 0x007c	     */		or	%g0,%i2,%o2

!  591		      !    verify_binary_tree(table,child1,new_shift_count);

/* 0x0080	 591 */		or	%g0,%i3,%i2
/* 0x0084	     */		ba	.L900001506
/* 0x0088	     */		ld	[%i1+16],%o0 ! volatile
                       .L77000119:
/* 0x008c	     */		ret
/* 0x0090	     */		restore	%g0,%g0,%g0
/* 0x0094	   0 */		.type	verify_binary_tree,2
/* 0x0094	     */		.size	verify_binary_tree,(.-verify_binary_tree)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE wn_hverify
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hverify
                       wn_hverify:
/* 000000	     */		save	%sp,-96,%sp

!  592		      !  }
!  593		      !}
!  596		      !void wn_hverify(wn_htab table)
!  597		      !{
!  598		      !  wn_binary_tree_data binary_tree;
!  600		      !  wn_assert((ptr)(table->phash_func) != NULL);

/* 0x0004	 600 */		ld	[%i0+4],%o0 ! volatile
/* 0x0008	     */		cmp	%o0,0
/* 0x000c	     */		bne,a	.L900001610
/* 0x0010	     */		ld	[%i0+8],%o0 ! volatile
                       .L77000124:
/* 0x0014	     */		sethi	%hi(.L198),%o0
/* 0x0018	     */		add	%o0,%lo(.L198),%o0
/* 0x001c	     */		call	wn_assert_routine,2	! Result = %g0
/* 0x0020	     */		or	%g0,600,%o1

!  601		      !  wn_assert((ptr)(table->pkeys_eq_func) != NULL);

/* 0x0024	 601 */		ld	[%i0+8],%o0 ! volatile
                       .L900001610:
/* 0x0028	 601 */		cmp	%o0,0
/* 0x002c	     */		bne,a	.L900001609
/* 0x0030	     */		ld	[%i0+12],%o0 ! volatile
                       .L77000126:
/* 0x0034	     */		sethi	%hi(.L201),%o0
/* 0x0038	     */		add	%o0,%lo(.L201),%o0
/* 0x003c	     */		call	wn_assert_routine,2	! Result = %g0
/* 0x0040	     */		or	%g0,601,%o1

!  602		      !  wn_assert((ptr)(table->palloc_copy_func) != NULL);

/* 0x0044	 602 */		ld	[%i0+12],%o0 ! volatile
                       .L900001609:
/* 0x0048	 602 */		cmp	%o0,0
/* 0x004c	     */		bne,a	.L900001608
/* 0x0050	     */		ld	[%i0+16],%o0 ! volatile
                       .L77000128:
/* 0x0054	     */		sethi	%hi(.L204),%o0
/* 0x0058	     */		add	%o0,%lo(.L204),%o0
/* 0x005c	     */		call	wn_assert_routine,2	! Result = %g0
/* 0x0060	     */		or	%g0,602,%o1

!  603		      !  wn_assert((ptr)(table->pfree_func) != NULL);

/* 0x0064	 603 */		ld	[%i0+16],%o0 ! volatile
                       .L900001608:
/* 0x0068	 603 */		cmp	%o0,0
/* 0x006c	     */		bne,a	.L900001607
/* 0x0070	     */		ld	[%i0+20],%o0 ! volatile
                       .L77000130:
/* 0x0074	     */		sethi	%hi(.L207),%o0
/* 0x0078	     */		add	%o0,%lo(.L207),%o0
/* 0x007c	     */		call	wn_assert_routine,2	! Result = %g0
/* 0x0080	     */		or	%g0,603,%o1

!  604		      !  wn_assert(table->group != NULL);

/* 0x0084	 604 */		ld	[%i0+20],%o0 ! volatile
                       .L900001607:
/* 0x0088	 604 */		cmp	%o0,0
/* 0x008c	     */		bne,a	.L900001606
/* 0x0090	     */		ld	[%i0],%i1 ! volatile
                       .L77000132:
/* 0x0094	     */		sethi	%hi(.L210),%o0
/* 0x0098	     */		add	%o0,%lo(.L210),%o0
/* 0x009c	     */		call	wn_assert_routine,2	! Result = %g0
/* 0x00a0	     */		or	%g0,604,%o1

!  606		      !  binary_tree = table->binary_tree;

/* 0x00a4	 606 */		ld	[%i0],%i1 ! volatile
                       .L900001606:

!  608		      !  if(binary_tree != NULL)

/* 0x00a8	 608 */		cmp	%i1,0
/* 0x00ac	     */		be	.L77000136
/* 0x00b0	     */		nop
                       .L77000134:

!  609		      !  {
!  610		      !    verify_binary_tree(table,binary_tree,0);

/* 0x00b4	 610 */		call	verify_binary_tree,3	! Result = %g0	! (tail call)
/* 0x00b8	     */		restore	%g0,0,%o2
                       .L77000136:
/* 0x00bc	     */		ret
/* 0x00c0	     */		restore	%g0,%g0,%g0
/* 0x00c4	   0 */		.type	wn_hverify,2
/* 0x00c4	     */		.size	wn_hverify,(.-wn_hverify)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	4
!
! SUBROUTINE analyze_binary_tree
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       analyze_binary_tree:
/* 000000	     */		save	%sp,-96,%sp
/* 0x0004	     */		or	%g0,%i0,%l1

!  611		      !  }
!  612		      !}
!  616		      !#if 0
!  617		      !wn_hdump(table)
!  619		      !wn_htab table;
!  621		      !{
!  622		      !  dump_binary_tree(table->binary_tree,0);
!  623		      !}
!  627		      !local dump_binary_tree(binary_tree,level)
!  629		      !wn_binary_tree_data binary_tree;
!  630		      !int level;
!  632		      !{
!  633		      !  if(binary_tree != NULL)
!  634		      !  {
!  635		      !    print_spaces(2*level);
!  636		      !    printf("* hashed_key = %d\n",binary_tree->hashed_key);
!  638		      !    print_spaces(2*level);
!  639		      !    printf("left_children:\n");
!  640		      !    dump_binary_tree(binary_tree->left_child,level+1);
!  642		      !    print_spaces(2*level);
!  643		      !    printf("right_children:\n");
!  644		      !    dump_binary_tree(binary_tree->right_child,level+1);
!  645		      !  }
!  646		      !}
!  650		      !local print_spaces(n)
!  652		      !int n;
!  654		      !{
!  655		      !  int i;
!  657		      !  for(i=0;i<n;i++)
!  658		      !  {
!  659		      !    printf(" ");
!  660		      !  }
!  661		      !}
!  662		      !#endif
!  665		      !#if 1
!  666		      !#include <math.h>
!  669		      !local void analyze_binary_tree
!  670		      !(
!  671		      !  wn_binary_tree_data binary_tree,
!  672		      !  wn_binary_tree_data parent,
!  673		      !  int *pnum_entries,
!  674		      !  int *pdepth_sum,
!  675		      !  int *pnum_collisions,
!  676		      !  int level
!  677		      !)
!  678		      !{
!  679		      !  if(binary_tree != NULL)

/* 0x0008	 679 */		cmp	%i0,0
/* 0x000c	     */		be	.L77000144
/* 0x0010	     */		or	%g0,%i5,%l0
                       .L77000139:

!  680		      !  {
!  681		      !    (*pnum_entries)++;

/* 0x0014	 681 */		ld	[%i2],%o0 ! volatile
                       .L900001706:
/* 0x0018	 681 */		add	%o0,1,%o0
/* 0x001c	     */		st	%o0,[%i2] ! volatile

!  682		      !    (*pdepth_sum) += level;
!  684		      !    if((parent != NULL)&&(parent->hashed_key == binary_tree->hashed_key))

/* 0x0020	 684 */		cmp	%i1,0
/* 0x0024	 682 */		ld	[%i3],%o0 ! volatile
/* 0x0028	     */		add	%o0,%l0,%o0
/* 0x002c	     */		st	%o0,[%i3] ! volatile
/* 0x0030	 684 */		be,a	.L900001707
/* 0x0034	     */		add	%l0,1,%l0
                       .L77000140:
/* 0x0038	     */		ld	[%l1+8],%o0 ! volatile
/* 0x003c	     */		ld	[%i1+8],%o1 ! volatile
/* 0x0040	     */		cmp	%o1,%o0
/* 0x0044	     */		bne,a	.L900001707
/* 0x0048	     */		add	%l0,1,%l0
                       .L77000141:

!  685		      !    {
!  686		      !      ++(*pnum_collisions);

/* 0x004c	 686 */		ld	[%i4],%o0 ! volatile

!  687		      !    }
!  689		      !    analyze_binary_tree((binary_tree->children)[0],binary_tree,
!  690		      !                        pnum_entries,pdepth_sum,pnum_collisions,level+1);

/* 0x0050	 690 */		add	%l0,1,%l0
/* 0x0054	 686 */		add	%o0,1,%o0
/* 0x0058	     */		st	%o0,[%i4] ! volatile
                       .L900001707:
/* 0x005c	 690 */		ld	[%l1],%o0 ! volatile
/* 0x0060	     */		or	%g0,%l1,%o1
/* 0x0064	     */		or	%g0,%i2,%o2
/* 0x0068	     */		or	%g0,%i3,%o3
/* 0x006c	     */		or	%g0,%i4,%o4
/* 0x0070	     */		call	analyze_binary_tree,6	! Result = %g0
/* 0x0074	     */		or	%g0,%l0,%o5

!  691		      !    analyze_binary_tree((binary_tree->children)[1],binary_tree,
!  692		      !                        pnum_entries,pdepth_sum,pnum_collisions,level+1);

/* 0x0078	 692 */		ld	[%l1+4],%o0 ! volatile
/* 0x007c	     */		or	%g0,%l1,%i1
/* 0x0080	     */		or	%g0,%o0,%l1
/* 0x0084	     */		cmp	%o0,0
/* 0x0088	     */		bne,a	.L900001706
/* 0x008c	     */		ld	[%i2],%o0 ! volatile
                       .L77000144:
/* 0x0090	     */		ret
/* 0x0094	     */		restore	%g0,%g0,%g0
/* 0x0098	   0 */		.type	analyze_binary_tree,2
/* 0x0098	     */		.size	analyze_binary_tree,(.-analyze_binary_tree)

	.section	".text",#alloc,#execinstr
/* 000000	   0 */		.align	8
!
! CONSTANT POOL
!
                       .L_const_seg_900001801:
/* 000000	   0 */		.word	1073741824,0
/* 0x0008	   0 */		.align	4
!
! SUBROUTINE wn_hanalyze
!
! OFFSET    SOURCE LINE	LABEL	INSTRUCTION

                       	.global wn_hanalyze
                       wn_hanalyze:
/* 000000	     */		save	%sp,-152,%sp

!  693		      !  }
!  694		      !}
!  697		      !void wn_hanalyze(wn_htab table)
!  698		      !{
!  699		      !  int num_entries,depth_sum,num_collisions;
!  700		      !  double avg_depth,merit;
!  702		      !  num_entries = depth_sum = num_collisions = 0;

/* 0x0004	 702 */		st	%g0,[%fp-8]

!  704		      !  analyze_binary_tree(table->binary_tree,(wn_binary_tree_data)NULL,
!  705		      !		      &num_entries,&depth_sum,&num_collisions,1);

/* 0x0008	 705 */		add	%fp,-4,%o2
/* 0x000c	     */		add	%fp,-8,%o3
/* 0x0010	 702 */		st	%g0,[%fp-4]
/* 0x0014	 705 */		add	%fp,-12,%o4
/* 0x0018	     */		or	%g0,0,%o1
/* 0x001c	 702 */		st	%g0,[%fp-12]
/* 0x0020	 705 */		ld	[%i0],%o0 ! volatile
/* 0x0024	     */		call	analyze_binary_tree,6	! Result = %g0
/* 0x0028	     */		or	%g0,1,%o5

!  707		      !  if(num_entries > 0)

/* 0x002c	 707 */		ld	[%fp-4],%o1
/* 0x0030	     */		cmp	%o1,0
/* 0x0034	     */		ble	.L77000152
/* 0x0038	     */		ld	[%fp-8],%f2
                       .L77000151:

!  708		      !  {
!  709		      !    avg_depth = ((double)depth_sum)/((double)num_entries);

/* 0x003c	 709 */		ld	[%fp-4],%f4
/* 0x0040	     */		fitod	%f2,%f2
/* 0x0044	     */		fitod	%f4,%f4

!  710		      !    merit = avg_depth/(log((double)num_entries)/log(2.0));

/* 0x0048	 710 */		std	%f4,[%sp+96]
/* 0x004c	     */		ld	[%sp+96],%o0
/* 0x0050	     */		ld	[%sp+100],%o1
/* 0x0054	 709 */		fdivd	%f2,%f4,%f30
/* 0x0058	 710 */		call	log,2	! Result = %f0
/* 0x005c	     */		std	%f30,[%sp+112]
/* 0x0060	     */		sethi	%hi(.L_const_seg_900001801),%o0
/* 0x0064	     */		ldd	[%o0+%lo(.L_const_seg_900001801)],%f2
/* 0x0068	     */		std	%f2,[%sp+104]
/* 0x006c	     */		ld	[%sp+104],%o0
/* 0x0070	     */		ld	[%sp+108],%o1
/* 0x0074	     */		fmovs	%f0,%f30
/* 0x0078	     */		fmovs	%f1,%f31
/* 0x007c	     */		call	log,2	! Result = %f0
/* 0x0080	     */		std	%f30,[%sp+128]
/* 0x0084	     */		ldd	[%sp+128],%f30

!  712		      !    printf("num_entries = %d,avg depth=%lf,merit=%lf,#collisions=%d\n",
!  713		      !           num_entries,avg_depth,merit,num_collisions);

/* 0x0088	 713 */		ld	[%fp-12],%o0
/* 0x008c	 710 */		fdivd	%f30,%f0,%f2
/* 0x0090	     */		ldd	[%sp+112],%f30
/* 0x0094	 713 */		st	%o0,[%sp+92]
/* 0x0098	     */		sethi	%hi(.L372),%o0
/* 0x009c	     */		std	%f30,[%sp+112]
/* 0x00a0	     */		add	%o0,%lo(.L372),%o0
/* 0x00a4	     */		ld	[%fp-4],%o1
/* 0x00a8	     */		ld	[%sp+112],%o2
/* 0x00ac	     */		ld	[%sp+116],%o3
/* 0x00b0	 710 */		fdivd	%f30,%f2,%f2
/* 0x00b4	 713 */		std	%f2,[%sp+120]
/* 0x00b8	     */		ld	[%sp+120],%o4
/* 0x00bc	     */		call	printf,6	! Result = %g0
/* 0x00c0	     */		ld	[%sp+124],%o5
/* 0x00c4	     */		ret
/* 0x00c8	     */		restore	%g0,%g0,%g0
                       .L77000152:

!  714		      !  }
!  715		      !  else
!  716		      !  {
!  717		      !    printf("num_entries = %d,avg depth=--,merit=--,#collisions=--\n",
!  718		      !           num_entries);

/* 0x00cc	 718 */		sethi	%hi(.L374),%o0
/* 0x00d0	     */		call	printf,2	! Result = %g0
/* 0x00d4	     */		add	%o0,%lo(.L374),%o0
/* 0x00d8	     */		ret
/* 0x00dc	     */		restore	%g0,%g0,%g0
/* 0x00e0	   0 */		.type	wn_hanalyze,2
/* 0x00e0	     */		.size	wn_hanalyze,(.-wn_hanalyze)

! Begin Disassembling Stabs
	.xstabs	".stab.index","Xa ; V=3.1 ; R=SC4.0 18 Oct 1995 C 4.0",60,0,0,0	! (/tmp/acomp.10442.1.s:1)
	.xstabs	".stab.index","/remote/dtg442/naylor/wnlib/acc/hash; /depot/SUNWspro-V4N1/SC4.0/bin/acc -S -fast -I../h  -c wnhtab.c -Qoption acomp -xp",52,0,0,0	! (/tmp/acomp.10442.1.s:2)
! End Disassembling Stabs

! Begin Disassembling Ident
	.ident	"@(#)math.h\t2.5\t95/02/07"	! (/tmp/acomp.10442.1.s:28)
	.ident	"@(#)floatingpoint.h\t2.4 94/06/09"	! (/tmp/acomp.10442.1.s:29)
	.ident	"@(#)stdio.h\t1.39\t95/12/04 SMI"	! (/tmp/acomp.10442.1.s:30)
	.ident	"@(#)feature_tests.h\t1.7\t94/12/06 SMI"	! (/tmp/acomp.10442.1.s:31)
	.ident	"@(#)va_list.h\t1.6\t96/01/26 SMI"	! (/tmp/acomp.10442.1.s:32)
	.ident	"@(#)ieeefp.h\t2.7 94/11/09"	! (/tmp/acomp.10442.1.s:33)
	.ident	"acomp: SC4.0 18 Oct 1995 C 4.0"	! (/tmp/acomp.10442.1.s:41)
! End Disassembling Ident
