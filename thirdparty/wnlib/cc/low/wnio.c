/**********************************************************************

wn_ioerrfpush(pfunc)     (*pfunc)(operation,file,failure)
wn_ioerrfpop()

wn_search_path_for_file(&result,path,filename)

wn_file wn_fopen(file_name,mode)
wn_fclose(stream)
wn_fgets(s,n,stream)
wn_fputs(s,stream)
bool wn_feof(f)
int wn_getchar()
int wn_getc(stream)
wn_ungetc(c,stream)
int wn_putc(c,stream)
wn_fflush(stream)

wn_file wn_popen(command,mode)
wn_pclose(stream)

int wn_open(path,flags,mode)
wn_close(d)
wn_write(d,buf,nbytes)
wn_read(d,buf,nbytes)

wn_mv(from,to)
wn_rm(path)
wn_rmf(path)
wn_mkdir(dir_name,mode)
wn_rmdir(dir_name)
wn_cd(dir_name)
wn_pushd(dir_name)
wn_popd()
wn_pwd(dir_name)

bool wn_file_exists(path)
bool wn_is_directory(path)

**********************************************************************/
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
#include <errno.h>
#ifdef vms
#include <stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#endif
#define wnioC
#include "wnio.h"


#define DEFAULT_OPEN_MODE    00644
#define DEFAULT_MKDIR_MODE   00755

#define STRING_LEN           500


local wn_stack io_stack = NULL,
               directory_stack = NULL;

#ifdef sun
extern char *sprintf();
#endif



local void default_error_function(operation,file,failure)

char operation[],file[],failure[];

{
  (void)fprintf(wn_stderr,"io failure: op=<%s>,file=<%s>,failure=<%s>\n",
                operation,file,failure);
  exit(-1);
}

typedef void (*void_func)();
local void_func pcurrent_error_func = (default_error_function);

wn_ioerrfpush(pfunc)

void (*pfunc)(/*operation,file,failure*/);

{
  if(io_stack == NULL)
  {
    wn_gppush(wn_defaultgp());

    wn_mkstack(&io_stack);

    wn_gppop();
  }

  wn_push(io_stack,(ptr)pcurrent_error_func);
  pcurrent_error_func = pfunc;
}



wn_ioerrfpop()
{
  pcurrent_error_func = (void_func)wn_stacktop(io_stack);
  wn_pop(io_stack);
}



local translate_errno(pfailure)

char *pfailure[];

{
  extern char *sys_errlist[];

  *pfailure = sys_errlist[errno];
  /*
  static char failure[300];

  (void)sprintf(failure,"%d",errno);
  *pfailure = failure;
  */
}



wn_search_path_for_file(presult,path,filename)

char **presult,*path,*filename;

{
  int i;
  static char filepath[257];

  *presult = NULL;

  while(*path != '\0')
  {
    while((*path == ' ')||(*path == '\t'))  /* skip blanks */
    {
      ++path;
    }

    if(*path == '\0')
    {
      break;
    }

    i = 0;
    while(not((*path == ' ')||(*path == '\t')||(*path == '\0')))
    {
      filepath[i] = *path;
      ++path; ++i;
    }
    filepath[i] = '\0';
    wn_strcat(filepath,"/");
    wn_strcat(filepath,filename);

    if(wn_file_exists(filepath))
    {
      *presult = filepath;

      break;
    }
  }
}



wn_file wn_fopen(file_name,type)

char file_name[],type[];

{
  extern FILE *fopen();
  char *failure;
  wn_file ret;

  errno = 0;

  ret = fopen(file_name,type);

  if(ret == NULL)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_fopen",file_name,failure);
  }

  return(ret);
}



wn_fclose(f)

wn_file f;

{
  int ret;
  char *failure;

  errno = 0;

  ret = fclose(f);

  if(ret == EOF)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_fclose","?",failure);
  }

  return(ret);
}



wn_fgets(s,n,stream)

char *s;
int n;
wn_file stream;

{
  char *ret;
  char *failure;

  errno = 0;

  ret = fgets(s,n,stream);

  if((ret == NULL)&&(errno != 0))
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_fgets","?",failure);
  }

  return((int)ret);
}
   


wn_fputs(s,stream)

char *s;
wn_file stream;

{
  int ret;
  char *failure;

  errno = 0;

  ret = fputs(s,stream);

  if((ret == EOF)&&(not(wn_streq(s,""))))
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_fputs","?",failure);
  }

  return(ret);
}
   


int wn_getchar()
{
  return(wn_getc(wn_stdin));
}



int wn_getc(stream)

wn_file stream;

{
  int ret;
  char *failure;

  errno = 0;

  ret = getc(stream);

  if((ret == EOF)&&(errno != 0))
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_getc","?",failure);
  }

  return(ret);
}
   


wn_ungetc(c,stream)

int c;
wn_file stream;

{
  int ret;
  char *failure;

  errno = 0;

  ret = ungetc(c,stream);

  if(ret == EOF)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_ungetc","?",failure);
  }

  return(ret);
}
   


int wn_putc(c,stream)

char c;
wn_file stream;

{
  int ret;
  char *failure;

  errno = 0;

  ret = putc(c,stream);

  if((ret == EOF)&&(errno != 0))
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_getc","?",failure);
  }

  return(ret);
}



bool wn_feof(f)

wn_file f;

{
  return(feof(f));
}



int wn_fflush(stream)

wn_file stream;

{
  int ret;
  char *failure;

  ret = fflush(stream);

  if(ret != 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_fflush","?",failure);
  }

  return(ret);
}



wn_file wn_popen(command,type)

char command[],type[];

{
  extern FILE *popen();
  char *failure;
  wn_file ret;

  errno = 0;

  ret = popen(command,type);

  if(ret == NULL)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_popen",command,failure);
  }

  return(ret);
}



wn_pclose(f)

wn_file f;

{
  int ret;
  char *failure;

  errno = 0;

  ret = pclose(f);

  if(ret == EOF)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_pclose","?",failure);
  }

  return(ret);
}



int wn_open(path,flags,mode)

char *path;
int flags,mode;

{
  int ret;
  char *failure;

  errno = 0;

  ret = open(path,flags,mode);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_open",path,failure);
  }

  return(ret);
}
   


wn_close(d)

int d;

{
  int ret;
  char *failure;

  errno = 0;

  ret = close(d);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_close","?",failure);
  }

  return(ret);
}
   


wn_write(d,buf,nbytes)

int d;
char *buf;
int nbytes;

{
  int ret;
  char *failure;

  errno = 0;

  ret = write(d,buf,nbytes);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_write","?",failure);
  }

  return(ret);
}
   


wn_read(d,buf,nbytes)

int d;
char *buf;
int nbytes;

{
  int ret;
  char *failure;

  errno = 0;

  ret = read(d,buf,nbytes);

  if(ret <= 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_read","?",failure);
  }

  return(ret);
}
   


wn_mv(from,to)

char from[],to[];

{
  int ret;
  char *failure;

  errno = 0;

  ret = rename(from,to);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_mv","?",failure);
  }

  return(ret);
}



wn_rm(path)

char path[];

{
  int ret;
  char *failure;

  errno = 0;

#ifdef vms
  ret = delete(path);
#else
  ret = unlink(path);
#endif

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_rm","?",failure);
  }

  return(ret);
}



wn_rmf(path)

char *path;

{
#ifdef vms
  (void)delete(path);
#else
  (void)unlink(path);
#endif
}



wn_mkdir(dir_name)

char dir_name[];

{
  int ret;
  char *failure;

  errno = 0;

  ret = mkdir(dir_name,DEFAULT_MKDIR_MODE);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_mkdir","?",failure);
  }

  return(ret);
}



wn_rmdir(dir_name)

char dir_name[];

{
  int ret;
  char *failure;

  errno = 0;

#ifdef vms
  ret = -1; /* no implementation is available on vms */
#else
  ret = rmdir(dir_name);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_rmdir","?",failure);
  }
#endif

  return(ret);
}



wn_cd(dir_name)

char dir_name[];

{
  int ret;
  char *failure;

  errno = 0;

  ret = chdir(dir_name);

  if(ret < 0)
  {
    translate_errno(&failure);
    (*pcurrent_error_func)("wn_cd","?",failure);
  }

  return(ret);
}



wn_pushd(dir_name)

char dir_name[];

{
  char old_dir[STRING_LEN],*old_dir_copy;

  wn_gppush(wn_defaultgp());

  wn_pwd(old_dir);
  wn_stracpy(&old_dir_copy,old_dir);

  if(directory_stack == NULL)
  {
    wn_mkstack(&directory_stack);
  }
  wn_push(directory_stack,(ptr)old_dir_copy);

  wn_cd(dir_name);

  wn_gppop();
}



wn_popd()
{
  char *old_dir_copy;

  wn_gppush(wn_defaultgp());

  old_dir_copy = (char *)wn_stacktop(directory_stack);
  wn_pop(directory_stack);

  wn_cd(old_dir_copy);

  wn_free((ptr)old_dir_copy);

  wn_gppop();
}



wn_pwd(dir_name)

char dir_name[];

{
#ifdef vms
  extern char *getcwd();
#else
  extern char *getwd();
#endif
  int ret;
  char *failure;

#ifdef vms
  ret = (int)getcwd(dir_name,STRING_LEN);
#else
  ret = (int)getwd(dir_name);
#endif

  if(ret == 0)
  {
    failure = dir_name;
    (*pcurrent_error_func)("wn_pwd","<none>",failure);
  }

  return(ret);
}



bool wn_file_exists(path)

char path[];

{
#ifdef vms
  char dirpath[255];
#endif
  struct stat statbuf;

  if (stat(path,&statbuf) < 0)
  {
#ifdef vms
    strcpy(dirpath,path);
    strcat(dirpath,".dir;1");
    if (stat(dirpath,&statbuf) < 0)
    {
      return(FALSE);
    }
    else
    {
      return(TRUE);
    }
#else
    return(FALSE);
#endif
  }
  else
  {
    return(TRUE);
  }
}



bool wn_is_directory(path)

char path[];

{
  struct stat statbuf;
#ifdef vms
  char dirpath[255];

  strcpy(dirpath,path);
  strcat(dirpath,".dir;1");
#else
  char *dirpath = path;
#endif

  if (stat(dirpath,&statbuf) < 0)
  {
    return(FALSE);
  }
  else
  {
    if (statbuf.st_mode & S_IFDIR)
    {
      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
  }
}
