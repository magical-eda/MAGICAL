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
 * @file lefiProp.hpp
 */

#ifndef lefiProp_h
#define lefiProp_h

//#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>
#include <stdio.h>

namespace LefParser {

// Struct holds the data for one property.
class lefiProp {
public:
  lefiProp();
  void Init();

  void Destroy();
  ~lefiProp();

  void setPropType(const char* typ, const char* string);
  void setRange(double left, double right);
  void setNumber(double num);
  void setPropInteger();
  void setPropReal();
  void setPropString();
  void setPropQString(const char* string);
  void setPropNameMapString(const char* string);
  void clear();

  const char* string() const;
  const char* propType() const;
  const char* propName() const;
  char  dataType() const;
      // either I:integer R:real S:string Q:quotedstring
      // N:property name is not defined in the property definition section
  int hasNumber() const;
  int hasRange() const;
  int hasString() const;
  int hasNameMapString() const;
  double number() const;
  double left() const;
  double right() const;

  void bumpSize(int size);
  void bumpName(int size);

  void print(FILE* f) const;

protected:
  char* propType_;      // "design" "net" "macro" ...
  char* propName_;      // name.
  int nameSize_;        // allocated size of name.
  char hasRange_;       // either 0:NO or 1:YES.
  char hasNumber_;      // either 0:NO or 1:YES.
  char hasNameMapString_;
  char dataType_;       // either I:integer R:real S:string Q:quotedstring.
			// N:property name is not defined.
  char* stringData_;    // if it is a string the data is here.
  int stringLength_;    // allocated size of stringData.
  double left_, right_; // if it has a range the numbers are here.
  double d_;            // if it is a real or int the number is here.
};

} // namespace LefParser

#endif
