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

#include <stdio.h>
#include "wnlib.h"
#include "wnrtab.h"
#include "wnargp.h"
#include "wnsll.h"
#include "wnhtab.h"



#define EMPTY 0
#define WORD  1
#define PUNCT 2


local int window_size,output_length,seed;

local wn_htab word_table;
local wn_sll input_doc;

local char **window;

local int c;

local int col_count;
local char *last_word;



local void parse_input(argc,argv)
 
int argc;
char *argv[];
  
{
  static char *window_size_string,*output_length_string,*seed_string;
  static wn_arg_format arg_format[] =
  {
    WN_POS_STRING(&window_size_string,"window_size",WN_REQUIRED),
    WN_POS_STRING(&output_length_string,"output_length",WN_REQUIRED),
    WN_POS_STRING(&seed_string,"seed",WN_OPTIONAL),
    WN_ARG_TERMINATE
  };

  seed_string = "0";

  wn_parse_args(argc,argv,arg_format);

  sscanf(window_size_string,"%d",&window_size);
  sscanf(output_length_string,"%d",&output_length);
  sscanf(seed_string,"%d",&seed);
}


local void initialize()
{
  extern int wn_strhash();
  extern bool wn_streq();
  extern void wn_ptrcpy();
  extern void wn_do_nothing();

  wn_seed_random_number_generator(seed);

  wn_mkhtab(&word_table,(wn_strhash),(wn_streq),(wn_ptrcpy),(wn_do_nothing));
  
  input_doc = NULL;

  window = (char **)wn_zalloc(window_size*sizeof(char *));

  c = ' ';

  col_count = 0;
  last_word = "";
}


local bool parse_blanks()
{
  int count;

  count = 0;

  while(TRUE)
  {
    if(!isspace(c))
    {
      break;
    }

    c = getchar();

    ++count;
  }

  return(count > 0);
}


local bool my_isalnum(c)

int c;

{
  return(
	  isalnum(c)
	    ||
          (c == '_')
        );
}


local bool parse_word(word)

char word[];

{
  int count;

  count = 0;

  if(parse_blanks())
  {
    strcpy(word," ");

    return(TRUE);
  }

  if((c == EOF)||isspace(c))
  {
    return(FALSE);
  }
  else if(my_isalnum(c))
  {
    do
    {
      word[count] = c;
      ++count;
      c = getchar();
    } while(my_isalnum(c));
  }
  else /* other printable character */ 
  {
    word[count] = c;
    ++count;
    c = getchar();
  }

  word[count] = '\0';

  return(TRUE);
}


local bool read_word(pword)

char **pword;

{
  char word_buffer[200];
  char *word_buffer_copy;

  if(!parse_word(word_buffer))
  {
    return(FALSE);
  }

  if(!wn_hget(pword,word_table,word_buffer))
  {
    wn_stracpy(&word_buffer_copy,word_buffer);

    wn_hins(word_buffer_copy,word_table,word_buffer_copy);

    *pword = word_buffer_copy;
  }

  return(TRUE);
}


local void read_input()
{
  char *word;

  while(read_word(&word))
  {
    wn_sllins(&input_doc,word);
  }

  wn_sllrev(&input_doc);
}


local bool window_matches_loc(el)

wn_sll el;

{
  int i;
  char *word,*window_word;

  i = 0;

  for(;el!=NULL;el=el->next)
  {
    if(i >= window_size)
    {
      return(TRUE);
    }
    window_word = window[i];
    if(window_word == NULL)
    {
      return(TRUE);
    }

    word = (char *)(el->contents);

    if(window_word != word)
    {
      return(FALSE);
    }

    ++i;
  }

  return(FALSE);
}


local void count_window_matches(pcount)

int *pcount;

{
  wn_sll el;

  *pcount = 0;

  for(el=input_doc;el!=NULL;el=el->next)
  {
    if(window_matches_loc(el))
    {
      ++(*pcount);
    }
  }
}


local void get_nth_window_match_tail(pword,index)

char **pword;
int index;

{
  wn_sll el;
  int count;

  count = 0;

  for(el=input_doc;el!=NULL;el=el->next)
  {
    if(window_matches_loc(el))
    {
      if(count == index)
      {
	int i;

	for(i=0;(i<window_size)&&(window[i] != NULL);++i)
	{
	  el=el->next;
	}

	*pword = (char *)(el->contents);

	return;
      }

      ++count;
    }
  }
}


local void add_word_to_window(word)

char *word;

{
  int i;

  for(i=0;i<window_size;++i)
  {
    if(window[i] == NULL)
    {
      window[i] = word;
      return;
    }
  }

  for(i=0;i<window_size-1;++i)
  {
    window[i] = window[i+1]; 
  }

  window[window_size-1] = word;
}


local void shrink_window()
{
  int i;

  for(i=0;i<window_size-1;++i)
  {
    window[i] = window[i+1]; 
  }

  window[window_size-1] = NULL;
}


local void generate_word(pword)

char **pword;

{
  int count,index;

try_again:
  count_window_matches(&count);
  if(count == 0)
  {
    shrink_window();
    goto try_again;
  }

  index = wn_random_mod_int(count);

  get_nth_window_match_tail(pword,index);

  add_word_to_window(*pword);
}


local void write_word(word)

char *word;

{
  if(wn_streq(word," "))
  {
    if(col_count > 60)
    {
      printf("\n");

      col_count = 0;
    }
    else if(
	     wn_streq(last_word,":")
	       ||
	     wn_streq(last_word,".")
	       ||
	     wn_streq(last_word,"!")
	       ||
	     wn_streq(last_word,"?")
	   )
    {
      printf("  ");

      col_count += 2;
    }
    else
    {
      printf(" ");

      col_count += 1;
    }
  }
  else
  {
    printf("%s",word);

    col_count += strlen(word);
  }

  last_word = word;

  fflush(stdout);
}


local void write_output()
{
  char *word;
  int i;

  window[0] = (char *)(input_doc->contents);
  write_word(window[0]);

  for(i=0;i<output_length;++i)
  {
    generate_word(&word);
    write_word(word);
  }

  printf("\n");
}


void main(argc,argv)

int argc;
char *argv[];

{
  parse_input(argc,argv);

  wn_gpmake("no_free");

  initialize();

  read_input();
  write_output();

  wn_gpfree();
}


