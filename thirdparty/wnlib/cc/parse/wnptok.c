/******************************************************************************

bool wn_parse_blanks(stream,num)
bool wn_parse_blanks_with_std_comments(stream,num)
bool wn_parse_blanks_with_comments(stream,pparse_comment,num)
bool wn_parse_comments(stream,pparse_comment)

bool wn_parse_standard_comment(stream)
bool wn_parse_simple_comment(stream,start_comment,fin_comment)
bool wn_parse_recursive_comment(stream,start_comment,fin_comment)

bool wn_parse_literal(stream,literal)

bool wn_parse_literal_id(stream,id)
bool wn_parse_literal_general_id(stream,begin_id_char_type,id_char_type,id)

bool wn_parse_id(stream,&id)
bool wn_parse_numeric_id(stream,&id)
bool wn_parse_general_id(stream,begin_id_char_type,id_char_type,&id)

bool wn_parse_int(stream,&i)
bool wn_parse_double(stream,&f)

bool wn_parse_quoted_string(stream,start_char,&string,fin_char)
bool wn_parse_quoted_string_with_backslash(stream,start_char,&string,fin_char)

bool wn_scan_for_literal(stream,literal)

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
#include "wncstr.h"
#include "wnary.h"
#include "wnio.h"



#define NUMBER_OF_CHARS  (1<<8)
#define CHAR_MASK        (NUMBER_OF_CHARS-1)


local bool initialized = FALSE;

char *wn_char_type_buf,
     *wn_blank_char_type,
     *wn_begin_id_char_type,*wn_id_char_type,
     *wn_begin_int_char_type,*wn_int_char_type,
     *wn_begin_double_char_type;



wn_initialize_parse_tokens()
{
  if(not(initialized))
  {
    wn_gppush(wn_defaultgp());


    wn_make_char_type(&wn_char_type_buf,"");

    wn_make_char_type(&wn_blank_char_type," \n\t");
    wn_blank_char_type['\0'&CHAR_MASK] = TRUE;  /* must do this explicitly */

    wn_make_char_type(&wn_begin_id_char_type,
                      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
    wn_make_char_type(&wn_id_char_type,
           "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_1234567890");

    wn_make_char_type(&wn_begin_int_char_type,
                      "-+1234567890");
    wn_make_char_type(&wn_int_char_type,
                      "1234567890");

    wn_make_char_type(&wn_begin_double_char_type,
                      "-+.1234567890");


    initialized = TRUE;

    wn_gppop();
  }
}



wn_make_char_type(pchar_type,chars)

char **pchar_type,chars[];

{
  *pchar_type = (char *)wn_alloc(NUMBER_OF_CHARS);

  wn_load_char_type(*pchar_type,chars);
}



wn_load_char_type(char_type,chars)

char char_type[NUMBER_OF_CHARS],*chars;

{
  wn_arrayfill(char_type,NUMBER_OF_CHARS,FALSE,char); 

  wn_set_char_type(char_type,chars);
}



wn_set_char_type(char_type,chars)

char char_type[NUMBER_OF_CHARS],*chars;

{
  for(;*chars != '\0';chars++)
  {
    char_type[(*chars)&CHAR_MASK] = TRUE;
  }
}



wn_reset_char_type(char_type,chars)

char char_type[NUMBER_OF_CHARS],*chars;

{
  for(;*chars != '\0';chars++)
  {
    char_type[(*chars)&CHAR_MASK] = FALSE;
  }
}



wn_invert_char_type(char_type)

char char_type[NUMBER_OF_CHARS];

{
  int i;

  for(i=0;i<NUMBER_OF_CHARS;i++)
  {
    if(char_type[i])
    {
      char_type[i] = FALSE;
    }
    else
    {
      char_type[i] = TRUE;
    }
  }
}


  
bool wn_parse_blanks(stream,num)

wn_cstream stream;
int num;

{
  int start_mark,finish_mark;

  start_mark = wn_current_mark(stream);

  wn_scan_for_not_char_type(stream,wn_blank_char_type);

  finish_mark = wn_current_mark(stream);

  if(num > (finish_mark-start_mark))
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }
  else
  {
    return(TRUE);
  }
}



bool wn_parse_blanks_with_std_comments(stream,num)

wn_cstream stream;
int num;

{
  bool wn_parse_standard_comment();

  return(wn_parse_blanks_with_comments(stream,
                                       (wn_parse_standard_comment),
                                       num));
}



bool wn_parse_blanks_with_comments(stream,pparse_comment,num)

wn_cstream stream;
bool (*pparse_comment)(/*stream*/);
int num;

{
  int start_mark,fin_mark,blanks_start_mark,blanks_fin_mark,
      blanks_parsed;

  start_mark = wn_current_mark(stream);

  while(TRUE)
  {
    blanks_start_mark = wn_current_mark(stream);

    wn_scan_for_not_char_type(stream,wn_blank_char_type);

    blanks_fin_mark = wn_current_mark(stream);

    blanks_parsed = blanks_fin_mark-blanks_start_mark;

    if(not((*pparse_comment)(stream)))
    {
      if(blanks_parsed == 0)
      {
        break;
      }
    }
  }

  fin_mark = wn_current_mark(stream);

  if(num > (fin_mark-start_mark))
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }
  else
  {
    return(TRUE);
  }
}



bool wn_parse_comments(stream,pparse_comment)

wn_cstream stream;
bool (*pparse_comment)(/*stream*/);

{
  while((*pparse_comment)(stream))
    ;

  return(TRUE);
}



bool wn_parse_standard_comment(stream)

wn_cstream stream;

{
  return(wn_parse_recursive_comment(stream,"/*","*/"));
}



bool wn_parse_simple_comment(stream,start_comment,fin_comment)

wn_cstream stream;
char start_comment[],fin_comment[];

{
  int start_mark;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_literal(stream,start_comment)))
  {
    return(FALSE);
  }

  if(not(wn_scan_for_literal(stream,fin_comment)))
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }

  wn_parse_literal(stream,fin_comment);

  return(TRUE);
}



bool wn_parse_recursive_comment(stream,start_comment,fin_comment)

wn_cstream stream;
char start_comment[],fin_comment[];

{
  char search_chars[3],c;
  int start_mark,comment_depth;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_literal(stream,start_comment)))
  {
    return(FALSE);
  }

  comment_depth = 1;

  search_chars[0] = start_comment[0];
  search_chars[1] = fin_comment[0];
  search_chars[2] = '\0';
  wn_set_char_type(wn_char_type_buf,search_chars);

  while(TRUE)
  {
    if(not(wn_scan_for_char_type(stream,wn_char_type_buf)))
    {
      wn_reset_char_type(wn_char_type_buf,search_chars);
      wn_set_current_mark(stream,start_mark);

      return(FALSE);
    }

    if(wn_parse_literal(stream,fin_comment))
    {
      comment_depth--;

      if(comment_depth == 0)  /* end of comment */
      {
        wn_reset_char_type(wn_char_type_buf,search_chars);

        return(TRUE);
      }
    }
    else if(wn_parse_literal(stream,start_comment))
    {
      comment_depth++;
    }
    else
    {
      wn_parse_char(stream,&c);  /* skip char */
    }
  }
}



#if 0  /* wcn -- deleted 10/5/88 */
bool wn_parse_literal(stream,literal)

wn_cstream stream;
char *literal;

{
  int fail_mark;

  fail_mark = wn_current_mark(stream);

  while(*literal != '\0')
  {
    if(not(wn_parse_literal_char(stream,*literal)))
    {
      wn_set_current_mark(stream,fail_mark);

      return(FALSE);
    }

    literal++;
  }

  return(TRUE);
}
#endif



bool wn_parse_literal_id(stream,id)

wn_cstream stream;
char *id;

{
  return(wn_parse_literal_general_id(stream,
                                     wn_begin_id_char_type,wn_id_char_type,
                                     id));
}



bool wn_parse_literal_general_id(stream,begin_id_char_type,id_char_type,id)

wn_cstream stream;
char *begin_id_char_type,*id_char_type;
char *id;

{
  int start_mark,fin_mark;
  char c;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_char_of_type(stream,&c,begin_id_char_type)))
  {
    return(FALSE);
  }

  wn_scan_for_not_char_type(stream,id_char_type);

  fin_mark = wn_current_mark(stream);

  if(wn_between_marks_equal_string(id,stream,start_mark,fin_mark))
  {
    return(TRUE);
  }
  else
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }
}



bool wn_parse_id(stream,pid)

wn_cstream stream;
char **pid;

{
  return(wn_parse_general_id(stream,
                             wn_begin_id_char_type,wn_id_char_type,
                             pid));
}



bool wn_parse_numeric_id(stream,pid)

wn_cstream stream;
char **pid;

{
  return(wn_parse_general_id(stream,
                             wn_id_char_type,wn_id_char_type,
                             pid));
}



bool wn_parse_general_id(stream,begin_id_char_type,id_char_type,pid)

wn_cstream stream;
char *begin_id_char_type,*id_char_type;
char **pid;

{
  int start_mark,fin_mark;
  char c;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_char_of_type(stream,&c,begin_id_char_type)))
  {
    return(FALSE);
  }

  wn_scan_for_not_char_type(stream,id_char_type);

  fin_mark = wn_current_mark(stream);

  *pid = (char *)wn_alloc(fin_mark-start_mark+1);

  wn_copy_between_marks_to_string(*pid,stream,start_mark,fin_mark);

  return(TRUE);
}



bool wn_parse_int(stream,pi)

wn_cstream stream;
int *pi;

{
  int start_mark,fin_mark,len;
  char c,buf[50];

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_char_of_type(stream,&c,wn_begin_int_char_type)))
  {
    return(FALSE);
  }

  wn_scan_for_not_char_type(stream,wn_int_char_type);

  fin_mark = wn_current_mark(stream);

  len = fin_mark-start_mark;

  if(len >= 50)
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }

  wn_copy_between_marks_to_string(buf,stream,start_mark,fin_mark);

  if((len == 1) && ((buf[0] == '-')||(buf[0] == '+')))
  { 
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }

  /*
  sscanf(buf,"%d",pi);
  */
  *pi = atoi(buf);

  return(TRUE);
}



bool wn_parse_double(stream,pf)

wn_cstream stream;
double *pf;

{
  extern double atof();
  int start_mark,e_mark,fin_mark,dummy,len;
  char buf[200],c;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_char_of_type(stream,&c,wn_begin_double_char_type)))
  {
    return(FALSE);
  }

  wn_scan_for_not_char_type(stream,wn_int_char_type);

  if(wn_parse_literal_char(stream,'.'))
  {
    wn_scan_for_not_char_type(stream,wn_int_char_type);
  }
    
  e_mark = wn_current_mark(stream);

  if(
      wn_parse_literal_char(stream,'e')
	||
      wn_parse_literal_char(stream,'E')
    )
  {
    if(not(wn_parse_int(stream,&dummy)))
    {
      wn_set_current_mark(stream,e_mark);
    }
  }

  fin_mark = wn_current_mark(stream);

  len = fin_mark-start_mark;

  if(len >= 50)
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }

  wn_copy_between_marks_to_string(buf,stream,start_mark,fin_mark);

  if((len == 1)&&((buf[0] == '-')||(buf[0] == '+')||(buf[0] == '.')))
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }
  else
  {
    switch(buf[0])
    {
      case('.'):
      {
        if((buf[1] == 'e')||(buf[1] == 'E'))
        {
          wn_set_current_mark(stream,start_mark);

          return(FALSE);
        }          
      } break;
      case('-'):
      case('+'):
      {
        switch(buf[1])
        {
          case('e'):
          case('E'):
          {
            wn_set_current_mark(stream,start_mark);

            return(FALSE);
          }
          case('.'):
          {
            if((len >= 2)&&((buf[2] == 'e')||(buf[2] == 'E')))
            {
              wn_set_current_mark(stream,start_mark);

              return(FALSE);
            }
          } break;
        }
      } break;
    }
  }

  /*
  sscanf(buf,"%f",&f);
  *pf = (double)f;
  */
  *pf = atof(buf);

  return(TRUE);
}



bool wn_parse_quoted_string(stream,start_char,pstring,fin_char)

wn_cstream stream;
char start_char,**pstring,fin_char;

{
  int start_mark,fin_mark;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_literal_char(stream,start_char)))
  {
    return(FALSE);
  }

  if(not(wn_scan_for_char(stream,fin_char)))
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }

  fin_mark = wn_current_mark(stream);

  start_mark++;   /* skip start_char */

  *pstring = (char *)wn_alloc(fin_mark-start_mark+1);

  wn_copy_between_marks_to_string(*pstring,stream,start_mark,fin_mark);

  wn_set_current_mark(stream,fin_mark+1);  /* skip fin_char */

  return(TRUE);
}



bool wn_parse_quoted_string_with_backslash(stream,start_char,pstring,fin_char)

wn_cstream stream;
char start_char,**pstring,fin_char;

{
  char c;
  int start_mark,fin_mark,
      number_of_chars,i;
  bool success,fin;

  start_mark = wn_current_mark(stream);

  if(not(wn_parse_literal_char(stream,start_char)))
  {
    return(FALSE);
  }

  if(not(find_end_of_quoted_string(stream,fin_char,&number_of_chars)))
  {
    wn_set_current_mark(stream,start_mark);

    return(FALSE);
  }

  fin_mark = wn_current_mark(stream);

  start_mark++;   /* skip start_char */
  wn_set_current_mark(stream,start_mark);

  *pstring = (char *)wn_alloc(number_of_chars+1);

  for(i=0;i<number_of_chars;++i)
  {
    success = parse_quoted_string_char(stream,fin_char,&fin,&c);
    wn_assert(success && not(fin));

    (*pstring)[i] = c;
  }

  (*pstring)[i] = '\0';

  wn_set_current_mark(stream,fin_mark+1);  /* skip fin_char */

  return(TRUE);
}



local bool find_end_of_quoted_string(stream,fin_char,pnumber_of_chars)

wn_cstream stream;
char fin_char;
int *pnumber_of_chars;

{
  bool success,fin;
  char c;

  *pnumber_of_chars = 0;

  while(TRUE)
  {
    success = parse_quoted_string_char(stream,fin_char,&fin,&c);

    if(not(success))
    {
      return(FALSE);
    }

    if(fin)
    {
      return(TRUE);
    }

    ++(*pnumber_of_chars);
  }
}



local bool parse_quoted_string_char(stream,fin_char,pfin,pc)

wn_cstream stream;
char fin_char,*pc;
bool *pfin;

{
  char c,c2;
  bool success;

  success = wn_parse_char(stream,&c);
  if(not(success))
  {
    return(FALSE);
  }

  if(c == fin_char)
  {
    *pfin = TRUE;
    *pc = '\0';
    return(TRUE);
  }
  else if(c == '\\')
  {
    success = wn_parse_char(stream,&c2);  /* lookahead */
    if(not(success))
    {
      return(FALSE);
    }

    if(c2 == fin_char)
    {
      *pfin = FALSE;
      *pc = c2;
      return(TRUE);
    }
    else
    {
      *pfin = FALSE;
      *pc = c;
      wn_set_current_mark(stream,wn_current_mark(stream)-1); /* backup 
								lookahead */
      return(TRUE);
    }
  }
  else
  {
    *pfin = FALSE;
    *pc = c;
    return(TRUE);
  }
}



bool wn_scan_for_literal(stream,literal)

wn_cstream stream;
char literal[];

{
  int start_mark,literal_mark;
  char c;

  start_mark = wn_current_mark(stream);

  while(wn_scan_for_char(stream,literal[0]))
  {
    literal_mark = wn_current_mark(stream);

    if(wn_parse_literal(stream,literal))
    {
      wn_set_current_mark(stream,literal_mark);

      return(TRUE);
    }

    wn_parse_char(stream,&c); /* false alarm, skip found char */
  }

  wn_set_current_mark(stream,start_mark);
  return(FALSE);
}




