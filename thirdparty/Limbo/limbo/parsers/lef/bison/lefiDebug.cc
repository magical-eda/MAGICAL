/*
 *     This  file  is  part  of  the  Cadence  LEF/DEF  Open   Source
 *  Distribution,  Product Version 5.7, and is subject to the Cadence
 *  LEF/DEF Open Source License Agreement.   Your  continued  use  of
 *  this file constitutes your acceptance of the terms of the LEF/DEF
 *  Open Source License and an agreement to abide by its  terms.   If
 *  you  don't  agree  with  this, you must remove this and any other
 *  files which are part of the distribution and  destroy any  copies
 *  made.
 *
 *     For updates, support, or to become part of the LEF/DEF Community,
 *  check www.openeda.org for details.
 */

#include <limbo/parsers/lef/bison/lefiDebug.hpp>
//#include <limbo/parsers/lef/bison/lefrReader.hpp>
//#include "lex.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference


namespace LefParser {

char lefDebug[100];

/*******************
 *  Debug flags:
 *  0 -
 *  1 - malloc debug
 *  2 - print each history size bump up.
 *  3 -
 *  4 -
 *  5 -
 *  6 -
 *  7 -
 *  8 -
 *  9 -
 * 10 -
 * 11 - lexer debug
 * 12 -
 * 13 - print each lex token as read in.
 *
 ******************************/



/* Set flag */
void lefiSetDebug(int num, int value) {
  lefDebug[num] = value;
}



/* Read flag */
int lefiDebug(int num) {
  return lefDebug[num];
}

//extern LEFI_LOG_FUNCTION lefiErrorLogFunction;

void lefiError(const char* str) {
  /*if (lefiErrorLogFunction)
    (*lefiErrorLogFunction)(str);
  else*/
    fprintf(stderr, "%s", str);
}


static char lefiShift [] = {
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  ' ',  '!',  '"',  '#',  '$',  '%',  '&', '\'', 
  '(',  ')',  '*',  '+',  ',',  '-',  '.',  '/', 
  '0',  '1',  '2',  '3',  '4',  '5',  '6',  '7', 
  '8',  '9',  ':',  ';',  '<',  '=',  '>',  '?', 
  '@',  'A',  'B',  'C',  'D',  'E',  'F',  'G', 
  'H',  'I',  'J',  'K',  'L',  'M',  'N',  'O', 
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W', 
  'X',  'Y',  'Z',  '[', '\\',  ']',  '^',  '_', 
  '`',  'A',  'B',  'C',  'D',  'E',  'F',  'G', 
  'H',  'I',  'J',  'K',  'l',  'M',  'N',  'O', 
  'P',  'Q',  'R',  'S',  'T',  'U',  'V',  'W', 
  'X',  'Y',  'Z',  '{',  '|',  '}',  '~', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
 '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};


const char* lefUpperCase(const char* str) {
  static char* shiftBuf = 0;
  static int shiftBufLength = 0;
  char* place = (char*)str;
  char* to;
  int len = strlen(str) + 1;

  if (len > shiftBufLength) {
    if (shiftBuf == 0) {
      len = len < 64 ? 64 : len;
      shiftBuf = (char*)lefMalloc(len);
      shiftBufLength = len;
    } else {
      lefFree(shiftBuf);
      shiftBuf = (char*)malloc(len);
      shiftBufLength = len;
    }
  }

  to = shiftBuf;
  while (*place) {
    int i = (int)*place;
      place++;
    *to++ = lefiShift[i];
  }
  *to = '\0';

  return shiftBuf;
}


} // namespace LefParser
