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
 * @file lefiPropType.hpp
 */

#ifndef lefiPropType_h
#define lefiPropType_h

//#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>
#include <stdio.h>

namespace LefParser {

// Struct holds the data type for one property, if the property is
// either REAL or INTEGER.
class lefiPropType {
public:
  lefiPropType();
  void Init();

  void Destroy();
  ~lefiPropType();

  void setPropType(const char* name, const char type);
  void Clear();

  char propType(const char* name) const;
  void bumpProps();

protected:
  int    numProperties_;
  int    propertiesAllocated_;
  char** propNames_;      // name.
  char*  propTypes_;      // 'R' == "REAL", 'I' == "INTEGER"
};

}

#endif
