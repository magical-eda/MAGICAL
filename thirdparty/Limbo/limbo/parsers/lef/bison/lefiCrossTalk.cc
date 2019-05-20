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
#include <limbo/parsers/lef/bison/lefiCrossTalk.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiNoiseVictim
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiNoiseVictim::lefiNoiseVictim(double d) {
  this->lefiNoiseVictim::Init(d);
}


void lefiNoiseVictim::Init(double d) {
  this->length_ = d;

  this->numNoises_ = 0;
  this->noisesAllocated_ = 2;
  this->noises_ = (double*)lefMalloc(sizeof(double)*2);
}


void lefiNoiseVictim::clear() {
  this->numNoises_ = 0;
}


void lefiNoiseVictim::Destroy() {
  this->lefiNoiseVictim::clear();
  lefFree((char*)(this->noises_));
}


lefiNoiseVictim::~lefiNoiseVictim() {
  this->lefiNoiseVictim::Destroy();
}


void lefiNoiseVictim::addVictimNoise(double d) {
  if (this->numNoises_ == this->noisesAllocated_) {
    int max;
    double* ne;
    int i;

    if (this->noisesAllocated_ == 0) {
      max = this->noisesAllocated_ = 2;
      this->numNoises_ = 0;
    } else
      max = this->noisesAllocated_ = this->numNoises_ * 2;
    ne = (double*)lefMalloc(sizeof(double)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->noises_[i];
    lefFree((char*)(this->noises_));
    this->noises_ = ne;
  }
  this->noises_[this->numNoises_] = d;
  this->numNoises_ += 1;
}


int lefiNoiseVictim::numNoises() {
  return this->numNoises_;
}


double lefiNoiseVictim::noise(int index) {
  return this->noises_[index];
}


double lefiNoiseVictim::length() {
  return this->length_;
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiNoiseResistance
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiNoiseResistance::lefiNoiseResistance() {
  this->lefiNoiseResistance::Init();
}


void lefiNoiseResistance::Init() {
  this->numNums_ = 0;
  this->numsAllocated_ = 1;
  this->nums_ = (double*)lefMalloc(sizeof(double)*1);

  this->numVictims_ = 0;
  this->victimsAllocated_ = 2;
  this->victims_ = (lefiNoiseVictim**)lefMalloc(sizeof(
	 lefiNoiseVictim*)*2);
}


void lefiNoiseResistance::clear() {
  int i;
  lefiNoiseVictim* r;
  int max = this->numVictims_;

  for (i = 0; i < max; i++) {
    r = this->victims_[i];
    r->lefiNoiseVictim::Destroy();
    lefFree((char*)r);
  }
  this->numVictims_ = 0;
  this->numNums_ = 0;
}


void lefiNoiseResistance::Destroy() {
  this->lefiNoiseResistance::clear();
  lefFree((char*)(this->nums_));
  lefFree((char*)(this->victims_));
}


lefiNoiseResistance::~lefiNoiseResistance() {
  this->lefiNoiseResistance::Destroy();
}


void lefiNoiseResistance::addResistanceNumber(double d) {
  if (this->numNums_ == this->numsAllocated_) {
    int max;
    double* ne;
    int i;

    if (this->numsAllocated_ == 0) {
      max = this->numsAllocated_ = 2;
      this->numNums_ = 0;
    } else
      max = this->numsAllocated_ = this->numNums_ * 2;
    ne = (double*)lefMalloc(sizeof(double)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->nums_[i];
    lefFree((char*)(this->nums_));
    this->nums_ = ne;
  }
  this->nums_[this->numNums_] = d;
  this->numNums_ += 1;
}


void lefiNoiseResistance::addVictimNoise(double d) {
  lefiNoiseVictim* r = this->victims_[this->numVictims_-1];
  r->lefiNoiseVictim::addVictimNoise(d);
}


void lefiNoiseResistance::addVictimLength(double d) {
  lefiNoiseVictim* r;
  if (this->numVictims_ == this->victimsAllocated_) {
    int max;
    lefiNoiseVictim** ne;
    int i;

    if (this->victimsAllocated_ == 0) {
      max = this->victimsAllocated_ =  2;
      this->numVictims_ = 0;
    } else
      max = this->victimsAllocated_ = this->numVictims_ * 2;

    ne = (lefiNoiseVictim**)lefMalloc (sizeof(lefiNoiseVictim*)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->victims_[i];
    lefFree((char*)(this->victims_));
    this->victims_ = ne;
  }
  r = (lefiNoiseVictim*)lefMalloc(sizeof(lefiNoiseVictim));
  r->lefiNoiseVictim::Init(d);
  this->victims_[this->numVictims_] = r;
  this->numVictims_ += 1;
}


int lefiNoiseResistance::numVictims() {
  return this->numVictims_;
}


lefiNoiseVictim* lefiNoiseResistance::victim(int index) {
  return this->victims_[index];
}


int lefiNoiseResistance::numNums() {
  return this->numNums_;
}


double lefiNoiseResistance::num(int index) {
  return this->nums_[index];
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiNoiseEdge
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiNoiseEdge::lefiNoiseEdge() {
  this->lefiNoiseEdge::Init();
}


void lefiNoiseEdge::Init() {
  this->edge_ = 0;

  this->numResistances_ = 0;
  this->resistancesAllocated_ = 2;
  this->resistances_ = (lefiNoiseResistance**)lefMalloc(sizeof(
         lefiNoiseResistance*)*2);
}


void lefiNoiseEdge::clear() {
  int i;
  lefiNoiseResistance* r;
  int maxr = this->numResistances_;

  for (i = 0; i < maxr; i++) {
    r = this->resistances_[i];
    r->lefiNoiseResistance::Destroy();
    lefFree((char*)r);
  }

  this->edge_ = 0;
  this->numResistances_ = 0;
}


void lefiNoiseEdge::Destroy() {
  this->lefiNoiseEdge::clear();
  lefFree((char*)(this->resistances_));
}


lefiNoiseEdge::~lefiNoiseEdge() {
  this->lefiNoiseEdge::Destroy();
}


void lefiNoiseEdge::addEdge(double d) {
  this->edge_ = d;
}


void lefiNoiseEdge::addResistanceNumber(double d) {
  lefiNoiseResistance* r = this->resistances_[this->numResistances_-1];
  r->lefiNoiseResistance::addResistanceNumber(d);
}
 
 
void lefiNoiseEdge::addResistance() {
  lefiNoiseResistance* r;
  if (this->numResistances_ == this->resistancesAllocated_) {
    int max;
    lefiNoiseResistance** ne;
    int i;

    if (this->resistancesAllocated_ == 0) {
      max = this->resistancesAllocated_ = 2;
      this->numResistances_ = 0;
    } else
      max = this->resistancesAllocated_ = this->numResistances_ * 2;
    ne = (lefiNoiseResistance**)lefMalloc (sizeof(lefiNoiseResistance*)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->resistances_[i];
    lefFree((char*)(this->resistances_));
    this->resistances_ = ne;
  }
  r = (lefiNoiseResistance*)lefMalloc(sizeof(lefiNoiseResistance));
  r->lefiNoiseResistance::Init();
  this->resistances_[this->numResistances_] = r;
  this->numResistances_ += 1;
}


void lefiNoiseEdge::addVictimNoise(double d) {
  lefiNoiseResistance* r = this->resistances_[this->numResistances_-1];
  r->lefiNoiseResistance::addVictimNoise(d);
}


void lefiNoiseEdge::addVictimLength(double d) {
  lefiNoiseResistance* r = this->resistances_[this->numResistances_-1];
  r->lefiNoiseResistance::addVictimLength(d);
}


int lefiNoiseEdge::numResistances() {
  return this->numResistances_;
}


lefiNoiseResistance* lefiNoiseEdge::resistance(int index) {
  return this->resistances_[index];
}


double lefiNoiseEdge::edge() {
  return this->edge_;
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiNoiseTable
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiNoiseTable::lefiNoiseTable() {
  this->lefiNoiseTable::Init();
}


void lefiNoiseTable::Init() {
  this->numEdges_ = 0;
  this->edgesAllocated_ = 2;
  this->edges_ = (lefiNoiseEdge**)lefMalloc(sizeof(lefiNoiseEdge*)*2);
}


void lefiNoiseTable::clear() {
  int i;
  lefiNoiseEdge* r;
  int max = this->numEdges_;

  for (i = 0; i < max; i++) {
    r = this->edges_[i];
    r->lefiNoiseEdge::Destroy();
    lefFree((char*)r);
  }
  this->numEdges_ = 0;
}


void lefiNoiseTable::Destroy() {
  this->lefiNoiseTable::clear();
  lefFree((char*)(this->edges_));
}


lefiNoiseTable::~lefiNoiseTable() {
  this->lefiNoiseTable::Destroy();
}


void lefiNoiseTable::setup(int i) {
  this->num_ = i;
  this->lefiNoiseTable::clear();
}


void lefiNoiseTable::newEdge() {
  lefiNoiseEdge* r;
  if (this->numEdges_ == this->edgesAllocated_) {
    int max;
    lefiNoiseEdge** ne;
    int i;

    if (this->edgesAllocated_ == 0) {
      max = this->edgesAllocated_ = 2;
      this->numEdges_ = 0;
    } else
      max = this->edgesAllocated_ = this->numEdges_ * 2;
    ne = (lefiNoiseEdge**)lefMalloc (sizeof(lefiNoiseEdge*)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->edges_[i];
    lefFree((char*)(this->edges_));
    this->edges_ = ne;
  }
  r = (lefiNoiseEdge*)lefMalloc(sizeof(lefiNoiseEdge));
  r->lefiNoiseEdge::Init();
  this->edges_[this->numEdges_] = r;
  this->numEdges_ += 1;
}


void lefiNoiseTable::addEdge(double d) {
  lefiNoiseEdge* r = this->edges_[this->numEdges_-1];
  r->lefiNoiseEdge::addEdge(d);
}


void lefiNoiseTable::addResistance() {
  lefiNoiseEdge* r = this->edges_[this->numEdges_-1];
  r->lefiNoiseEdge::addResistance();
}

void lefiNoiseTable::addResistanceNumber(double d) {
  lefiNoiseEdge* r = this->edges_[this->numEdges_-1];
  r->lefiNoiseEdge::addResistanceNumber(d);
}


void lefiNoiseTable::addVictimLength(double d) {
  lefiNoiseEdge* r = this->edges_[this->numEdges_ - 1];
  r->lefiNoiseEdge::addVictimLength(d);
}


void lefiNoiseTable::addVictimNoise(double d) {
  lefiNoiseEdge* r = this->edges_[this->numEdges_ - 1];
  r->lefiNoiseEdge::addVictimNoise(d);
}


int lefiNoiseTable::num() {
  return this->num_;
}


int lefiNoiseTable::numEdges() {
  return this->numEdges_;
}


lefiNoiseEdge* lefiNoiseTable::edge(int index) {
  return this->edges_[index];
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiCorrectionVictim
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiCorrectionVictim::lefiCorrectionVictim(double d) {
  this->lefiCorrectionVictim::Init(d);
}


void lefiCorrectionVictim::Init(double d) {
  this->length_ = d;

  this->numCorrections_ = 0;
  this->correctionsAllocated_ = 2;
  this->corrections_ = (double*)lefMalloc(sizeof(double)*2);
}


void lefiCorrectionVictim::clear() {
  this->numCorrections_ = 0;
}


void lefiCorrectionVictim::Destroy() {
  this->lefiCorrectionVictim::clear();
  lefFree((char*)(this->corrections_));
}


lefiCorrectionVictim::~lefiCorrectionVictim() {
  this->lefiCorrectionVictim::Destroy();
}


void lefiCorrectionVictim::addVictimCorrection(double d) {
  if (this->numCorrections_ == this->correctionsAllocated_) {
    int max;
    double* ne;
    int i;

    if (this->correctionsAllocated_ == 0) {
      max = this->correctionsAllocated_ = 2;
      this->numCorrections_ = 0;
    } else
      max = this->correctionsAllocated_ = this->numCorrections_ * 2;
    ne = (double*)lefMalloc(sizeof(double)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->corrections_[i];
    lefFree((char*)(this->corrections_));
    this->corrections_ = ne;
  }
  this->corrections_[this->numCorrections_] = d;
  this->numCorrections_ += 1;
}


int lefiCorrectionVictim::numCorrections() {
  return this->numCorrections_;
}


double lefiCorrectionVictim::correction(int index) {
  return this->corrections_[index];
}


double lefiCorrectionVictim::length() {
  return this->length_;
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiCorrectionResistance
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiCorrectionResistance::lefiCorrectionResistance() {
  this->lefiCorrectionResistance::Init();
}


void lefiCorrectionResistance::Init() {
  this->numNums_ = 0;
  this->numsAllocated_ = 1;
  this->nums_ = (double*)lefMalloc(sizeof(double)*1);

  this->numVictims_ = 0;
  this->victimsAllocated_ = 2;
  this->victims_ = (lefiCorrectionVictim**)lefMalloc(sizeof(
	 lefiCorrectionVictim*)*2);
}


void lefiCorrectionResistance::clear() {
  int i;
  lefiCorrectionVictim* r;
  int max = this->numVictims_;

  for (i = 0; i < max; i++) {
    r = this->victims_[i];
    r->lefiCorrectionVictim::Destroy();
    lefFree((char*)r);
  }
  this->numVictims_ = 0;
  this->numNums_ = 0;
}


void lefiCorrectionResistance::Destroy() {
  this->lefiCorrectionResistance::clear();
  lefFree((char*)(this->nums_));
  lefFree((char*)(this->victims_));
}


lefiCorrectionResistance::~lefiCorrectionResistance() {
  this->lefiCorrectionResistance::Destroy();
}


void lefiCorrectionResistance::addResistanceNumber(double d) {
  if (this->numNums_ == this->numsAllocated_) {
    int max;
    double* ne;
    int i;

    if (this->numsAllocated_) {
      max = this->numsAllocated_ = 2;
      this->numNums_ = 0;
    } else
      max = this->numsAllocated_ = this->numNums_ * 2;
    ne = (double*)lefMalloc(sizeof(double)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->nums_[i];
    lefFree((char*)(this->nums_));
    this->nums_ = ne;
  }
  this->nums_[this->numNums_] = d;
  this->numNums_ += 1;
}


void lefiCorrectionResistance::addVictimCorrection(double d) {
  lefiCorrectionVictim* r = this->victims_[this->numVictims_-1];
  r->lefiCorrectionVictim::addVictimCorrection(d);
}


void lefiCorrectionResistance::addVictimLength(double d) {
  lefiCorrectionVictim* r;
  if (this->numVictims_ == this->victimsAllocated_) {
    int max;
    lefiCorrectionVictim** ne;
    int i;

    if (this->victimsAllocated_ == 0) {
      max = this->victimsAllocated_ = 2;
      this->numVictims_ = 0;
    } else
      max = this->victimsAllocated_ = this->numVictims_ * 2;
    ne = (lefiCorrectionVictim**)lefMalloc (sizeof(lefiCorrectionVictim*)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->victims_[i];
    lefFree((char*)(this->victims_));
    this->victims_ = ne;
  }
  r = (lefiCorrectionVictim*)lefMalloc(sizeof(lefiCorrectionVictim));
  r->lefiCorrectionVictim::Init(d);
  this->victims_[this->numVictims_] = r;
  this->numVictims_ += 1;
}


int lefiCorrectionResistance::numVictims() {
  return this->numVictims_;
}


lefiCorrectionVictim* lefiCorrectionResistance::victim(int index) {
  return this->victims_[index];
}


int lefiCorrectionResistance::numNums() {
  return this->numNums_;
}


double lefiCorrectionResistance::num(int index) {
  return this->nums_[index];
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiCorrectionEdge
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
 
 
lefiCorrectionEdge::lefiCorrectionEdge() {
  this->lefiCorrectionEdge::Init();
}
 
 
void lefiCorrectionEdge::Init() {
  this->edge_ = 0;
 
  this->numResistances_ = 0;
  this->resistancesAllocated_ = 2;
  this->resistances_ = (lefiCorrectionResistance**)lefMalloc(sizeof(
         lefiCorrectionResistance*)*2);
}
 
 
void lefiCorrectionEdge::clear() {
  int i;
  lefiCorrectionResistance* r;
  int maxr = this->numResistances_;
 
  for (i = 0; i < maxr; i++) {
    r = this->resistances_[i];
    r->lefiCorrectionResistance::Destroy();
    lefFree((char*)r);
  }
 
  this->edge_ = 0;
  this->numResistances_ = 0;
}
 
 
void lefiCorrectionEdge::Destroy() {
  this->lefiCorrectionEdge::clear();
  lefFree((char*)(this->resistances_));
}
 
 
lefiCorrectionEdge::~lefiCorrectionEdge() {
  this->lefiCorrectionEdge::Destroy();
}
 
 
void lefiCorrectionEdge::addEdge(double d) {
  this->edge_ = d;
}
 
 
void lefiCorrectionEdge::addResistanceNumber(double d) {
  lefiCorrectionResistance* r = this->resistances_[this->numResistances_-1];
  r->lefiCorrectionResistance::addResistanceNumber(d);
}
 
 
void lefiCorrectionEdge::addResistance() {
  lefiCorrectionResistance* r;
  if (this->numResistances_ == this->resistancesAllocated_) {
    int max;
    lefiCorrectionResistance** ne;
    int i;

    if (this->resistancesAllocated_ == 0) {
      max = this->resistancesAllocated_ = 2;
      this->numResistances_ = 0;
    } else
      max = this->resistancesAllocated_ = this->numResistances_ * 2;
    ne = (lefiCorrectionResistance**)lefMalloc
           (sizeof(lefiCorrectionResistance*)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->resistances_[i];
    lefFree((char*)(this->resistances_));
    this->resistances_ = ne;
  }
  r = (lefiCorrectionResistance*)lefMalloc(sizeof(lefiCorrectionResistance));
  r->lefiCorrectionResistance::Init();
  this->resistances_[this->numResistances_] = r;
  this->numResistances_ += 1;
}
 
 
void lefiCorrectionEdge::addVictimCorrection(double d) {
  lefiCorrectionResistance* r = this->resistances_[this->numResistances_-1];
  r->lefiCorrectionResistance::addVictimCorrection(d);
}
 
 
void lefiCorrectionEdge::addVictimLength(double d) {
  lefiCorrectionResistance* r = this->resistances_[this->numResistances_-1];
  r->lefiCorrectionResistance::addVictimLength(d);
}
 
 
int lefiCorrectionEdge::numResistances() {
  return this->numResistances_;
}
 
 
lefiCorrectionResistance* lefiCorrectionEdge::resistance(int index) {
  return this->resistances_[index];
}
 
 
double lefiCorrectionEdge::edge() {
  return this->edge_;
}
 

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//
//          lefiCorrectionTable
//
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////


lefiCorrectionTable::lefiCorrectionTable() {
  this->lefiCorrectionTable::Init();
}


void lefiCorrectionTable::Init() {
  this->numEdges_ = 0;
  this->edgesAllocated_ = 2;
  this->edges_ = (lefiCorrectionEdge**)lefMalloc(sizeof(lefiCorrectionEdge*)*2);
}


void lefiCorrectionTable::clear() {
  int i;
  lefiCorrectionEdge* r;
  int max = this->numEdges_;

  for (i = 0; i < max; i++) {
    r = this->edges_[i];
    r->lefiCorrectionEdge::Destroy();
    lefFree((char*)r);
  }
  this->numEdges_ = 0;
}


void lefiCorrectionTable::Destroy() {
  this->lefiCorrectionTable::clear();
  lefFree((char*)(this->edges_));
}


lefiCorrectionTable::~lefiCorrectionTable() {
  this->lefiCorrectionTable::Destroy();
}


void lefiCorrectionTable::setup(int i) {
  this->num_ = i;
  this->lefiCorrectionTable::clear();
}


void lefiCorrectionTable::newEdge() {
  lefiCorrectionEdge* r;
  if (this->numEdges_ == this->edgesAllocated_) {
    int max;
    lefiCorrectionEdge** ne;
    int i;

    if (this->edgesAllocated_ == 0) {
      max = this->edgesAllocated_ = 2;
      this->numEdges_ = 0;
    } else
      max = this->edgesAllocated_ = this->numEdges_ * 2;
    ne = (lefiCorrectionEdge**)lefMalloc (sizeof(lefiCorrectionEdge*)*max);
    max /= 2;
    for (i = 0; i < max; i++)
      ne[i] = this->edges_[i];
    lefFree((char*)(this->edges_));
    this->edges_ = ne;
  }
  r = (lefiCorrectionEdge*)lefMalloc(sizeof(lefiCorrectionEdge));
  r->lefiCorrectionEdge::Init();
  this->edges_[this->numEdges_] = r;
  this->numEdges_ += 1;
}


void lefiCorrectionTable::addEdge(double d) {
  lefiCorrectionEdge* r = this->edges_[this->numEdges_-1];
  r->lefiCorrectionEdge::addEdge(d);
}


void lefiCorrectionTable::addResistanceNumber(double d) {
  lefiCorrectionEdge* r = this->edges_[this->numEdges_-1];
  r->lefiCorrectionEdge::addResistanceNumber(d);
}


void lefiCorrectionTable::addResistance() {
  lefiCorrectionEdge* r = this->edges_[this->numEdges_-1];
  r->lefiCorrectionEdge::addResistance();
}


void lefiCorrectionTable::addVictimLength(double d) {
  lefiCorrectionEdge* r = this->edges_[this->numEdges_ - 1];
  r->lefiCorrectionEdge::addVictimLength(d);
}


void lefiCorrectionTable::addVictimCorrection(double d) {
  lefiCorrectionEdge* r = this->edges_[this->numEdges_ - 1];
  r->lefiCorrectionEdge::addVictimCorrection(d);
}


int lefiCorrectionTable::num() {
  return this->num_;
}


int lefiCorrectionTable::numEdges() {
  return this->numEdges_;
}


lefiCorrectionEdge* lefiCorrectionTable::edge(int index) {
  return this->edges_[index];
}

} // namespace LefParser
