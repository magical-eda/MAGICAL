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



local FILE *read_tty,*write_tty,*out_f;
local int tape_number,char_number,tape_char_number,tape_size;


local void print_status_and_quit()
{
  fprintf(write_tty,"shatter: <interrupt>\n");
  fprintf(write_tty,"shatter: tape #%d interrupted. %d bytes written.\n",
		      tape_number,tape_char_number);
  fflush(write_tty);

  exit(-1);
}


void main()
{
  int c,code;
  char command[500],tty_input[500];

  signal(SIGINT,(print_status_and_quit));

  read_tty = fopen("/dev/tty","r");
  write_tty = fopen("/dev/tty","w");

  strcpy(command,"");
  tape_size = 1;
  char_number = 0;

  for(tape_number=0;;++tape_number)
  {
    fprintf(write_tty,"shatter: INSERT TAPE #%d\n",tape_number);
    fprintf(write_tty,"shatter: enter tape size <return for last size> : ");
    fflush(write_tty);
    fgets(tty_input, 500,read_tty);
    if(wn_streq(tty_input,"\n"))
    {
      fprintf(write_tty,"shatter: using tape size = %d\n",tape_size);
      fflush(write_tty);
    }
    else
    {
      sscanf(tty_input,"%d",&tape_size);
    }

    fprintf(write_tty,
	    "shatter: enter output command <return for last command> : ");
    fflush(write_tty);
    fgets(tty_input, 500,read_tty);
    if(wn_streq(tty_input,"\n"))
    {
      fprintf(write_tty,"shatter: using command : %s\n",command);
      fflush(write_tty);
    }
    else
    {
      strcpy(command,tty_input);
    }

    out_f = popen(command,"w");
    if(out_f == NULL)
    {
      fprintf(write_tty,"shatter: illegal command <%s>.\n",command);
      fflush(write_tty);
      exit(-1);
    }

    for(tape_char_number=0;tape_char_number<tape_size;++tape_char_number)
    {
      c = getchar();
      if(c == EOF)
      {
	goto finished;
      }

      code = putc(c,out_f);
      ++char_number;

      if(code == EOF)
      {
        fprintf(write_tty,"shatter: end of file on output, char = %d.\n",
			  tape_char_number);
        fflush(write_tty);
        exit(-1);
      }
    }

    fprintf(write_tty,"shatter: %d bytes written!!!!!!\n",
		      tape_char_number);
    pclose(out_f);

    fprintf(write_tty,"shatter: tape #%d finished.!!!!!!\n",
		      tape_number);
  }

finished:
  pclose(out_f);

  fprintf(write_tty,"shatter: tape #%d finished. %d bytes written!!!!!!\n",
	            tape_number,tape_char_number);
  fprintf(write_tty,"shatter: total %d bytes written!!!!!!\n",
		    char_number);
  fprintf(write_tty,"shatter: finished!!!!!!\n");

  fclose(read_tty);
  fclose(write_tty);

  return(0);
}

