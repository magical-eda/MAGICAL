/******************************************************************************

wn_write_blanks(stream,num)
wn_write_blanks_until_column(stream,col)
wn_write_linefeed(stream)

wn_write_literal(stream,literal)
wn_write_id(stream,id)

wn_write_int(stream,i)
wn_write_double(stream,f)

wn_write_quoted_string(stream,start_char,string,fin_char)
wn_write_quoted_string_with_backslash(stream,start_char,string,fin_char)

wn_write_literal_n_times(stream,literal,n)

******************************************************************************/
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

#include <stdio.h>
#include <string.h>

#include "wnlib.h"
#include "wnasrt.h"
#include "wnmax.h"
#include "wnio.h"

#include "wncstr.h"



char wn_prefered_linefeed_char = '\n',
     wn_prefered_blank_char = ' ';


void wn_write_blanks(wn_cstream stream,int num)
{
  int i;

  for(i=0;i<num;i++)
  {
    wn_write_char(stream,wn_prefered_blank_char);
  }
}


void wn_write_linefeed(wn_cstream stream)
{
  wn_assert(stream->pretty_block_level == 0);

  wn_raw_write_literal_char(stream,wn_prefered_linefeed_char);

  wn_linefeed_at_mark(stream,wn_current_mark(stream));
}


void wn_write_blanks_until_column(wn_cstream stream,int col)
{
  int tab_blanks;

  tab_blanks = col - wn_column_of_current_mark(stream);

  wn_write_blanks(stream,wn_max(0,tab_blanks));
}


void wn_write_literal(wn_cstream stream,char *literal)
{
  wn_write_block(stream,literal,strlen(literal));
}


void wn_write_id(wn_cstream stream,char *id)
{
  wn_write_block(stream,id,strlen(id));
}


void wn_write_int(wn_cstream stream,int i)
{
  char buf[50];

  sprintf(buf,"%d",i);

  wn_write_block(stream,buf,strlen(buf));
}


void wn_write_double(wn_cstream stream,double f)
{
  char buf[50];

  sprintf(buf,"%g",(float)f);

  wn_write_block(stream,buf,strlen(buf));
}


void wn_write_quoted_string
(
  wn_cstream stream,
  char start_char,
  char* string,
  char fin_char
)
{
  wn_write_char(stream,start_char);
  wn_write_block(stream,string,strlen(string));
  wn_write_char(stream,fin_char);
}


void wn_write_quoted_string_with_backslash
(
  wn_cstream stream,
  char start_char,
  char *string,
  char fin_char
)
{
  char *pc;

  wn_write_char(stream,start_char);

  for(pc=string;*pc != '\0';++pc)
  {
    if(*pc == fin_char)
    {
      wn_write_char(stream,'\\');
    }

    wn_write_char(stream,*pc);
  }

  wn_write_char(stream,fin_char);
}


void wn_write_literal_n_times(wn_cstream stream,char *literal,int n)
{
  int i;

  for(i=0;i<n;++i)
  {
    wn_write_literal(stream,literal);
  }
}




