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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "lex.h"
#include <limbo/parsers/lef/bison/lefiViaRule.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

///////////////////////////////////////////
///////////////////////////////////////////
//
//    lefiViaRuleLayer
//
///////////////////////////////////////////
///////////////////////////////////////////

lefiViaRuleLayer::lefiViaRuleLayer() {
  this->lefiViaRuleLayer::Init();
}

void lefiViaRuleLayer::Init() {
  this->name_ = 0;
  this->overhang1_ = -1;
  this->overhang2_ = -1;
}

void lefiViaRuleLayer::clearLayerOverhang() {
  this->overhang1_ = -1;
  this->overhang2_ = -1;
}

void lefiViaRuleLayer::setName(const char* name) {
  int len = strlen(name) + 1;
  if (this->name_) lefFree(this->name_);
  this->name_ = (char*)lefMalloc(len);
  strcpy(this->name_, CASE(name));
  this->direction_ = '\0';
//  this->overhang1_ = -1;    already reset in clearLayerOverhang
//  this->overhang2_ = -1;
  this->hasWidth_ = 0;
  this->hasResistance_ = 0;
  this->hasOverhang_ = 0;
  this->hasMetalOverhang_ = 0;
  this->hasSpacing_ = 0;
  this->hasRect_ = 0;
}

void lefiViaRuleLayer::Destroy() {
  if (this->name_) lefFree(this->name_);
}

lefiViaRuleLayer::~lefiViaRuleLayer() {
  // Destroy will be called explicitly
  // so do nothing here.
}

void lefiViaRuleLayer::setHorizontal() {
  this->direction_ = 'H';
}

void lefiViaRuleLayer::setVertical() {
  this->direction_ = 'V';
}

// 5.5
void lefiViaRuleLayer::setEnclosure(double overhang1, double overhang2){
  this->overhang1_ = overhang1;
  this->overhang2_ = overhang2;
}

void lefiViaRuleLayer::setWidth(double minW, double maxW) {
  this->hasWidth_ = 1;
  this->widthMin_ = minW;
  this->widthMax_ = maxW;
}

void lefiViaRuleLayer::setOverhang(double d) {
  this->hasOverhang_ = 1;
  this->overhang_ = d;
}

// 5.6
void lefiViaRuleLayer::setOverhangToEnclosure(double d) {
  if ((this->overhang1_ != -1) && (this->overhang2_ != -1))
    return;                    // both overhang1_ & overhang2_ are set
  if (this->overhang1_ == -1)
    this->overhang1_ = d;      // set value to overhang1_
  else
    this->overhang2_ = d;      // overhang1_ already set, set to overhang2_
  return;
}

// 5.5
void lefiViaRuleLayer::setMetalOverhang(double d) {
  this->hasMetalOverhang_ = 1;
  this->metalOverhang_ = d;
}

void lefiViaRuleLayer::setResistance(double d) {
  this->hasResistance_ = 1;
  this->resistance_ = d;
}

void lefiViaRuleLayer::setSpacing(double x, double y) {
  this->hasSpacing_ = 1;
  this->spacingStepX_ = x;
  this->spacingStepY_ = y;
}

void lefiViaRuleLayer::setRect(double xl, double yl,
          double xh, double yh) {
  this->hasRect_ = 1;
  this->xl_ = xl;
  this->yl_ = yl;
  this->xh_ = xh;
  this->yh_ = yh;
}

int lefiViaRuleLayer::hasRect() const {
  return this->hasRect_;
}

int lefiViaRuleLayer::hasDirection() const {
  return this->direction_ ? 1 : 0 ;
}

// 5.5
int lefiViaRuleLayer::hasEnclosure() const {
  return this->overhang1_ == -1 ? 0 : 1;
}

int lefiViaRuleLayer::hasWidth() const {
  return this->hasWidth_;
}

int lefiViaRuleLayer::hasResistance() const {
  return this->hasResistance_;
}

int lefiViaRuleLayer::hasOverhang() const {
  return this->hasOverhang_;
}

int lefiViaRuleLayer::hasMetalOverhang() const {
  return this->hasMetalOverhang_;
}

int lefiViaRuleLayer::hasSpacing() const {
  return this->hasSpacing_;
}

char* lefiViaRuleLayer::name() const {
  return this->name_;
}

int lefiViaRuleLayer::isHorizontal() const {
  return this->direction_ == 'H' ? 1 : 0;
}

int lefiViaRuleLayer::isVertical() const {
  return this->direction_ == 'V' ? 1 : 0;
}

// 5.5
double lefiViaRuleLayer::enclosureOverhang1() const {
  return this->overhang1_;
}

// 5.5
double lefiViaRuleLayer::enclosureOverhang2() const {
  return this->overhang2_;
}

double lefiViaRuleLayer::widthMin() const {
  return this->widthMin_;
}

double lefiViaRuleLayer::widthMax() const {
  return this->widthMax_;
}

double lefiViaRuleLayer::overhang() const {
  return this->overhang_;
}

double lefiViaRuleLayer::metalOverhang() const {
  return this->metalOverhang_;
}

double lefiViaRuleLayer::resistance() const {
  return this->resistance_;
}

double lefiViaRuleLayer::spacingStepX() const {
  return this->spacingStepX_;
} 

double lefiViaRuleLayer::spacingStepY() const {
  return this->spacingStepY_;
}

double lefiViaRuleLayer::xl() const {
  return this->xl_;
}

double lefiViaRuleLayer::yl() const {
  return this->yl_;
}

double lefiViaRuleLayer::xh() const {
  return this->xh_;
}

double lefiViaRuleLayer::yh() const {
  return this->yh_;
}

void lefiViaRuleLayer::print(FILE* f) const {
  fprintf(f, "  Layer %s", this->name_);

  if (this->lefiViaRuleLayer::isHorizontal()) fprintf(f, " HORIZONTAL");
  if (this->lefiViaRuleLayer::isVertical()) fprintf(f, " VERTICAL");
  fprintf(f, "\n");

  if (this->lefiViaRuleLayer::hasWidth())
    fprintf(f, "    WIDTH %g %g\n", this->lefiViaRuleLayer::widthMin(),
      this->lefiViaRuleLayer::widthMax());

  if (this->lefiViaRuleLayer::hasResistance())
    fprintf(f, "    RESISTANCE %g\n", this->lefiViaRuleLayer::resistance());

  if (this->lefiViaRuleLayer::hasOverhang())
    fprintf(f, "    OVERHANG %g\n", this->lefiViaRuleLayer::overhang());

  if (this->lefiViaRuleLayer::hasMetalOverhang())
    fprintf(f, "    METALOVERHANG %g\n",
    this->lefiViaRuleLayer::metalOverhang());
 
  if (this->lefiViaRuleLayer::hasSpacing())
    fprintf(f, "    SPACING %g %g\n",
    this->lefiViaRuleLayer::spacingStepX(),
    this->lefiViaRuleLayer::spacingStepY());

  if (this->lefiViaRuleLayer::hasRect())
    fprintf(f, "    RECT %g,%g %g,%g\n",
    this->lefiViaRuleLayer::xl(), this->lefiViaRuleLayer::yl(),
    this->lefiViaRuleLayer::xh(), this->lefiViaRuleLayer::yh());
}

///////////////////////////////////////////
///////////////////////////////////////////
//
//    lefiViaRule
//
///////////////////////////////////////////
///////////////////////////////////////////

lefiViaRule::lefiViaRule() {
  this->lefiViaRule::Init();
}

void lefiViaRule::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);
  this->viasAllocated_ = 2;
  this->vias_ = (char**)lefMalloc(sizeof(char*)*2);
  this->layers_[0].lefiViaRuleLayer::Init();
  this->layers_[1].lefiViaRuleLayer::Init();
  this->layers_[2].lefiViaRuleLayer::Init();
  this->numLayers_ = 0;
  this->numVias_ = 0;
  this->numProps_ = 0;
  this->propsAllocated_ = 1;
  this->names_   = (char**)lefMalloc(sizeof(char*));
  this->values_  = (char**)lefMalloc(sizeof(char*));
  this->dvalues_ = (double*)lefMalloc(sizeof(double));
  this->types_   = (char*)lefMalloc(sizeof(char));
}

void lefiViaRule::clear() {
  int i;
  this->hasGenerate_ = 0;
  this->hasDefault_ = 0;
  for (i = 0; i < this->numProps_; i++) {
    lefFree(this->names_[i]);
    lefFree(this->values_[i]);
    this->dvalues_[i] = 0;
  }
  this->numProps_ = 0;
  this->numLayers_ = 0;
  for (i = 0; i < this->numVias_; i++) {
    lefFree(this->vias_[i]);
  }
  this->numVias_ = 0;
}

void lefiViaRule::clearLayerOverhang() {
  this->layers_[0].lefiViaRuleLayer::clearLayerOverhang();
  this->layers_[1].lefiViaRuleLayer::clearLayerOverhang();
}

void lefiViaRule::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
  this->lefiViaRule::clear();
}

void lefiViaRule::Destroy() {
  this->lefiViaRule::clear();
  lefFree(this->name_);
  lefFree((char*)(this->vias_));
  lefFree((char*)(this->names_));
  lefFree((char*)(this->values_));
  lefFree((char*)(this->dvalues_));
  lefFree((char*)(this->types_));
  this->layers_[0].lefiViaRuleLayer::Destroy();
  this->layers_[1].lefiViaRuleLayer::Destroy();
  this->layers_[2].lefiViaRuleLayer::Destroy();
}

lefiViaRule::~lefiViaRule() {
  this->lefiViaRule::Destroy();
}

void lefiViaRule::setGenerate() {
  this->hasGenerate_ = 1;
}

void lefiViaRule::setDefault() {
  this->hasDefault_ = 1;
}

void lefiViaRule::addViaName(const char* name) {
  // Add one of possibly many via names
  int len = strlen(name) + 1;
  if (this->numVias_ == this->viasAllocated_) {
    int i;
    char** nn;
    if (this->viasAllocated_ == 0)
       this->viasAllocated_ = 2;
    else
       this->viasAllocated_ *= 2;
    nn = (char**)lefMalloc(sizeof(char*) * this->viasAllocated_);
    for (i = 0; i < this->numVias_; i++)
      nn[i] = this->vias_[i];
    lefFree((char*)(this->vias_));
    this->vias_ = nn;
  }
  this->vias_[this->numVias_] = (char*)lefMalloc(len);
  strcpy(this->vias_[this->numVias_], CASE(name));
  this->numVias_ += 1;
}

void lefiViaRule::setRect(double xl, double yl, double xh, double yh) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setRect(xl, yl, xh, yh);
}

void lefiViaRule::setSpacing(double x, double y) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setSpacing(x, y);
}

void lefiViaRule::setWidth(double x, double y) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setWidth(x, y);
}

void lefiViaRule::setResistance(double d) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setResistance(d);
}

void lefiViaRule::setOverhang(double d) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setOverhang(d);
}

// 5.6, try to set value to layers_[0] & layers_[1]
void lefiViaRule::setOverhangToEnclosure(double d) {
  this->layers_[0].lefiViaRuleLayer::setOverhangToEnclosure(d);
  this->layers_[1].lefiViaRuleLayer::setOverhangToEnclosure(d);
}

void lefiViaRule::setMetalOverhang(double d) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setMetalOverhang(d);
}

void lefiViaRule::setVertical() {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setVertical();
}

void lefiViaRule::setHorizontal() {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setHorizontal();
}

void lefiViaRule::setEnclosure(double overhang1, double overhang2) {
  this->layers_[this->numLayers_-1].lefiViaRuleLayer::setEnclosure(overhang1,
          overhang2);
}

void lefiViaRule::setLayer(const char* name) {
  if (this->numLayers_ == 3)
    { lefiError("ERROR (LEFPARS-1430): too many via rule layers"); return; }
  // This routine sets and creates the active layer.
  this->layers_[this->numLayers_].lefiViaRuleLayer::setName(name);
  this->numLayers_ += 1;
}

int lefiViaRule::hasGenerate() const {
  return this->hasGenerate_;
} 

int lefiViaRule::hasDefault() const {
  return this->hasDefault_;
} 

int lefiViaRule::numLayers() const {
  // There are 2 or 3 layers in a rule.
  // numLayers() tells how many.
  // If a third layer exists then it is the cut layer.
  return this->numLayers_;
}

lefiViaRuleLayer* lefiViaRule::layer(int index) {
  if (index < 0 || index > 2) return 0;
  return &(this->layers_[index]);
}

char* lefiViaRule::name() const {
  return this->name_;
}

void lefiViaRule::print(FILE* f) const {
  int i;
  fprintf(f, "VIA RULE %s", this->lefiViaRule::name());
  if (this->lefiViaRule::hasGenerate())
    fprintf(f, " GENERATE");
  fprintf(f, "\n");

  for (i = 0; i < this->lefiViaRule::numLayers(); i++) {
    this->layers_[i].lefiViaRuleLayer::print(f);
  }

  for (i = 0; i < this->lefiViaRule::numVias(); i++) {
    fprintf(f, "  Via %s\n", this->lefiViaRule::viaName(i));
  }
}

int lefiViaRule::numVias() const {
  return this->numVias_;
}

char* lefiViaRule::viaName(int index) const {
  if (index < 0 || index >= this->numVias_) return 0;
  return this->vias_[index];
}

int lefiViaRule::numProps() const {
  return this->numProps_;
}

void lefiViaRule::addProp(const char* name, const char* value,
                          const char type) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim;
    char**  nn;
    char**  nv;
    double* nd;
    char*  nt;

    if (this->propsAllocated_ == 0)
       this->propsAllocated_ = 1;    // initialize propsAllocated_
    max = this->propsAllocated_ *= 2;
    lim = this->numProps_;
    nn = (char**)lefMalloc(sizeof(char*)*max);
    nv = (char**)lefMalloc(sizeof(char*)*max);
    nd = (double*)lefMalloc(sizeof(double)*max);
    nt = (char*)lefMalloc(sizeof(char)*max);
    for (i = 0; i < lim; i++) {
      nn[i] = this->names_[i];
      nv[i] = this->values_[i];
      nd[i] = this->dvalues_[i];
      nt[i] = this->types_[i];
    }
    lefFree((char*)(this->names_));
    lefFree((char*)(this->values_));
    lefFree((char*)(this->dvalues_));
    lefFree((char*)(this->types_));
    this->names_ = nn;
    this->values_ = nv;
    this->dvalues_ = nd;
    this->types_ = nt;
  }
  this->names_[this->numProps_] = (char*)lefMalloc(sizeof(char)*len);
  strcpy(this->names_[this->numProps_],name);
  len = strlen(value) + 1;
  this->values_[this->numProps_] = (char*)lefMalloc(sizeof(char)*len);
  strcpy(this->values_[this->numProps_],value);
  this->dvalues_[this->numProps_] = 0;
  this->types_[this->numProps_] = type;
  this->numProps_ += 1;
}

void lefiViaRule::addNumProp(const char* name, const double d,
                             const char* value, const char type) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim;
    char**  nn;
    char**  nv;
    double* nd;
    char*  nt;

    if (this->propsAllocated_ == 0)
       this->propsAllocated_ = 1;    // initialize propsAllocated_
    max = this->propsAllocated_ *= 2;
    lim = this->numProps_;
    nn = (char**)lefMalloc(sizeof(char*)*max);
    nv = (char**)lefMalloc(sizeof(char*)*max);
    nd = (double*)lefMalloc(sizeof(double)*max);
    nt = (char*)lefMalloc(sizeof(char)*max);
    for (i = 0; i < lim; i++) {
      nn[i] = this->names_[i];
      nv[i] = this->values_[i];
      nd[i] = this->dvalues_[i];
      nt[i] = this->types_[i];
    }
    lefFree((char*)(this->names_));
    lefFree((char*)(this->values_));
    lefFree((char*)(this->dvalues_));
    lefFree((char*)(this->types_));
    this->names_ = nn;
    this->values_ = nv;
    this->dvalues_ = nd;
    this->types_ = nt;
  }
  this->names_[this->numProps_] = (char*)lefMalloc(sizeof(char)*len);
  strcpy(this->names_[this->numProps_],name);
  len = strlen(value) + 1;
  this->values_[this->numProps_] = (char*)lefMalloc(sizeof(char)*len);
  strcpy(this->values_[this->numProps_],value);
  this->dvalues_[this->numProps_] = d;
  this->types_[this->numProps_] = type;
  this->numProps_ += 1;
}

const char* lefiViaRule::propName(int i) const {
  char msg[160];
  if (i < 0 || i >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1430): The index number %d given for the VIARULE PROPERTY is invalid.\nValid index is from 0 to %d", i, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->names_[i];
}

const char* lefiViaRule::propValue(int i) const {
  char msg[160];
  if (i < 0 || i >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1430): The index number %d given for the VIARULE PROPERTY is invalid.\nValid index is from 0 to %d", i, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->values_[i];
}

double lefiViaRule::propNumber(int i) const {
  char msg[160];
  if (i < 0 || i >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1430): The index number %d given for the VIARULE PROPERTY is invalid.\nValid index is from 0 to %d", i, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->dvalues_[i];
}

char lefiViaRule::propType(int i) const {
  char msg[160];
  if (i < 0 || i >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1430): The index number %d given for the VIARULE PROPERTY is invalid.\nValid index is from 0 to %d", i, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->types_[i];
}

int lefiViaRule::propIsNumber(int i) const {
  char msg[160];
  if (i < 0 || i >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1430): The index number %d given for the VIARULE PROPERTY is invalid.\nValid index is from 0 to %d", i, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->dvalues_[i] ? 1 : 0;
}

int lefiViaRule::propIsString(int i) const {
  char msg[160];
  if (i < 0 || i >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1430): The index number %d given for the VIARULE PROPERTY is invalid.\nValid index is from 0 to %d", i, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->dvalues_[i] ? 0 : 1;
}

} // namespace LefParser
