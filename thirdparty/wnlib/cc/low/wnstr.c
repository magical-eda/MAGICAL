/*************************************************************************

  wn_strncpy(out,in,n)  * differs from strncpy: copies AT MOST n,
                          null-terminates ALWAYS *
  wn_strcpy(out,in)
  wn_stracpy(&out,in)

  wn_strncat(out,in,n)  * differs from strncat: OUT has length AT MOST n,
                          null-terminates ALWAYS *
  wn_strcat(out,in)
  wn_stracat(&out,s1,s2)
  wn_stracat3(&out,s1,s2,s3)
  wn_stracat4(&out,s1,s2,s3,s4)
  wn_stracat5(&out,s1,s2,s3,s4,s5)
  wn_stracat6(&out,s1,s2,s3,s4,s5,s6)

  int wn_strcmp(s1,s2)
  int wn_numstrcmp(s1,s2)
  bool wn_streq(s1,s2)
  bool wn_streqnc(s1,s2)

  int wn_strlen(s)

  bool wn_char_in_string(c,s)

*************************************************************************/
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
#include <ctype.h>
#include "wnlib.h"



extern char *strcpy(),*strcat();



wn_stracpy(pout,in)

char *pout[],in[];

{
  char *out;
  int len;

  len = strlen(in);

  *pout = out = (char *)wn_alloc(len+1);

  wn_blkcpy((ptr)out,(ptr)in,len);
  out += len;

  *out = '\0';
}



wn_strcpy(out,in)

char out[],in[];

{
  (void)strcpy(out,in);
}



wn_strncpy(out,in,n)

register char *out,*in;
register int n;

{
  register char ci;

  ci = *in;

  while((n > 0)&&(ci != '\0'))
  {
    *out = ci;

    ++out; ++in;
    --n;

    ci = *in;
  }

  *out = '\0';
}



wn_stracat(pout,s1,s2)

char *pout[],s1[],s2[];

{
  int l1,l2;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);

  *pout = out = (char *)wn_alloc(l1+l2+1);

  wn_blkcpy((ptr)out,(ptr)s1,l1);
  out += l1;
  wn_blkcpy((ptr)out,(ptr)s2,l2);
  out += l2;

  *out = '\0';
}



wn_stracat3(pout,s1,s2,s3)

char *pout[],s1[],s2[],s3[];

{
  int l1,l2,l3;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);
  l3 = strlen(s3);

  *pout = out = (char *)wn_alloc(l1+l2+l3+1);

  wn_blkcpy((ptr)out,(ptr)s1,l1);
  out += l1;
  wn_blkcpy((ptr)out,(ptr)s2,l2);
  out += l2;
  wn_blkcpy((ptr)out,(ptr)s3,l3);
  out += l3;

  *out = '\0';
}



wn_stracat4(pout,s1,s2,s3,s4)

char *pout[],s1[],s2[],s3[],s4[];

{
  int l1,l2,l3,l4;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);
  l3 = strlen(s3);
  l4 = strlen(s4);

  *pout = out = (char *)wn_alloc(l1+l2+l3+l4+1);

  wn_blkcpy((ptr)out,(ptr)s1,l1);
  out += l1;
  wn_blkcpy((ptr)out,(ptr)s2,l2);
  out += l2;
  wn_blkcpy((ptr)out,(ptr)s3,l3);
  out += l3;
  wn_blkcpy((ptr)out,(ptr)s4,l4);
  out += l4;

  *out = '\0';
}



wn_stracat5(pout,s1,s2,s3,s4,s5)

char *pout[],s1[],s2[],s3[],s4[],s5[];

{
  int l1,l2,l3,l4,l5;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);
  l3 = strlen(s3);
  l4 = strlen(s4);
  l5 = strlen(s5);

  *pout = out = (char *)wn_alloc(l1+l2+l3+l4+l5+1);

  wn_blkcpy((ptr)out,(ptr)s1,l1);
  out += l1;
  wn_blkcpy((ptr)out,(ptr)s2,l2);
  out += l2;
  wn_blkcpy((ptr)out,(ptr)s3,l3);
  out += l3;
  wn_blkcpy((ptr)out,(ptr)s4,l4);
  out += l4;
  wn_blkcpy((ptr)out,(ptr)s5,l5);
  out += l5;

  *out = '\0';
}



wn_stracat6(pout,s1,s2,s3,s4,s5,s6)

char *pout[],s1[],s2[],s3[],s4[],s5[],s6[];

{
  int l1,l2,l3,l4,l5,l6;
  char *out;

  l1 = strlen(s1);
  l2 = strlen(s2);
  l3 = strlen(s3);
  l4 = strlen(s4);
  l5 = strlen(s5);
  l6 = strlen(s6);

  *pout = out = (char *)wn_alloc(l1+l2+l3+l4+l5+l6+1);

  wn_blkcpy((ptr)out,(ptr)s1,l1);
  out += l1;
  wn_blkcpy((ptr)out,(ptr)s2,l2);
  out += l2;
  wn_blkcpy((ptr)out,(ptr)s3,l3);
  out += l3;
  wn_blkcpy((ptr)out,(ptr)s4,l4);
  out += l4;
  wn_blkcpy((ptr)out,(ptr)s5,l5);
  out += l5;
  wn_blkcpy((ptr)out,(ptr)s6,l6);
  out += l6;

  *out = '\0';
}



wn_strcat(out,in)

char out[],in[];

{
  (void)strcat(out,in);
}



wn_strncat(out,in,n)

register char out[],in[];
register int n;

{
  register char ci;

  while((n > 0)&&(*out != '\0'))
  {
    ++out;
    --n;
  }

  ci = *in;

  while((n > 0)&&(ci != '\0'))
  {
    *out = ci;

    ++out; ++in;
    --n;

    ci = *in;
  }

  *out = '\0';
}



int wn_strcmp(s1,s2)

char s1[],s2[];

{
  return(strcmp(s1,s2));
}



int wn_numstrcmp(s1,s2)

register char *s1,*s2;

{
  register char c1,c2;
  register char *ss1,*ss2;
  register int diff;

  while(TRUE)
  {
    c1 = *s1;  c2 = *s2;

    if(isdigit(c1)&&isdigit(c2))  /* compare numeric strings */
    {
      ss1 = s1;  ss2 = s2;

      for(++s1;isdigit(*s1);++s1)
        ;
      for(++s2;isdigit(*s2);++s2)
        ;
       
      diff = (s1 - ss1)-(s2 - ss2);  /* which numeric string longer */

      if(diff != 0)
      {
        return(diff);
      }

      while(TRUE)
      {
        if(ss1 == s1)
        {
          break;
        }

        diff = (*ss1) - (*ss2);

        if(diff != 0)
        {
          return(diff);
        }

        ++ss1;  ++ss2;
      }
    }
    else
    {
      if(c1 == '\0')
      {
        if(c2 == '\0')
        {
          return(0);
        }
        else  /* (c1 == '\0')&&(c2 != '\0') */
        {
          return(-1);
        }
      }
      else /* c1 != '\0' */
      {
        if(c2 == '\0')
        {
          return(1);
        }
      }

      diff = c1 - c2;

      if(diff != 0)
      {
        return(diff);
      }

      ++s1;  ++s2;
    }
  }
}



int wn_strncmp(s1,s2,n)

char s1[],s2[];
int n;

{
  return(strncmp(s1,s2,n));
}



bool wn_streq(s1,s2)

register char *s1,*s2;

{
  register char c1;

  c1 = *s1;

  while(c1 == *s2)
  {
    if(c1 == '\0')
    {
      return(TRUE);
    }

    ++s1; ++s2;

    c1 = *s1;
  }

  return(FALSE);
}



bool wn_streqnc(s1,s2)

register char s1[],s2[];

{
  register char c1,c2;

  while(TRUE)
  {
    c1 = *s1;
    c2 = *s2;
    
    if(islower(c1))      /* upcase c1 */
    {
      c1 = toupper(c1);
    }
    if(islower(c2))      /* upcase c2 */
    {
      c2 = toupper(c2);
    }

    if(c1 != c2)
    {
      return(FALSE);
    }
    
    if(c1 == '\0')
    {
      return(TRUE);
    }

    ++s1; ++s2;
  }
}



int wn_strlen(s)

char s[];

{
  return(strlen(s));
}



bool wn_char_in_string(c,s)

register char c,*s;

{
  register char sc;

  sc = *s;

  while(sc != '\0')
  {
    if(c == sc)
    {
      return(TRUE);
    }

    ++s;

    sc = *s;
  }

  return(FALSE);
}


