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
#include <limbo/parsers/lef/bison/lefiArray.hpp>
#include <limbo/parsers/lef/bison/lefiMisc.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

///////////////////////////////////////////////
///////////////////////////////////////////////
//
//      lefiArrayFloorPlan
//
///////////////////////////////////////////////
///////////////////////////////////////////////


void lefiArrayFloorPlan::Init(const char* name) {
  int len = strlen(name)+1;
  this->name_ = (char*)lefMalloc(len);
  strcpy(this->name_, CASE(name));
  this->numPatterns_ = 0;
  this->patternsAllocated_ = 2;
  this->types_ = (char**)lefMalloc(sizeof(char*)*2);
  this->patterns_ = (lefiSitePattern**)lefMalloc(sizeof(lefiSitePattern*)*2);
}


void lefiArrayFloorPlan::Destroy() {
  lefiSitePattern* s;
  int i;
  for (i = 0; i < this->numPatterns_; i++) {
    s = this->patterns_[i];
    s->lefiSitePattern::Destroy();
    lefFree((char*)s);
    lefFree((char*)(this->types_[i]));
  }
  lefFree((char*)(this->types_));
  lefFree((char*)(this->patterns_));
  lefFree(this->name_);
}


void lefiArrayFloorPlan::addSitePattern(const char* typ, lefiSitePattern* s) {
  int len = strlen (typ) + 1;
  if (this->numPatterns_ == this->patternsAllocated_) {
    int i;
    int lim;
    char** nc;
    lefiSitePattern** np;

    if (this->patternsAllocated_ == 0)
      lim = this->patternsAllocated_ = 2;
    else
      lim = this->patternsAllocated_ = this->patternsAllocated_ * 2;
    nc = (char**)lefMalloc(sizeof(char*)*lim);
    np = (lefiSitePattern**) lefMalloc(sizeof(lefiSitePattern*)*lim);
    lim /= 2;
    for (i = 0; i < lim; i++) {
      nc[i] = this->types_[i];
      np[i] = this->patterns_[i];
    }
    lefFree((char*)(this->types_));
    lefFree((char*)(this->patterns_));
    this->types_ = nc;
    this->patterns_ = np;
  }
  this->types_[this->numPatterns_] = (char*)lefMalloc(len);
  strcpy(this->types_[this->numPatterns_], typ);
  this->patterns_[this->numPatterns_] = s;
  this->numPatterns_ += 1;
}


int lefiArrayFloorPlan::numPatterns() const {
  return this->numPatterns_;
}


lefiSitePattern* lefiArrayFloorPlan::pattern(int index) {
  return this->patterns_[index];
}


char* lefiArrayFloorPlan::typ(int index) {
  return this->types_[index];
}


const char* lefiArrayFloorPlan::name() {
  return this->name_;
}


///////////////////////////////////////////////
///////////////////////////////////////////////
//
//      lefiArray
//
///////////////////////////////////////////////
///////////////////////////////////////////////



lefiArray::lefiArray() {
  this->lefiArray::Init();
}


void lefiArray::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);

  this->numPatterns_ = 0;
  this->patternsAllocated_ = 0;
  this->pattern_ = 0;
  this->lefiArray::bump((void***)(&(this->pattern_)), this->numPatterns_, &(this->patternsAllocated_));

  this->numCan_ = 0;
  this->canAllocated_ = 0;
  this->canPlace_ = 0;
  this->lefiArray::bump((void***)(&(this->canPlace_)), this->numCan_, &(this->canAllocated_));

  this->numCannot_ = 0;
  this->cannotAllocated_ = 0;
  this->cannotOccupy_ = 0;
  this->lefiArray::bump((void***)(&(this->cannotOccupy_)), this->numCannot_, &(this->cannotAllocated_));

  this->numTracks_ = 0;
  this->tracksAllocated_ = 0;
  this->track_ = 0;
  this->lefiArray::bump((void***)(&(this->track_)), this->numTracks_, &(this->tracksAllocated_));

  this->numG_ = 0;
  this->gAllocated_ = 0;
  this->gcell_ = 0;
  this->lefiArray::bump((void***)(&(this->gcell_)), this->numG_, &(this->gAllocated_));

  this->numDefault_ = 0;
  this->defaultAllocated_ = 4;
  this->minPins_ = (int*)lefMalloc(sizeof(int)*4);
  this->caps_ = (double*)lefMalloc(sizeof(double)*4);

  this->floorPlansAllocated_ = 0;
  this->numFloorPlans_ = 0;
  this->floors_ = 0;
  this->lefiArray::bump((void***)(&(this->floors_)), this->numFloorPlans_, &(this->floorPlansAllocated_));
}


void lefiArray::Destroy() {
  this->lefiArray::clear();

  lefFree((char*)(this->name_));
  lefFree((char*)(this->caps_));
  lefFree((char*)(this->minPins_));
  lefFree((char*)(this->floors_));
  lefFree((char*)(this->track_));
  lefFree((char*)(this->gcell_));
  lefFree((char*)(this->cannotOccupy_));
  lefFree((char*)(this->canPlace_));
  lefFree((char*)(this->pattern_));
}


lefiArray::~lefiArray() {
  this->lefiArray::Destroy();
}


void lefiArray::addSitePattern(lefiSitePattern* s) {
/*
  if (this->numPatterns_ == this->patternsAllocated_)
    this->lefiArray::bump((void***)(&(this->pattern_)), this->numPatterns_,
    &(this->patternsAllocated_));
*/
  if (this->numPatterns_ == this->patternsAllocated_) {
    lefiSitePattern** tpattern;
    int i;

    if (this->patternsAllocated_ == 0)
        this->patternsAllocated_ = 2;
    else
        this->patternsAllocated_ = this->patternsAllocated_ * 2;
    tpattern = (lefiSitePattern**)lefMalloc(sizeof(lefiSitePattern*) *
               this->patternsAllocated_);
    for (i = 0; i < this->numPatterns_; i++) {
        tpattern[i] = this->pattern_[i];
    }
    if (this->pattern_)
        lefFree((char*)(this->pattern_));
    this->pattern_ = tpattern;
/*
    this->lefiArray::bump((void***)(&(this->pattern_)), this->numPatterns_,
    &(this->patternsAllocated_));
*/
  }

  this->pattern_[this->numPatterns_] = s;
  this->numPatterns_ += 1;
}


void lefiArray::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}


void lefiArray::setTableSize(int tsize) {
  this->tableSize_ = tsize;
  this->hasDefault_ = 1;
}


void lefiArray::addDefaultCap(int minPins, double cap) {
  if (this->numDefault_ == this->defaultAllocated_) {
    int i;
    int lim;
    double* nc;
    int* np;

    if (this->defaultAllocated_ == 0)
      lim = this->defaultAllocated_ = 2;
    else
      lim = this->defaultAllocated_ * 2;
    this->defaultAllocated_ = lim;
    nc = (double*)lefMalloc(sizeof(double)*lim);
    np = (int*)lefMalloc(sizeof(int)*lim);
    lim /= 2;
    for (i = 0; i < lim; i++) {
      nc[i] = this->caps_[i];
      np[i] = this->minPins_[i];
    }
    lefFree((char*)(this->caps_));
    lefFree((char*)(this->minPins_));
    this->caps_ = nc;
    this->minPins_ = np;
  }
  this->caps_[this->numDefault_] = cap;
  this->minPins_[this->numDefault_] = minPins;
  this->numDefault_ += 1;
}


void lefiArray::addCanPlace(lefiSitePattern* s) {
  if (this->numCan_ == this->canAllocated_) {
    lefiSitePattern** cplace;
    int i;

    if (this->canAllocated_ == 0)
        this->canAllocated_ = 2;
    else
        this->canAllocated_ = this->canAllocated_ * 2;
    cplace = (lefiSitePattern**)lefMalloc(sizeof(lefiSitePattern*) *
             this->canAllocated_);
    for (i = 0; i < this->numCan_; i++) {
        cplace[i] = this->canPlace_[i];
    }
    if (this->canPlace_)
        lefFree((char*)(this->canPlace_));
    this->canPlace_ = cplace;
  }
/*
    this->lefiArray::bump((void***)(&(this->canPlace_)), this->numCan_, &(this->canAllocated_));
*/
  this->canPlace_[this->numCan_] = s;
  this->numCan_ += 1;
}


void lefiArray::addCannotOccupy(lefiSitePattern* s) {
  if (this->numCannot_ == this->cannotAllocated_) {
    lefiSitePattern** cnplace;
    int i;

    if (this->cannotAllocated_ == 0)
        this->cannotAllocated_ = 2;
    else
        this->cannotAllocated_ = this->cannotAllocated_ * 2;
    cnplace = (lefiSitePattern**)lefMalloc(sizeof(lefiSitePattern*) *
             this->cannotAllocated_);
    for (i = 0; i < this->numCannot_; i++) {
        cnplace[i] = this->cannotOccupy_[i];
    }
    if (this->cannotOccupy_)
        lefFree((char*)(this->cannotOccupy_));
    this->cannotOccupy_ = cnplace;
  }
/*
  if (this->numCannot_ == this->cannotAllocated_)
    this->lefiArray::bump((void***)(&(this->cannotOccupy_)), this->numCannot_,
    &(this->cannotAllocated_));
*/
  this->cannotOccupy_[this->numCannot_] = s;
  this->numCannot_ += 1;
}


void lefiArray::addTrack(lefiTrackPattern* t) {
  if (this->numTracks_ == this->tracksAllocated_) {
    lefiTrackPattern** tracks;
    int i;

    if (this->tracksAllocated_ == 0)
        this->tracksAllocated_ = 2;
    else
        this->tracksAllocated_ = this->tracksAllocated_ * 2;
    tracks = (lefiTrackPattern**)lefMalloc(sizeof(lefiTrackPattern*) *
             this->tracksAllocated_);
    for (i = 0; i < this->numTracks_; i++) {
        tracks[i] = this->track_[i];
    }
    if (this->track_)
        lefFree((char*)(this->track_));
    this->track_ = tracks;
  }
/*
  if (this->numTracks_ == this->tracksAllocated_)
    this->lefiArray::bump((void***)(&(this->track_)), this->numTracks_, &(this->tracksAllocated_));
*/
  this->track_[this->numTracks_] = t;
  this->numTracks_ += 1;
}


void lefiArray::addGcell(lefiGcellPattern* g) {
  if (this->numG_ == this->gAllocated_) {
    lefiGcellPattern** cells;
    int i;

    if (this->gAllocated_ == 0)
        this->gAllocated_ = 2;
    else
        this->gAllocated_ = this->gAllocated_ * 2;
    cells = (lefiGcellPattern**)lefMalloc(sizeof(lefiGcellPattern*) *
             this->gAllocated_);
    for (i = 0; i < this->numG_; i++) {
        cells[i] = this->gcell_[i];
    }
    if (this->gcell_)
        lefFree((char*)(this->gcell_));
    this->gcell_ = cells;
  }
/*
  if (this->numG_ == this->gAllocated_)
    this->lefiArray::bump((void***)(&(this->gcell_)), this->numG_, &(this->gAllocated_));
*/
  this->gcell_[this->numG_] = g;
  this->numG_ += 1;
}


void lefiArray::addFloorPlan(const char* name) {
  lefiArrayFloorPlan* f;
  if (this->numFloorPlans_ == this->floorPlansAllocated_) {
    int i;
    lefiArrayFloorPlan** tf;

    if (this->floorPlansAllocated_ == 0)
        this->floorPlansAllocated_ = 2;
    else
        this->floorPlansAllocated_ = this->floorPlansAllocated_ * 2;
    tf = (lefiArrayFloorPlan**)lefMalloc(sizeof(lefiArrayFloorPlan*) *
             this->floorPlansAllocated_);
    for (i = 0; i < this->numFloorPlans_; i++) {
        tf[i] = this->floors_[i];
    }
    if (this->floors_)
        lefFree((char*)(this->floors_));
    this->floors_ = tf;
  }
/*
  if (this->numFloorPlans_ == this->floorPlansAllocated_) {
    this->lefiArray::bump((void***)(&(this->floors_)), this->numFloorPlans_,
    &(this->floorPlansAllocated_));
  }
*/
  f = (lefiArrayFloorPlan*)lefMalloc(sizeof(lefiArrayFloorPlan));
  f->lefiArrayFloorPlan::Init(name);
  this->floors_[this->numFloorPlans_] = f;
  this->numFloorPlans_ += 1;
}


void lefiArray::addSiteToFloorPlan(const char* typ, lefiSitePattern* s) {
  lefiArrayFloorPlan* f = this->floors_[this->numFloorPlans_-1];
  f->lefiArrayFloorPlan::addSitePattern(typ, s);
}


void lefiArray::bump(void*** arr, int used, int* allocated) {
  int size = *allocated * 2;
  int i;
  void** newa;
  if (size == 0) size = 2;
  newa = (void**)lefMalloc(sizeof(void*)*size);

  for (i = 0; i < used; i++) {
    newa[i] = (*arr)[i];
  }

  if (*arr)
     lefFree((char*)(*arr));
  *allocated = size;
  *arr = newa;
}


void lefiArray::clear() {
  int i;
  lefiSitePattern* p;
  lefiGcellPattern* g;
  lefiTrackPattern* t;
  lefiArrayFloorPlan* f;

  for (i = 0; i < this->numPatterns_; i++) {
    p = this->pattern_[i];
    p->lefiSitePattern::Destroy();
    lefFree((char*)p);
  }
  this->numPatterns_ = 0;

  for (i = 0; i < this->numCan_; i++) {
    p = this->canPlace_[i];
    p->lefiSitePattern::Destroy();
    lefFree((char*)p);
  }
  this->numCan_ = 0;

  for (i = 0; i < this->numCannot_; i++) {
    p = this->cannotOccupy_[i];
    p->lefiSitePattern::Destroy();
    lefFree((char*)p);
  }
  this->numCannot_ = 0;

  for (i = 0; i < this->numTracks_; i++) {
    t = this->track_[i];
    t->lefiTrackPattern::Destroy();
    lefFree((char*)t);
  }
  this->numTracks_ = 0;

  for (i = 0; i < this->numG_; i++) {
    g = this->gcell_[i];
    g->lefiGcellPattern::Destroy();
    lefFree((char*)g);
  }
  this->numG_ = 0;

  this->hasDefault_ = 0;
  this->tableSize_ = 0;
  this->numDefault_ = 0;

  for (i = 0; i < this->numFloorPlans_; i++) {
    f = this->floors_[i];
    f->lefiArrayFloorPlan::Destroy();
    lefFree((char*)f);
  }
  this->numFloorPlans_ = 0;

}


int lefiArray::numSitePattern() {
  return this->numPatterns_;
}


int lefiArray::numCanPlace() {
  return this->numCan_;
}


int lefiArray::numCannotOccupy() {
  return this->numCannot_;
}


int lefiArray::numTrack() {
  return this->numTracks_;
}


int lefiArray::numGcell() {
  return this->numG_;
}


int lefiArray::hasDefaultCap() {
  return this->hasDefault_;
}


const char* lefiArray::name() const {
  return this->name_;
}


lefiSitePattern* lefiArray::sitePattern(int index) {
  return this->pattern_[index];
}


lefiSitePattern* lefiArray::canPlace(int index) {
  return this->canPlace_[index];
}


lefiSitePattern* lefiArray::cannotOccupy(int index) {
  return this->cannotOccupy_[index];
}


lefiTrackPattern* lefiArray::track(int index) {
  return this->track_[index];
}


lefiGcellPattern* lefiArray::gcell(int index) {
  return this->gcell_[index];
}


int lefiArray::tableSize() {
  return this->tableSize_;
}


int lefiArray::numDefaultCaps() {
  return this->numDefault_;
}


int lefiArray::defaultCapMinPins(int index) {
  return this->minPins_[index];
}


double lefiArray::defaultCap(int index) {
  return this->caps_[index];
}


int lefiArray::numFloorPlans() {
  return this->numFloorPlans_;
}


const char* lefiArray::floorPlanName(int index) {
  lefiArrayFloorPlan* f = this->floors_[index];
  return f->lefiArrayFloorPlan::name();
}


int lefiArray::numSites(int index) {
  lefiArrayFloorPlan* f = this->floors_[index];
  return f->lefiArrayFloorPlan::numPatterns();
}


const char* lefiArray::siteType(int index, int j) {
  lefiArrayFloorPlan* f = this->floors_[index];
  return f->lefiArrayFloorPlan::typ(j);
}


lefiSitePattern* lefiArray::site(int index, int j) {
  lefiArrayFloorPlan* f = this->floors_[index];
  return f->lefiArrayFloorPlan::pattern(j);
}


void lefiArray::print(FILE* f) const {
  fprintf(f, "ARRAY %s\n", this->lefiArray::name());
}

} // namespace LefParser
