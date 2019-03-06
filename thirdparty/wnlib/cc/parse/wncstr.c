/******************************************************************************

wn_make_cstream(&stream,pmove_block,move_block_ptr,block_size,parse_or_write)
wn_free_cstream(stream)

wn_set_pretty_linefeed_routine(stream,(write_linefeed));
wn_set_line_length_limit(stream,len)
wn_start_pretty_block(stream)
wn_end_pretty_block(stream)

int wn_current_mark(stream)
bool wn_set_current_mark(stream,mark)
wn_flush_to_mark(stream,mark)

char *wn_cstream_filename(stream)
wn_set_cstream_filename(stream,name)

int wn_column_of_mark(stream,mark)
int wn_column_of_current_mark(stream)
int wn_linenum_of_mark(stream,mark)
int wn_linenum_of_current_mark(stream)
wn_linefeed_at_mark(stream,mark)

*******************************************************************************

bool wn_parse_char(stream,&c)
bool wn_parse_char_of_type(stream,&c,char_type)
bool wn_parse_literal_char(stream,c)
bool wn_parse_eos(stream)
bool wn_parse_empty(stream)

bool wn_scan_for_eos(stream)
bool wn_scan_for_char(stream,c)
bool wn_scan_for_char_type(stream,char_type)
bool wn_scan_for_not_char_type(stream,char_type)

wn_copy_between_marks_to_string(string,stream,start_mark,fin_mark)
bool wn_between_marks_equal_string(string,stream,start_mark,fin_mark)
wn_write_between_marks(out_stream,stream,start_mark,fin_mark)

bool wn_cstream_debug(stream)

*******************************************************************************

wn_write_char(stream,c)
wn_write_literal_char(stream,c)
wn_write_eos(stream)
wn_write_empty(stream)

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
#include "wnio.h"
#include "wncstr.h"
#include "wninr.h"
#include "wnmax.h"
#include "wnary.h"



wn_make_cstream(pstream,pmove_block,move_block_ptr,block_size,parse_or_write)

wn_cstream *pstream;
void (*pmove_block)(/* block,&actually_moved,request_moved,move_block_ptr */);
ptr move_block_ptr;
int block_size,parse_or_write;

{
  switch(parse_or_write)
  {
    case(WN_WRITE):
    {
      make_write_cstream(pstream,pmove_block,move_block_ptr);
    } break;
    case(WN_PARSE):
    {
      make_parse_cstream(pstream,pmove_block,move_block_ptr,block_size);
    } break;
    default:
      wn_crash();
  }
}

    

local make_write_cstream(pstream,pmove_block,move_block_ptr)

wn_cstream *pstream;
void (*pmove_block)(/* block,&actually_moved,request_moved,move_block_ptr */);
ptr move_block_ptr;

{
  extern void wn_write_linefeed();
  *pstream = wn_znew(wn_cstream);

  (*pstream)->move_block_ptr = move_block_ptr;
  (*pstream)->pmove_block = pmove_block;

  (*pstream)->parse_or_write = WN_WRITE;

  (*pstream)->last_linefeed_mark = 0;
  wn_set_line_length_limit(*pstream,80);
  wn_set_pretty_linefeed_routine(*pstream,(wn_write_linefeed));

  (*pstream)->filename = NULL;
  (*pstream)->number_of_linefeeds = 0;

  (*pstream)->group = wn_curgp();
}



local make_parse_cstream(pstream,pmove_block,move_block_ptr,block_size)

wn_cstream *pstream;
void (*pmove_block)(/* block,&actually_moved,request_moved,move_block_ptr */);
ptr move_block_ptr;

{
  wn_initialize_parse_tokens();

  *pstream = wn_znew(wn_cstream);

  (*pstream)->current_mark = 0;
  (*pstream)->low_mark = 0;
  (*pstream)->high_mark = 0;

  wn_mkinthtab(&((*pstream)->block_table));

  (*pstream)->current_block_mark = -1;
  (*pstream)->current_block = NULL;
  (*pstream)->move_block_ptr = move_block_ptr;
  (*pstream)->pmove_block = pmove_block;

  (*pstream)->block_size = block_size;
  (*pstream)->mod_mask = block_size-1;
  (*pstream)->trunc_mask = ~(block_size-1);

  (*pstream)->parse_or_write = WN_PARSE;

  (*pstream)->end_of_stream = FALSE;

  (*pstream)->last_linefeed_mark = -1;
  (*pstream)->filename = NULL;
  (*pstream)->number_of_linefeeds = 0;
  wn_mkintbtree(&((*pstream)->linefeeds_by_mark));

  (*pstream)->group = wn_curgp();

  wn_set_current_mark(*pstream,0);
}



wn_free_cstream(stream)

wn_cstream stream;

{
  void free_block();

  wn_gppush(stream->group);

  wn_hact(stream->block_table,(free_block));
  wn_freehtab(stream->block_table);

  if(stream->filename != NULL)
  {
    wn_free((ptr)(stream->filename));
  }

  if(stream->linefeeds_by_mark != NULL)
  {
    wn_freebtree(stream->linefeeds_by_mark);
  }

  if(stream->pretty_buffer != NULL)
  {
    wn_free((ptr)(stream->pretty_buffer));
  }

  wn_free((ptr)stream);

  wn_gppop();
}



/*ARGSUSED*/ local void free_block(block,block_mark)

char *block;
int block_mark;

{
  wn_free((ptr)block);
}



wn_set_pretty_linefeed_routine(stream,ppretty_linefeed_routine)

wn_cstream stream;
void (*ppretty_linefeed_routine)(/* stream */);

{
  stream->ppretty_linefeed_routine = ppretty_linefeed_routine;
}



wn_set_line_length_limit(stream,len)

wn_cstream stream;
int len;

{
  stream->line_length_limit = len;
}



wn_start_pretty_block(stream)

wn_cstream stream;

{
  if(stream->pretty_block_level == 0)
  {
    if(stream->pretty_buffer == NULL)
    {
      wn_gppush(stream->group);

      stream->pretty_buffer = (char *)wn_alloc(1024);

      wn_gppop();
    }
  }

  (stream->pretty_block_level)++;
}



wn_end_pretty_block(stream)

wn_cstream stream;

{
  int actually_moved;

  (stream->pretty_block_level)--;

  wn_assert(stream->pretty_block_level >= 0);

  if(stream->pretty_block_level == 0)
  {
    if(
        (wn_current_mark(stream)+stream->pretty_buffer_len - 
           stream->last_linefeed_mark) 
          >
        stream->line_length_limit
      )
    {
      (*(stream->ppretty_linefeed_routine))(stream);
    }
         
    (*(stream->pmove_block))(stream->pretty_buffer,
                             &actually_moved,stream->pretty_buffer_len,
                             &(stream->move_block_ptr));
    wn_current_mark(stream) += stream->pretty_buffer_len;

    stream->pretty_buffer_len = 0;
  }
}



bool wn_set_current_mark(stream,mark)

wn_cstream stream;
int mark;

{
  bool success;
  int current_block_mark;

  wn_assert(mark >= stream->low_mark);

  stream->current_mark = mark;

  if(mark >= stream->high_mark)
  {
    success = set_high_mark(stream,mark+1);

    if(not(success))
    {
      return(FALSE);
    }
  }

  current_block_mark = (stream->trunc_mask)&mark;
  if(current_block_mark != stream->current_block_mark)
  {
    wn_hget((ptr *)&(stream->current_block),stream->block_table,
            (ptr)current_block_mark);
    stream->current_block_mark = current_block_mark;
  }

  return(TRUE);
}



local bool set_high_mark(stream,mark)

wn_cstream stream;
int mark;

{
  char *block;
  int start_mark,stop_mark,block_mark,actually_moved;
  bool full_block;

  if(stream->end_of_stream)
  {
    return(FALSE);
  }

  wn_assert(mark > stream->high_mark);

  wn_gppush(stream->group);

  full_block = TRUE;

  start_mark = (stream->trunc_mask)&stream->high_mark;
  stop_mark = (stream->trunc_mask)&(mark + stream->block_size - 1);

  for(block_mark=start_mark;block_mark<stop_mark;
      block_mark+=stream->block_size)
  {
    block = (char *)wn_alloc(stream->block_size);
    (*(stream->pmove_block))(block,&actually_moved,stream->block_size,
                             &(stream->move_block_ptr));

    wn_hins((ptr)block,stream->block_table,(ptr)block_mark);

    find_linefeeds_of_block(stream,block,actually_moved,block_mark);

    if(actually_moved < stream->block_size)
    {
      stream->end_of_stream = TRUE;
      full_block = FALSE;

      break;
    }
  }

  if(full_block)
  {
    stream->high_mark = block_mark;
  }
  else 
  {
    stream->high_mark = block_mark+actually_moved;
  }

  wn_gppop();

  return(mark <= stream->high_mark);
}



local find_linefeeds_of_block(stream,block,size,block_mark)

wn_cstream stream;
char *block;
int size,block_mark;

{
  extern char wn_prefered_linefeed_char;
  int index,sum_index;

  sum_index = 0;

  while(TRUE)
  {
    wn_arrayscan(&index,block,size,
                 (wn_element == wn_prefered_linefeed_char),
                 char);

    if(index == -1)  /* none found */
    {
      break;
    }

    sum_index += index;

    wn_linefeed_at_mark(stream,block_mark+sum_index);

    sum_index += 1;

    block += index+1;
    size -= index+1;
  }
}



wn_flush_to_mark(stream,mark)

wn_cstream stream;
int mark;

{
  wn_gppush(stream->group);

  switch(stream->parse_or_write)
  {
    case(WN_PARSE):
    {
      flush_parse_to_mark(stream,mark);
    } break;
    case(WN_WRITE):
    {
    } break;
    default:
      wn_crash();
  }

  wn_gppop();
}



local flush_parse_to_mark(stream,mark)

wn_cstream stream;
int mark;

{
  int start_mark,stop_mark,block_mark;
  char *block;

  wn_assert(wn_inrange(stream->low_mark,<=,mark,<=,stream->high_mark));

  flush_linefeeds_to_parse_mark(stream,mark);

  start_mark = (stream->trunc_mask)&stream->low_mark;
  stop_mark = (stream->trunc_mask)&mark;

  for(block_mark=start_mark;block_mark<stop_mark;
      block_mark+=stream->block_size)
  {
    wn_hget((ptr *)&block,stream->block_table,(ptr)block_mark);
    wn_assert(block != NULL);

    wn_free((ptr)block);
    wn_hdel(stream->block_table,(ptr)block_mark);
  }

  stream->low_mark = mark;
}



local flush_linefeeds_to_parse_mark(stream,mark)

wn_cstream stream;
int mark;

{
  wn_bhandle handle;

  while(TRUE)
  {
    wn_bget(&handle,stream->linefeeds_by_mark,(ptr)NULL,WN_B_MIN);

    if(
        (handle == NULL)
          ||
        (((int)(handle->key)) >= mark)
      )
    {
      break;
    }

    wn_bdel(handle,stream->linefeeds_by_mark);
  }
}



char *wn_cstream_filename(stream)

wn_cstream stream;

{
  return(stream->filename);
}



wn_set_cstream_filename(stream,name)

wn_cstream stream;
char *name;

{
  wn_gppush(stream->group);

  wn_stracpy(&(stream->filename),name);

  wn_gppop();
}



int wn_column_of_current_mark(stream)

wn_cstream stream;

{
  return(wn_column_of_mark(stream,wn_current_mark(stream)));
}



int wn_column_of_mark(stream,mark)

wn_cstream stream;
int mark;

{
  int ret;

  switch(stream->parse_or_write)
  {
    case(WN_PARSE):
    {
      wn_crash();
    } break;
    case(WN_WRITE):
    {
      ret = mark - stream->last_linefeed_mark;
    } break;
    default:
      wn_crash();
  }

  return(ret);
}



int wn_linenum_of_current_mark(stream)

wn_cstream stream;

{
  return(wn_linenum_of_mark(stream,wn_current_mark(stream)));
}



int wn_linenum_of_mark(stream,mark)

wn_cstream stream;
int mark;

{
  int linenum_of_parse_mark(),ret;

  switch(stream->parse_or_write)
  {
    case(WN_PARSE):
    {
      ret = linenum_of_parse_mark(stream,mark);
    } break;
    case(WN_WRITE):
    {
      ret = stream->number_of_linefeeds;
    } break;
    default:
      wn_crash();
  }

  return(ret+1);  /* most editors start line numbers from 1, not 0 */
}



local int linenum_of_parse_mark(stream,mark)

wn_cstream stream;
int mark;

{
  wn_bhandle handle;

  wn_assert(
    wn_inrange(stream->low_mark,<=,stream->current_mark,<=,stream->high_mark));

  wn_bget(&handle,stream->linefeeds_by_mark,(ptr)mark,WN_B_GE);

  if(handle == NULL)
  {
    return(stream->number_of_linefeeds);
  }
  else
  {
    return((int)(handle->contents));
  }
}



wn_linefeed_at_mark(stream,mark)

wn_cstream stream;
int mark;

{
  if(mark > stream->last_linefeed_mark)
  {
    switch(stream->parse_or_write)
    {
      case(WN_PARSE):
      {
        linefeed_at_parse_mark(stream,mark);
      } break;
      case(WN_WRITE):
      {
        ;
      } break;
      default:
        wn_crash();
    }

    stream->last_linefeed_mark = mark;

    (stream->number_of_linefeeds)++;
  }
}



local linefeed_at_parse_mark(stream,mark)

wn_cstream stream;
int mark;

{
  wn_bhandle handle;

  wn_bins(&handle,stream->linefeeds_by_mark,(ptr)mark);

  handle->contents = (ptr)(stream->number_of_linefeeds);
}



#if 0
local char *access_char(stream)

wn_cstream stream;

{
  extern char wn_prefered_linefeed_char;
  char *pc;

  /*
  wn_assert(
     wn_inrange(stream->low_mark,<=,stream->current_mark,<,stream->high_mark));
     */

  pc = &((stream->current_block)[(stream->mod_mask)&(stream->current_mark)]);

  /*
  if(
      (*pc == wn_prefered_linefeed_char)
        &&
      (stream->current_mark > stream->last_linefeed_mark)
    )
  {
    wn_linefeed_at_mark(stream,stream->current_mark);
  }
  */

  return(pc);
}
#endif
#define access_char(stream) \
(\
  &((stream->current_block)[(stream->mod_mask)&(stream->current_mark)])\
)



bool wn_parse_char(stream,pc)

wn_cstream stream;
char *pc;

{
  if(wn_parse_eos(stream))
  { 
    *pc = '\0';

    return(FALSE);
  }
  else
  {
    *pc = *access_char(stream);

    wn_set_current_mark(stream,wn_current_mark(stream)+1);

    return(TRUE);
  }
}



#define CHAR_MASK    ((1<<8)-1)

bool wn_parse_char_of_type(stream,pc,char_type)

wn_cstream stream;
char *pc,char_type[];

{
  if(wn_parse_eos(stream))
  { 
    *pc = '\0';

    return(FALSE);
  }
  else
  {
    *pc = *access_char(stream);

    if(char_type[(*pc)&CHAR_MASK])
    {
      wn_set_current_mark(stream,wn_current_mark(stream)+1);

      return(TRUE);
    }
    else
    {
      *pc = '\0';

      return(FALSE);
    }   
  }
}



bool wn_parse_literal(stream,literal)

wn_cstream stream;
register char *literal;

{
  int fail_mark,max_len;
  register char literal_char;
  register int i;
  register char *block;

  fail_mark = wn_current_mark(stream);

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);

    if(max_len == 0)
    {
      if(*literal == '\0')
      {
        return(TRUE);
      }
      else
      {
        wn_set_current_mark(stream,fail_mark);

        return(FALSE);
      }
    }

    block = access_char(stream);

    for(i=0;i<max_len;i++)
    {
      literal_char = *literal;

      if(literal_char == '\0')
      {
        wn_set_current_mark(stream,wn_current_mark(stream)+i);

        return(TRUE);
      }

      if(*block != literal_char)
      {
        wn_set_current_mark(stream,fail_mark);

        return(FALSE);
      }

      block++; literal++;
    }

    wn_set_current_mark(stream,wn_current_mark(stream)+max_len);
  }
}



bool wn_parse_literal_char(stream,c)

wn_cstream stream;
char c;

{
  if(wn_parse_eos(stream))
  { 
    return(FALSE);
  }
  else
  {
    char parsed_char;

    parsed_char = *access_char(stream);

    if(parsed_char == c)
    {
      wn_set_current_mark(stream,wn_current_mark(stream)+1);

      return(TRUE);
    }
    else
    {
      return(FALSE);
    }
  }
}



bool wn_parse_eos(stream)

wn_cstream stream;

{
  if(stream->current_mark < stream->high_mark)
  {
    return(FALSE);
  }
  else
  {
    if(stream->end_of_stream)
    {
      return(TRUE);
    }
    else
    {
      return(not(set_high_mark(stream,wn_current_mark(stream))));
    }    
  }
}



/*ARGSUSED*/ bool wn_parse_empty(stream)

wn_cstream stream;

{
  return(TRUE);
}



#if 0
bool wn_scan_for_char(stream,c)

wn_cstream stream;
char c;

{
  int fail_mark;
  char parse_c;

  fail_mark = wn_current_mark(stream);

  while(TRUE)
  {
    if(not(wn_parse_char(stream,&parse_c)))
    {
      wn_set_current_mark(stream,fail_mark);  
    
      return(FALSE);
    }

    if(parse_c == c)
    {
      wn_set_current_mark(stream,wn_current_mark(stream)-1);

      return(TRUE);
    }
  }
}
#endif



bool wn_scan_for_eos(stream)

wn_cstream stream;

{
  char c;

  while(wn_parse_char(stream,&c))
    ;

  return(TRUE);
}



bool wn_scan_for_char(stream,c)

wn_cstream stream;
register char c;

{
  int fail_mark,max_len,index;
  char *block;

  fail_mark = wn_current_mark(stream);

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);

    if(max_len == 0)
    {
      break;
    }

    block = access_char(stream);

    wn_arrayscan(&index,block,max_len,(wn_element == c),char);

    if(index >= 0)  /* found it */
    {
      wn_set_current_mark(stream,wn_current_mark(stream)+index);

      return(TRUE);
    }

    wn_set_current_mark(stream,wn_current_mark(stream)+max_len);
  }

  wn_set_current_mark(stream,fail_mark);  
    
  return(FALSE);
}



#if 0
bool wn_scan_for_char_type(stream,char_type)

wn_cstream stream;
char char_type[];

{
  int fail_mark;
  char parse_c;

  fail_mark = wn_current_mark(stream);

  while(TRUE)
  {
    if(not(wn_parse_char(stream,&parse_c)))
    {
      wn_set_current_mark(stream,fail_mark);  
    
      return(FALSE);
    }

    if(char_type[parse_c&CHAR_MASK])
    {
      wn_set_current_mark(stream,wn_current_mark(stream)-1);

      return(TRUE);
    }
  }
}
#endif
bool wn_scan_for_char_type(stream,char_type)

wn_cstream stream;
register char char_type[];

{
  int fail_mark,max_len,index;
  char *block;

  fail_mark = wn_current_mark(stream);

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);

    if(max_len == 0)
    {
      break;
    }

    block = access_char(stream);

    wn_arrayscan(&index,block,max_len,(char_type[wn_element&CHAR_MASK]),char);

    if(index >= 0)  /* found it */
    {
      wn_set_current_mark(stream,wn_current_mark(stream)+index);

      return(TRUE);
    }

    wn_set_current_mark(stream,wn_current_mark(stream)+max_len);
  }

  wn_set_current_mark(stream,fail_mark);  
    
  return(FALSE);
}



#if 0
bool wn_scan_for_not_char_type(stream,char_type)

wn_cstream stream;
char char_type[];

{
  char parse_c;

  while(TRUE)
  {
    if(not(wn_parse_char(stream,&parse_c)))
    {
      return(TRUE);
    }

    if(not(char_type[parse_c&CHAR_MASK]))
    {
      wn_set_current_mark(stream,wn_current_mark(stream)-1);

      return(TRUE);
    }
  }
}
#endif
#if 1
bool wn_scan_for_not_char_type(stream,char_type)

wn_cstream stream;
register char char_type[];

{
  int max_len,index;
  char *block;

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);

    if(max_len == 0)
    {
      break;
    }

    block = access_char(stream);

    wn_arrayscan(&index,block,max_len,
		 (not(char_type[wn_element&CHAR_MASK])),char);

    if(index >= 0)  /* found it */
    {
      wn_set_current_mark(stream,wn_current_mark(stream)+index);

      return(TRUE);
    }

    wn_set_current_mark(stream,wn_current_mark(stream)+max_len);
  }

  return(TRUE);
}
#endif



wn_write_between_marks(out_stream,stream,start_mark,fin_mark)

wn_cstream out_stream,stream;
int start_mark,fin_mark;

{
  int save_mark,len,max_len,desired_len;

  save_mark = wn_current_mark(stream);

  wn_set_current_mark(stream,start_mark);

  desired_len = fin_mark-start_mark;

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);
    len = wn_min(max_len,desired_len);

    wn_write_block(out_stream,access_char(stream),len);

    desired_len -= len;

    if(desired_len == 0)
    {
      break;
    }

    start_mark += len;

    wn_set_current_mark(stream,start_mark);
  }

  wn_set_current_mark(stream,save_mark);
}



wn_copy_between_marks_to_string(string,stream,start_mark,fin_mark)

char *string;
wn_cstream stream;
int start_mark,fin_mark;

{
  int save_mark,len,max_len,desired_len;

  save_mark = wn_current_mark(stream);

  wn_set_current_mark(stream,start_mark);

  desired_len = fin_mark-start_mark;

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);
    len = wn_min(max_len,desired_len);

    wn_blkcpy((ptr)string,(ptr)(access_char(stream)),len);

    string += len;
    desired_len -= len;

    if(desired_len == 0)
    {
      break;
    }

    start_mark += len;

    wn_set_current_mark(stream,start_mark);
  }

  *string = '\0';  /* terminate string */

  wn_set_current_mark(stream,save_mark);
}



bool wn_between_marks_equal_string(string,stream,start_mark,fin_mark)

char *string;
wn_cstream stream;
int start_mark,fin_mark;

{
  int save_mark,len,max_len,desired_len,string_len;

  string_len = wn_strlen(string);
  desired_len = fin_mark-start_mark;

  if(string_len != desired_len)
  {
    return(FALSE);
  }

  save_mark = wn_current_mark(stream);

  wn_set_current_mark(stream,start_mark);

  while(TRUE)
  {
    get_longest_block_parse_len(&max_len,stream);
    len = wn_min(max_len,desired_len);

    if(not(wn_blkeq((ptr)string,len,(ptr)(access_char(stream)),len)))
    {
      wn_set_current_mark(stream,save_mark);

      return(FALSE);
    }

    string += len;
    desired_len -= len;

    if(desired_len == 0)
    {
      break;
    }

    start_mark += len;

    wn_set_current_mark(stream,start_mark);
  }

  wn_set_current_mark(stream,save_mark);

  return(TRUE);
}



local get_longest_block_parse_len(pmax_len,stream)

int *pmax_len;
wn_cstream stream;

{
  int max1,max2;

  max1 = stream->high_mark - stream->current_mark;
  max2 = stream->block_size 
           - (stream->current_mark & stream->mod_mask);

  *pmax_len = wn_min(max1,max2);
}



bool wn_cstream_debug(stream)

wn_cstream stream;

{
  int mark,low_mark,high_mark,current_mark;
  char c;

  current_mark = wn_current_mark(stream);

  low_mark = wn_max(current_mark-20,stream->low_mark);
  high_mark = current_mark+20;

  fprintf(wn_stderr,"%d (line),%d (char): ||||)",
	  wn_linenum_of_current_mark(stream),current_mark);
  wn_set_current_mark(stream,low_mark);

  for(mark=low_mark;mark<high_mark;mark++)
  {
    if(mark == current_mark)
    {
      fprintf(wn_stderr,">");
    }

    if(wn_parse_eos(stream))
    {
      fprintf(wn_stderr,"#");
    }
    else
    {
      wn_parse_char(stream,&c);  

      fprintf(wn_stderr,"%c",c);
    }

    if(mark == current_mark)
    {
      fprintf(wn_stderr,"<");
    }
  }
    
  fprintf(wn_stderr,"(||||\n");

  wn_fflush(wn_stderr);

  wn_set_current_mark(stream,current_mark);

  return(TRUE);
}



wn_write_block(stream,block,len)

wn_cstream stream;
char *block;
int len;

{
  int actually_moved;

  if(stream->pretty_block_level > 0)
  {
    wn_blkcpy((ptr)&((stream->pretty_buffer)[stream->pretty_buffer_len]),
              (ptr)block,
              len);
    (stream->pretty_buffer_len) += len;
  }
  else
  {
    wn_current_mark(stream) += len;
    (*(stream->pmove_block))(block,&actually_moved,len,
                             &(stream->move_block_ptr));
  }
}



wn_write_char(stream,c)

wn_cstream stream;
char c;

{
  int actually_moved;

  if(stream->pretty_block_level > 0)
  {
    (stream->pretty_buffer)[stream->pretty_buffer_len] = c;
    (stream->pretty_buffer_len)++;
  }
  else
  {
    wn_current_mark(stream) += 1;
    (*(stream->pmove_block))(&c,&actually_moved,1,&(stream->move_block_ptr));
  }
}



wn_write_literal_char(stream,c)

wn_cstream stream;
char c;

{
  int actually_moved;

  if(stream->pretty_block_level > 0)
  {
    (stream->pretty_buffer)[stream->pretty_buffer_len] = c;
    (stream->pretty_buffer_len)++;
  }
  else
  {
    wn_current_mark(stream) += 1;
    (*(stream->pmove_block))(&c,&actually_moved,1,&(stream->move_block_ptr));
  }
}



wn_raw_write_literal_char(stream,c)

wn_cstream stream;
char c;

{
  int actually_moved;

  wn_current_mark(stream) += 1;
  (*(stream->pmove_block))(&c,&actually_moved,1,&(stream->move_block_ptr));
}



/*ARGSUSED*/ wn_write_eos(stream)

wn_cstream stream;

{
  wn_assert(stream->pretty_block_level == 0);
}



/*ARGSUSED*/ wn_write_empty(stream)

wn_cstream stream;

{
}




