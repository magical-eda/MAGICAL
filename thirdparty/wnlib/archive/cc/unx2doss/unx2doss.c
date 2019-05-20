/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

/*****************************************************************
**
**	This program will translate all the files whose names are
**  given on the command line from unix format (lines ending with
**  \n) to dos format (lines ending with \r\n).  It assumes no one
**  wants to put a \r anywhere without a following \n, which is
**  the case for text files.
**	This program will work when run on unix or on dos.
**	Note that if this program is run on a file that is already
**  in dos format, no changes will be made to the file, so you
**  don't have to worry about accidentally running it twice on
**  the same file.
**
**	This program is part of wnlib and is in the public domain and may
**  be used by anybody for any purpose.  It comes asis with no form of
**  warrantee whatsoever.
**
**	Note to developers: this program is used for building
**  wnlib, that is, it is to be run prior to wnlib being built.
**  therefore, it cannot include or link with any wnlib code.
**
**  Note that on microsoft, to get wildcard expansion, you have
**  to link with setargv.obj.
**
**	Written: 2004.08.07
**	Author:  Bill Chapman
**
*****************************************************************/

static void lo_translate_one_fp_to_fp(FILE *fp_in, FILE *fp_out)
{
  int c;

  while ((c = getc(fp_in)) != EOF)
  {
	if ('\r' == c)
	/**/	       continue;	/* ignore cr's on input */

	if ('\n' == c)
	/**/	       putc('\r', fp_out);

	putc(c, fp_out);
  }
} /* lo_translate_one_fp_to_fp */


static void lo_translate_file_in_place(char *filename)
{
  FILE *fp_in, *fp_out;
  char *tmpfn = "unx2doss.tmp";
  struct stat statbuf;
  int sts;

  /* ignore directories */
  sts = stat(filename, &statbuf);
  if (sts) {
    fprintf(stderr, "unx2doss: couldn't find file %s, skipping\n",
    /**/						filename);
    return;
  }
  if (statbuf.st_mode & S_IFDIR)
  {
    fprintf(stderr, "unx2doss: skipping directory %s\n", filename);
    return;
  }

  fp_in = fopen(filename, "rb");
  assert(fp_in);
  fp_out = fopen(tmpfn, "wb");
  assert(fp_out);

  lo_translate_one_fp_to_fp(fp_in, fp_out);

  sts = fclose(fp_out);
  assert(!sts);
  sts = fclose(fp_in);
  assert(!sts);

  sts = unlink(filename);
  assert(!sts);

  sts = rename(tmpfn, filename);
  assert(!sts);
} /* lo_translate_file_in_place */


int main(int argc, char **argv)
{
  int i;

  for (i = 1;  i < argc;  ++i)
  {
    lo_translate_file_in_place(argv[i]);
  }

  return 0;
} /* main */
