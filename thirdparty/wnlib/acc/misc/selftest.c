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

#include <stdio.h>
#include <stdlib.h>
#define __NO_STRING_INLINES  /* long strcmp macro in asserts gets c89 upset */
#include <string.h>
#include <fcntl.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnmemw.h"
#include "wnbvec.h"
#include "wnrndd.h"
#include "wnrnd.h"
#include "wnstr.h"


#include "wnio.h"


local void lo_test_bitvect(void)
{
  wn_bitvect bv, bv2, bv3;
  int bv_length;
  int bit_count;
  bool ibool;
  int i, j;

  fprintf(stderr,"testing bit vector stuff...\n");

  for (i = 1000;  i > 0;  --i) {
    if (i < 600)
    {
      bv_length = i;
    }
    else
    {
      bv_length = wn_random_int_between(601, 3000);
    }

    wn_bitvect_make(&bv,  bv_length);
    wn_bitvect_make(&bv2, bv_length);
    wn_bitvect_make(&bv3, bv_length);

#   if 0
      /*   purify didn't like this, said it was reads before sets, which it
      ** was */
      /*   somehow only 64 bit purify caught this, it didn't show up on 32 bit
      ** purify */

      /* randomly tromp on it */
      for (j = 0;  j < wn_min(bv_length/2, 40);  ++j)
      {
	wn_bitvect_set_bit(TRUE, bv,  wn_random_int_between(0, bv_length));
	wn_bitvect_set_bit(TRUE, bv2, wn_random_int_between(0, bv_length));
	wn_bitvect_set_bit(TRUE, bv3, wn_random_int_between(0, bv_length));
      }
#   endif

    wn_bitvect_set_all_false(bv,  bv_length);
    wn_bitvect_set_all_false(bv2, bv_length);

    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv, j);
      wn_assert(FALSE == ibool);
    }

    for (j = 0;  j < bv_length;  j += 7)
    {
      wn_bitvect_set_bit(TRUE, bv, j);
    }

    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv, j);
      wn_assert(ibool == ! (j % 7));
    }

    for (j = 0;  j < bv_length;  j += 9)
    {
      wn_bitvect_set_bit(FALSE, bv, j);
    }

    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv, j);
      wn_assert(ibool == (! (j % 7)  &&  (j % 9)));
    }

    wn_bitvect_set_all_false(bv,  bv_length);
    for (j = 0;  j < bv_length;  j += 5)
    {
      wn_bitvect_set_bit(TRUE, bv, j);
    }
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv, j);
      wn_assert(ibool == ! (j % 5));
    }

    wn_bitvect_set_all_false(bv2,  bv_length);
    for (j = 0;  j < bv_length;  j += 17)
    {
      wn_bitvect_set_bit(TRUE, bv2, j);
    }
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv2, j);
      wn_assert(ibool == ! (j % 17));
    }

    wn_bitvect_set_all_true(bv2,  bv_length);
    for (j = 0;  j < bv_length;  j += 5)
    {
      wn_bitvect_set_bit(FALSE, bv2, j);
    }
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv2, j);
      wn_assert(ibool == !! (j % 5));
    }
    wn_bitvect_not(bv2, bv2, j);
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv2, j);
      wn_assert(ibool == ! (j % 5));
    }

    wn_bitvect_set_all_false(bv2,  bv_length);
    for (j = 0;  j < bv_length;  j += 11)
    {
      wn_bitvect_set_bit(TRUE, bv2, j);
    }
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv2, j);
      wn_assert(ibool == ! (j % 11));
    }

    wn_bitvect_set_all_false(bv3,  bv_length);
    wn_bitvect_and(bv3, bv2, bv, bv_length);
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv3, j);
      wn_assert(ibool == (!(j % 11)  &&  !(j % 5)));
    }

    wn_bitvect_or( bv3, bv2, bv, bv_length);
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv3, j);
      wn_assert(ibool == (!(j % 11)  ||  !(j % 5)));
    }

    wn_bitvect_xor(bv3, bv2, bv, bv_length);
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv3, j);
      wn_assert(ibool == (!(j % 11)  !=  !(j % 5)));
    }

    wn_bitvect_not(bv3, bv, bv_length);
    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv3, j);
      wn_assert(ibool == !!(j % 5));
    }

    wn_bitvect_set_all_true(bv,  bv_length);
    wn_bitvect_set_all_true(bv2, bv_length);

    for (j = 0;  j < bv_length;  ++j)
    {
      wn_bitvect_get_bit(&ibool, bv,  j);
      wn_assert(TRUE == ibool);
      wn_bitvect_get_bit(&ibool, bv2, j);
      wn_assert(TRUE == ibool);
    }

    wn_assert(wn_bitvect_count_set_bits(bv, bv_length) == bv_length);
    wn_bitvect_set_all_false(bv,  bv_length);
    wn_assert(wn_bitvect_count_set_bits(bv, bv_length) == 0);

    bit_count = 0;
    wn_bitvect_set_all_false(bv,  bv_length);
    for (j = 0;  j < bv_length;  ++j)
    {
      if (wn_random_bit())
      {
	wn_bitvect_set_bit(TRUE, bv, j);
	++bit_count;
      }
    }
    wn_assert(wn_bitvect_count_set_bits(bv, bv_length) == bit_count);
    wn_assert(wn_bitvect_xor_all_bits(bv, bv_length) == (bit_count & 1));
    wn_assert(wn_bitvect_count_set_bits(bv, bv_length) == bit_count);

    wn_bitvect_free(bv);
    wn_bitvect_free(bv2);
    wn_bitvect_free(bv3);
  } /* for i */

  fprintf(stderr,"  ok!!!!!!\n");
} /* lo_test_bitvect */


local void lo_test_io(void)
{
  char *filename = "a.tmp", *dirmid = "bdir.tmp", *cp_filename = "cp.tmp";
  char *full_mid, *full_cp;
  char dirtop[10000], buf[10000];
  char *stra = "arf arf\n";
  char *strb = "meow me";
  char *strc = "eow\nwoof woof\n";
  char *strd = "now be the time 4 all good men to come to the aid";
  char *full_filename, *full_dirmid;
  char *core1;
  wn_file fp, fp_cp;
  int fd;
  char *pc;
  int  c, c2, i;
  int chars_read, chars_written;
  bool sts;

  fprintf(stderr,"testing io stuff...\n");

  wn_gpmake("no_free");
    sts = !!wn_pwd(dirtop);
    wn_assert(sts);

    wn_stracat3(&full_filename, dirtop, WN_OSSLASH, filename);
    wn_stracat3(&full_dirmid, dirtop, WN_OSSLASH, dirmid);
    wn_stracat3(&full_mid, full_dirmid, WN_OSSLASH, filename);
    wn_stracat3(&full_cp, full_dirmid, WN_OSSLASH, cp_filename);

    wn_stracat3(&core1, full_dirmid, WN_OSSLASH, "core");

    wn_rmf(filename);
    wn_rmf(full_mid);
    wn_rmf(full_cp);
    wn_rmf("core");
    wn_rmf(core1);
#   ifndef WN_VMS
      if (wn_file_exists(dirmid))
      /**/			  wn_rmdir(dirmid);
#   endif

    wn_assert(!wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(!wn_file_exists(full_filename));
    wn_assert(!wn_is_directory(full_filename));

    fp = wn_fopen(filename, "wb");
    wn_assert(fp);
    wn_fflush(fp);
    wn_assert(wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(wn_file_exists(full_filename));
    wn_assert(!wn_is_directory(full_filename));

    wn_fputs("arf arf\n", fp);
    wn_fputs("meow meow\n", fp);
    {
      char *woofstr = "woof woof\n", *pc;
      for (pc = woofstr;  *pc;  ++pc) {
        wn_putc(*pc, fp);
      }
    }

    wn_fclose(fp);

    wn_assert(wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(wn_file_exists(full_filename));
    wn_assert(!wn_is_directory(full_filename));

    fp = wn_fopen(filename, "r");
    wn_assert(fp);

    wn_assert(!wn_feof(fp));

    sts = !!wn_fgets(buf, sizeof(buf), fp);
    wn_assert(sts);
    wn_assert(!strcmp(buf, stra));

    wn_assert(!wn_feof(fp));

    for (pc = strb;  *pc;  ++pc) {
      c = wn_getc(fp);
      wn_assert(EOF != c);
      /*     not sure if char is signed or unsigned on this platform, but
      ** it should be an 8 bit quantity */
      wn_assert(-128 <= c  &&  c <= 255);
      wn_assert(c == *pc);
    }

    c2 = wn_ungetc(pc[-1], fp);
    wn_assert('e' == c2);

    for (pc = strc;  *pc;  ++pc) {
      c = wn_getc(fp);
      wn_assert(EOF != c);
      /*     not sure if char is signed or unsigned on this platform, but
      ** it should be an 8 bit quantity */
      wn_assert(-128 <= c  &&  c <= 255);
      wn_assert(c == *pc);
    }

    /*   note we're at eof, but feof sometimes doesn't seem to realize this
	** until you actually try to read from the end */

    c2 = wn_ungetc(pc[-1], fp);
    wn_assert('\n' == c2);

    wn_assert(!wn_feof(fp));

    c = wn_getc(fp);
    wn_assert('\n' == c);

    c = wn_getc(fp);
    wn_assert(EOF == c);
    wn_assert(wn_feof(fp));

    wn_fclose(fp);

    wn_assert(wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(wn_file_exists(full_filename));
    wn_assert(!wn_is_directory(full_filename));

    wn_mkdir(dirmid);
    wn_assert(wn_file_exists(dirmid));
    wn_assert(wn_is_directory(dirmid));
    wn_assert(wn_file_exists(full_dirmid));
    wn_assert(wn_is_directory(full_dirmid));

    wn_stracat3( &full_filename, full_dirmid, WN_OSSLASH, filename);

    sts = wn_cp(filename, full_filename);
    wn_assert(!sts);
    wn_assert(wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert( wn_file_exists( full_filename));
    wn_assert(!wn_is_directory(full_filename));

    wn_rmf(full_filename);
    wn_assert(!wn_file_exists( full_filename));
    wn_assert(!wn_is_directory(full_filename));

    sts = wn_mv(filename, full_filename);
    wn_assert(!sts);

    wn_assert(!wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(wn_file_exists( full_filename));
    wn_assert(!wn_is_directory(full_filename));

    wn_assert(wn_file_exists( dirmid));
    wn_assert(wn_is_directory(dirmid));
    wn_assert(wn_file_exists( full_dirmid));
    wn_assert(wn_is_directory(full_dirmid));

    sts = wn_mv(full_filename, filename);
    wn_assert(!sts);

    wn_assert(wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(!wn_file_exists( full_filename));
    wn_assert(!wn_is_directory(full_filename));

    sts = wn_mv(filename, dirmid);
    wn_assert(!sts);

    wn_assert(!wn_file_exists(filename));
    wn_assert(!wn_is_directory(filename));
    wn_assert(wn_file_exists( full_filename));
    wn_assert(!wn_is_directory(full_filename));

    fd = wn_open(full_filename, O_RDWR, 0777);
    wn_assert(2 < fd);

    chars_read = wn_read(fd, buf, strlen(stra));
    wn_assert((int) strlen(stra) == chars_read);
    buf[chars_read] = 0;
    wn_assert(!strcmp(stra, buf));
    chars_written = wn_write(fd, strd, strlen(strd));
    wn_assert((int) strlen(strd) == chars_written);

    /* there's no wn_lseek, let's go back to the start of the file */
    wn_close(fd);
    fd = wn_open(full_filename, O_RDONLY, 0777);
    wn_assert(2 < fd);

    i = wn_read(fd, buf, strlen(stra));
    wn_assert((int) strlen(stra) == i);
    buf[i] = 0;
    wn_assert(!strcmp(stra, buf));

    i = wn_read(fd, buf, strlen(strd));
    wn_assert((int) strlen(strd) == i);
    buf[i] = 0;
    wn_assert(!strcmp(strd, buf));

    i = wn_read(fd, buf, sizeof(buf));
    wn_assert(0 == i);

    wn_close(fd);

    sts = wn_cd(full_dirmid);	wn_assert(!sts);
    wn_pwd(buf);
    wn_assert(!strcmp(buf, full_dirmid));

    wn_pushd("..");
    wn_pwd(buf);
    wn_assert(!strcmp(buf, dirtop));

    wn_popd();
    wn_pwd(buf);
    wn_assert(!strcmp(buf, full_dirmid));

#   ifndef WN_WINDOWS
      /* well, I tried getting wn_popen working on windows.
      ** total disaster - blue screen of death on first input. */
#     ifdef WN_WINDOWS
	sprintf(buf, "type %s", filename);
	fp = wn_popen(buf, "rt");
#     else
	sprintf(buf, "cat  %s", filename);
	fp = wn_popen(buf, "r");
#     endif

      sts = !!wn_fgets(buf, sizeof(buf), fp);
      wn_assert(sts);
      wn_assert(!strcmp(buf, stra));

      sts = !!wn_fgets(buf, sizeof(buf), fp);
      wn_assert(sts);
      wn_assert(!strcmp(buf, strd));

      sts = !!wn_fgets(buf, sizeof(buf), fp);
      wn_assert(!sts);

      wn_fclose(fp);
#   endif /* !WN_WINDOWS */

    wn_assert( wn_file_exists(filename));
    wn_cp(filename, cp_filename);

    fp    = wn_fopen(   filename, "rb");
    wn_assert(fp);
    fp_cp = wn_fopen(cp_filename, "rb");
    wn_assert(fp_cp);

    while (EOF != (c = wn_getc(fp))) {
      wn_assert(wn_getc(fp_cp) == c);
    }
    wn_assert(EOF == wn_getc(fp_cp));

    wn_fclose(fp);
    wn_fclose(fp_cp);

    wn_assert( wn_file_exists(full_filename));
    wn_rm(full_filename);
    wn_assert(!wn_file_exists(     filename));
    wn_assert(!wn_file_exists(full_filename));

    wn_assert( wn_file_exists(cp_filename));
    wn_rm(cp_filename);
    wn_assert(!wn_file_exists(cp_filename));

    sts = wn_cd("..");	wn_assert(!sts);

    wn_assert(wn_file_exists( dirmid));
    wn_assert(wn_is_directory(dirmid));
    wn_assert(wn_file_exists( full_dirmid));
    wn_assert(wn_is_directory(full_dirmid));
#   ifndef WN_VMS
      wn_rmdir(dirmid);
#   endif
    wn_assert(!wn_file_exists( dirmid));
    wn_assert(!wn_is_directory(dirmid));
    wn_assert(!wn_file_exists( full_dirmid));
    wn_assert(!wn_is_directory(full_dirmid));

    wn_gppop();

  fprintf(stderr,"  ok!!!!!!\n");
} /* lo_test_io */


int main(void) {
  lo_test_bitvect();
  lo_test_io();

  return 0;
} /* main */
