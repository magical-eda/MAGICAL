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

#ifdef vms
#include <file.h>
#else
#include <sys/file.h>
#endif

#include <stdio.h>


#define    STDIN     0
#define    STDOUT    1
#define    STDERR    2


typedef FILE wn_FILE;
typedef wn_FILE *wn_file;


extern wn_file wn_fopen();


#ifndef wnioC


#define fopen   fopen_illegal;
#define fclose  fopen_illegal;
#define fputs   fputs_illegal;
#define fgets   fgets_illegal;
#undef putc
#define putc    putc_illegal;
#undef getc
#define getc    getc_illegal;
#undef feof
#define feof    feof_illegal;

#define open    open_illegal;
#define close   close_illegal;
#define write   write_illegal;
#define read    read_illegal;


#define wn_fread fread
#define wn_fwrite fwrite


#endif

#define wn_stdout stdout
#define wn_stdin stdin
#define wn_stderr stderr

