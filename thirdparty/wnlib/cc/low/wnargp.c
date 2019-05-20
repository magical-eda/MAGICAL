/******************************************************************************

wn_push_parse_args_error_print(ppassed_print_error)
wn_pop_parse_args_error_print()

wn_parse_args(argc,argv,arg_array)

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
#include "wnargp.h"



local bool initialized = FALSE;

local char *command_name;

local void (*pprint_error)(/* string */);
local wn_stack print_error_stack;



local initialize()
{
  void default_print_error();

  if(initialized)
  {
    return;
  }

  wn_gppush(wn_defaultgp());

  wn_mkstack(&print_error_stack);
  pprint_error = (default_print_error);

  wn_gppop();

  initialized = TRUE;
}



local void default_print_error(string)

char string[];

{
  fprintf(wn_stderr,"%s\n",string);

  exit(-1);
}



wn_push_parse_args_error_print(ppassed_print_error)

void (*ppassed_print_error)(/* string */);

{
  initialize();

  wn_push(print_error_stack,pprint_error);

  pprint_error = ppassed_print_error;
}



wn_pop_parse_args_error_print()
{
  pprint_error = (void (*)())wn_stacktop(print_error_stack);

  wn_pop(print_error_stack);
}



wn_parse_args(argc,argv,arg_array)

int argc;
char *argv[];
wn_arg_format arg_array[];

{
  initialize();

  wn_assert(argc > 0);

  command_name = *argv; 
  ++argv;  --argc;

  null_out_arg_values(arg_array);

  parse_args(argc,argv,arg_array);

  check_for_missing_required_args(arg_array);
}



local null_out_arg_values(arg_array)

wn_arg_format *arg_array;

{
  for(;(*arg_array).type != 0;++arg_array)
  {
    switch((*arg_array).type)
    {
      case(WN_POS_STRING_TYPE):
      {
	(*arg_array).positional_arg_present = FALSE;

        if((*arg_array).required)
        {
          *(char **)((*arg_array).value_pointer) = NULL;
        }
      } break;
      case(WN_KEY_BOOL_TYPE):
      {
        *(bool *)((*arg_array).value_pointer) = FALSE;
      } break;
      case(WN_KEY_STRING_TYPE):
      {
        if((*arg_array).required)
        {
          *(char **)((*arg_array).value_pointer) = NULL;
        }
      } break;
      default:
        wn_crash();
    }
  }
}



local parse_args(argc,argv,arg_array)

int argc;
char *argv[];
wn_arg_format arg_array[];

{
  parse_keyword_args(argc,argv,arg_array);
  parse_positional_args(argc,argv,arg_array);
}



local parse_keyword_args(argc,argv,arg_array)

int argc;
char *argv[];
wn_arg_format arg_array[];

{
  char *arg,*keyword;
  wn_arg_format *matching_arg_element;

  while(argc > 0)
  {
    arg = *argv;

    --argc;  ++argv;

    if(arg[0] == '-')
    {
      if(arg[1] == '\0')    /* - */
      {
        break;  /* read_naked_args, no more keyword args possible */
      }
      else
      {
        keyword = arg+1;   /* skip - */

        find_matching_arg_element(&matching_arg_element,arg_array,keyword);

        if(matching_arg_element == NULL)
        {
          print_usage_message_and_exit(arg_array);
        }
      
        switch((*matching_arg_element).type)
        {
          case(WN_KEY_BOOL_TYPE):
          {
            *(bool *)((*matching_arg_element).value_pointer) = TRUE;
          } break;
          case(WN_KEY_STRING_TYPE):
          {
            if(argc <= 0)
            {
              print_usage_message_and_exit(arg_array);
            }
                
            arg = *argv;

            --argc;  ++argv;

            *(char **)((*matching_arg_element).value_pointer) = arg;
          } break;
          default:
            wn_crash();
        }
      }
    }
  }
}



local find_matching_arg_element(pmatching_arg_element,arg_array,keyword)

wn_arg_format **pmatching_arg_element,*arg_array;
char keyword[];

{
  *pmatching_arg_element = NULL;

  for(;(*arg_array).type != WN_TERMINATE_TYPE;++arg_array)
  {
    if(
        ((*arg_array).keyword != NULL)
          &&
        wn_streq(keyword,(*arg_array).keyword)
      )
    {
      *pmatching_arg_element = arg_array;

      break;
    }
  }
}



local parse_positional_args(argc,argv,arg_array)

int argc;
char *argv[];
wn_arg_format arg_array[];

{
  int count;

  count_positional_args(&count,argc,argv,arg_array);
  allocate_positional_args(count,arg_array);

  fill_positional_args(argc,argv,arg_array);
}



local count_positional_args(pcount,argc,argv,arg_array)

int *pcount,argc;
char *argv[];
wn_arg_format arg_array[];

{
  bool read_naked_args;
  int position;
  char *arg,*keyword;
  wn_arg_format *matching_arg_element;

  read_naked_args = FALSE;
  position = 0;

  while(argc > 0)
  {
    arg = *argv;

    --argc;  ++argv;

    if(read_naked_args)
    {
      ++position;
    }
    else
    {
      if(arg[0] == '-')
      {
        if(arg[1] == '\0')    /* - */
        {
          read_naked_args = TRUE;
        }
        else
        {
          keyword = arg+1;   /* skip - */

          find_matching_arg_element(&matching_arg_element,arg_array,keyword);

          if(matching_arg_element == NULL)
          {
            wn_crash();
          }
      
          switch((*matching_arg_element).type)
          {
            case(WN_KEY_BOOL_TYPE):
	    {
	    } break;
            case(WN_KEY_STRING_TYPE):
            {
              if(argc <= 0)
              {
		wn_crash();
              }
                
              --argc;  ++argv;  /* skip over value */
            } break;
            default:
              wn_crash();
          }
        }
      }
      else
      {
        ++position;
      }
    }
  }

  *pcount = position;
}



local allocate_positional_args(count,arg_array)

int count;
wn_arg_format arg_array[];

{
  wn_arg_format *arg_array_ptr;

  for(arg_array_ptr=arg_array;
      (*arg_array_ptr).type != WN_TERMINATE_TYPE;
      ++arg_array_ptr)
  {
    if((*arg_array_ptr).type == WN_POS_STRING_TYPE)
    {
      if((*arg_array_ptr).required)
      {
        if(count == 0)
        {
          print_usage_message_and_exit(arg_array);
        }

	(*arg_array_ptr).positional_arg_present = TRUE;

	--count;
      }
    }
  }

  for(arg_array_ptr=arg_array;
      (*arg_array_ptr).type != WN_TERMINATE_TYPE;
      ++arg_array_ptr)
  {
    if((*arg_array_ptr).type == WN_POS_STRING_TYPE)
    {
      if(not((*arg_array_ptr).required))
      {
        if(count == 0)
        {
	  break;
        }

	(*arg_array_ptr).positional_arg_present = TRUE;

	--count;
      }
    }
  }

  if(count != 0)  /* not all args used, or not all requirements met */
  {
    print_usage_message_and_exit(arg_array);
  }
}



local fill_positional_args(argc,argv,arg_array)

int argc;
char *argv[];
wn_arg_format arg_array[];

{
  bool read_naked_args;
  char *arg,*keyword;
  wn_arg_format *positional_arg_element,*matching_arg_element;

  read_naked_args = FALSE;
  positional_arg_element = arg_array;

  while(argc > 0)
  {
    arg = *argv;

    --argc;  ++argv;

    if(read_naked_args)
    {
      find_next_positional_arg(&positional_arg_element);

      *(char **)((*positional_arg_element).value_pointer) = arg;

      ++positional_arg_element;
    }
    else
    {
      if(arg[0] == '-')
      {
        if(arg[1] == '\0')    /* - */
        {
          read_naked_args = TRUE;
        }
        else
        {
          keyword = arg+1;   /* skip - */

          find_matching_arg_element(&matching_arg_element,arg_array,keyword);

          if(matching_arg_element == NULL)
          {
            wn_crash();
          }
      
          switch((*matching_arg_element).type)
          {
            case(WN_KEY_BOOL_TYPE):
	    {
	    } break;
            case(WN_KEY_STRING_TYPE):
            {
              if(argc <= 0)
              {
		wn_crash();
              }
                
              --argc;  ++argv;  /* skip over value */
            } break;
            default:
              wn_crash();
          }
        }
      }
      else
      {
        find_next_positional_arg(&positional_arg_element);

        *(char **)((*positional_arg_element).value_pointer) = arg;

        ++positional_arg_element;
      }
    }
  }
}



local find_next_positional_arg(pmatching_arg_element)

wn_arg_format **pmatching_arg_element;

{
  for(;(**pmatching_arg_element).type != WN_TERMINATE_TYPE;
      ++(*pmatching_arg_element))
  {
    if((**pmatching_arg_element).type == WN_POS_STRING_TYPE)
    {
      if((**pmatching_arg_element).positional_arg_present)
      {
	return;
      }
    }
  }

  wn_crash();
}



local check_for_missing_required_args(arg_array)

wn_arg_format *arg_array;

{
  wn_arg_format *arg_element;

  for(arg_element=arg_array;(*arg_element).type != 0;arg_element++)
  {
    if((*arg_element).required)
    {
      switch((*arg_element).type)
      {
        case(WN_POS_STRING_TYPE):
        {
          if(*(char **)((*arg_element).value_pointer) == NULL)
          {
            print_usage_message_and_exit(arg_array);
          }
        } break;
        case(WN_KEY_STRING_TYPE):
        {
          if(*(char **)((*arg_element).value_pointer) == NULL)
          {
            print_usage_message_and_exit(arg_array);
          }
        } break;
        default:
          wn_crash();
      }
    }
  }
}



local print_usage_message_and_exit(arg_array)

wn_arg_format *arg_array;

{
  print_usage_message(arg_array);

  exit(-1);
}



local print_usage_message(arg_array)

wn_arg_format *arg_array;

{
  char error_message_string[500],buf[100];

  sprintf(buf,"usage: %s",command_name);
  wn_strcpy(error_message_string,buf);

  for(;(*arg_array).type != 0;arg_array++)
  {
    print_arg_element_usage(error_message_string,arg_array);
  }

  (*pprint_error)(error_message_string);
}



local print_arg_element_usage(error_message_string,arg_element)

char error_message_string[];
wn_arg_format *arg_element;

{
  char buf[100];

  sprintf(buf," ");
  wn_strcat(error_message_string,buf);

  if(not((*arg_element).required))
  {
    sprintf(buf,"[");
    wn_strcat(error_message_string,buf);
  } 

  switch((*arg_element).type)
  {
    case(WN_POS_STRING_TYPE):
    {
      sprintf(buf,"%s",(*arg_element).arg_name);
      wn_strcat(error_message_string,buf);
    } break;
    case(WN_KEY_BOOL_TYPE):
    {
      sprintf(buf,"-%s",(*arg_element).keyword);
      wn_strcat(error_message_string,buf);
    } break;
    case(WN_KEY_STRING_TYPE):
    {
      sprintf(buf,"-%s %s",(*arg_element).keyword,(*arg_element).arg_name);
      wn_strcat(error_message_string,buf);
    } break;
    default:
      wn_crash();
  }

  if(not((*arg_element).required))
  {
    sprintf(buf,"]");
    wn_strcat(error_message_string,buf);
  } 
}




