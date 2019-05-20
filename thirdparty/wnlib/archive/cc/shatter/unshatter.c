/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/

/******************************************************************************
******************************************************************************/

#include <signal.h>
#include <stdio.h>
#include "wnlib.h"

#include "wncstr.h"



local FILE *read_tty,*write_tty,*in_f;
local int tape_number,char_number,tape_char_number;


void main()
{
  int c,code;
  char command[500],tty_input[500];

  read_tty = fopen("/dev/tty","r");
  write_tty = fopen("/dev/tty","w");

  strcpy(command,"");
  char_number = 0;

  for(tape_number=0;;++tape_number)
  {
    fprintf(write_tty,"unshatter: INSERT TAPE #%d\n",tape_number);
    fprintf(write_tty,
	    "unshatter: enter input command <return for last command> : ");
    fflush(write_tty);
    fgets(tty_input, 500,read_tty);
    if(wn_streq(tty_input,"\n"))
    {
      fprintf(write_tty,"unshatter: using command : %s\n",command);
      fflush(write_tty);
    }
    else if(wn_streq(tty_input,"quit\n"))
    {
      break;
    }
    else
    {
      strcpy(command,tty_input);
    }

    in_f = popen(command,"r");
    if(in_f == NULL)
    {
      fprintf(write_tty,"unshatter: illegal command <%s>.\n",command);
      fflush(write_tty);
      exit(-1);
    }

    for(tape_char_number=0;;++tape_char_number)
    {
      c = getc(in_f);
      if(c == EOF)
      {
	break;
      }

      code = putchar(c);
      ++char_number;

      if(code == EOF)
      {
        fprintf(write_tty,"unshatter: end of file on output, char = %d.\n",
			  tape_char_number);
        fflush(write_tty);
        exit(-1);
      }
    }

    fprintf(write_tty,"unshatter: %d bytes read!!!!!!\n",
		      tape_char_number);
    pclose(in_f);

    fprintf(write_tty,"unshatter: tape #%d finished.!!!!!!\n",
		      tape_number);
  }

  fprintf(write_tty,"unshatter: tape #%d finished. %d bytes read!!!!!!\n",
	            tape_number,tape_char_number);
  fprintf(write_tty,"unshatter: total %d bytes read!!!!!!\n",
		    char_number);
  fprintf(write_tty,"unshatter: finished!!!!!!\n");

  fclose(read_tty);
  fclose(write_tty);

  return(0);
}

