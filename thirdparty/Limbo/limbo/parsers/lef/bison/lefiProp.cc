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

#include <stdlib.h>
#include <string.h>
//#include "lex.h"
#include <limbo/parsers/lef/bison/lefiProp.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

lefiProp::lefiProp() {
  this->lefiProp::Init();
}


void lefiProp::Init() {
  this->stringLength_ = 16;
  this->stringData_ = (char*)lefMalloc(16);
  this->nameSize_ = 16;
  this->propName_ = (char*)lefMalloc(16);
  this->lefiProp::clear();
}


void lefiProp::Destroy() {
  lefFree(this->stringData_);
  lefFree(this->propName_);
}


lefiProp::~lefiProp() {
  this->lefiProp::Destroy();
}


void lefiProp::setPropType(const char* typ, const char* string) {
  int len;
  this->propType_ = (char*)typ;
  if ((len = strlen(string)+1) > this->nameSize_)
    this->lefiProp::bumpName(len);
  strcpy(this->propName_, CASE(string));
}


void lefiProp::setRange(double left, double right) {
  this->hasRange_ = 1;
  this->left_ = left;
  this->right_ = right;
}


void lefiProp::setNumber(double d) {
  this->hasNumber_ = 1;
  this->d_ = d;
}


void lefiProp::setPropInteger() {
  this->dataType_ = 'I';
}


void lefiProp::setPropReal() {
  this->dataType_ = 'R';
}


void lefiProp::setPropString() {
  this->dataType_ = 'S';
}


void lefiProp::setPropQString(const char* string) {
  int len;
  this->dataType_ = 'Q';
  if ((len = strlen(string)+1) > this->stringLength_)
    this->lefiProp::bumpSize(len);
  strcpy(this->stringData_, CASE(string));
}


void lefiProp::setPropNameMapString(const char* string) {
  int len;
  this->dataType_ = 'N';
  this->hasNameMapString_ = 1;
  if ((len = strlen(string)+1) > this->stringLength_)
    this->lefiProp::bumpSize(len);
  strcpy(this->stringData_, CASE(string));
}


const char* lefiProp::string() const {
  return this->stringData_;
}


const char* lefiProp::propType() const {
  return this->propType_;
}


int lefiProp::hasNumber() const {
  return (int)(this->hasNumber_);
}


int lefiProp::hasRange() const {
  return (int)(this->hasRange_);
}


double lefiProp::number() const {
  return this->d_;
}


double lefiProp::left() const {
  return this->left_;
}


double lefiProp::right() const {
  return this->right_;
}


void lefiProp::bumpSize(int size) {
  lefFree(this->stringData_);
  this->stringData_ = (char*)lefMalloc(size);
  this->stringLength_ = size;
  *(this->stringData_) = '\0';
}


void lefiProp::bumpName(int size) {
  lefFree(this->propName_);
  this->propName_ = (char*)lefMalloc(size);
  this->nameSize_ = size;
  *(this->propName_) = '\0';
}



void lefiProp::clear() {
  if (this->stringData_)
     *(this->stringData_) = '\0';
  if (this->stringData_)
     *(this->propName_) = '\0';
  this->propType_ = 0;
  this->hasRange_ = 0;
  this->hasNumber_ = 0;
  this->hasNameMapString_ = 0;
  this->dataType_ = 'B'; /* bogus */
  this->d_ = this->left_ = this->right_ = 0.0;
}


int lefiProp::hasString() const {
  return *(this->stringData_) ? 1 : 0 ;
}


const char* lefiProp::propName() const {
  return (this->propName_);
}


char lefiProp::dataType() const {
  return (this->dataType_);
}


void lefiProp::print(FILE* f) const {
  fprintf(f, "Prop type '%s'\n", this->lefiProp::propType());
  if (this->lefiProp::hasString()) {
    fprintf(f, "  string '%s'\n", this->lefiProp::string());
  }
  if (this->lefiProp::hasNumber()) {
    fprintf(f, "  number %5.2f\n", this->lefiProp::number());
  }
  if (this->lefiProp::hasRange()) {
    fprintf(f, "  range %5.2f - %5.2f\n",
      this->lefiProp::left(), this->lefiProp::right());
  }
}

} // namespace LefParser
