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
#ifndef wnioH
#define wnioH


WN_EXTERN_BEGIN

#if !defined WN_WINDOWS		/* don't need file.h at all in windows */
# if  defined WN_VMS
#   include <file.h>
#  else
#   include <sys/file.h>
# endif
#endif /* !windows */

#include <stdio.h>


#define    STDIN     0
#define    STDOUT    1
#define    STDERR    2


typedef FILE wn_FILE;
typedef wn_FILE *wn_file;
typedef void (*error_func_type)(char operation[],char file[],char failure[]);

#if defined(WN_GUARD_WNIO)  &&  defined(WN_DONT_GUARD_WNIO)
# error inconsistent flags set about guarding wnio calls
#endif
#if !defined(WN_DONT_GUARD_WNIO)

# define fopen   wnio_fopen_illegal_see_wnio_h;
# define fclose  wnio_fclose_illegal_see_wnio_h;
# define fputs   wnio_fputs_illegal_see_wnio_h;
# define fgets   wnio_fgets_illegal_see_wnio_h;
# undef putc
# define putc    wnio_putc_illegal_see_wnio_h;
# undef getc
# define getc    wnio_getc_illegal_see_wnio_h;
# undef feof
# define feof    wnio_feof_illegal_see_wnio_h;

# define open    wnio_open_illegal_see_wnio_h;
# define close   wnio_close_illegal_see_wnio_h;
# define write   wnio_write_illegal_see_wnio_h;
# define read    wnio_read_illegal_see_wnio_h;

#endif /* !WN_DONT_GUARD_WNIO */


#define wn_fread fread
#define wn_fwrite fwrite


#define wn_stdout stdout
#define wn_stdin stdin
#define wn_stderr stderr


extern void wn_ioerrfpush(error_func_type pfunc);
extern void wn_ioerrfpop(void);

extern void wn_search_path_for_file(char **presult,char *path,char *filename);

extern wn_file wn_fopen(char file_name[],char type[]);
extern int wn_fclose(wn_file f);

extern int wn_fgets(char *s,int n,wn_file stream);
extern int wn_fputs(char *s,wn_file stream);
extern int wn_getchar(void);
extern int wn_getc(wn_file stream);
extern int wn_ungetc(int c,wn_file stream);
extern int wn_putc(char c,wn_file stream);
extern bool wn_feof(wn_file f);
extern int wn_fflush(wn_file stream);

#ifndef WN_WINDOWS
  /* these aren't working on windows yet */
  extern wn_file wn_popen(char command[],char type[]);
  extern int     wn_pclose(wn_file f);
#endif

extern int wn_open(char *path,int flags,int mode);
extern int wn_close(int d);

extern int wn_write(int d,char *buf,int nbytes);
extern int wn_read(int d,char *buf,int nbytes);

extern int wn_mv(char from[],char to[]);
extern int wn_cp(char from[],char to[]);
extern int wn_rm(char path[]);
extern void wn_rmf(char *path);
extern int wn_mkdir(char dir_name[]);
#if !defined(WN_VMS)
  extern int wn_rmdir(char dir_name[]);
#endif
extern int wn_cd(char dir_name[]);
extern void wn_pushd(char dir_name[]);
extern void wn_popd(void);
extern int wn_pwd(char dir_name[]);	/* note this assumes the
**	buffer 'dir_name' is at least WN_MAX_FILE_NAME_PATH_LEN
** 	in length. */
extern bool wn_file_exists(char path[]);
extern bool wn_is_directory(char path[]);

/*     buffer size for storing file names - max length, in chars, of an absolute
** file name, + 1 (for the \0) */
#define WN_MAX_FILE_NAME_PATH_LEN	(1024*10)

WN_EXTERN_END

#endif /* wnioH */

