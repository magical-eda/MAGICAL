/****************************************************************************

COPYRIGHT NOTICE:

  The source code in this directory is provided free of
  charge to anyone who wants it.  It is in the public domain
  and therefore may be used by anybody for any purpose.  It
  is provided "AS IS" with no warranty of any kind
  whatsoever.  For further details see the README files in
  the wnlib parent directory.

****************************************************************************/


#include "wnlib.h"
#include "wnasrt.h"
#include "wnio.h"


#define NORMAL             0
#define IN_COMMENT         1
#define IN_DOUBLE_QUOTE    2
#define IN_SINGLE_QUOTE    3

#define SLASH_SEEN         1
#define STAR_SEEN          2
#define EQUAL_SEEN         3
#define EQUAL_PREFIX_SEEN  4

#define F_SEEN             1
#define FO_SEEN            2
#define FOR_SEEN           3


#define not(x)  (!(x))


local void read_and_check_file(wn_file f,char file_name[])
{
  char c;
  int line_num,paren_depth,for_paren_depth;
  int state,sub_state,for_state;

  (void)fprintf(stderr,"%s:\n",file_name);

  state = NORMAL;
  sub_state = NORMAL;
  for_state = NORMAL;
  line_num = 1;
  paren_depth = 0;
  for_paren_depth = 0;

  while(not(wn_feof(f)))
  {
    c = wn_getc(f);

    if(wn_feof(f))
    {
      break;
    }
    
    switch(state)
    {
      case(NORMAL):
      {
        if((paren_depth > 0)&&(for_state != FOR_SEEN)&&
           (sub_state == EQUAL_SEEN)&&(c != '='))
        {
          (void)fprintf(stderr,"%s(%d): assignment in parentheses.\n",
                        file_name,line_num);
        }

        switch(c)
        {
          case('('):
          {
            paren_depth++;

            sub_state = NORMAL;
          } break;
          case(')'):
          {
            paren_depth--;

            if(paren_depth < 0)
            {  
              (void)fprintf(stderr,"%s(%d): too many right parentheses.\n",
                            file_name,line_num);
              paren_depth = 0;
            }

            if(paren_depth < for_paren_depth)
            {
              for_state = NORMAL;
              for_paren_depth = 0;
            }

            sub_state = NORMAL;
          } break;
          case('\"'):
          {
            sub_state = NORMAL;
            for_state = NORMAL;
            state = IN_DOUBLE_QUOTE;
          } break;
          case('\''):
          {
            sub_state = NORMAL;
            for_state = NORMAL;
            state = IN_SINGLE_QUOTE;
          } break;
          case('='):
          {
            switch(sub_state)
            {
              case(NORMAL):
              {
                sub_state = EQUAL_SEEN;
              } break;
              case(EQUAL_SEEN):
              case(EQUAL_PREFIX_SEEN):
              {
                sub_state = NORMAL;
              } break;
            }
          } break;
          case('>'):
          case('<'):
          case('!'):
          {
            sub_state = EQUAL_PREFIX_SEEN;
          } break;
          case('/'):
          {
            sub_state = SLASH_SEEN;
          } break;
          case('*'):
          {
            if(sub_state == SLASH_SEEN)
            {
              state = IN_COMMENT;
              sub_state = NORMAL;
            }
            else
            {
              sub_state = NORMAL;
            }
          } break;        
          case('\\'):
          {
            c = wn_getc(f);  /* skip char */

            sub_state = NORMAL;
          } break;
          case(';'):
          {
            sub_state = NORMAL;

            if(paren_depth < for_paren_depth)
            {
              for_state = NORMAL;
            }
          } break;
          case('f'):
          {
            sub_state = NORMAL;

            if(for_state != FOR_SEEN)
            {
                for_state = F_SEEN;
            }
          } break;
          case('o'):
          {
            sub_state = NORMAL;

            if(for_state != FOR_SEEN)
            {
              if(for_state == F_SEEN)
              {
                for_state = FO_SEEN;
              }
              else
              {
                for_state = NORMAL;
              }
            }
          } break;
          case('r'):
          {
            sub_state = NORMAL;

            if(for_state != FOR_SEEN)
            {
              if(for_state == FO_SEEN)
              {
                for_state = FOR_SEEN;
                for_paren_depth = paren_depth+1;
              }
              else
              {
                for_state = NORMAL;
              }
            }
          } break;
          default:
          {
            sub_state = NORMAL;
          } break;        
        }
      } break;
      case(IN_COMMENT):
      {
        switch(sub_state)
        {
          case(NORMAL):
          {
            if(c == '*')
            { 
              sub_state = STAR_SEEN;
            }
          } break;
          case(STAR_SEEN):
          {
            if(c == '/')
            {
              sub_state = NORMAL;
              state = NORMAL;
            }
            else if(c == '*')
            { 
              sub_state = STAR_SEEN;
            }
            else
            {
              sub_state = NORMAL;
            }
          } break;
          default:
            wn_assert_notreached();
        }
      } break;
      case(IN_DOUBLE_QUOTE):
      {
        switch(c)
        {
          case('\"'):
          {
            state = NORMAL;
          } break;
          case('\\'):
          {
            c = wn_getc(f);  /* skip char */
          } break;
          case('\n'):
          {
            (void)fprintf(stderr,"%s(%d): end of line in double quote.\n",
                          file_name,line_num);
            state = NORMAL;
          } break;
        }
      } break;
      case(IN_SINGLE_QUOTE):
      {
        switch(c)
        {
          case('\''):
          {
            state = NORMAL;
          } break;
          case('\\'):
          {
            c = wn_getc(f);  /* skip char */
          } break;
          case('\n'):
          {
            (void)fprintf(stderr,"%s(%d): end of line in single quote.\n",
                          file_name,line_num);
            state = NORMAL;
          } break;
        }
      } break;
      default:
        wn_assert_notreached();
    }

    if(c == '\n')
    {
      line_num++;
    }
  }

  if(paren_depth > 0)
  {
    (void)fprintf(stderr,"%s(%d): end of file in parentheses.\n",
                  file_name,line_num);
    return;
  }

  switch(state)
  {
    case(NORMAL):
      break;
    case(IN_COMMENT):
    {
      (void)fprintf(stderr,"%s(%d): end of file in comment.\n",
                    file_name,line_num);
      return;
    } break;
    case(IN_DOUBLE_QUOTE):
    {
      (void)fprintf(stderr,"%s(%d): end of file in double quote.\n",
                    file_name,line_num);
      return;
    } break;
    case(IN_SINGLE_QUOTE):
    {
      (void)fprintf(stderr,"%s(%d): end of file in single quote.\n",
                    file_name,line_num);
      return;
    } break;
    default:
      wn_assert_notreached();
  }
}


local void wn_lint(char file_name[])
{
  wn_file f;

  f = wn_fopen(file_name,"r");

  read_and_check_file(f,file_name);
  wn_fflush(stderr);

  wn_fclose(f);
}


void main(int argc,char *argv[])
{
  char *file_name;
  int i;

  if(argc <= 1)
  {
    (void)fprintf(stderr,"usage: wnlint <file1> <file2> ...\n");

    return;
  }

  for(i=1;i<argc;i++)
  {
    file_name = argv[i];

    wn_lint(file_name);
  }    
}



