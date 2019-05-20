/******************************************************************************

wn_make_file_parse_cstream(&stream,f)
wn_make_file_write_cstream(&stream,f)
wn_make_string_parse_cstream(&stream,string)

void wn_file_parse_move_block(block,pactually_moved,request_moved,pf)
void wn_file_write_move_block(block,pactually_moved,request_moved,pf)

void wn_string_parse_move_block(block,pactually_moved,request_moved,pstring)

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
#include "wnmax.h"

#include "wncstr.h"



local void wn_file_parse_move_block
(
  char block[],
  int *pactually_moved,
  int request_moved,
  wn_file *pf
)
{
  *pactually_moved = wn_fread(block,1,request_moved,*pf);
}


local void wn_file_write_move_block
(
  char block[],
  int *pactually_moved,
  int request_moved,
  wn_file *pf
)
{
  *pactually_moved = wn_fwrite(block,1,request_moved,*pf);
}


local void wn_string_parse_move_block
(
  register char *block,
  int *pactually_moved,
  register int request_moved,
  char **pstring
)
{
  register char *string;
  register int actually_moved;

  actually_moved = 0;

  for(string = *pstring;*string != '\0';string++)
  {
    if(actually_moved >= request_moved)
    {
      break;
    }

    *block = *string;

    block++; actually_moved++;
  }

  *pactually_moved = actually_moved;
  *pstring = string;
}


void wn_make_file_parse_cstream(wn_cstream *pstream,wn_file f)
{
  wn_make_cstream(pstream,
		  (void (*)(char *,int *,int,ptr))(wn_file_parse_move_block),
		  (ptr)f,2048,WN_PARSE);
}


void wn_make_file_write_cstream(wn_cstream *pstream,wn_file f)
{
  wn_make_cstream(pstream,
		  (void (*)(char *,int *,int,ptr))(wn_file_write_move_block),
		  (ptr)f,2048,WN_WRITE);
}


void wn_make_string_parse_cstream(wn_cstream *pstream,char string[])
{
  wn_make_cstream(pstream,
		  (void (*)(char *,int *,int,ptr))(wn_string_parse_move_block),
		  (ptr)string,32,
                  WN_PARSE);
}



