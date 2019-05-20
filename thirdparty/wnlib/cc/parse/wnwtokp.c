/******************************************************************************

wn_write_blanks_pretty(stream,num)
wn_write_blanks_pretty_until_column(stream,col)

wn_write_literal_pretty(stream,literal)
wn_write_id_pretty(stream,id)

wn_write_int_pretty(stream,i)
wn_write_double_pretty(stream,f)

wn_write_quoted_string_pretty(stream,start_char,string,fin_char)

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

#include "wnlib.h"
#include "wnmax.h"
#include "wncstr.h"



local write_pretty_linefeed_if_needed(stream,len)

wn_cstream stream;
int len;

{
  if(
      (stream->pretty_block_level == 0)
        &&
      (
        (wn_current_mark(stream)+len - stream->last_linefeed_mark)
          >
        stream->line_length_limit
      )
    )
  {
    (*(stream->ppretty_linefeed_routine))(stream);
  }
}



wn_write_blanks_pretty(stream,num)

wn_cstream stream;
int num;

{
  if(stream->pretty_block_level != 0)
  {
    wn_write_blanks(stream,num);
  }
  else
  {
    if(
        (wn_current_mark(stream)+num - stream->last_linefeed_mark)
          >
        stream->line_length_limit
      )
    {
      (*(stream->ppretty_linefeed_routine))(stream);
    }
    else
    {
      wn_write_blanks(stream,num);
    }
  }
}



wn_write_blanks_pretty_until_column(stream,col)

wn_cstream stream;
int col;

{
  int tab_blanks;

  tab_blanks = col - wn_column_of_current_mark(stream);

  wn_write_blanks_pretty(stream,wn_max(0,tab_blanks));
}



wn_write_literal_pretty(stream,literal)

wn_cstream stream;
char *literal;

{
  int len;

  len = wn_strlen(literal);

  write_pretty_linefeed_if_needed(stream,len);

  wn_write_block(stream,literal,len);
}



wn_write_id_pretty(stream,id)

wn_cstream stream;
char *id;

{
  int len;

  len = wn_strlen(id);

  write_pretty_linefeed_if_needed(stream,len);

  wn_write_block(stream,id,len);
}



wn_write_int_pretty(stream,i)

wn_cstream stream;
int i;

{
  int len;
  char buf[50];

  sprintf(buf,"%d",i);

  len = wn_strlen(buf);

  write_pretty_linefeed_if_needed(stream,len);

  wn_write_block(stream,buf,len);
}



wn_write_double_pretty(stream,f)

wn_cstream stream;
double f;

{
  int len;
  char buf[50];

  sprintf(buf,"%g",(float)f);

  len = wn_strlen(buf);

  write_pretty_linefeed_if_needed(stream,len);

  wn_write_block(stream,buf,len);
}



wn_write_quoted_string_pretty(stream,start_char,string,fin_char)

wn_cstream stream;
char start_char,*string,fin_char;

{
  int len;

  len = wn_strlen(string);

  write_pretty_linefeed_if_needed(stream,len+2);

  wn_write_char(stream,start_char);
  wn_write_block(stream,string,len);
  wn_write_char(stream,fin_char);
}



