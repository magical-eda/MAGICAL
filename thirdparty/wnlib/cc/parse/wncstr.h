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

  void (*pmove_block)(/*block,&actually_moved,request_moved,&move_block_ptr*/);
  ptr move_block_ptr; 

  int block_size,trunc_mask,mod_mask;

  int last_linefeed_mark,line_length_limit;
  void (*ppretty_linefeed_routine)(/*stream*/);
  int pretty_block_level,pretty_buffer_len;
  char *pretty_buffer;

  char *filename;
  int number_of_linefeeds;
  wn_btree linefeeds_by_mark;

  wn_memgp group;
};



#define wn_current_mark(_stream)  ((_stream)->current_mark)



extern char *wn_cstream_filename();
extern int wn_linenum_of_mark(),wn_linenum_of_current_mark();
    

