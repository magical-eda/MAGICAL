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

#include "wnargp.h"


bool recover;
char *control_filename,*error_filename,*in_filename,*out_filename;
int si = 5;
unsigned su = 2000000000;	/*     gcc can't cope with large
				** unsigned constants */
double d = 9.0;


int main(int argc,char *argv[])
{
  static wn_arg_format arg_format[] =
  {
    WN_KEY_BOOL(&recover,"r"),
    WN_KEY_STRING(&control_filename,"g","control_file",WN_REQUIRED),
    WN_KEY_STRING(&error_filename,"log","error_file",WN_OPTIONAL),
    WN_KEY_INT(&si, "si", "si_units", WN_OPTIONAL),
    WN_KEY_UNSIGNED(&su, "u", "u_units", WN_OPTIONAL),
    WN_KEY_DOUBLE(&d, "d", "d_units", WN_OPTIONAL),
    WN_POS_STRING(&in_filename,"in_file",WN_REQUIRED),
    WN_POS_STRING(&out_filename,"out_file",WN_OPTIONAL),
    WN_ARG_TERMINATE
  };
  
  error_filename = "dastoinf.log";  /* defaults */
  in_filename = "net.inf";
  
  wn_parse_args(argc,argv,arg_format);

  printf("recover = %d\n",recover);
  printf("control filename = <%s>\n",control_filename);
  printf("error_filename = <%s>\n",error_filename);
  printf("si_units = <%d>\n", si);
  printf("u_units = <%u>\n", su);
  printf("d_units = <%g>\n", d);
  printf("in_filename = <%s>\n", in_filename);
  printf("out_filename = <%s>\n", out_filename ? out_filename : "NULL");

  return(0);
}
