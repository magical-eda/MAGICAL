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
/**
 * @file lefiDebug.hpp
 */

#ifndef lefiDebug_h
#define lefiDebug_h 1

#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>

namespace LefParser {

/* Set flag */
EXTERN void lefiSetDebug PROTO_PARAMS((int num, int value)) ;

/* Read flag */
EXTERN int lefiDebug PROTO_PARAMS((int num)) ;

/* Error reporting routine */
EXTERN void lefiError PROTO_PARAMS (( const char* msg )) ;

/* for auto upshifting names in case insensitive files */
extern int lefNamesCaseSensitive;
extern int lefReaderCaseSensitive;
extern const char* lefUpperCase(const char* c);
extern int lefrShiftCase;
/*
#define CASE(x) (lefNamesCaseSensitive ? x : lefUpperCase(x)) 
*/
/*
#define CASE(x) (if !lefNamesCaseSensitive && lefrShiftCase \
                     lefUpperCase(x);   \
                 else x;)
*/
#define CASE(x) (!LefDataBase::lefNamesCaseSensitive && LefDataBase::lefrShiftCase ? lefUpperCase(x) : x)

} // namespace LefParser

#endif
