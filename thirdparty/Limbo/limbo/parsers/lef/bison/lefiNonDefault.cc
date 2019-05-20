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
#include <limbo/parsers/lef/bison/lefiNonDefault.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
//#include <limbo/parsers/lef/bison/lefrCallBacks.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

// 6/16/2000 - Wanda da Rosa
// Make these variables in globals.  Can't use those defined
// in the class because it generates warning when it casts.
// Can't assign for example lefrViaCbkFnType to oldViaCbk_
// in the class because it requires to include lefrReader.hpp
// in lefiNonDefault.hpp.  But in lefrReader.hpp, it requires
// include lefiNonDefault.hpp, this creates a loop and is
// problematic...

#if 0
	// resolve this in Driver 
static lefrViaCbkFnType oldViaCbk;
static lefrSpacingCbkFnType oldSpacingCbk;
static lefrVoidCbkFnType oldSpacingBeginCbk;
static lefrVoidCbkFnType oldSpacingEndCbk;

static lefiNonDefault* nd = 0;  // PCR 909010 - For VIA in the nondefaultrule
#endif 

/////////////////////////////////////////
/////////////////////////////////////////
//
//     lefiNonDefault
//
/////////////////////////////////////////
/////////////////////////////////////////

lefiNonDefault::lefiNonDefault() { 
  this->lefiNonDefault::Init();
}

void lefiNonDefault::Init() { 
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);

  this->layersAllocated_ = 2;
  this->numLayers_ = 0;
  this->layerName_ = (char**)lefMalloc(sizeof(char*)*2);
  this->width_ = (double*)lefMalloc(sizeof(double)*2);
  this->diagWidth_ = (double*)lefMalloc(sizeof(double)*2);
  this->spacing_ = (double*)lefMalloc(sizeof(double)*2);
  this->wireExtension_ = (double*)lefMalloc(sizeof(double)*2);
  this->resistance_ = (double*)lefMalloc(sizeof(double)*2);
  this->capacitance_ = (double*)lefMalloc(sizeof(double)*2);
  this->edgeCap_ = (double*)lefMalloc(sizeof(double)*2);
  this->hasWidth_ = (char*)lefMalloc(sizeof(char)*2);
  this->hasDiagWidth_ = (char*)lefMalloc(sizeof(char)*2);
  this->hasSpacing_ = (char*)lefMalloc(sizeof(char)*2);
  this->hasWireExtension_ = (char*)lefMalloc(sizeof(char)*2);
  this->hasResistance_ = (char*)lefMalloc(sizeof(char)*2);
  this->hasCapacitance_ = (char*)lefMalloc(sizeof(char)*2);
  this->hasEdgeCap_ = (char*)lefMalloc(sizeof(char)*2);

  this->allocatedVias_ = 2;
  this->numVias_ = 0;
  this->viaRules_ = (lefiVia**)lefMalloc(sizeof(lefiVia*)*2);

  this->allocatedSpacing_ = 2;
  this->numSpacing_ = 0;
  this->spacingRules_ = (lefiSpacing**)lefMalloc(
          sizeof(lefiSpacing*)*2);

  this->numProps_ = 0;
  this->propsAllocated_ = 1;
  this->names_ = (char**)lefMalloc(sizeof(char*));
  this->values_ = (char**)lefMalloc(sizeof(char*));
  this->dvalues_ = (double*)lefMalloc(sizeof(double));
  this->types_ = (char*)lefMalloc(sizeof(char));

  this->hardSpacing_ = 0;
  this->numUseVias_ = 0;             // Won't be allocated until they are used
  this->allocatedUseVias_ = 0;
  this->numUseViaRules_ = 0;
  this->allocatedUseViaRules_ = 0;
  this->numMinCuts_ = 0;
  this->allocatedMinCuts_ = 0;
}

void lefiNonDefault::Destroy() { 
  this->lefiNonDefault::clear();

  lefFree(this->name_);

  lefFree((char*)(this->layerName_));
  lefFree((char*)(this->width_));
  lefFree((char*)(this->diagWidth_));
  lefFree((char*)(this->spacing_));
  lefFree((char*)(this->wireExtension_));
  lefFree((char*)(this->resistance_));
  lefFree((char*)(this->capacitance_));
  lefFree((char*)(this->edgeCap_));
  lefFree(this->hasWidth_);
  lefFree(this->hasDiagWidth_);
  lefFree(this->hasSpacing_);
  lefFree(this->hasWireExtension_);
  lefFree(this->hasResistance_);
  lefFree(this->hasCapacitance_);
  lefFree(this->hasEdgeCap_);

  lefFree((char*)(this->viaRules_));

  lefFree((char*)(this->spacingRules_));
  lefFree((char*)(this->names_));
  lefFree((char*)(this->values_));
  lefFree((char*)(this->dvalues_));
  lefFree((char*)(this->types_));
  if (this->allocatedUseVias_)
    lefFree((char*)(this->useViaName_));
  if (this->allocatedUseViaRules_)
    lefFree((char*)(this->useViaRuleName_));
  if (this->allocatedMinCuts_) {
    lefFree((char*)(this->cutLayerName_));
    lefFree((char*)(this->numCuts_));
  }
  this->allocatedUseVias_ = 0;
  this->allocatedUseViaRules_ = 0;
  this->allocatedMinCuts_ = 0;
}

lefiNonDefault::~lefiNonDefault() { 
  this->lefiNonDefault::Destroy();
}

void lefiNonDefault::clear() { 
  int i;
  lefiSpacing* sr;
  lefiVia* vr;

  for (i = 0; i < this->numProps_; i++) {
    lefFree(this->names_[i]);
    lefFree(this->values_[i]);
    this->dvalues_[i] = 0;
  }
  this->numProps_ = 0;
  for (i = 0; i < this->numLayers_; i++) {
    lefFree(this->layerName_[i]);
    this->layerName_[i] = 0;
  }
  this->numLayers_ = 0;
  for (i = 0; i < this->numVias_; i++) {
    vr = this->viaRules_[i];
    vr->lefiVia::Destroy();
    lefFree((char*)(this->viaRules_[i]));
    this->viaRules_[i] = 0;
  }
  this->numVias_ = 0;
  for (i = 0; i < this->numSpacing_; i++) {
    sr = this->spacingRules_[i];
    sr->lefiSpacing::Destroy();
    lefFree((char*)(this->spacingRules_[i]));
    this->spacingRules_[i] = 0;
  }
  this->numSpacing_ = 0;

  this->hardSpacing_ = 0;
  for (i = 0; i < this->numUseVias_; i++) {
    lefFree((char*)(this->useViaName_[i]));
  }
  this->numUseVias_ = 0;
  for (i = 0; i < this->numUseViaRules_; i++) {
    lefFree((char*)(this->useViaRuleName_[i]));
  }
  this->numUseViaRules_ = 0;
  for (i = 0; i < this->numMinCuts_; i++) {
    lefFree((char*)(this->cutLayerName_[i]));
  }
  this->numMinCuts_ = 0;
}

void lefiNonDefault::addViaRule(lefiVia const& v) {
  if (this->numVias_ == this->allocatedVias_) {
    int i;
    lefiVia** nv;

    if (this->allocatedVias_ == 0)
      this->allocatedVias_ = 2;
    else
      this->allocatedVias_ *= 2;
    nv = (lefiVia**)lefMalloc(sizeof(lefiVia*)* this->allocatedVias_);
    for (i = 0; i < this->numVias_; i++) {
      nv[i] = this->viaRules_[i];
    }
    lefFree((char*)(this->viaRules_));
    this->viaRules_ = nv;
    }
  this->viaRules_[this->numVias_++] = v.lefiVia::clone();
}

void lefiNonDefault::addSpacingRule(lefiSpacing const& s) {
if (this->numSpacing_ == this->allocatedSpacing_) {
  int i;
  lefiSpacing** ns;

  if (this->allocatedSpacing_ == 0)
    this->allocatedSpacing_ = 2;
  else
    this->allocatedSpacing_ *= 2;
  ns = (lefiSpacing**)lefMalloc(sizeof(lefiSpacing*)*
                   this->allocatedSpacing_);
    for (i = 0; i < this->numSpacing_; i++) {
      ns[i] = this->spacingRules_[i];
    }
    lefFree((char*)(this->spacingRules_));
    this->spacingRules_ = ns;
  }
  this->spacingRules_[this->numSpacing_++] = s.lefiSpacing::clone();
}

#if 0
int lefiNonDefaultViaCbk(lefrCallbackType_e t, lefiVia* v,
       lefiUserData ud) {
   // PCR 909010 - Not needed, nd info is saved in global variable 
   // lefiNonDefault* nd = (lefiNonDefault*)ud;

   if (t != lefrViaCbkType)
     lefiError("ERROR (LEFPARS-1400): Bad nondefaultvia callback");

   // We got a via in a nondefault rule so add it to
   // our internal list.
   nd->lefiNonDefault::addViaRule(v);
   return 0;
}

int lefiNonDefaultSpacingCbk(lefrCallbackType_e t,
       lefiSpacing* s, lefiUserData ud) {
   // PCR 909010 - Not needed, nd info is saved in global variable 
   // lefiNonDefault* nd = (lefiNonDefault*)ud;

   if (t != lefrSpacingCbkType)
     lefiError("ERROR (LEFPARS-1401): Bad nondefaultspacing callback");

   // We got a spacing in a nondefault rule so add it to
   // our internal list.
   nd->lefiNonDefault::addSpacingRule(s);
   return 0;
}
#endif 

void lefiNonDefault::setName(const char* name) { 
  int len = strlen(name) + 1;
  this->lefiNonDefault::clear();

#if 0
  // Use our callback functions because a via and spacing
  // rule is really part of the non default section.
  // this->oldViaCbk_ = (void*)lefrViaCbk;
  // this->oldSpacingCbk_ = (void*)lefrSpacingCbk;
  // this->oldSpacingBeginCbk_ = (void*)lefrSpacingBeginCbk;
  // this->oldSpacingEndCbk_ = (void*)lefrSpacingEndCbk;
  this->oldUserData_ = lefrGetUserData();
  oldViaCbk = lefrViaCbk;
  oldSpacingCbk = lefrSpacingCbk;
  oldSpacingBeginCbk = lefrSpacingBeginCbk;
  oldSpacingEndCbk = lefrSpacingEndCbk;
  lefrViaCbk = lefiNonDefaultViaCbk;
  lefrSpacingCbk = lefiNonDefaultSpacingCbk;
  lefrSpacingBeginCbk = 0;
  lefrSpacingEndCbk = 0;
  // pcr 909010, instead of saving the pointer in userData,
  // save it in the global nd.
  // lefrSetUserData((lefiUserData)this);

  nd = this;
#endif 

  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}

void lefiNonDefault::addLayer(const char* name) { 
  int len = strlen(name) + 1;
  if (this->numLayers_ == this->layersAllocated_) {
    int i;
    char** newl;
    double* neww;
    double* newd;
    double* news;
    double* newe;
    double* newc;
    double* newr;
    double* newec;
    char* newhw;
    char* newhd;
    char* newhs;
    char* newhe;
    char* newhc;
    char* newhr;
    char* newhec;

    if (this->layersAllocated_ == 0)
      this->layersAllocated_ = 2;
    else
      this->layersAllocated_ *= 2;
    newl = (char**)lefMalloc(sizeof(char*)*
		  this->layersAllocated_);
    newe = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    neww = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    newd = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    news = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    newc = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    newr = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    newec = (double*)lefMalloc(sizeof(double)*
		  this->layersAllocated_);
    newhe = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    newhw = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    newhd = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    newhs = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    newhc = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    newhr = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    newhec = (char*)lefMalloc(sizeof(char)*
		  this->layersAllocated_);
    for (i = 0; i < this->numLayers_; i++) {
      newl[i]  = this->layerName_[i];
      neww[i]  = this->width_[i];
      newd[i]  = this->diagWidth_[i];
      news[i]  = this->spacing_[i];
      newe[i]  = this->wireExtension_[i];
      newc[i]  = this->capacitance_[i];
      newr[i]  = this->resistance_[i];
      newec[i]  = this->edgeCap_[i];
      newhe[i]  = this->hasWireExtension_[i];
      newhw[i]  = this->hasWidth_[i];
      newhd[i]  = this->hasDiagWidth_[i];
      newhs[i]  = this->hasSpacing_[i];
      newhc[i]  = this->hasCapacitance_[i];
      newhr[i]  = this->hasResistance_[i];
      newhec[i] = this->hasEdgeCap_[i];
    }
    lefFree((char*)(this->layerName_));
    lefFree((char*)(this->width_));
    lefFree((char*)(this->diagWidth_));
    lefFree((char*)(this->spacing_));
    lefFree((char*)(this->wireExtension_));
    lefFree((char*)(this->capacitance_));
    lefFree((char*)(this->resistance_));
    lefFree((char*)(this->edgeCap_));
    lefFree((char*)(this->hasWireExtension_));
    lefFree((char*)(this->hasWidth_));
    lefFree((char*)(this->hasDiagWidth_));
    lefFree((char*)(this->hasSpacing_));
    lefFree((char*)(this->hasCapacitance_));
    lefFree((char*)(this->hasResistance_));
    lefFree((char*)(this->hasEdgeCap_));
    this->layerName_ = newl;
    this->width_ = neww;
    this->diagWidth_ = newd;
    this->spacing_ = news;
    this->wireExtension_ = newe;
    this->capacitance_ = newc;
    this->resistance_ = newr;
    this->edgeCap_ = newec;
    this->hasWidth_ = newhw;
    this->hasDiagWidth_ = newhd;
    this->hasSpacing_ = newhs;
    this->hasWireExtension_ = newhe;
    this->hasCapacitance_ = newhc;
    this->hasResistance_ = newhr;
    this->hasEdgeCap_ = newhec;
  }
  this->layerName_[this->numLayers_] = (char*)lefMalloc(len);
  strcpy(this->layerName_[this->numLayers_], CASE(name));
  this->width_[this->numLayers_] = 0.0;
  this->diagWidth_[this->numLayers_] = 0.0;
  this->spacing_[this->numLayers_] = 0.0;
  this->wireExtension_[this->numLayers_] = 0.0;
  this->capacitance_[this->numLayers_] = 0.0;
  this->resistance_[this->numLayers_] = 0.0;
  this->edgeCap_[this->numLayers_] = 0.0;
  this->hasWidth_[this->numLayers_] = '\0';
  this->hasDiagWidth_[this->numLayers_] = '\0';
  this->hasSpacing_[this->numLayers_] = '\0';
  this->hasWireExtension_[this->numLayers_] = '\0';
  this->hasCapacitance_[this->numLayers_] = '\0';
  this->hasResistance_[this->numLayers_] = '\0';
  this->hasEdgeCap_[this->numLayers_] = '\0';
  this->numLayers_ += 1;
}

void lefiNonDefault::addWidth(double num) { 
  this->width_[this->numLayers_-1] = num;
  this->hasWidth_[this->numLayers_-1] = 1;
}

void lefiNonDefault::addDiagWidth(double num) { 
  this->diagWidth_[this->numLayers_-1] = num;
  this->hasDiagWidth_[this->numLayers_-1] = 1;
}

void lefiNonDefault::addSpacing(double num) { 
  this->spacing_[this->numLayers_-1] = num;
  this->hasSpacing_[this->numLayers_-1] = 1;
}

void lefiNonDefault::addWireExtension(double num) { 
  this->wireExtension_[this->numLayers_-1] = num;
  this->hasWireExtension_[this->numLayers_-1] = 1;
}

void lefiNonDefault::addCapacitance(double num) { 
  this->capacitance_[this->numLayers_-1] = num;
  this->hasCapacitance_[this->numLayers_-1] = 1;
}

void lefiNonDefault::addResistance(double num) { 
  this->resistance_[this->numLayers_-1] = num;
  this->hasResistance_[this->numLayers_-1] = 1;
}

void lefiNonDefault::addEdgeCap(double num) { 
  this->edgeCap_[this->numLayers_-1] = num;
  this->hasEdgeCap_[this->numLayers_-1] = 1;
}

void lefiNonDefault::setHardspacing() {
  this->hardSpacing_ = 1;
}

void lefiNonDefault::addUseVia(const char* name) {
  if (this->numUseVias_ == this->allocatedUseVias_) {
    int i;
    char** vn;

    if (this->allocatedUseVias_ == 0)
      this->allocatedUseVias_ = 2;
    else
      this->allocatedUseVias_ *= 2;
    vn = (char**)lefMalloc(sizeof(char*)* this->allocatedUseVias_);
    for (i = 0; i < this->numUseVias_; i++) {
      vn[i] = this->useViaName_[i];
    }
    if (this->numUseVias_)
      lefFree((char*)(this->useViaName_));
    this->useViaName_ = vn;
  } 
  this->useViaName_[this->numUseVias_] = (char*)lefMalloc(strlen(name)+1);
  strcpy(this->useViaName_[this->numUseVias_], CASE(name));
  this->numUseVias_ += 1;
}

void lefiNonDefault::addUseViaRule(const char* name) {
  if (this->numUseViaRules_ == this->allocatedUseViaRules_) {
    int i;
    char** vn;

    if (this->allocatedUseViaRules_ == 0)
      this->allocatedUseViaRules_ = 2;
    else
      this->allocatedUseViaRules_ *= 2;
    vn = (char**)lefMalloc(sizeof(char*)* this->allocatedUseViaRules_);
    for (i = 0; i < this->numUseViaRules_; i++) {
      vn[i] = this->useViaRuleName_[i];
    }
    if (this->numUseViaRules_)
      lefFree((char*)(this->useViaRuleName_));
    this->useViaRuleName_ = vn;
  } 
  this->useViaRuleName_[this->numUseViaRules_]=(char*)lefMalloc(strlen(name)+1);
  strcpy(this->useViaRuleName_[this->numUseViaRules_], CASE(name));
  this->numUseViaRules_ += 1;
}

void lefiNonDefault::addMinCuts(const char* name, int numCuts) {
  if (this->numMinCuts_ == this->allocatedMinCuts_) {
    int i;
    char** cn;
    int*   nc;

    if (this->allocatedMinCuts_ == 0)
      this->allocatedMinCuts_ = 2;
    else
      this->allocatedMinCuts_ *= 2;
    cn = (char**)lefMalloc(sizeof(char*)* this->allocatedMinCuts_);
    nc = (int*)lefMalloc(sizeof(int)* this->allocatedMinCuts_);
    for (i = 0; i < this->numMinCuts_; i++) {
      cn[i] = this->cutLayerName_[i];
      nc[i] = this->numCuts_[i];
    }
    if (this->numMinCuts_) {
      lefFree((char*)(this->cutLayerName_));
      lefFree((char*)(this->numCuts_));
    }
    this->cutLayerName_ = cn;
    this->numCuts_ = nc;
  }
  this->cutLayerName_[this->numMinCuts_] = (char*)lefMalloc(strlen(name)+1);
  strcpy(this->cutLayerName_[this->numMinCuts_], CASE(name));
  this->numCuts_[this->numMinCuts_] = numCuts;
  this->numMinCuts_ += 1;
}

void lefiNonDefault::end() { 
#if 0
  // Return the callbacks to their normal state.
  // lefrSetViaCbk((lefrViaCbkFnType)this->oldViaCbk_);
  // lefrSetSpacingCbk((lefrSpacingCbkFnType)this->oldSpacingCbk_);
  // lefrSetSpacingBeginCbk((lefrVoidCbkFnType)this->oldSpacingBeginCbk_);
  // lefrSetSpacingEndCbk((lefrVoidCbkFnType)this->oldSpacingEndCbk_);
  lefrSetViaCbk(oldViaCbk);
  lefrSetSpacingCbk(oldSpacingCbk);
  lefrSetSpacingBeginCbk(oldSpacingBeginCbk);
  lefrSetSpacingEndCbk(oldSpacingEndCbk);
  // pcr 909010 - global var nd is used to pass nondefault rule data
  // lefrSetUserData(this->oldUserData_);
  nd = 0;
#endif 
}

int lefiNonDefault::numLayers() const { 
  return this->numLayers_;
}

const char* lefiNonDefault::layerName(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->layerName_[index];
}

int lefiNonDefault::hasLayerWidth(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasWidth_[index];
}

double lefiNonDefault::layerWidth(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->width_[index];
}

int lefiNonDefault::hasLayerDiagWidth(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasDiagWidth_[index];
}

double lefiNonDefault::layerDiagWidth(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->diagWidth_[index];
}

int lefiNonDefault::hasLayerWireExtension(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasWireExtension_[index];
}

int lefiNonDefault::hasLayerSpacing(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasSpacing_[index];
}

double lefiNonDefault::layerWireExtension(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->wireExtension_[index];
}

double lefiNonDefault::layerSpacing(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->spacing_[index];
}

int lefiNonDefault::hasLayerResistance(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasResistance_[index];
}

double lefiNonDefault::layerResistance(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->resistance_[index];
}

int lefiNonDefault::hasLayerCapacitance(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasCapacitance_[index];
}

double lefiNonDefault::layerCapacitance(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->capacitance_[index];
}

int lefiNonDefault::hasLayerEdgeCap(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->hasEdgeCap_[index];
}

double lefiNonDefault::layerEdgeCap(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1402): The index number %d given for the NONDEFAULT LAYER is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0.0;
  }
  return this->edgeCap_[index];
}

int lefiNonDefault::numVias() const { 
  return this->numVias_;
}

lefiVia* lefiNonDefault::viaRule(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numVias_) {
    sprintf (msg, "ERROR (LEFPARS-1403): The index number %d given for the NONDEFAULT VIA is invalid.\nValid index is from 0 to %d", index, this->numVias_);
    lefiError (msg);
    return 0;
  }
  return this->viaRules_[index];
}

int lefiNonDefault::numSpacingRules() const { 
  return this->numSpacing_;
}

lefiSpacing* lefiNonDefault::spacingRule(int index) const { 
  char msg[160];
  if (index < 0 || index >= this->numSpacing_) {
    sprintf (msg, "ERROR (LEFPARS-1404): The index number %d given for the NONDEFAULT SPACING is invalid.\nValid index is from 0 to %d", index, this->numSpacing_);
    lefiError (msg);
    return 0;
  }
  return this->spacingRules_[index];
}

const char* lefiNonDefault::name() const {
  return this->name_;
}

int lefiNonDefault::hasHardspacing() const {
  return this->hardSpacing_;
}

int lefiNonDefault::numUseVia() const { 
  return this->numUseVias_;
}

const char* lefiNonDefault::viaName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numUseVias_) {
    sprintf (msg, "ERROR (LEFPARS-1405): The index number %d given for the NONDEFAULT USE VIA is invalid.\nValid index is from 0 to %d", index, this->numUseVias_);
    lefiError (msg);
    return 0;
  }
  return this->useViaName_[index];
}

int lefiNonDefault::numUseViaRule() const { 
  return this->numUseViaRules_;
}

const char* lefiNonDefault::viaRuleName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numUseViaRules_) {
    sprintf (msg, "ERROR (LEFPARS-1406): The index number %d given for the NONDEFAULT USE VIARULE is invalid.\nValid index is from 0 to %d", index, this->numUseViaRules_);
    lefiError (msg);
    return 0;
  }
  return this->useViaRuleName_[index];
}

int lefiNonDefault::numMinCuts() const { 
  return this->numMinCuts_;
}

const char* lefiNonDefault::cutLayerName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numMinCuts_) {
    sprintf (msg, "ERROR (LEFPARS-1407): The index number %d given for the NONDEFAULT CUT is invalid.\nValid index is from 0 to %d", index, this->numMinCuts_);
    lefiError (msg);
    return 0;
  }
  return this->cutLayerName_[index];
}

int lefiNonDefault::numCuts(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numMinCuts_) {
    sprintf (msg, "ERROR (LEFPARS-1407): The index number %d given for the NONDEFAULT CUT is invalid.\nValid index is from 0 to %d", index, this->numMinCuts_);
    lefiError (msg);
    return 0;
  }
  return this->numCuts_[index];
}

void lefiNonDefault::print(FILE* f) const { 
  int i;
  lefiSpacing* s;
  lefiVia* v;

  fprintf(f, "Nondefault rule %s\n",
    this->lefiNonDefault::name());
  fprintf(f, "%d layers   %d vias   %d spacing rules\n",
    this->lefiNonDefault::numLayers(),
    this->lefiNonDefault::numVias(),
    this->lefiNonDefault::numSpacingRules());

  for (i = 0; i < this->lefiNonDefault::numLayers(); i++) {
    fprintf(f, "  Layer %s\n", this->lefiNonDefault::layerName(i));
    if (this->lefiNonDefault::hasLayerWidth(i))
      fprintf(f, "    WIDTH %g\n", this->lefiNonDefault::layerWidth(i));
    if (this->lefiNonDefault::hasLayerDiagWidth(i))
      fprintf(f, "    DIAGWIDTH %g\n", this->lefiNonDefault::layerDiagWidth(i));
    if (this->lefiNonDefault::hasLayerSpacing(i))
      fprintf(f, "    SPACING %g\n", this->lefiNonDefault::layerSpacing(i));
    if (this->lefiNonDefault::hasLayerWireExtension(i))
      fprintf(f, "    WIREEXTENSION %g",
      this->lefiNonDefault::layerWireExtension(i));
    if (this->lefiNonDefault::hasLayerResistance(i))
      fprintf(f, "    RESISTANCE RPERSQ %g\n",
              this->lefiNonDefault::layerResistance(i));
    if (this->lefiNonDefault::hasLayerCapacitance(i))
      fprintf(f, "    CAPACITANCE CPERSQDIST %g\n",
              this->lefiNonDefault::layerCapacitance(i));
    if (this->lefiNonDefault::hasLayerEdgeCap(i))
      fprintf(f, "    EDGECAPACITANCE %g\n",
              this->lefiNonDefault::layerEdgeCap(i));
  }

  for (i = 0; i < this->lefiNonDefault::numVias(); i++) {
    v = this->lefiNonDefault::viaRule(i);
    v->lefiVia::print(f);
    }

  for (i = 0; i < this->lefiNonDefault::numSpacingRules(); i++) {
    s = this->lefiNonDefault::spacingRule(i);
    s->lefiSpacing::print(f);
    }
}

int lefiNonDefault::numProps() const {
  return this->numProps_;
}

void lefiNonDefault::addProp(const char* name, const char* value,
                             const char type) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim = this->numProps_;
    char**  nn;
    char**  nv;
    double* nd;
    char*   nt;

    if (this->propsAllocated_ == 0) 
      max = this->propsAllocated_ = 2;
    else
      max = this->propsAllocated_ *= 2;
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
    this->names_   = nn;
    this->values_  = nv;
    this->dvalues_ = nd;
    this->types_   = nt;
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

void lefiNonDefault::addNumProp(const char* name, const double d,
                                const char* value, const char type) {
  int len = strlen(name) + 1;
  if (this->numProps_ == this->propsAllocated_) {
    int i;
    int max;
    int lim = this->numProps_;
    char**  nn;
    char**  nv;
    double* nd;
    char*   nt;

    if (this->propsAllocated_ == 0)
       max = this->propsAllocated_ = 2;
    else
       max = this->propsAllocated_ *= 2;
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
    this->names_   = nn;
    this->values_  = nv;
    this->dvalues_ = nd;
    this->types_   = nt;
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

const char* lefiNonDefault::propName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1408): The index number %d given for the NONDEFAULT PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->names_[index];
}

const char* lefiNonDefault::propValue(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1408): The index number %d given for the NONDEFAULT PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->values_[index];
}

double lefiNonDefault::propNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1408): The index number %d given for the NONDEFAULT PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->dvalues_[index];
}

char lefiNonDefault::propType(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1408): The index number %d given for the NONDEFAULT PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->types_[index];
}

int lefiNonDefault::propIsNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1408): The index number %d given for the NONDEFAULT PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->dvalues_[index] ? 1 : 0;
}

int lefiNonDefault::propIsString(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProps_) {
    sprintf (msg, "ERROR (LEFPARS-1408): The index number %d given for the NONDEFAULT PROPERTY is invalid.\nValid index is from 0 to %d", index, this->numProps_);
    lefiError (msg);
    return 0;
  }
  return this->dvalues_[index] ? 0 : 1;
}

} // namespace LefParser
