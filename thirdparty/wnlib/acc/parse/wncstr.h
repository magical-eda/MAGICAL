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
#ifndef wncstrH
#define wncstrH



#include "wnlib.h"
#include "wnmem.h"
#include "wnio.h"
#include "wnhtab.h"
#include "wnbtr.h"


#define  WN_PARSE   1
#define  WN_WRITE   2


typedef struct wn_cstream_struct *wn_cstream;
struct wn_cstream_struct
{
  int parse_or_write;

  wn_htab block_table;
  int current_block_mark;
  char *current_block;

  int current_mark,low_mark,high_mark;
  bool end_of_stream;

  void (*pmove_block)
  (
    char block[],
    int *pactually_moved,
    int request_moved,
    ptr pf
  );

  ptr move_block_ptr; 

  int block_size,trunc_mask,mod_mask;

  int last_linefeed_mark,line_length_limit;
  void (*ppretty_linefeed_routine)(wn_cstream stream);
  int pretty_block_level,pretty_buffer_len;
  char *pretty_buffer;

  char *filename;
  int number_of_linefeeds;
  wn_btree linefeeds_by_mark;

  wn_memgp group;
};


#define wn_current_mark(_stream)  ((_stream)->current_mark)

EXTERN void wn_make_cstream
(
  wn_cstream *pstream,
  void (*pmove_block)
  (
    char block[],
    int *pactually_moved,
    int request_moved,
    ptr pf
  ),
  ptr move_block_ptr,
  int block_size,
  int parse_or_write
);
EXTERN void wn_free_cstream(wn_cstream stream);
EXTERN void wn_set_pretty_linefeed_routine
(
  wn_cstream stream,
  void (*ppretty_linefeed_routine)(wn_cstream stream)
);
EXTERN void wn_set_line_length_limit(wn_cstream stream,int len);
EXTERN void wn_start_pretty_block(wn_cstream stream);
EXTERN void wn_end_pretty_block(wn_cstream stream);
EXTERN bool wn_set_current_mark(wn_cstream stream,int mark);
EXTERN void wn_flush_to_mark(wn_cstream stream,int mark);
EXTERN char *wn_cstream_filename(wn_cstream stream);
EXTERN void wn_set_cstream_filename(wn_cstream stream,char *name);
EXTERN int wn_column_of_current_mark(wn_cstream stream);
EXTERN int wn_column_of_mark(wn_cstream stream,int mark);
EXTERN int wn_linenum_of_mark(wn_cstream stream,int mark);
EXTERN int wn_linenum_of_current_mark(wn_cstream stream);
EXTERN void wn_linefeed_at_mark(wn_cstream stream,int mark);
EXTERN bool wn_parse_char(wn_cstream stream,char *pc);
EXTERN bool wn_parse_char_of_type(wn_cstream stream,char *pc,char char_type[]);
EXTERN bool wn_parse_literal(wn_cstream stream,char *literal);
EXTERN bool wn_parse_literal_char(wn_cstream stream,char c);
EXTERN bool wn_parse_eos(wn_cstream stream);
EXTERN bool wn_parse_empty(wn_cstream stream);
EXTERN bool wn_scan_for_eos(wn_cstream stream);
EXTERN bool wn_scan_for_char(wn_cstream stream,char c);
EXTERN bool wn_scan_for_char_type(wn_cstream stream,char char_type[]);
EXTERN bool wn_scan_for_not_char_type(wn_cstream stream,char char_type[]);
EXTERN void wn_write_between_marks
(
  wn_cstream out_stream,
  wn_cstream stream,
  int start_mark,
  int fin_mark
);
EXTERN void wn_copy_between_marks_to_string
(
  char *string,
  wn_cstream stream,
  int start_mark,
  int fin_mark
);
EXTERN bool wn_between_marks_equal_string
(
  char *string,
  wn_cstream stream,
  int start_mark,
  int fin_mark
);
EXTERN bool wn_cstream_debug(wn_cstream stream);
EXTERN void wn_write_block(wn_cstream stream,char *block,int len);
EXTERN void wn_write_char(wn_cstream stream,char c);
EXTERN void wn_write_literal_char(wn_cstream stream,char c);
EXTERN void wn_raw_write_literal_char(wn_cstream stream,char c);
EXTERN void wn_write_eos(wn_cstream stream);
EXTERN void wn_write_empty(wn_cstream stream);


EXTERN void wn_make_file_parse_cstream(wn_cstream *pstream,wn_file f);
EXTERN void wn_make_file_write_cstream(wn_cstream *pstream,wn_file f);
EXTERN void wn_make_string_parse_cstream(wn_cstream *pstream,char string[]);


EXTERN void wn_write_blanks(wn_cstream stream,int num);
EXTERN void wn_write_linefeed(wn_cstream stream);
EXTERN void wn_write_blanks_until_column(wn_cstream stream,int col);
EXTERN void wn_write_literal(wn_cstream stream,char *literal);
EXTERN void wn_write_id(wn_cstream stream,char *id);
EXTERN void wn_write_int(wn_cstream stream,int i);
EXTERN void wn_write_double(wn_cstream stream,double f);
EXTERN void wn_write_quoted_string
(
  wn_cstream stream,
  char start_char,
  char* string,
  char fin_char
);
EXTERN void wn_write_quoted_string_with_backslash
(
  wn_cstream stream,
  char start_char,
  char *string,
  char fin_char
);
EXTERN void wn_write_literal_n_times(wn_cstream stream,char *literal,int n);


EXTERN void wn_write_blanks_pretty(wn_cstream stream,int num);
EXTERN void wn_write_blanks_pretty_until_column(wn_cstream stream,int col);
EXTERN void wn_write_literal_pretty(wn_cstream stream,char *literal);
EXTERN void wn_write_id_pretty(wn_cstream stream,char *id);
EXTERN void wn_write_int_pretty(wn_cstream stream,int i);
EXTERN void wn_write_double_pretty(wn_cstream stream,double f);
EXTERN void wn_write_quoted_string_pretty
(
  wn_cstream stream,
  char start_char,
  char *string,
  char fin_char
);


EXTERN void wn_make_char_type(char **pchar_type,char chars[]);
EXTERN void wn_copy_char_type(char to[],char from[]);
EXTERN void wn_load_char_type(char char_type[],char *chars);
EXTERN void wn_set_char_type(char char_type[],char *chars);
EXTERN void wn_reset_char_type(char char_type[],char *chars);
EXTERN void wn_invert_char_type(char char_type[]);
  
EXTERN bool wn_parse_blanks(wn_cstream stream,int num);
EXTERN bool wn_parse_blanks_with_std_comments(wn_cstream stream,int num);
EXTERN bool wn_parse_blanks_with_comments
(
  wn_cstream stream,
  bool (*pparse_comment)(wn_cstream stream),
  int num
);
EXTERN bool wn_parse_comments
(
  wn_cstream stream,
  bool (*pparse_comment)(wn_cstream stream)
);
EXTERN bool wn_parse_standard_comment(wn_cstream stream);
EXTERN bool wn_parse_simple_comment
(
  wn_cstream stream,
  char start_comment[],
  char fin_comment[]
);
EXTERN bool wn_parse_recursive_comment
(
  wn_cstream stream,
  char start_comment[],
  char fin_comment[]
);
EXTERN bool wn_parse_literal_id(wn_cstream stream,char *id);
EXTERN bool wn_parse_literal_general_id
(
  wn_cstream stream,
  char *begin_id_char_type,
  char *id_char_type,
  char *id
);
EXTERN bool wn_parse_id(wn_cstream stream,char **pid);
EXTERN bool wn_parse_numeric_id(wn_cstream stream,char **pid);
EXTERN bool wn_parse_general_id
(
  wn_cstream stream,
  char *begin_id_char_type,
  char *id_char_type,
  char **pid
);
EXTERN bool wn_parse_int(wn_cstream stream,int *pi);
EXTERN bool wn_parse_double(wn_cstream stream,double *pf);
EXTERN bool wn_parse_quoted_string
(
  wn_cstream stream,
  char start_char,
  char **pstring,
  char fin_char
);
EXTERN bool wn_parse_quoted_string_with_backslash
(
  wn_cstream stream,
  char start_char,
  char **pstring,
  char fin_char
);
EXTERN bool wn_scan_for_literal(wn_cstream stream,char literal[]);


#endif
