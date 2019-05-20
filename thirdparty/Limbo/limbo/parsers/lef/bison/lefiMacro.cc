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

#include <string.h>
#include <stdlib.h>
//#include "lex.h"
#include <limbo/parsers/lef/bison/lefiMacro.hpp>
#include <limbo/parsers/lef/bison/lefiMisc.hpp>
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
#include <limbo/parsers/lef/bison/lefiUtil.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {

////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiObstruction
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiObstruction::lefiObstruction() {
  this->lefiObstruction::Init();
}


void lefiObstruction::Init() {
  this->geometries_ = 0;
}


lefiObstruction::~lefiObstruction() {
  this->lefiObstruction::Destroy();
}


void lefiObstruction::Destroy() {
  this->lefiObstruction::clear();
}


void lefiObstruction::clear() {
  if (this->geometries_) {
    this->geometries_->lefiGeometries::Destroy();
    lefFree((char*)(this->geometries_));
   }
  this->geometries_ = 0;
}


void lefiObstruction::setGeometries(lefiGeometries* g) {
  this->lefiObstruction::clear();
  this->geometries_ = g;
}


lefiGeometries* lefiObstruction::geometries() const {
  return this->geometries_;
}

void lefiObstruction::swap(lefiObstruction& rhs) {
    std::swap(this->geometries_, rhs.geometries_);
}


void lefiObstruction::print(FILE* f) const {
  lefiGeometries* g;

  fprintf(f, "  Obstruction\n");

  g = this->geometries_;
  if (g)
      g->lefiGeometries::print(f);

}

////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiDensity
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiDensity::lefiDensity() {
  this->lefiDensity::Init();
}


void lefiDensity::Init() {
  this->numLayers_ = 0;
  this->layersAllocated_ = 0;
  this->layerName_ = 0;
}


lefiDensity::~lefiDensity() {
  this->lefiDensity::Destroy();
}


void lefiDensity::Destroy() {
  this->lefiDensity::clear();
}


void lefiDensity::clear() {
  int i;

  for (i = 0; i < this->numLayers_; i++) {
    lefFree(this->layerName_[i]);
    lefFree((char*)this->rects_[i]);
    lefFree((char*)this->densityValue_[i]);
    this->numRects_[i] = 0;
    this->rectsAllocated_[i] = 0;
  }
  lefFree(this->layerName_);
  this->layerName_ = 0;
  this->numLayers_ = 0;
  this->layersAllocated_ = 0;
}


void lefiDensity::addLayer(const char* name) {
  if (this->numLayers_ == this->layersAllocated_) {
    int    i;
    char** ln;                   // layerName
    int*   nr;                   // number of rect within the layer
    int*   ra;                   // number of rect allocated within the layer
    struct lefiGeomRect** rs;    // rect value
    double**       dv;           // density value
    
    this->layersAllocated_ = (this->layersAllocated_ == 0) ?
      2 : this->layersAllocated_ * 2;
    ln = (char**)lefMalloc(sizeof(char*)*this->layersAllocated_);
    nr = (int*)lefMalloc(sizeof(int)*this->layersAllocated_);
    ra = (int*)lefMalloc(sizeof(int)*this->layersAllocated_);
    rs = (struct lefiGeomRect**)lefMalloc(sizeof(struct lefiGeomRect*)
         *this->layersAllocated_);
    dv = (double**)lefMalloc(sizeof(double*)*this->layersAllocated_);
    for (i = 0; i < this->numLayers_; i++) {
      ln[i] = this->layerName_[i];
      nr[i] = this->numRects_[i];
      ra[i] = this->rectsAllocated_[i];
      rs[i] = this->rects_[i];
      dv[i] = this->densityValue_[i]; 
    }
    lefFree((char*)(this->layerName_));
    lefFree((char*)(this->numRects_));
    lefFree((char*)(this->rectsAllocated_));
    lefFree((char*)(this->rects_));
    lefFree((char*)(this->densityValue_));
    this->layerName_ = ln;
    this->numRects_ = nr;
    this->rectsAllocated_ = ra;
    this->rects_ = rs;
    this->densityValue_ = dv;
  }
  this->layerName_[this->numLayers_] = strdup(name);
  this->numRects_[this->numLayers_] = 0;
  this->rectsAllocated_[this->numLayers_] = 0;
  this->numLayers_ += 1;
}


void lefiDensity::addRect(double x1, double y1, double x2, double y2,
                          double value) {
  struct lefiGeomRect *rs, *ors;
  double *dv, *odv;
  int    i;
  struct lefiGeomRect p;

  if (this->numRects_[this->numLayers_-1] ==
    this->rectsAllocated_[this->numLayers_-1]) {

    this->rectsAllocated_[this->numLayers_-1] =
      (this->rectsAllocated_[this->numLayers_-1] == 0) ?
      2 : this->rectsAllocated_[this->numLayers_-1] * 2;
    rs = (struct lefiGeomRect*)lefMalloc(sizeof(struct lefiGeomRect)*
         this->rectsAllocated_[this->numLayers_-1]);
    dv = (double*)lefMalloc(sizeof(double)*
         this->rectsAllocated_[this->numLayers_-1]);
    ors = this->rects_[this->numLayers_-1];
    odv = this->densityValue_[this->numLayers_-1];
    for (i = 0; i < this->numRects_[this->numLayers_-1]; i++) {
      rs[i] = ors[i];    // assign data from old rect & density value to
      dv[i] = odv[i];    // new, larger array
    }
    if (this->numRects_[this->numLayers_-1] > 0) {  // free only if non-0
      lefFree((char*)this->rects_[this->numLayers_-1]);
      lefFree((char*)this->densityValue_[this->numLayers_-1]);
    }
    this->rects_[this->numLayers_-1] = rs;
    this->densityValue_[this->numLayers_-1] = dv;    
  } else {
    rs = this->rects_[this->numLayers_-1];
    dv = this->densityValue_[this->numLayers_-1];
  }
  p.xl = x1;
  p.yl = y1;
  p.xh = x2;
  p.yh = y2;
  rs[this->numRects_[this->numLayers_-1]] = p;
  dv[this->numRects_[this->numLayers_-1]] = value;
  this->numRects_[this->numLayers_-1] += 1;
}


int lefiDensity::numLayer() const {
  return this->numLayers_;
}


char* lefiDensity::layerName(int index) const {
  return this->layerName_[index];
}


int lefiDensity::numRects(int index) const {
  return this->numRects_[index];
}


struct lefiGeomRect lefiDensity::getRect(int index, int rectIndex) const {
  struct lefiGeomRect *rs;

  rs = this->rects_[index];
  return rs[rectIndex];
}


double lefiDensity::densityValue(int index, int rectIndex) const {
  double *dv;

  dv = this->densityValue_[index];
  return dv[rectIndex];
}


void lefiDensity::print(FILE* f) const {
  int i, j;

  // 11/8/2004 - Added feed back from users
  fprintf(f, "  DENSITY\n");
  for (i = 0; i < this->numLayers_; i++) {
    fprintf(f, "    LAYER %s\n", this->layerName_[i]);
    for (j = 0; j < this->numRects_[i]; j++) {
      fprintf(f, "      RECT %g %g %g %g ", this->rects_[i][j].xl,
              this->rects_[i][j].yl, this->rects_[i][j].xh,
              this->rects_[i][j].yh);
      fprintf(f, "%g\n", this->densityValue_[i][j]);
    }
  }
}

////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiPinAntennaModel
//
////////////////////////////////////////////
////////////////////////////////////////////

lefiPinAntennaModel::lefiPinAntennaModel() {
  this->lefiPinAntennaModel::Init();
}

void lefiPinAntennaModel::Init() {
  this->numAntennaGateArea_ = 0;
  this->antennaGateAreaAllocated_ = 1;
  this->antennaGateArea_ = (double*)lefMalloc(sizeof(double));
  this->antennaGateAreaLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaMaxAreaCar_ = 0;
  this->antennaMaxAreaCarAllocated_ = 1;
  this->antennaMaxAreaCar_ = (double*)lefMalloc(sizeof(double));
  this->antennaMaxAreaCarLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaMaxSideAreaCar_ = 0;
  this->antennaMaxSideAreaCarAllocated_ = 1;
  this->antennaMaxSideAreaCar_ = (double*)lefMalloc(sizeof(double));
  this->antennaMaxSideAreaCarLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaMaxCutCar_ = 0;
  this->antennaMaxCutCarAllocated_ = 1;
  this->antennaMaxCutCar_ = (double*)lefMalloc(sizeof(double));
  this->antennaMaxCutCarLayer_ = (char**)lefMalloc(sizeof(char*));

  this->oxide_ = 0;
  this->hasReturn_ = 0;

}

lefiPinAntennaModel::~lefiPinAntennaModel() {
  this->lefiPinAntennaModel::Destroy();
}

void lefiPinAntennaModel::Destroy() {
  this->lefiPinAntennaModel::clear();
}

void lefiPinAntennaModel::clear() {
  int i;

  if (this->oxide_)
    lefFree((char*)(this->oxide_));
  else       // did not declare
    return;
  this->oxide_ = 0;
  this->hasReturn_ = 0;

  for (i = 0; i < this->numAntennaGateArea_; i++) {
    if (this->antennaGateAreaLayer_[i])
       lefFree(this->antennaGateAreaLayer_[i]);
  }
  this->numAntennaGateArea_ = 0;

  for (i = 0; i < this->numAntennaMaxAreaCar_; i++) {
    if (this->antennaMaxAreaCarLayer_[i])
       lefFree(this->antennaMaxAreaCarLayer_[i]);
  }
  this->numAntennaMaxAreaCar_ = 0;

  for (i = 0; i < this->numAntennaMaxSideAreaCar_; i++) {
    if (this->antennaMaxSideAreaCarLayer_[i])
       lefFree(this->antennaMaxSideAreaCarLayer_[i]);
  }
  this->numAntennaMaxSideAreaCar_ = 0;

  for (i = 0; i < this->numAntennaMaxCutCar_; i++) {
    if (this->antennaMaxCutCarLayer_[i])
       lefFree(this->antennaMaxCutCarLayer_[i]);
  }
  this->numAntennaMaxCutCar_ = 0;
  lefFree((char*)(this->antennaGateArea_));
  lefFree((char*)(this->antennaGateAreaLayer_));
  lefFree((char*)(this->antennaMaxAreaCar_));
  lefFree((char*)(this->antennaMaxAreaCarLayer_));
  lefFree((char*)(this->antennaMaxSideAreaCar_));
  lefFree((char*)(this->antennaMaxSideAreaCarLayer_));
  lefFree((char*)(this->antennaMaxCutCar_));
  lefFree((char*)(this->antennaMaxCutCarLayer_));

}

// 5.5
void lefiPinAntennaModel::setAntennaModel(int aOxide) {
  switch (aOxide) {
     case 1:
           this->oxide_ = strdup("OXIDE1");
           break;
     case 2:
           this->oxide_ = strdup("OXIDE2");
           break;
     case 3:
           this->oxide_ = strdup("OXIDE3");
           break;
     case 4:
           this->oxide_ = strdup("OXIDE4");
           break;
     default:
           this->oxide_ = NULL;
           break;
  }
}

void lefiPinAntennaModel::addAntennaGateArea(double val, const char* layer) {
  int len;
  if (this->numAntennaGateArea_ == this->antennaGateAreaAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaGateArea_;
    double* nd;
    char** nl;

   
    if (this->antennaGateAreaAllocated_ == 0)
      max = this->antennaGateAreaAllocated_ = 2;
    else
      max = this->antennaGateAreaAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaGateAreaLayer_[i];
      nd[i] = this->antennaGateArea_[i];
    }
    lefFree((char*)(this->antennaGateAreaLayer_));
    lefFree((char*)(this->antennaGateArea_));
    this->antennaGateAreaLayer_ = nl;
    this->antennaGateArea_ = nd;
  }
  this->antennaGateArea_[this->numAntennaGateArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaGateAreaLayer_[this->numAntennaGateArea_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaGateAreaLayer_[this->numAntennaGateArea_],
       layer);
  } else
    this->antennaGateAreaLayer_[this->numAntennaGateArea_] = NULL;
  this->numAntennaGateArea_ += 1;
}

void lefiPinAntennaModel::addAntennaMaxAreaCar(double val, const char* layer) {
  int len;
  if (this->numAntennaMaxAreaCar_ == this->antennaMaxAreaCarAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMaxAreaCar_;
    double* nd;
    char** nl;

    if (this->antennaMaxAreaCarAllocated_ == 0)
      max = this->antennaMaxAreaCarAllocated_ = 2;
    else
      max = this->antennaMaxAreaCarAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMaxAreaCarLayer_[i];
      nd[i] = this->antennaMaxAreaCar_[i];
    }
    lefFree((char*)(this->antennaMaxAreaCarLayer_));
    lefFree((char*)(this->antennaMaxAreaCar_));
    this->antennaMaxAreaCarLayer_ = nl;
    this->antennaMaxAreaCar_ = nd;
  }
  this->antennaMaxAreaCar_[this->numAntennaMaxAreaCar_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaMaxAreaCarLayer_[this->numAntennaMaxAreaCar_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaMaxAreaCarLayer_[this->numAntennaMaxAreaCar_],
       layer);
  } else
    this->antennaMaxAreaCarLayer_[this->numAntennaMaxAreaCar_] = NULL;
  this->numAntennaMaxAreaCar_ += 1;
}

void lefiPinAntennaModel::addAntennaMaxSideAreaCar(double val, const char* layer) {
  int len;
  if (this->numAntennaMaxSideAreaCar_ == this->antennaMaxSideAreaCarAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMaxSideAreaCar_;
    double* nd;
    char** nl;

    if (this->antennaMaxSideAreaCarAllocated_ == 0)
      max = this->antennaMaxSideAreaCarAllocated_ = 2;
    else
      max = this->antennaMaxSideAreaCarAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMaxSideAreaCarLayer_[i];
      nd[i] = this->antennaMaxSideAreaCar_[i];
    }
    lefFree((char*)(this->antennaMaxSideAreaCarLayer_));
    lefFree((char*)(this->antennaMaxSideAreaCar_));
    this->antennaMaxSideAreaCarLayer_ = nl;
    this->antennaMaxSideAreaCar_ = nd;
  }
  this->antennaMaxSideAreaCar_[this->numAntennaMaxSideAreaCar_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaMaxSideAreaCarLayer_[this->numAntennaMaxSideAreaCar_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaMaxSideAreaCarLayer_[this->numAntennaMaxSideAreaCar_],
       layer);
  } else
    this->antennaMaxSideAreaCarLayer_[this->numAntennaMaxSideAreaCar_] = NULL;
  this->numAntennaMaxSideAreaCar_ += 1;
}

void lefiPinAntennaModel::addAntennaMaxCutCar(double val, const char* layer) {
  int len;
  if (this->numAntennaMaxCutCar_ == this->antennaMaxCutCarAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMaxCutCar_;
    double* nd;
    char** nl;

    if (this->antennaMaxCutCarAllocated_ == 0)
      max = this->antennaMaxCutCarAllocated_ = 2;
    else
      max = this->antennaMaxCutCarAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMaxCutCarLayer_[i];
      nd[i] = this->antennaMaxCutCar_[i];
    }
    lefFree((char*)(this->antennaMaxCutCarLayer_));
    lefFree((char*)(this->antennaMaxCutCar_));
    this->antennaMaxCutCarLayer_ = nl;
    this->antennaMaxCutCar_ = nd;
  }
  this->antennaMaxCutCar_[this->numAntennaMaxCutCar_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaMaxCutCarLayer_[this->numAntennaMaxCutCar_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaMaxCutCarLayer_[this->numAntennaMaxCutCar_],
       layer);
  } else
    this->antennaMaxCutCarLayer_[this->numAntennaMaxCutCar_] = NULL;
  this->numAntennaMaxCutCar_ += 1;
}

void lefiPinAntennaModel::setAntennaReturnFlag(int flag) {
  this->hasReturn_ = flag;
  return;
}

int lefiPinAntennaModel::hasAntennaGateArea() const {
  return this->numAntennaGateArea_ ? 1 : 0;
}

int lefiPinAntennaModel::hasAntennaMaxAreaCar() const {
  return this->numAntennaMaxAreaCar_ ? 1 : 0;
}

int lefiPinAntennaModel::hasAntennaMaxSideAreaCar() const {
  return this->numAntennaMaxSideAreaCar_ ? 1 : 0;
}

int lefiPinAntennaModel::hasAntennaMaxCutCar() const {
  return this->numAntennaMaxCutCar_ ? 1 : 0;
}

// 5.5
char* lefiPinAntennaModel::antennaOxide() const {
  return this->oxide_;
}

const char* lefiPinAntennaModel::antennaGateAreaLayer(int i) const {
  return this->antennaGateAreaLayer_[i];
}

const char* lefiPinAntennaModel::antennaMaxAreaCarLayer(int i) const {
  return this->antennaMaxAreaCarLayer_[i];
}

const char* lefiPinAntennaModel::antennaMaxSideAreaCarLayer(int i) const {
  return this->antennaMaxSideAreaCarLayer_[i];
}

const char* lefiPinAntennaModel::antennaMaxCutCarLayer(int i) const {
  return this->antennaMaxCutCarLayer_[i];
}

int lefiPinAntennaModel::numAntennaGateArea() const {
  return this->numAntennaGateArea_;
}

int lefiPinAntennaModel::numAntennaMaxAreaCar() const {
  return this->numAntennaMaxAreaCar_;
}

int lefiPinAntennaModel::numAntennaMaxSideAreaCar() const {
  return this->numAntennaMaxSideAreaCar_;
}

int lefiPinAntennaModel::numAntennaMaxCutCar() const {
  return this->numAntennaMaxCutCar_;
}

double lefiPinAntennaModel::antennaGateArea(int i) const {
  return this->antennaGateArea_[i];
}

double lefiPinAntennaModel::antennaMaxAreaCar(int i) const {
  return this->antennaMaxAreaCar_[i];
}

double lefiPinAntennaModel::antennaMaxSideAreaCar(int i) const {
  return this->antennaMaxSideAreaCar_[i];
}

double lefiPinAntennaModel::antennaMaxCutCar(int i) const {
  return this->antennaMaxCutCar_[i];
}

int lefiPinAntennaModel::hasReturn() const {
  return this->hasReturn_;
}

////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiPin
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiPin::lefiPin() {
  this->lefiPin::Init();
}
lefiPin::lefiPin(lefiPin const& rhs)
{
	this->lefiPin::copy(rhs);
}


void lefiPin::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);

  this->numForeigns_ = 0;
  this->foreignAllocated_ = 0;
  this->hasForeignOrient_ = 0;
  this->hasForeignPoint_ = 0;
  this->foreignOrient_ = 0;
  this->foreignX_ = 0;
  this->foreignY_ = 0;
  this->foreign_ = 0;

  this->portsAllocated_ = 2;
  this->ports_ = (lefiGeometries**)lefMalloc(sizeof(lefiGeometries*)*2);
  this->numPorts_ = 0;
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  this->propNames_ = 0;
  this->propValues_ = 0;
  this->propNums_ = 0;
  this->propTypes_ = 0;
  this->LEQ_ = 0;
  this->mustjoin_ = 0;
  this->lowTable_ = 0;
  this->highTable_ = 0;
  this->taperRule_ = 0;
  this->antennaModel_ = 0;
  this->numAntennaModel_ = 0;
  this->antennaModelAllocated_ = 0;
  this->netEpxr_ = 0;
  this->ssPinName_ = 0;
  this->gsPinName_ = 0;

  this->lefiPin::bump(&(this->LEQ_), 16, &(this->LEQSize_));
  this->lefiPin::bump(&(this->mustjoin_), 16, &(this->mustjoinSize_));
  this->lefiPin::bump(&(this->lowTable_), 16, &(this->lowTableSize_));
  this->lefiPin::bump(&(this->highTable_), 16, &(this->highTableSize_));

  this->numAntennaSize_ = 0;
  this->antennaSizeAllocated_ = 1;
  this->antennaSize_ = (double*)lefMalloc(sizeof(double));
  this->antennaSizeLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaMetalArea_ = 0;
  this->antennaMetalAreaAllocated_ = 1;
  this->antennaMetalArea_ = (double*)lefMalloc(sizeof(double));
  this->antennaMetalAreaLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaMetalLength_ = 0;
  this->antennaMetalLengthAllocated_ = 1;
  this->antennaMetalLength_ = (double*)lefMalloc(sizeof(double));
  this->antennaMetalLengthLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaPartialMetalArea_ = 0;
  this->antennaPartialMetalAreaAllocated_ = 1;
  this->antennaPartialMetalArea_ = (double*)lefMalloc(sizeof(double));
  this->antennaPartialMetalAreaLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaPartialMetalSideArea_ = 0;
  this->antennaPartialMetalSideAreaAllocated_ = 1;
  this->antennaPartialMetalSideArea_ = (double*)lefMalloc(sizeof(double));
  this->antennaPartialMetalSideAreaLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaPartialCutArea_ = 0;
  this->antennaPartialCutAreaAllocated_ = 1;
  this->antennaPartialCutArea_ = (double*)lefMalloc(sizeof(double));
  this->antennaPartialCutAreaLayer_ = (char**)lefMalloc(sizeof(char*));

  this->numAntennaDiffArea_ = 0;
  this->antennaDiffAreaAllocated_ = 1;
  this->antennaDiffArea_ = (double*)lefMalloc(sizeof(double));
  this->antennaDiffAreaLayer_ = (char**)lefMalloc(sizeof(char*));
}
void lefiPin::copy(lefiPin const& rhs)
{
	this->Init();
	// nameSize_ 
	// name_
	this->setName(rhs.name());

	this->hasLEQ_ = rhs.hasLEQ_;
	this->hasDirection_ = rhs.hasDirection_;
	this->hasUse_ = rhs.hasUse_;
	this->hasShape_ = rhs.hasShape_;
	this->hasMustjoin_ = rhs.hasMustjoin_;
	this->hasOutMargin_ = rhs.hasOutMargin_;
	this->hasOutResistance_ = rhs.hasOutResistance_;
	this->hasInMargin_ = rhs.hasInMargin_;
	this->hasPower_ = rhs.hasPower_;
	this->hasLeakage_ = rhs.hasLeakage_;
	this->hasMaxload_ = rhs.hasMaxload_;
	this->hasMaxdelay_ = rhs.hasMaxdelay_;
	this->hasCapacitance_ = rhs.hasCapacitance_;
	this->hasResistance_ = rhs.hasResistance_;
	this->hasPulldownres_ = rhs.hasPulldownres_;
	this->hasTieoffr_ = rhs.hasTieoffr_;
	this->hasVHI_ = rhs.hasVHI_; 
	this->hasVLO_ = rhs.hasVLO_;
	this->hasRiseVoltage_ = rhs.hasRiseVoltage_;
	this->hasFallVoltage_ = rhs.hasFallVoltage_;
	this->hasRiseThresh_ = rhs.hasRiseThresh_;
	this->hasFallThresh_ = rhs.hasFallThresh_;
	this->hasRiseSatcur_ = rhs.hasRiseSatcur_;
	this->hasFallSatcur_ = rhs.hasFallSatcur_;
	this->hasCurrentSource_ = rhs.hasCurrentSource_;
	this->hasTables_ = rhs.hasTables_;
	this->hasAntennasize_ = rhs.hasAntennasize_;
	this->hasRiseSlewLimit_ = rhs.hasRiseSlewLimit_;
	this->hasFallSlewLimit_ = rhs.hasFallSlewLimit_;

	// foreign_, foreignX_, foreignY_, foreignOrient_
	// hasForeignPoint_, hasForeignOrient_, numForeigns_, foreignAllocated_
	if (rhs.hasForeign())
	{
		for (int i = 0; i < rhs.numForeigns_; ++i)
		{
			this->addForeign(rhs.foreignName(i), rhs.hasForeignPoint(i), 
					rhs.foreignX(i), rhs.foreignY(i), rhs.foreignOrient(i));
		}
	}
	// hasLEQ_, LEQSize_, LEQ_ 
	if (rhs.hasLEQ()) this->setLEQ(rhs.LEQ());
	// hasMustjoin_, mustjoinSize_, mustjoin_
	if (rhs.hasMustjoin()) this->setMustjoin(rhs.mustjoin());
	// hasOutMargin_, outMarginH_, outMarginL_
	if (rhs.hasOutMargin()) this->setOutMargin(rhs.outMarginHigh(), rhs.outMarginLow());
	// hasOutResistance_, outResistanceH_, outResistanceL_
	if (rhs.hasOutResistance()) this->setOutResistance(rhs.outResistanceHigh(), rhs.outResistanceLow());
	if (rhs.hasInMargin()) this->setInMargin(rhs.inMarginHigh(), rhs.inMarginLow());
	if (rhs.hasPower()) this->setPower(rhs.power());
	if (rhs.hasLeakage()) this->setLeakage(rhs.leakage());
	if (rhs.hasMaxload()) this->setMaxload(rhs.maxload());
	if (rhs.hasMaxdelay()) this->setMaxdelay(rhs.maxdelay());
	if (rhs.hasCapacitance()) this->setCapacitance(rhs.capacitance());
	if (rhs.hasResistance()) this->setResistance(rhs.resistance());
	if (rhs.hasPulldownres()) this->setPulldownres(rhs.pulldownres());
	if (rhs.hasTieoffr()) this->setTieoffr(rhs.tieoffr());
	if (rhs.hasVHI()) this->setVHI(rhs.VHI());
	if (rhs.hasVLO()) this->setVLO(rhs.VLO());
	if (rhs.hasRiseVoltage()) this->setRiseVoltage(rhs.riseVoltage());
	if (rhs.hasFallVoltage()) this->setFallVoltage(rhs.fallVoltage());
	if (rhs.hasRiseThresh()) this->setRiseThresh(rhs.riseThresh());
	if (rhs.hasFallThresh()) this->setFallThresh(rhs.fallThresh());
	if (rhs.hasRiseSatcur()) this->setRiseSatcur(rhs.riseSatcur());
	if (rhs.hasFallSatcur()) this->setFallSatcur(rhs.fallSatcur());
	if (rhs.hasTables()) this->setTables(rhs.tableHighName(), rhs.tableLowName());
	if (rhs.hasRiseSlewLimit()) this->setRiseSlewLimit(rhs.riseSlewLimit());
	if (rhs.hasFallSlewLimit()) this->setFallSlewLimit(rhs.fallSlewLimit());

	// 5.5 AntennaModel
//	if (rhs.hasAntennaModel()) 
//	{
		for (int i = 0; i < rhs.numAntennaModel(); ++i)
		{
			if (strcmp(rhs.antennaModel(i)->antennaOxide(), "OXIDE1") == 0)
				this->addAntennaModel(1);
			else if (strcmp(rhs.antennaModel(i)->antennaOxide(), "OXIDE2") == 0)
				this->addAntennaModel(2);
			else if (strcmp(rhs.antennaModel(i)->antennaOxide(), "OXIDE3") == 0)
				this->addAntennaModel(3);
			else if (strcmp(rhs.antennaModel(i)->antennaOxide(), "OXIDE4") == 0)
				this->addAntennaModel(4);
			else // something very wrong, normally this can't happen
				lefiError("ERROR (LEFPARS-1351): There is an unexpected lef parser bug which cause it unable to retrieve ANTENNAMODEL data with the given index.");
		}
		this->curAntennaModelIndex_ = rhs.curAntennaModelIndex_;
//	}
	if (rhs.hasAntennaSize())
	{
		for (int i = 0; i < rhs.numAntennaSize(); ++i)
			this->addAntennaSize(rhs.antennaSize(i), rhs.antennaSizeLayer(i));
	}
	if (rhs.hasAntennaMetalArea())
	{
		for (int i = 0; i < rhs.numAntennaMetalArea(); ++i)
			this->addAntennaMetalArea(rhs.antennaMetalArea(i), antennaMetalAreaLayer(i));
	}
	if (rhs.hasAntennaMetalLength())
	{
		for (int i = 0; i < rhs.numAntennaMetalLength(); ++i)
			this->addAntennaMetalLength(rhs.antennaMetalLength(i), antennaMetalAreaLayer(i));
	}
	if (rhs.hasAntennaPartialMetalArea())
	{
		for (int i = 0; i < rhs.numAntennaPartialMetalArea(); ++i)
			this->addAntennaPartialMetalArea(rhs.antennaPartialMetalArea(i), rhs.antennaPartialMetalAreaLayer(i));
	}
	if (rhs.hasAntennaPartialMetalSideArea())
	{
		for (int i = 0; i < rhs.numAntennaPartialMetalSideArea(); ++i)
			this->addAntennaPartialMetalSideArea(rhs.antennaPartialMetalSideArea(i), rhs.antennaPartialMetalSideAreaLayer(i));
	}
	if (rhs.hasAntennaPartialCutArea())
	{
		for (int i = 0; i < rhs.numAntennaPartialCutArea(); ++i)
			this->addAntennaPartialCutArea(rhs.antennaPartialCutArea(i), rhs.antennaPartialCutAreaLayer(i));
	}
	if (rhs.hasAntennaDiffArea())
	{
		for (int i = 0; i < rhs.numAntennaDiffArea(); ++i)
			this->addAntennaDiffArea(rhs.antennaDiffArea(i), rhs.antennaDiffAreaLayer(i));
	}
	if (rhs.hasTaperRule()) this->setTaperRule(rhs.taperRule());
	if (rhs.hasNetExpr()) this->setNetExpr(rhs.netExpr());
	if (rhs.hasSupplySensitivity()) this->setSupplySensitivity(rhs.supplySensitivity());
	if (rhs.hasGroundSensitivity()) this->setGroundSensitivity(rhs.groundSensitivity());
	if (rhs.hasDirection()) this->setDirection(rhs.direction());
	if (rhs.hasUse()) this->setUse(rhs.use());
	if (rhs.hasShape()) this->setShape(rhs.shape());
	if (rhs.hasCurrentSource()) this->setCurrentSource(rhs.currentSource());
	// setNumProperty is actually generalized version of setProperty
	for (int i = 0; i < rhs.numProperties(); ++i)
		this->setNumProperty(rhs.propName(i), rhs.propNum(i), rhs.propValue(i), rhs.propType(i));

	for (int i = 0; i < rhs.numPorts(); ++i)
    {
        lefiGeometries* g = (lefiGeometries*)lefMalloc(sizeof(lefiGeometries));
        g->Init();
        g->copy(*rhs.port(i));
		this->addPort(g);
    }
}


lefiPin::~lefiPin() {
  this->lefiPin::Destroy();
}


void lefiPin::Destroy() {

  this->lefiPin::clear();
  lefFree(this->name_);
  lefFree((char*)(this->ports_));
  lefFree(this->LEQ_);
  lefFree(this->mustjoin_);
  lefFree(this->lowTable_);
  lefFree(this->highTable_);
  if (this->propNames_)
     lefFree((char*)(this->propNames_));
  this->propNames_ = 0;
  if (this->propValues_)
     lefFree((char*)(this->propValues_));
  this->propValues_ = 0;
  if (this->propNums_)
     lefFree((char*)(this->propNums_));
  this->propNums_ = 0;
  if (this->propTypes_)
     lefFree((char*)(this->propTypes_));
  this->propTypes_ = 0;
  lefFree((char*)(this->antennaSize_));
  lefFree((char*)(this->antennaSizeLayer_));
  lefFree((char*)(this->antennaMetalArea_));
  lefFree((char*)(this->antennaMetalAreaLayer_));
  lefFree((char*)(this->antennaMetalLength_));
  lefFree((char*)(this->antennaMetalLengthLayer_));
  lefFree((char*)(this->antennaPartialMetalArea_));
  lefFree((char*)(this->antennaPartialMetalAreaLayer_));
  lefFree((char*)(this->antennaPartialMetalSideArea_));
  lefFree((char*)(this->antennaPartialMetalSideAreaLayer_));
  lefFree((char*)(this->antennaPartialCutArea_));
  lefFree((char*)(this->antennaPartialCutAreaLayer_));
  lefFree((char*)(this->antennaDiffArea_));
  lefFree((char*)(this->antennaDiffAreaLayer_));
  if (this->foreignAllocated_) {
     lefFree((char*)(this->hasForeignOrient_));
     lefFree((char*)(this->hasForeignPoint_));
     lefFree((char*)(this->foreignOrient_));
     lefFree((char*)(this->foreignX_));
     lefFree((char*)(this->foreignY_));
     lefFree((char*)(this->foreign_));
     this->foreignAllocated_ = 0;
  }
}


void lefiPin::clear() {
  int i;
  lefiGeometries* g;
  lefiPinAntennaModel* aModel;

  for (i = 0; i < this->numPorts_; i++) {
    g = this->ports_[i];
    g->lefiGeometries::Destroy();
    lefFree((char*)g);
  }
  this->numPorts_ = 0;
  this->portsAllocated_ = 0;
  
  this->hasLEQ_ = 0;
  this->hasDirection_ = 0;
  this->hasUse_ = 0;
  this->hasShape_ = 0;
  this->hasMustjoin_ = 0;
  this->hasOutMargin_ = 0;
  this->hasOutResistance_ = 0;
  this->hasInMargin_ = 0;
  this->hasPower_ = 0;
  this->hasLeakage_ = 0;
  this->hasMaxload_ = 0;
  this->hasMaxdelay_ = 0;
  this->hasCapacitance_ = 0;
  this->hasResistance_ = 0;
  this->hasPulldownres_ = 0;
  this->hasTieoffr_ = 0;
  this->hasVHI_ = 0;
  this->hasVLO_ = 0;
  this->hasRiseVoltage_ = 0;
  this->hasFallVoltage_ = 0;
  this->hasRiseThresh_ = 0;
  this->hasFallThresh_ = 0;
  this->hasRiseSatcur_ = 0;
  this->hasFallSatcur_ = 0;
  this->hasCurrentSource_ = 0;
  this->hasRiseSlewLimit_ = 0;
  this->hasFallSlewLimit_ = 0;
  this->hasTables_ = 0;
  strcpy(this->use_, "SIGNAL");

  for (i = 0; i < this->numForeigns_; i++) {
    this->hasForeignOrient_[i] = 0;
    this->hasForeignPoint_[i] = 0;
    this->foreignOrient_[i] = -1;
    lefFree((char*)(this->foreign_[i]));
  }
  this->numForeigns_ = 0;

  for (i = 0; i < this->numAntennaSize_; i++) {
    if (this->antennaSizeLayer_[i])
       lefFree(this->antennaSizeLayer_[i]);
  }
  this->numAntennaSize_ = 0;

  for (i = 0; i < this->numAntennaMetalLength_; i++) {
    if (this->antennaMetalLengthLayer_[i])
       lefFree(this->antennaMetalLengthLayer_[i]);
  }
  this->numAntennaMetalLength_ = 0;

  for (i = 0; i < this->numAntennaMetalArea_; i++) {
    if (this->antennaMetalAreaLayer_[i])
       lefFree(this->antennaMetalAreaLayer_[i]);
  }
  this->numAntennaMetalArea_ = 0;

  for (i = 0; i < this->numAntennaPartialMetalArea_; i++) {
    if (this->antennaPartialMetalAreaLayer_[i])
       lefFree(this->antennaPartialMetalAreaLayer_[i]);
  }
  this->numAntennaPartialMetalArea_ = 0;

  for (i = 0; i < this->numAntennaPartialMetalSideArea_; i++) {
    if (this->antennaPartialMetalSideAreaLayer_[i])
       lefFree(this->antennaPartialMetalSideAreaLayer_[i]);
  }
  this->numAntennaPartialMetalSideArea_ = 0;

  for (i = 0; i < this->numAntennaPartialCutArea_; i++) {
    if (this->antennaPartialCutAreaLayer_[i])
       lefFree(this->antennaPartialCutAreaLayer_[i]);
  }
  this->numAntennaPartialCutArea_ = 0;

  for (i = 0; i < this->numAntennaDiffArea_; i++) {
    if (this->antennaDiffAreaLayer_[i])
       lefFree(this->antennaDiffAreaLayer_[i]);
  }
  this->numAntennaDiffArea_ = 0;

  if (this->numAntennaModel_ > 0) {
    for (i = 0; i < 4 ; i++) {        // 5.5
      aModel = this->antennaModel_[i];
      aModel->lefiPinAntennaModel::Destroy();
    }
  }
  for (i = 0; i < this->antennaModelAllocated_; i++) {  // 5.5
    lefFree((char*)this->antennaModel_[i]);
  }
  if (this->antennaModel_)                              // 5.5
     lefFree((char*)this->antennaModel_);
  this->antennaModel_ = 0;             // 5.5
  this->numAntennaModel_ = 0;          // 5.5
  this->curAntennaModelIndex_ = 0;     // 5.5
  this->antennaModelAllocated_ = 0;    // 5.5

  for (i = 0; i < this->numProperties_; i++) {
    lefFree(this->propNames_[i]);
    lefFree(this->propValues_[i]);
  }
  this->numProperties_ = 0;
  this->propertiesAllocated_ = 0;
  if (this->taperRule_) { lefFree(this->taperRule_); this->taperRule_ = 0; }
  if (this->netEpxr_) {
     lefFree(this->netEpxr_);
     this->netEpxr_ = 0;
  }
  if (this->ssPinName_) {
     lefFree(this->ssPinName_);
     this->ssPinName_ = 0;
  }
  if (this->gsPinName_) {
     lefFree(this->gsPinName_);
     this->gsPinName_ = 0;
  }
}


void lefiPin::bump(char** array, int len, int* size) {
  if (*array) lefFree(*array);
  if (len > 0)
     *array = (char*)lefMalloc(len);
  else
     *array = 0;
  *size = len;
}
 

void lefiPin::setName(const char* name) {
  int len = strlen(name) + 1;
  this->lefiPin::clear();
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
}


void lefiPin::addPort(lefiGeometries* g) {
  if (this->numPorts_ == this->portsAllocated_) {
    int i;
    lefiGeometries** ng;
    if (this->portsAllocated_ == 0)
       this->portsAllocated_ = 2;
    else
       this->portsAllocated_ *= 2;
    ng = (lefiGeometries**)lefMalloc(
       sizeof(lefiGeometries*)*this->portsAllocated_);
    for (i = 0; i < this->numPorts_; i++)
      ng[i] = this->ports_[i];
    lefFree((char*)(this->ports_));
    this->ports_ = ng;
  }
  this->ports_[this->numPorts_++] = g;
}


void lefiPin::addForeign(const char* name, int hasPnt,
              double x, double y, int orient) {
  int i;
  int*    hfo;
  int*    hfp;
  int*    fo;
  double* fx;
  double* fy;
  char**  f;

  if (this->foreignAllocated_ == this->numForeigns_) {
     if (this->foreignAllocated_ == 0)
        this->foreignAllocated_ = 16; // since it involves char*, it will
                                      // costly in the number is too small
     else
        this->foreignAllocated_ *= 2;
     hfo = (int*)lefMalloc(sizeof(int)*this->foreignAllocated_);
     hfp = (int*)lefMalloc(sizeof(int)*this->foreignAllocated_);
     fo  = (int*)lefMalloc(sizeof(int)*this->foreignAllocated_);
     fx  = (double*)lefMalloc(sizeof(double)*this->foreignAllocated_);
     fy  = (double*)lefMalloc(sizeof(double)*this->foreignAllocated_);
     f   = (char**)lefMalloc(sizeof(char*)*this->foreignAllocated_);
     if (this->numForeigns_ != 0) {
        for (i = 0; i < this->numForeigns_; i++) {
           hfo[i] = this->hasForeignOrient_[i];
           hfp[i] = this->hasForeignPoint_[i];
           fo[i]  = this->foreignOrient_[i];
           fx[i]  = this->foreignX_[i];
           fy[i]  = this->foreignY_[i];
           f[i]   = this->foreign_[i];
        }
        lefFree((char*)(this->hasForeignOrient_));
        lefFree((char*)(this->hasForeignPoint_));
        lefFree((char*)(this->foreignOrient_));
        lefFree((char*)(this->foreignX_));
        lefFree((char*)(this->foreignY_));
        lefFree((char*)(this->foreign_));
     }
     this->hasForeignOrient_ = hfo;
     this->hasForeignPoint_ = hfp;
     this->foreignOrient_ = fo;
     this->foreignX_ = fx;
     this->foreignY_ = fy;
     this->foreign_ = f;
  }

 
  // orient=-1 means no orient was specified.
  if (orient != -1)
    this->hasForeignOrient_[this->numForeigns_] = 1; 
  else
    this->hasForeignOrient_[this->numForeigns_] = -1; 

  this->hasForeignPoint_[this->numForeigns_] = hasPnt;
  this->foreignOrient_[this->numForeigns_] = orient;
  this->foreignX_[this->numForeigns_] = x;
  this->foreignY_[this->numForeigns_] = y;
  this->foreign_[this->numForeigns_] = (char*)lefMalloc(strlen(name)+1);
  strcpy(this->foreign_[this->numForeigns_], CASE(name));
  this->numForeigns_ += 1;
}


void lefiPin::setLEQ(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->LEQSize_)
    this->lefiPin::bump(&(this->LEQ_), len, &(this->LEQSize_));
  strcpy(this->LEQ_, CASE(name));
  this->hasLEQ_ = 1;
}


void lefiPin::setDirection(const char* name) {
  strcpy(this->direction_, CASE(name));
  this->hasDirection_ = 1;
}


void lefiPin::setUse(const char* name) {
  strcpy(this->use_, CASE(name));
  this->hasUse_ = 1;
}


void lefiPin::setShape(const char* name) {
  strcpy(this->shape_, CASE(name));
  this->hasShape_ = 1;
}


void lefiPin::setMustjoin(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->mustjoinSize_)
    this->lefiPin::bump(&(this->mustjoin_), len, &(this->mustjoinSize_));
  strcpy(this->mustjoin_, CASE(name));
  this->hasMustjoin_ = 1;
}


void lefiPin::setOutMargin(double high, double low) {
  this->outMarginH_ = high;
  this->outMarginL_ = low;
  this->hasOutMargin_ = 1;
}


void lefiPin::setOutResistance(double high, double low) {
  this->outResistanceH_ = high;
  this->outResistanceL_ = low;
  this->hasOutResistance_ = 1;
}


void lefiPin::setInMargin(double high, double low) {
  this->inMarginH_ = high;
  this->inMarginL_ = low;
  this->hasInMargin_ = 1;
}


void lefiPin::setPower(double power) {
  this->power_ = power;
  this->hasPower_ = 1;
}


void lefiPin::setLeakage(double current) {
  this->leakage_ = current;
  this->hasLeakage_ = 1;
}


void lefiPin::setMaxload(double capacitance) {
  this->maxload_ = capacitance;
  this->hasMaxload_ = 1;
}


void lefiPin::setMaxdelay(double dtime) {
  this->maxdelay_ = dtime;
  this->hasMaxdelay_ = 1;
}


void lefiPin::setCapacitance(double capacitance) {
  this->capacitance_ = capacitance;
  this->hasCapacitance_ = 1;
}


void lefiPin::setResistance(double resistance) {
  this->resistance_ = resistance;
  this->hasResistance_ = 1;
}


void lefiPin::setPulldownres(double resistance) {
  this->pulldownres_ = resistance;
  this->hasPulldownres_ = 1;
}


void lefiPin::setTieoffr(double resistance) {
  this->tieoffr_ = resistance;
  this->hasTieoffr_ = 1;
}


void lefiPin::setVHI(double voltage) {
  this->VHI_ = voltage;
  this->hasVHI_ = 1;
}


void lefiPin::setVLO(double voltage) {
  this->VLO_ = voltage;
  this->hasVLO_ = 1;
}


void lefiPin::setRiseVoltage(double voltage) {
  this->riseVoltage_ = voltage;
  this->hasRiseVoltage_ = 1;
}


void lefiPin::setFallVoltage(double voltage) {
  this->fallVoltage_ = voltage;
  this->hasFallVoltage_ = 1;
}


void lefiPin::setFallSlewLimit(double num) {
  this->fallSlewLimit_ = num;
  this->hasFallSlewLimit_ = 1;
}


void lefiPin::setRiseSlewLimit(double num) {
  this->riseSlewLimit_ = num;
  this->hasRiseSlewLimit_ = 1;
}


void lefiPin::setRiseThresh(double capacitance) {
  this->riseThresh_ = capacitance;
  this->hasRiseThresh_ = 1;
}


void lefiPin::setTaperRule(const char* name) {
  int len = strlen(name) + 1;
  this->taperRule_ = (char*)lefMalloc(len);
  strcpy(this->taperRule_, name);
}


void lefiPin::setNetExpr(const char* name) {
  this->netEpxr_ = strdup(name);
}


void lefiPin::setSupplySensitivity(const char* pinName) {
  this->ssPinName_ = strdup(pinName);
}


void lefiPin::setGroundSensitivity(const char* pinName) {
  this->gsPinName_ = strdup(pinName);
}


void lefiPin::setFallThresh(double capacitance) {
  this->fallThresh_ = capacitance;
  this->hasFallThresh_ = 1;
}


void lefiPin::setRiseSatcur(double current) {
  this->riseSatcur_ = current;
  this->hasRiseSatcur_ = 1;
}


void lefiPin::setFallSatcur(double current) {
  this->fallSatcur_ = current;
  this->hasFallSatcur_ = 1;
}


void lefiPin::setCurrentSource(const char* name) {
  strcpy(this->currentSource_, CASE(name));
  this->hasCurrentSource_ = 1;
}


void lefiPin::setTables(const char* highName, const char* lowName) {
  int len = strlen(highName) + 1;
  if (len > this->highTableSize_)
    this->lefiPin::bump(&(this->highTable_), len, &(this->highTableSize_));
  strcpy(this->highTable_, CASE(highName));
  len = strlen(lowName) + 1;
  if (len > this->lowTableSize_)
    this->lefiPin::bump(&(this->lowTable_), len, &(this->lowTableSize_));
  strcpy(this->lowTable_, CASE(lowName));
  this->hasTables_ = 1;
}


void lefiPin::setProperty(const char* name, const char* value,
                          const char type) {
  int len;
  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiPin::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->propNums_[this->numProperties_] = 0;
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}


void lefiPin::setNumProperty(const char* name, double d,
                             const char* value, const char type) {
  int len;
 
  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiPin::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->propNums_[this->numProperties_] = d;
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}


void lefiPin::bumpProps() {
  int lim = this->propertiesAllocated_;
  int news ;
  char** newpn;
  char** newpv;
  double* newd;
  char*   newt;

  news = lim ? lim + lim : 2;

  newpn = (char**)lefMalloc(sizeof(char*)*news);
  newpv = (char**)lefMalloc(sizeof(char*)*news);
  newd = (double*)lefMalloc(sizeof(double)*news);
  newt = (char*)lefMalloc(sizeof(char)*news);
 
  lim = this->propertiesAllocated_ = news;
 
  if (lim > 2) {
     int i;
     for (i = 0; i < this->numProperties_; i++) {
       newpn[i] = this->propNames_[i];
       newpv[i] = this->propValues_[i];
       newd[i] = this->propNums_[i];
       newt[i] = this->propTypes_[i];
     }
  }
  if (this->propNames_)
     lefFree((char*)(this->propNames_));
  if (this->propValues_)
     lefFree((char*)(this->propValues_));
  if (this->propNums_)
     lefFree((char*)(this->propNums_));
  if (this->propTypes_)
     lefFree((char*)(this->propTypes_));
  this->propNames_ = newpn;
  this->propValues_ = newpv;
  this->propNums_ = newd;
  this->propTypes_ = newt;
}
 

int lefiPin::hasForeign() const {
  return (this->numForeigns_) ? 1 : 0 ;
}


int lefiPin::hasForeignOrient(int index) const {
  return (this->hasForeignOrient_[index] == -1) ? 0 : 1 ;
}


int lefiPin::hasForeignPoint(int index) const {
  return this->hasForeignPoint_[index];
}


int lefiPin::hasLEQ() const {
  return this->hasLEQ_;
}


int lefiPin::hasDirection() const {
  return this->hasDirection_;
}


int lefiPin::hasUse() const {
  return this->hasUse_;
}


int lefiPin::hasShape() const {
  return this->hasShape_;
}


int lefiPin::hasMustjoin() const {
  return this->hasMustjoin_;
}


int lefiPin::hasOutMargin() const {
  return this->hasOutMargin_;
}


int lefiPin::hasOutResistance() const {
  return this->hasOutResistance_;
}


int lefiPin::hasInMargin() const {
  return this->hasInMargin_;
}


int lefiPin::hasPower() const {
  return this->hasPower_;
}


int lefiPin::hasLeakage() const {
  return this->hasLeakage_;
}


int lefiPin::hasMaxload() const {
  return this->hasMaxload_;
}


int lefiPin::hasMaxdelay() const {
  return this->hasMaxdelay_;
}


int lefiPin::hasCapacitance() const {
  return this->hasCapacitance_;
}


int lefiPin::hasResistance() const {
  return this->hasResistance_;
}


int lefiPin::hasPulldownres() const {
  return this->hasPulldownres_;
}


int lefiPin::hasTieoffr() const {
  return this->hasTieoffr_;
}


int lefiPin::hasVHI() const {
  return this->hasVHI_;
}


int lefiPin::hasVLO() const {
  return this->hasVLO_;
}


int lefiPin::hasFallSlewLimit() const {
  return this->hasFallSlewLimit_;
}


int lefiPin::hasRiseSlewLimit() const {
  return this->hasRiseSlewLimit_;
}


int lefiPin::hasRiseVoltage() const {
  return this->hasRiseVoltage_;
}


int lefiPin::hasFallVoltage() const {
  return this->hasFallVoltage_;
}


int lefiPin::hasRiseThresh() const {
  return this->hasRiseThresh_;
}


int lefiPin::hasFallThresh() const {
  return this->hasFallThresh_;
}


int lefiPin::hasRiseSatcur() const {
  return this->hasRiseSatcur_;
}


int lefiPin::hasFallSatcur() const {
  return this->hasFallSatcur_;
}


int lefiPin::hasCurrentSource() const {
  return this->hasCurrentSource_;
}


int lefiPin::hasTables() const {
  return this->hasTables_;
}


int lefiPin::hasAntennaSize() const {
  return this->numAntennaSize_ ? 1 : 0;
}


int lefiPin::hasAntennaMetalLength() const {
  return this->numAntennaMetalLength_ ? 1 : 0;
}


int lefiPin::hasAntennaMetalArea() const {
  return this->numAntennaMetalArea_ ? 1 : 0;
}


int lefiPin::hasAntennaPartialMetalArea() const {
  return this->numAntennaPartialMetalArea_ ? 1 : 0;
}


int lefiPin::hasAntennaPartialMetalSideArea() const {
  return this->numAntennaPartialMetalSideArea_ ? 1 : 0;
}


int lefiPin::hasAntennaPartialCutArea() const {
  return this->numAntennaPartialCutArea_ ? 1 : 0;
}


int lefiPin::hasAntennaDiffArea() const {
  return this->numAntennaDiffArea_ ? 1 : 0;
}


int lefiPin::hasTaperRule() const {
  return this->taperRule_ ? 1 : 0 ;
}


int lefiPin::hasNetExpr() const {
  return this->netEpxr_ ? 1 : 0;
}


int lefiPin::hasSupplySensitivity() const {
  return this->ssPinName_ ? 1 : 0 ;
}


int lefiPin::hasGroundSensitivity() const {
  return this->gsPinName_ ? 1 : 0 ;
}


const char* lefiPin::name() const {
  return this->name_;
}


const char* lefiPin::taperRule() const {
  return this->taperRule_;
}


const char* lefiPin::netExpr() const {
  return this->netEpxr_;
}


const char* lefiPin::supplySensitivity() const {
  return this->ssPinName_;
}


const char* lefiPin::groundSensitivity() const {
  return this->gsPinName_;
}


int lefiPin::numPorts() const {
  return this->numPorts_;
}


lefiGeometries* lefiPin::port(int index) const {
  char msg[160];
  if (index <0 || index > this->numPorts_) {
    sprintf (msg, "ERROR (EMSPARS-1350): The index number %d given for the macro PIN is invalid.\nValid index is from 0 to %d", index, this->numPorts_);
    lefiError(msg);
    return 0;
  }
  return this->ports_[index];
}


int lefiPin::numForeigns() const {
  return this->numForeigns_;
}


const char* lefiPin::foreignName(int index) const {
  return this->foreign_[index];
}


int lefiPin::foreignOrient(int index) const {
  return this->foreignOrient_[index];
}


const char* lefiPin::foreignOrientStr(int index) const {
  return (lefiOrientStr(this->foreignOrient_[index]));
}


double lefiPin::foreignX(int index) const {
  return this->foreignX_[index];
}


double lefiPin::foreignY(int index) const {
  return this->foreignY_[index];
}


const char* lefiPin::LEQ() const {
  return this->LEQ_;
}


const char* lefiPin::direction() const {
  return this->direction_;
}


const char* lefiPin::use() const {
  return this->use_;
}


const char* lefiPin::shape() const {
  return this->shape_;
}


const char* lefiPin::mustjoin() const {
  return this->mustjoin_;
}


double lefiPin::outMarginHigh() const {
  return this->outMarginH_;
}


double lefiPin::outMarginLow() const {
  return this->outMarginL_;
}


double lefiPin::outResistanceHigh() const {
  return this->outResistanceH_;
}


double lefiPin::outResistanceLow() const {
  return this->outResistanceL_;
}


double lefiPin::inMarginHigh() const {
  return this->inMarginH_;
}


double lefiPin::inMarginLow() const {
  return this->inMarginL_;
}


double lefiPin::power() const {
  return this->power_;
}


double lefiPin::leakage() const {
  return this->leakage_;
}


double lefiPin::maxload() const {
  return this->maxload_;
}


double lefiPin::maxdelay() const {
  return this->maxdelay_;
}


double lefiPin::capacitance() const {
  return this->capacitance_;
}


double lefiPin::resistance() const {
  return this->resistance_;
}


double lefiPin::pulldownres() const {
  return this->pulldownres_;
}


double lefiPin::tieoffr() const {
  return this->tieoffr_;
}


double lefiPin::VHI() const {
  return this->VHI_;
}


double lefiPin::VLO() const {
  return this->VLO_;
}


double lefiPin::fallSlewLimit() const {
  return this->fallSlewLimit_;
}


double lefiPin::riseSlewLimit() const {
  return this->riseSlewLimit_;
}


double lefiPin::riseVoltage() const {
  return this->riseVoltage_;
}


double lefiPin::fallVoltage() const {
  return this->fallVoltage_;
}


double lefiPin::riseThresh() const {
  return this->riseThresh_;
}


double lefiPin::fallThresh() const {
  return this->fallThresh_;
}


double lefiPin::riseSatcur() const {
  return this->riseSatcur_;
}


double lefiPin::fallSatcur() const {
  return this->fallSatcur_;
}


const char* lefiPin::currentSource() const {
  return this->currentSource_;
}


const char* lefiPin::tableHighName() const {
  return this->highTable_;
}


const char* lefiPin::tableLowName() const {
  return this->lowTable_;
}


const char* lefiPin::antennaSizeLayer(int i) const {
  return this->antennaSizeLayer_[i];
}


const char* lefiPin::antennaMetalAreaLayer(int i) const {
  return this->antennaMetalAreaLayer_[i];
}


const char* lefiPin::antennaMetalLengthLayer(int i) const {
  return this->antennaMetalLengthLayer_[i];
}


const char* lefiPin::antennaPartialMetalAreaLayer(int i) const {
  return this->antennaPartialMetalAreaLayer_[i];
}


const char* lefiPin::antennaPartialMetalSideAreaLayer(int i) const {
  return this->antennaPartialMetalSideAreaLayer_[i];
}


const char* lefiPin::antennaPartialCutAreaLayer(int i) const {
  return this->antennaPartialCutAreaLayer_[i];
}


const char* lefiPin::antennaDiffAreaLayer(int i) const {
  return this->antennaDiffAreaLayer_[i];
}


int lefiPin::numAntennaSize() const {
  return this->numAntennaSize_;
}


int lefiPin::numAntennaMetalArea() const {
  return this->numAntennaMetalArea_;
}


int lefiPin::numAntennaMetalLength() const {
  return this->numAntennaMetalLength_;
}


int lefiPin::numAntennaPartialMetalArea() const {
  return this->numAntennaPartialMetalArea_;
}


int lefiPin::numAntennaPartialMetalSideArea() const {
  return this->numAntennaPartialMetalSideArea_;
}


int lefiPin::numAntennaPartialCutArea() const {
  return this->numAntennaPartialCutArea_;
}


int lefiPin::numAntennaDiffArea() const {
  return this->numAntennaDiffArea_;
}


double lefiPin::antennaSize(int i) const {
  return this->antennaSize_[i];
}


double lefiPin::antennaMetalArea(int i) const {
  return this->antennaMetalArea_[i];
}


double lefiPin::antennaMetalLength(int i) const {
  return this->antennaMetalLength_[i];
}


double lefiPin::antennaPartialMetalArea(int i) const {
  return this->antennaPartialMetalArea_[i];
}


double lefiPin::antennaPartialMetalSideArea(int i) const {
  return this->antennaPartialMetalSideArea_[i];
}


double lefiPin::antennaPartialCutArea(int i) const {
  return this->antennaPartialCutArea_[i];
}


double lefiPin::antennaDiffArea(int i) const {
  return this->antennaDiffArea_[i];
}


void lefiPin::addAntennaMetalLength(double val, const char* layer) {
  int len;
  if (this->numAntennaMetalLength_ == this->antennaMetalLengthAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMetalLength_;
    double* nd;
    char** nl;

    if (this->antennaMetalLengthAllocated_ == 0)
      max = this->antennaMetalLengthAllocated_ = 2;
    else
      max = this->antennaMetalLengthAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMetalLengthLayer_[i];
      nd[i] = this->antennaMetalLength_[i];
    }
    lefFree((char*)(this->antennaMetalLengthLayer_));
    lefFree((char*)(this->antennaMetalLength_));
    this->antennaMetalLengthLayer_ = nl;
    this->antennaMetalLength_ = nd;
  }
  this->antennaMetalLength_[this->numAntennaMetalLength_] = val;
  if (layer) {    // layer can be null, since it is optional
     len = strlen(layer) + 1;
     this->antennaMetalLengthLayer_[this->numAntennaMetalLength_] =
        (char*)lefMalloc(len);
     strcpy(this->antennaMetalLengthLayer_[this->numAntennaMetalLength_],
        layer);
  } else
    this->antennaMetalLengthLayer_[this->numAntennaMetalLength_] = NULL;
  this->numAntennaMetalLength_ += 1;
}

// 5.5
void lefiPin::addAntennaModel(int oxide) {
  // For version 5.5 only OXIDE1, OXIDE2, OXIDE3, & OXIDE4
  // are defined within a macro pin
  lefiPinAntennaModel* amo;
  int i;

  if (this->numAntennaModel_ == 0) {   // does not have antennaModel
     this->antennaModel_ = (lefiPinAntennaModel**)
              lefMalloc(sizeof(lefiPinAntennaModel*)*4);
     this->antennaModelAllocated_ = 4;
     for (i = 0; i < 4; i++) {
        this->antennaModel_[i] = (lefiPinAntennaModel*)
                                 lefMalloc(sizeof(lefiPinAntennaModel));
        this->antennaModel_[i]->lefiPinAntennaModel::setAntennaModel(0);
                                 // just initialize it first
     }
     this->numAntennaModel_++;
     this->antennaModelAllocated_ = 4;
     amo = this->antennaModel_[0];
     this->curAntennaModelIndex_ = 0;
  } else {
        // Can't just put oxide-1 as index since lef file may start
        // with OXIDE2 instead of OXIDE1 first
        amo = this->antennaModel_[oxide-1];
        this->curAntennaModelIndex_ = oxide-1; 
        // Oxide has not defined yet
        if (!(amo->lefiPinAntennaModel::antennaOxide()))
           this->numAntennaModel_++;
        else {       // oxide has defined, overide with the new one
           amo->lefiPinAntennaModel::clear();
        }
  }
  amo->lefiPinAntennaModel::Init();
  amo->lefiPinAntennaModel::setAntennaModel(oxide);
  return;
} 

// 5.5
int lefiPin::numAntennaModel() const {
  return this->numAntennaModel_;
}

// 5.5
lefiPinAntennaModel* lefiPin::antennaModel(int index) const {
  int j = index;
  lefiPinAntennaModel* amo;

  if (index == 0) {   // reset all the return flags to 0, beginning of the loop
     int i;
     for (i = 0; i < 4; i++)
        this->antennaModel_[i]->lefiPinAntennaModel::setAntennaReturnFlag(0);
  }
  while (j < 4) {
     amo = this->antennaModel_[j];
     if (!(amo->lefiPinAntennaModel::antennaOxide()) &&
         (amo->lefiPinAntennaModel::hasReturn() == 0)) 
        j++;
     else
        break;
     if (j == 4) {  // something very wrong, normally this can't happen
        lefiError("ERROR (LEFPARS-1351): There is an unexpected lef parser bug which cause it unable to retrieve ANTENNAMODEL data with the given index.");
        return 0;
     }
  }
  // If it arrived here, it is saved, mark the antennaModel has returned
  this->antennaModel_[j]->lefiPinAntennaModel::setAntennaReturnFlag(1);
  return this->antennaModel_[j];
}

void lefiPin::addAntennaSize(double val, const char* layer) {
  int len;
  if (this->numAntennaSize_ == this->antennaSizeAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaSize_;
    double* nd;
    char** nl;

    if (this->antennaSizeAllocated_ == 0)
      max = this->antennaSizeAllocated_ = 2;
    else
      max = this->antennaSizeAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaSizeLayer_[i];
      nd[i] = this->antennaSize_[i];
    }
    lefFree((char*)(this->antennaSizeLayer_));
    lefFree((char*)(this->antennaSize_));
    this->antennaSizeLayer_ = nl;
    this->antennaSize_ = nd;
  }
  this->antennaSize_[this->numAntennaSize_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaSizeLayer_[this->numAntennaSize_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaSizeLayer_[this->numAntennaSize_],
       layer);
  } else
    this->antennaSizeLayer_[this->numAntennaSize_] = NULL;
  this->numAntennaSize_ += 1;
}


void lefiPin::addAntennaMetalArea(double val, const char* layer) {
  int len;
  if (this->numAntennaMetalArea_ == this->antennaMetalAreaAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaMetalArea_;
    double* nd;
    char** nl;

    if (this->antennaMetalAreaAllocated_ == 0)
      max = this->antennaMetalAreaAllocated_ = 2;
    else
      max = this->antennaMetalAreaAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaMetalAreaLayer_[i];
      nd[i] = this->antennaMetalArea_[i];
    }
    lefFree((char*)(this->antennaMetalAreaLayer_));
    lefFree((char*)(this->antennaMetalArea_));
    this->antennaMetalAreaLayer_ = nl;
    this->antennaMetalArea_ = nd;
  }
  this->antennaMetalArea_[this->numAntennaMetalArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaMetalAreaLayer_[this->numAntennaMetalArea_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaMetalAreaLayer_[this->numAntennaMetalArea_],
       layer);
  } else
    this->antennaMetalAreaLayer_[this->numAntennaMetalArea_] = NULL;
  this->numAntennaMetalArea_ += 1;
}


void lefiPin::addAntennaPartialMetalArea(double val, const char* layer) {
  int len;
  if (this->numAntennaPartialMetalArea_ == this->antennaPartialMetalAreaAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaPartialMetalArea_;
    double* nd;
    char** nl;

    if (this->antennaPartialMetalAreaAllocated_ == 0)
      max = this->antennaPartialMetalAreaAllocated_ = 2;
    else
      max = this->antennaPartialMetalAreaAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaPartialMetalAreaLayer_[i];
      nd[i] = this->antennaPartialMetalArea_[i];
    }
    lefFree((char*)(this->antennaPartialMetalAreaLayer_));
    lefFree((char*)(this->antennaPartialMetalArea_));
    this->antennaPartialMetalAreaLayer_ = nl;
    this->antennaPartialMetalArea_ = nd;
  }
  this->antennaPartialMetalArea_[this->numAntennaPartialMetalArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaPartialMetalAreaLayer_[this->numAntennaPartialMetalArea_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaPartialMetalAreaLayer_[this->numAntennaPartialMetalArea_],
       layer);
  } else
    this->antennaPartialMetalAreaLayer_[this->numAntennaPartialMetalArea_] = NULL;
  this->numAntennaPartialMetalArea_ += 1;
}


void lefiPin::addAntennaPartialMetalSideArea(double val, const char* layer) {
  int len;
  if (this->numAntennaPartialMetalSideArea_ == this->antennaPartialMetalSideAreaAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaPartialMetalSideArea_;
    double* nd;
    char** nl;

    if (this->antennaPartialMetalSideAreaAllocated_ == 0)
      max = this->antennaPartialMetalSideAreaAllocated_ = 2;
    else
      max = this->antennaPartialMetalSideAreaAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaPartialMetalSideAreaLayer_[i];
      nd[i] = this->antennaPartialMetalSideArea_[i];
    }
    lefFree((char*)(this->antennaPartialMetalSideAreaLayer_));
    lefFree((char*)(this->antennaPartialMetalSideArea_));
    this->antennaPartialMetalSideAreaLayer_ = nl;
    this->antennaPartialMetalSideArea_ = nd;
  }
  this->antennaPartialMetalSideArea_[this->numAntennaPartialMetalSideArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaPartialMetalSideAreaLayer_[this->numAntennaPartialMetalSideArea_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaPartialMetalSideAreaLayer_[this->numAntennaPartialMetalSideArea_],
       layer);
  } else
    this->antennaPartialMetalSideAreaLayer_[this->numAntennaPartialMetalSideArea_] = NULL;
  this->numAntennaPartialMetalSideArea_ += 1;
}

void lefiPin::addAntennaPartialCutArea(double val, const char* layer) {
  int len;
  if (this->numAntennaPartialCutArea_ == this->antennaPartialCutAreaAllocated_)
{
    int i;
    int max;
    int lim = this->numAntennaPartialCutArea_;
    double* nd;
    char**  nl;

    if (this->antennaPartialCutAreaAllocated_ == 0)
      max = this->antennaPartialCutAreaAllocated_ = 2;
    else
      max = this->antennaPartialCutAreaAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaPartialCutAreaLayer_[i];
      nd[i] = this->antennaPartialCutArea_[i];
    }
    lefFree((char*)(this->antennaPartialCutAreaLayer_));
    lefFree((char*)(this->antennaPartialCutArea_));
    this->antennaPartialCutAreaLayer_ = nl;
    this->antennaPartialCutArea_ = nd;
  }
  this->antennaPartialCutArea_[this->numAntennaPartialCutArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaPartialCutAreaLayer_[this->numAntennaPartialCutArea_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaPartialCutAreaLayer_[this->numAntennaPartialCutArea_],
       layer);
  } else
    this->antennaPartialCutAreaLayer_[this->numAntennaPartialCutArea_] = NULL;
  this->numAntennaPartialCutArea_ += 1;
}

void lefiPin::addAntennaDiffArea(double val, const char* layer) {
  int len;
  if (this->numAntennaDiffArea_ == this->antennaDiffAreaAllocated_) {
    int i;
    int max;
    int lim = this->numAntennaDiffArea_;
    double* nd;
    char** nl;

    if (this->antennaDiffAreaAllocated_ == 0)
      max = this->antennaDiffAreaAllocated_ = 2;
    else
      max = this->antennaDiffAreaAllocated_ *= 2;
    nd = (double*)lefMalloc(sizeof(double)*max);
    nl = (char**)lefMalloc(sizeof(double)*max);
    for (i = 0; i < lim; i++) {
      nl[i] = this->antennaDiffAreaLayer_[i];
      nd[i] = this->antennaDiffArea_[i];
    }
    lefFree((char*)(this->antennaDiffAreaLayer_));
    lefFree((char*)(this->antennaDiffArea_));
    this->antennaDiffAreaLayer_ = nl;
    this->antennaDiffArea_ = nd;
  }
  this->antennaDiffArea_[this->numAntennaDiffArea_] = val;
  if (layer) {  // layer can be null, since it is optional
    len = strlen(layer) + 1;
    this->antennaDiffAreaLayer_[this->numAntennaDiffArea_] =
       (char*)lefMalloc(len);
    strcpy(this->antennaDiffAreaLayer_[this->numAntennaDiffArea_],
       layer);
  } else
    this->antennaDiffAreaLayer_[this->numAntennaDiffArea_] = NULL;
  this->numAntennaDiffArea_ += 1;
}

void lefiPin::addAntennaGateArea(double val, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->lefiPin::addAntennaModel(1);
  this->antennaModel_[this->curAntennaModelIndex_]->lefiPinAntennaModel::addAntennaGateArea(val, layer);
}

void lefiPin::addAntennaMaxAreaCar(double val, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->lefiPin::addAntennaModel(1);
  this->antennaModel_[this->curAntennaModelIndex_]->lefiPinAntennaModel::addAntennaMaxAreaCar(val,
            layer);
}

void lefiPin::addAntennaMaxSideAreaCar(double val, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->lefiPin::addAntennaModel(1);
  this->antennaModel_[this->curAntennaModelIndex_]->lefiPinAntennaModel::addAntennaMaxSideAreaCar(val,
            layer);
}

void lefiPin::addAntennaMaxCutCar(double val, const char* layer) {
  if (this->numAntennaModel_ == 0)    // haven't created any antennaModel yet
     this->lefiPin::addAntennaModel(1);
  this->antennaModel_[this->curAntennaModelIndex_]->lefiPinAntennaModel::addAntennaMaxCutCar(val,
            layer);
}

int lefiPin::numProperties() const {
  return this->numProperties_;
}

const char* lefiPin::propName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNames_[index];
}
 
const char* lefiPin::propValue(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propValues_[index];
}

double lefiPin::propNum(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNums_[index];
}
 
char lefiPin::propType(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propTypes_[index];
}

int lefiPin::propIsNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNums_[index] ? 1 : 0;
}

int lefiPin::propIsString(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNums_[index] ? 0 : 1;
}

void lefiPin::print(FILE* f) const {
  int i;
  lefiGeometries* g;

  fprintf(f, "  Pin %s\n", this->lefiPin::name());

  for (i = 0; i < this->lefiPin::numPorts(); i++) {
    fprintf(f, "    Port %d ", i);
    g = this->lefiPin::port(i);
    g->lefiGeometries::print(f);
  }

}

////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiMacro
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiMacro::lefiMacro() {
  this->lefiMacro::Init();
}


void lefiMacro::Init() {
  this->name_ = 0;
  this->generator_ = 0;
  this->EEQ_ = 0;
  this->LEQ_ = 0;
  this->gen1_ = 0;
  this->gen2_ = 0;
  this->foreign_ = 0;
  this->siteName_ = 0;
  this->clockType_ = 0;
  this->propNames_ = 0;
  this->propValues_ = 0;
  this->propTypes_ = 0;

  this->lefiMacro::bump(&(this->name_), 16, &(this->nameSize_));
  this->lefiMacro::bump(&(this->generator_), 16, &(this->generatorSize_));
  this->lefiMacro::bump(&(this->EEQ_), 16, &(this->EEQSize_));
  this->lefiMacro::bump(&(this->LEQ_), 16, &(this->LEQSize_));
  this->lefiMacro::bump(&(this->gen1_), 16, &(this->gen1Size_));
  this->lefiMacro::bump(&(this->gen2_), 16, &(this->gen2Size_));
  this->lefiMacro::bump(&(this->siteName_), 16, &(this->siteNameSize_));
  this->lefiMacro::bump(&(this->clockType_), 16, &(this->clockTypeSize_));

  this->propertiesAllocated_ = 2;
  this->numProperties_ = 0;
  this->propNames_ = (char**)lefMalloc(sizeof(char*)*2);
  this->propValues_ = (char**)lefMalloc(sizeof(char*)*2);
  this->propNums_ = (double*)lefMalloc(sizeof(double)*2);
  this->propTypes_ = (char*)lefMalloc(sizeof(char)*2);

  this->numSites_ = 0;
  this->sitesAllocated_ = 0;
  this->pattern_ = 0;
  this->numForeigns_ = 0;
  this->foreignAllocated_ = 0;

  this->lefiMacro::clear(); 
}


void lefiMacro::Destroy() {
  this->lefiMacro::clear();
  lefFree(this->name_);
  lefFree(this->generator_);
  lefFree(this->EEQ_);
  lefFree(this->LEQ_);
  lefFree(this->gen1_);
  lefFree(this->gen2_);
  lefFree(this->siteName_);
  lefFree(this->clockType_);
  lefFree((char*)(this->propNames_));
  lefFree((char*)(this->propValues_));
  lefFree((char*)(this->propNums_));
  lefFree((char*)(this->propTypes_));
  if (this->foreignAllocated_) {
     lefFree((char*)(this->hasForeignOrigin_));
     lefFree((char*)(this->hasForeignPoint_));
     lefFree((char*)(this->foreignOrient_));
     lefFree((char*)(this->foreignX_));
     lefFree((char*)(this->foreignY_));
     lefFree((char*)(this->foreign_));
     this->foreignAllocated_ = 0;
  }
}


lefiMacro::~lefiMacro() {
  this->lefiMacro::Destroy();
}


void lefiMacro::clear() {
  int i;

  this->hasClass_ = 0;
  this->hasGenerator_ = 0;
  this->hasGenerate_ = 0;
  this->hasPower_ = 0;
  this->hasOrigin_ = 0;
  this->hasSource_ = 0;
  this->hasEEQ_ = 0;
  this->hasLEQ_ = 0;
  this->hasSymmetry_ = 0;
  this->hasSiteName_ = 0;
  this->hasClockType_ = 0;
  this->hasSize_ = 0;
  this->isInverter_ = 0;
  this->isBuffer_ = 0;

  for (i = 0; i < this->numForeigns_; i++) {
    this->hasForeignOrigin_[i] = 0;
    this->hasForeignPoint_[i] = 0;
    this->foreignOrient_[i] = -1;
    lefFree((char*)(this->foreign_[i]));
  }
  this->numForeigns_ = 0;

  if (this->pattern_) {
    for (i = 0; i < this->numSites_; i++) {
       this->pattern_[i]->lefiSitePattern::Destroy();
       lefFree((char*)(this->pattern_[i]));
    }
    this->numSites_ = 0;
    this->sitesAllocated_ = 0;
    lefFree((char*)(this->pattern_));
    this->pattern_ = 0;
  }

  for (i = 0; i < this->numProperties_; i++) {
    lefFree(this->propNames_[i]);
    lefFree(this->propValues_[i]);
  }
  this->numProperties_ = 0;

  // remove pins 
  for (std::vector<lefiPin*>::iterator it = m_vPin.begin();
		  it != m_vPin.end(); ++it)
	  delete *it;
  m_vPin.clear();

  // remove Obstruction 
  for (std::vector<lefiObstruction*>::iterator it = m_vObs.begin(); it != m_vObs.end(); ++it)
      delete *it;
  m_vObs.clear();
}


void lefiMacro::bump(char** array, int len, int* size) {
  if (*array) lefFree(*array);
  if (len)
     *array = (char*)lefMalloc(len);
  else
     *array = 0;
  *size = len;
}

void lefiMacro::setName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_)
    this->lefiMacro::bump(&(this->name_), len, &(this->nameSize_));
  strcpy(this->name_, CASE(name));
}


void lefiMacro::setGenerate(const char* name, const char* n2) {
  int len = strlen(name) + 1;
  if (len > this->gen1Size_)
    this->lefiMacro::bump(&(this->gen1_), len, &(this->gen1Size_));
  strcpy(this->gen1_, CASE(name));
  len = strlen(n2) + 1;
  if (len > this->gen2Size_)
    this->lefiMacro::bump(&(this->gen2_), len, &(this->gen2Size_));
  strcpy(this->gen2_, n2);
}


void lefiMacro::setGenerator(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->generatorSize_)
    this->lefiMacro::bump(&(this->generator_), len, &(this->generatorSize_));
  strcpy(this->generator_, CASE(name));
  this->hasGenerator_ = 1;
}


void lefiMacro::setInverter() {
  this->isInverter_ = 1;
}


void lefiMacro::setBuffer() {
  this->isBuffer_ = 1;
}


void lefiMacro::setSource(const char* name) {
  strcpy(this->source_, CASE(name));
  this->hasSource_ = 1;
}


void lefiMacro::setClass(const char* name) {
  strcpy(this->macroClass_, CASE(name));
  this->hasClass_ = 1;
}


void lefiMacro::setOrigin(double x, double y) {
  this->originX_ = x;
  this->originY_ = y;
  this->hasOrigin_ = 1;
}


void lefiMacro::setPower(double p) {
  this->power_ = p;
  this->hasPower_ = 1;
}


void lefiMacro::setEEQ(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->EEQSize_)
    this->lefiMacro::bump(&(this->EEQ_), len, &(this->EEQSize_));
  strcpy(this->EEQ_, CASE(name));
  this->hasEEQ_ = 1;
}


void lefiMacro::setLEQ(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->LEQSize_)
    this->lefiMacro::bump(&(this->LEQ_), len, &(this->LEQSize_));
  strcpy(this->LEQ_, CASE(name));
  this->hasLEQ_ = 1;
}


void lefiMacro::setProperty(const char* name, const char* value,
			    const char type) {
  int len;
  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiMacro::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->propNums_[this->numProperties_] = 0.0;
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}


void lefiMacro::setNumProperty(const char* name, double d,
			       const char* value, const char type) {
  int len;

  if (this->numProperties_ == this->propertiesAllocated_)
    this->lefiMacro::bumpProps();
  len = strlen(name) + 1;
  this->propNames_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propNames_[this->numProperties_], CASE(name));
  len = strlen(value) + 1;
  this->propValues_[this->numProperties_] = (char*)lefMalloc(len);
  strcpy(this->propValues_[this->numProperties_], CASE(value));
  this->propNums_[this->numProperties_] = d;
  this->propTypes_[this->numProperties_] = type;
  this->numProperties_ += 1;
}

 
void lefiMacro::bumpProps() {
  int lim;
  int news;
  char** newpn;
  char** newpv;
  double* newd;
  char*  newt;
  int i;

  if (this->propertiesAllocated_ <= 0)
     lim = 2;                 // starts with 4
  else 
     lim = this->propertiesAllocated_;
  news = lim + lim;
  newpn = (char**)lefMalloc(sizeof(char*)*news);
  newpv = (char**)lefMalloc(sizeof(char*)*news);
  newd = (double*)lefMalloc(sizeof(double)*news);
  newt = (char*)lefMalloc(sizeof(char)*news);

  this->propertiesAllocated_ = news;

  for (i = 0; i < lim; i++) {
    newpn[i] = this->propNames_[i];
    newpv[i] = this->propValues_[i];
    newd[i] = this->propNums_[i];
    newt[i] = this->propTypes_[i];
  }
  lefFree((char*)(this->propNames_));
  lefFree((char*)(this->propValues_));
  lefFree((char*)(this->propNums_));
  lefFree((char*)(this->propTypes_));
  this->propNames_ = newpn;
  this->propValues_ = newpv;
  this->propNums_ = newd;
  this->propTypes_ = newt;
}

void lefiMacro::addForeign(const char* name, int hasPnt,
	       double x, double y, int orient) {
  int     i;
  int*    hfo;
  int*    hfp;
  int*    fo;
  double* fx;
  double* fy;
  char**  f;

  if (this->foreignAllocated_ == this->numForeigns_) {
     if (this->foreignAllocated_ == 0)
	this->foreignAllocated_ = 16; // since it involves char*, it will
				      // costly in the number is too small
     else
	this->foreignAllocated_ *= 2;
     hfo = (int*)lefMalloc(sizeof(int)*this->foreignAllocated_);  
     hfp = (int*)lefMalloc(sizeof(int)*this->foreignAllocated_);  
     fo  = (int*)lefMalloc(sizeof(int)*this->foreignAllocated_);
     fx  = (double*)lefMalloc(sizeof(double)*this->foreignAllocated_);
     fy  = (double*)lefMalloc(sizeof(double)*this->foreignAllocated_);
     f   = (char**)lefMalloc(sizeof(char*)*this->foreignAllocated_);
     if (this->numForeigns_ != 0) {
	for (i = 0; i < this->numForeigns_; i++) {
	   hfo[i] = this->hasForeignOrigin_[i];
	   hfp[i] = this->hasForeignPoint_[i];
	   fo[i]  = this->foreignOrient_[i];
	   fx[i]  = this->foreignX_[i];
	   fy[i]  = this->foreignY_[i];
	   f[i]   = this->foreign_[i];
	}
	lefFree((char*)(this->hasForeignOrigin_));
	lefFree((char*)(this->hasForeignPoint_));
	lefFree((char*)(this->foreignOrient_));
	lefFree((char*)(this->foreignX_));
	lefFree((char*)(this->foreignY_));
	lefFree((char*)(this->foreign_));
     }
     this->hasForeignOrigin_ = hfo;
     this->hasForeignPoint_ = hfp;
     this->foreignOrient_ = fo;
     this->foreignX_ = fx;
     this->foreignY_ = fy;
     this->foreign_ = f;
  }

  // orient=-1 means no orient was specified.
  this->hasForeignOrigin_[this->numForeigns_] = orient; 
  this->hasForeignPoint_[this->numForeigns_] = hasPnt;
  this->foreignOrient_[this->numForeigns_] = orient;
  this->foreignX_[this->numForeigns_] = x;
  this->foreignY_[this->numForeigns_] = y;
  this->foreign_[this->numForeigns_] = (char*)lefMalloc(strlen(name)+1);
  strcpy(this->foreign_[this->numForeigns_], CASE(name));
  this->numForeigns_ += 1;

}

	 
void lefiMacro::setXSymmetry() {
  this->hasSymmetry_ |= 1;
}


void lefiMacro::setYSymmetry() {
  this->hasSymmetry_ |= 2;
}


void lefiMacro::set90Symmetry() {
  this->hasSymmetry_ |= 4;
}


void lefiMacro::setSiteName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->siteNameSize_) 
    this->lefiMacro::bump(&(this->siteName_), len, &(this->siteNameSize_));
  strcpy(this->siteName_, CASE(name));
  this->hasSiteName_ = 1;
}


void lefiMacro::setClockType(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->clockTypeSize_)
    this->lefiMacro::bump(&(this->clockType_), len, &(this->clockTypeSize_));
  strcpy(this->clockType_, CASE(name));
  this->hasClockType_ = 1;
}


void lefiMacro::setSitePattern(lefiSitePattern* p) {
  if (this->numSites_ == this->sitesAllocated_) {
     lefiSitePattern** np;
     int i, lim;
     if (this->sitesAllocated_ == 0) {
	lim = this->sitesAllocated_ = 4;
	np = (lefiSitePattern**) lefMalloc(sizeof(lefiSitePattern*)*lim);
     } else {
	lim = this->sitesAllocated_ * 2;
	this->sitesAllocated_ = lim;
	np = (lefiSitePattern**)lefMalloc(sizeof(lefiSitePattern*)*lim);
	lim /= 2;
	for (i = 0; i < lim; i++)
	  np[i] = this->pattern_[i];
	lefFree((char*)(this->pattern_));
     }
     this->pattern_ = np;
  }
  this->pattern_[this->numSites_] = p;
  this->numSites_ += 1;
}


void lefiMacro::setSize(double x, double y) {
  this->hasSize_ = 1;
  this->sizeX_ = x;
  this->sizeY_ = y;
}
	 

int lefiMacro::hasClass() const {
  return this->hasClass_;
}


int lefiMacro::hasSiteName() const {
  return this->hasSiteName_;
}


int lefiMacro::hasGenerator() const {
  return this->hasGenerator_;
}


int lefiMacro::hasPower() const {
  return this->hasPower_;
}


int lefiMacro::hasOrigin() const {
  return this->hasOrigin_;
}


int lefiMacro::hasSource() const {
  return this->hasSource_;
}


int lefiMacro::hasEEQ() const {
  return this->hasEEQ_;
}


int lefiMacro::hasLEQ() const {
  return this->hasLEQ_;
}


int lefiMacro::hasXSymmetry() const {
  return (this->hasSymmetry_ & 1) ? 1 : 0 ;
}


int lefiMacro::hasYSymmetry() const {
  return (this->hasSymmetry_ & 2) ? 1 : 0 ;
}


int lefiMacro::has90Symmetry() const {
  return (this->hasSymmetry_ & 4) ? 1 : 0 ;
}


int lefiMacro::hasSitePattern() const {
  return (this->pattern_) ? 1 : 0 ;
}


int lefiMacro::hasSize() const {
  return this->hasSize_;
}


int lefiMacro::hasForeign() const {
  return (this->numForeigns_) ? 1 : 0 ;
}


int lefiMacro::hasForeignOrigin(int index) const {
  return this->hasForeignOrigin_[index];
}


int lefiMacro::hasForeignOrient(int index) const {
  return (this->foreignOrient_[index] == -1) ? 0 : 1 ;
}


int lefiMacro::hasForeignPoint(int index) const {
  return this->hasForeignPoint_[index];
}


int lefiMacro::hasClockType() const {
  return this->hasClockType_;
}


int lefiMacro::numSitePattern() const {
  return this->numSites_;
}


int lefiMacro::numProperties() const {
  return this->numProperties_;
}


const char* lefiMacro::propName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNames_[index];
}


const char* lefiMacro::propValue(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propValues_[index];
}


double lefiMacro::propNum(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNums_[index];
}


char lefiMacro::propType(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propTypes_[index];
}


int lefiMacro::propIsNumber(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNums_[index] ? 1 : 0;
}


int lefiMacro::propIsString(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numProperties_) {
     sprintf (msg, "ERROR (EMSPARS-1352): The index number %d given for the macro property is invalid.\nValid index is from 0 to %d", index, this->numProperties_);
     lefiError(msg);
     return 0;
  }
  return this->propNums_[index] ? 0 : 1;
}


const char* lefiMacro::name() const {
  return this->name_;
}


const char* lefiMacro::macroClass() const {
  return this->macroClass_;
}


const char* lefiMacro::generator() const {
  return this->generator_;
}


const char* lefiMacro::EEQ() const {
  return this->EEQ_;
}


const char* lefiMacro::LEQ() const {
  return this->LEQ_;
}


const char* lefiMacro::source() const {
  return this->source_;
}


double lefiMacro::originX() const {
  return this->originX_;
}


double lefiMacro::originY() const {
  return this->originY_;
}


double lefiMacro::power() const {
  return this->power_;
}


void lefiMacro::generate(char** name1, char** name2) const {
  if (name1) *name1 = this->gen1_;
  if (name2) *name2 = this->gen2_;
}


lefiSitePattern* lefiMacro::sitePattern(int index) const {
  return this->pattern_[index];
}


const char* lefiMacro::siteName() const {
  return this->siteName_;
}


double lefiMacro::sizeX() const {
  return this->sizeX_;
}


double lefiMacro::sizeY() const {
  return this->sizeY_;
}


int lefiMacro::numForeigns() const {
  return this->numForeigns_;
}

int lefiMacro::foreignOrient(int index) const {
  return this->foreignOrient_[index];
}


const char* lefiMacro::foreignOrientStr(int index) const {
  return (lefiOrientStr(this->foreignOrient_[index]));
}


double lefiMacro::foreignX(int index) const {
  return this->foreignX_[index];
}


double lefiMacro::foreignY(int index) const {
  return this->foreignY_[index];
}


const char* lefiMacro::foreignName(int index) const {
  return this->foreign_[index];
}

int lefiMacro::numPins() const 
{
	return m_vPin.size();
}

lefiPin* lefiMacro::pin(unsigned int index) const 
{
	if (index >= m_vPin.size())
		return NULL;
	return m_vPin[index];
}

void lefiMacro::addPin(lefiPin const& p)
{
	m_vPin.push_back(new lefiPin (p));
}


const char* lefiMacro::clockType() const {
  return this->clockType_;
}


int lefiMacro::isBuffer() const {
  return this->isBuffer_;
}


int lefiMacro::isInverter() const {
  return this->isInverter_;
}


void lefiMacro::print(FILE* f) const {
  char* c1;
  char* c2;
  lefiSitePattern* sp;
  int i;

  fprintf(f, "MACRO %s\n", this->lefiMacro::name());

  if (this->lefiMacro::hasClass())
    fprintf(f, "  Class %s\n", this->lefiMacro::macroClass());

  if (this->lefiMacro::hasGenerator())
    fprintf(f, "  Generator %s\n", this->lefiMacro::generator());

  if (this->lefiMacro::hasGenerator()) {
    this->lefiMacro::generate(&c1, &c2);
    fprintf(f, "  Generate %s %s\n", c1, c2);
  }

  if (this->lefiMacro::hasPower())
    fprintf(f, "  Power %g\n", this->lefiMacro::power());

  if (this->lefiMacro::hasOrigin())
    fprintf(f, "  Origin %g,%g\n", this->lefiMacro::originX(),
       this->lefiMacro::originY());

  if (this->lefiMacro::hasEEQ())
    fprintf(f, "  EEQ %s\n", this->lefiMacro::EEQ());

  if (this->lefiMacro::hasLEQ())
    fprintf(f, "  LEQ %s\n", this->lefiMacro::LEQ());

  if (this->lefiMacro::hasSource())
    fprintf(f, "  Source %s\n", this->lefiMacro::source());

  if (this->lefiMacro::hasXSymmetry())
    fprintf(f, "  Symmetry X\n");

  if (this->lefiMacro::hasYSymmetry())
    fprintf(f, "  Symmetry Y\n");

  if (this->lefiMacro::has90Symmetry())
    fprintf(f, "  Symmetry R90\n");

  if (this->lefiMacro::hasSiteName())
    fprintf(f, "  Site name %s\n", this->lefiMacro::siteName());

  if (this->lefiMacro::hasSitePattern()) {
    for (i = 0; i < this->lefiMacro::numSitePattern(); i++) {
      sp = this->lefiMacro::sitePattern(i);
      fprintf(f, "  Site pattern ");
      sp->lefiSitePattern::print(f);
    }
  }

  if (this->lefiMacro::hasSize())
    fprintf(f, "  Size %g,%g\n", this->lefiMacro::sizeX(),
       this->lefiMacro::sizeY());

  if (this->lefiMacro::hasForeign()) {
    for (i = 0; i < this->lefiMacro::numForeigns(); i++) {
       fprintf(f, "  Foreign %s", this->lefiMacro::foreignName(i));
       if (this->lefiMacro::hasForeignOrigin(i))
         fprintf(f, "  %g,%g", this->lefiMacro::foreignX(i),
         this->lefiMacro::foreignY(i));
       if (this->lefiMacro::hasForeignOrient(i))
         fprintf(f, "  orient %s", this->lefiMacro::foreignOrientStr(i));
    fprintf(f, "\n");
    }
  }

  if (this->lefiMacro::hasClockType())
    fprintf(f, "  Clock type %s\n", this->lefiMacro::clockType());

  for (vector<lefiPin*>::const_iterator it = m_vPin.begin(); 
		  it != m_vPin.end(); ++it)
  {
	  (*it)->print(f);
  }
  // print Obstruction
  for (std::vector<lefiObstruction*>::const_iterator it = m_vObs.begin(); it != m_vObs.end(); ++it)
      (*it)->print(f);

  fprintf(f, "END MACRO %s\n", this->lefiMacro::name());
}


////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiTiming
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiTiming::lefiTiming() {
  this->lefiTiming::Init();
}


void lefiTiming::Init() {
  this->numFrom_ = 0;
  this->from_ = (char**)lefMalloc(sizeof(char*));
  this->fromAllocated_ = 1;
  this->numTo_ = 0;
  this->to_ = (char**)lefMalloc(sizeof(char*));
  this->toAllocated_ = 1;

  this->numOfAxisNumbers_ = 0;
  this->axisNumbers_ = (double*)lefMalloc(sizeof(double));
  this->axisNumbersAllocated_ = 1;

  this->numOfTableEntries_ = 0;
  this->tableEntriesAllocated_ = 1;
  this->table_ = (double*)lefMalloc(sizeof(double)*3);  /* three numbers per entry */

  this->lefiTiming::clear();
}


void lefiTiming::Destroy() {
  this->lefiTiming::clear();
  lefFree((char*)(this->from_));
  lefFree((char*)(this->to_));
  lefFree((char*)(this->axisNumbers_));
  lefFree((char*)(this->table_));
}


lefiTiming::~lefiTiming() {
  this->lefiTiming::Destroy();
}


void lefiTiming::addRiseFall(const char* risefall, double one, double two) {
  if (*risefall == 'r' || *risefall == 'R') {
    this->hasRiseIntrinsic_ = 1;
    this->nowRise_ = 1;
    this->riseIntrinsicOne_ = one;
    this->riseIntrinsicTwo_ = two;
  } else {
    this->nowRise_ = 0;
    this->hasFallIntrinsic_ = 1;
    this->fallIntrinsicOne_ = one;
    this->fallIntrinsicTwo_ = two;
  }
}


void lefiTiming::addRiseFallVariable(double one, double two) {
  if (this->nowRise_ == 1) {
    this->riseIntrinsicThree_ = one;
    this->riseIntrinsicFour_ = two;
  } else {
    this->fallIntrinsicThree_ = one;
    this->fallIntrinsicFour_ = two;
  }
}


void lefiTiming::setRiseRS(double one, double two) {
  this->hasRiseRS_ = 1;
  this->riseRSOne_ = one;
  this->riseRSTwo_ = two;
}


void lefiTiming::setFallRS(double one, double two) {
  this->hasFallRS_ = 1;
  this->fallRSOne_ = one;
  this->fallRSTwo_ = two;
}


void lefiTiming::setRiseCS(double one, double two) {
  this->hasRiseCS_ = 1;
  this->riseCSOne_ = one;
  this->riseCSTwo_ = two;
}


void lefiTiming::setFallCS(double one, double two) {
  this->hasFallCS_ = 1;
  this->fallCSOne_ = one;
  this->fallCSTwo_ = two;
}


void lefiTiming::setRiseAtt1(double one, double two) {
  this->hasRiseAtt1_ = 1;
  this->riseAtt1One_ = one;
  this->riseAtt1Two_ = two;
}


void lefiTiming::setFallAtt1(double one, double two) {
  this->hasFallAtt1_ = 1;
  this->fallAtt1One_ = one;
  this->fallAtt1Two_ = two;
}


void lefiTiming::setRiseTo(double one, double two) {
  this->hasRiseTo_ = 1;
  this->riseToOne_ = one;
  this->riseToTwo_ = two;
}


void lefiTiming::setFallTo(double one, double two) {
  this->hasFallTo_ = 1;
  this->fallToOne_ = one;
  this->fallToTwo_ = two;
}


void lefiTiming::addUnateness(const char* typ) {
  this->hasUnateness_ = 1;
  this->unateness_ = (char*)typ;
}


void lefiTiming::setStable(double one, double two, const char* typ) {
  this->hasStableTiming_ = 1;
  this->stableSetup_ = one;
  this->stableHold_ = two;
  this->stableRiseFall_ = (char*)typ;
}


void lefiTiming::addTableEntry(double one, double two, double three) {
  int i;
  double *n;
  if (this->numOfTableEntries_ >= this->tableEntriesAllocated_) {
    int lim;

    if (this->tableEntriesAllocated_ == 0)
       lim = this->tableEntriesAllocated_ = 2;
    else
       lim = this->tableEntriesAllocated_ *= 2;
    n = (double*)lefMalloc(sizeof(double)*3*lim);
    lim = this->numOfTableEntries_ * 3;
    for (i = 0; i < lim; i++) {
      n[i] = this->table_[i];
    }
    lefFree((char*)(this->table_));
    this->table_ = n;
  }
  i = this->numOfTableEntries_ * 3;
  this->table_[i++] = one;
  this->table_[i++] = two;
  this->table_[i] = three;
  this->numOfTableEntries_ += 1;
}


void lefiTiming::addTableAxisNumber(double one) {
  if (this->numOfAxisNumbers_ == this->axisNumbersAllocated_) {
    int i;
    int lim;
    double* n;

    if (this->axisNumbersAllocated_ == 0)
      lim = this->axisNumbersAllocated_ = 2;
    else
      lim = this->axisNumbersAllocated_ *= 2;
    n = (double*)lefMalloc(sizeof(double)*lim);
    lim = this->numOfAxisNumbers_;
    for (i = 0; i < lim; i++)
      n[i] = this->axisNumbers_[i];
    if (this->axisNumbersAllocated_ > 2)
      lefFree((char*)(this->axisNumbers_));
    this->axisNumbers_ = n;
  }
  this->axisNumbers_[(this->numOfAxisNumbers_)++] = one;
}


void lefiTiming::addRiseFallSlew(double one, double two,
	   double three, double four) {
  if (this->nowRise_) {
    this->hasRiseSlew_ = 1;
    this->riseSlewOne_ = one;
    this->riseSlewTwo_ = two;
    this->riseSlewThree_ = three;
    this->riseSlewFour_ = four;
  } else {
    this->hasFallSlew_ = 1;
    this->fallSlewOne_ = one;
    this->fallSlewTwo_ = two;
    this->fallSlewThree_ = three;
    this->fallSlewFour_ = four;
  }
}


void lefiTiming::addRiseFallSlew2(double one, double two, double three) {
  if (this->nowRise_) {
    this->hasRiseSlew2_ = 1;
    this->riseSlewFive_ = one;
    this->riseSlewSix_ = two;
    this->riseSlewSeven_ = three;
  } else {
    this->hasFallSlew2_ = 1;
    this->fallSlewFive_ = one;
    this->fallSlewSix_ = two;
    this->fallSlewSeven_ = three;
  }
}


void lefiTiming::addFromPin(const char* name) {
  if (this->numFrom_ == this->fromAllocated_) {
    int lim;
    int i;
    char** n;

    if (this->fromAllocated_ == 0)
      lim = this->fromAllocated_ = 2;
    else
      lim = this->fromAllocated_ *= 2;
    n = (char**)lefMalloc(sizeof(char*)*lim);
    lim = this->numFrom_;
    for (i = 0; i < lim; i++)
      n[i] = this->from_[i];
    lefFree((char*)(this->from_));
    this->from_ = n;
  }
  this->from_[(this->numFrom_)++] = (char*)name;
}


void lefiTiming::addToPin(const char* name) {
  if (this->numTo_ == this->toAllocated_) {
    int lim;
    int i;
    char** n;

    if (this->toAllocated_ == 0)
      lim = this->toAllocated_ = 2;
    else
      lim = this->toAllocated_ *= 2;
    n = (char**)lefMalloc(sizeof(char*)*lim);
    lim = this->numTo_;
    for (i = 0; i < lim; i++)
      n[i] = this->to_[i];
    lefFree((char*)(this->to_));
    this->to_ = n;
  }
  this->to_[(this->numTo_)++] = (char*)name;
}


void lefiTiming::addDelay(const char* risefall, const char* unateness,
        double one, double two, double three) {
  this->hasDelay_ = 1;
  this->delayRiseOrFall_ = (char*)risefall;
  this->delayUnateness_ = (char*)unateness;
  this->delayTableOne_ = one;
  this->delayTableTwo_ = two;
  this->delayTableThree_ = three;
}


void lefiTiming::addTransition(const char* risefall, const char* unateness,
        double one, double two, double three) {
  this->hasTransition_ = 1;
  this->transitionRiseOrFall_ = (char*)risefall;
  this->transitionUnateness_ = (char*)unateness;
  this->transitionTableOne_ = one;
  this->transitionTableTwo_ = two;
  this->transitionTableThree_ = three;
}


void lefiTiming::addSDF1Pin(const char* trigType, double one, double two,
        double three) {
  this->hasSDFonePinTrigger_ = 1;
  this->SDFtriggerType_ = (char*) trigType;
  this->SDFtriggerTableOne_ = one;
  this->SDFtriggerTableTwo_ = two;
  this->SDFtriggerTableThree_ = three;
}


void lefiTiming::addSDF2Pins(const char* trigType, const char* fromTrig,
        const char* toTrig, double one, double two, double three) {
  this->hasSDFtwoPinTrigger_ = 1;
  this->SDFtriggerType_ = (char*) trigType;
  this->SDFfromTrigger_ = (char*) fromTrig;
  this->SDFtoTrigger_ = (char*) toTrig;
  this->SDFtriggerTableOne_ = one;
  this->SDFtriggerTableTwo_ = two;
  this->SDFtriggerTableThree_ = three;
}


void lefiTiming::setSDFcondStart(const char* condStart) {
  this->SDFcondStart_ = (char*)condStart;
}


void lefiTiming::setSDFcondEnd(const char* condEnd) {
  this->SDFcondEnd_ = (char*)condEnd;
}


void lefiTiming::setSDFcond(const char* cond) {
  this->SDFcond_ = (char*)cond;
}


int lefiTiming::hasData() {
  return ((this->numFrom_) ? 1 : 0 );
}


void lefiTiming::clear() {
  this->numFrom_ = 0;
  this->numTo_ = 0;
  this->numOfAxisNumbers_ = 0;
  this->numOfTableEntries_ = 0;

  this->nowRise_ = 0;
  this->hasTransition_ = 0;
  this->hasDelay_ = 0;
  this->hasFallSlew_ = 0;
  this->hasFallSlew2_ = 0;
  this->hasRiseSlew_ = 0;
  this->hasRiseSlew2_ = 0;
  this->hasRiseIntrinsic_ = 0;
  this->hasFallIntrinsic_ = 0;
  this->hasRiseSlew_ = 0;
  this->hasFallSlew_ = 0;
  this->hasRiseSlew2_ = 0;
  this->hasFallSlew2_ = 0;
  this->hasRiseRS_ = 0;
  this->hasRiseCS_ = 0;
  this->hasFallRS_ = 0;
  this->hasFallCS_ = 0;
  this->hasUnateness_ = 0;
  this->hasFallAtt1_ = 0;
  this->hasRiseAtt1_ = 0;
  this->hasFallTo_ = 0;
  this->hasRiseTo_ = 0;
  this->hasStableTiming_ = 0;
  this->hasSDFonePinTrigger_ = 0;
  this->hasSDFtwoPinTrigger_ = 0;
  this->hasSDFcondStart_ = 0;
  this->hasSDFcondEnd_ = 0;
  this->hasSDFcond_ = 0;
}


int lefiTiming::numFromPins() {
  return this->numFrom_;
}


const char* lefiTiming::fromPin(int index) {
  return this->from_[index];
}


int lefiTiming::numToPins() {
  return this->numTo_;
}


const char* lefiTiming::toPin(int index) {
  return this->to_[index];
}


int lefiTiming::hasTransition() {
  return this->hasTransition_;
}


int lefiTiming::hasDelay() {
  return this->hasDelay_;
}


int lefiTiming::hasRiseSlew() {
  return this->hasRiseSlew_;
}


int lefiTiming::hasRiseSlew2() {
  return this->hasRiseSlew2_;
}


int lefiTiming::hasFallSlew() {
  return this->hasFallSlew_;
}


int lefiTiming::hasFallSlew2() {
  return this->hasFallSlew2_;
}


int lefiTiming::hasRiseIntrinsic() {
  return this->hasRiseIntrinsic_;
}


int lefiTiming::hasFallIntrinsic() {
  return this->hasFallIntrinsic_;
}


int lefiTiming::hasSDFonePinTrigger() {
  return this->hasSDFonePinTrigger_;
}


int lefiTiming::hasSDFtwoPinTrigger() {
  return this->hasSDFtwoPinTrigger_;
}


int lefiTiming::hasSDFcondStart() {
  return this->hasSDFcondStart_;
}


int lefiTiming::hasSDFcondEnd() {
  return this->hasSDFcondEnd_;
}


int lefiTiming::hasSDFcond() {
  return this->hasSDFcond_;
}


int lefiTiming::numOfAxisNumbers() {
  return this->numOfAxisNumbers_;
}


double* lefiTiming::axisNumbers() {
  return this->axisNumbers_;
}


int lefiTiming::numOfTableEntries() {
  return this->numOfTableEntries_;
}


void lefiTiming::tableEntry(int num, double* one, double* two,
          double* three) {
  num *= 3;
  *one = this->table_[num];
  num++;
  *two = this->table_[num];
  num++;
  *three = this->table_[num];
}


const char* lefiTiming::delayRiseOrFall() {
  return this->delayRiseOrFall_;
}


const char* lefiTiming::delayUnateness() {
  return this->delayUnateness_;
}


double lefiTiming::delayTableOne() {
  return this->delayTableOne_;
}


double lefiTiming::delayTableTwo() {
  return this->delayTableTwo_;
}


double lefiTiming::delayTableThree() {
  return this->delayTableThree_;
}


const char* lefiTiming::transitionRiseOrFall() {
  return this->transitionRiseOrFall_;
}


const char* lefiTiming::transitionUnateness() {
  return this->transitionUnateness_;
}


double lefiTiming::transitionTableOne() {
  return this->transitionTableOne_;
}


double lefiTiming::transitionTableTwo() {
  return this->transitionTableTwo_;
}


double lefiTiming::transitionTableThree() {
  return this->transitionTableThree_;
}


double lefiTiming::riseIntrinsicOne() {
  return this->riseIntrinsicOne_;
}


double lefiTiming::riseIntrinsicTwo() {
  return this->riseIntrinsicTwo_;
}


double lefiTiming::riseIntrinsicThree() {
  return this->riseIntrinsicThree_;
}


double lefiTiming::riseIntrinsicFour() {
  return this->riseIntrinsicFour_;
}


double lefiTiming::fallIntrinsicOne() {
  return this->fallIntrinsicOne_;
}


double lefiTiming::fallIntrinsicTwo() {
  return this->fallIntrinsicTwo_;
}


double lefiTiming::fallIntrinsicThree() {
  return this->fallIntrinsicThree_;
}


double lefiTiming::fallIntrinsicFour() {
  return this->fallIntrinsicFour_;
}


double lefiTiming::riseSlewOne() {
  return this->riseSlewOne_;
}


double lefiTiming::riseSlewTwo() {
  return this->riseSlewTwo_;
}


double lefiTiming::riseSlewThree() {
  return this->riseSlewThree_;
}


double lefiTiming::riseSlewFour() {
  return this->riseSlewFour_;
}


double lefiTiming::riseSlewFive() {
  return this->riseSlewFive_;
}


double lefiTiming::riseSlewSix() {
  return this->riseSlewSix_;
}


double lefiTiming::riseSlewSeven() {
  return this->riseSlewSeven_;
}


double lefiTiming::fallSlewOne() {
  return this->fallSlewOne_;
}


double lefiTiming::fallSlewTwo() {
  return this->fallSlewTwo_;
}


double lefiTiming::fallSlewThree() {
  return this->fallSlewThree_;
}


double lefiTiming::fallSlewFour() {
  return this->fallSlewFour_;
}


double lefiTiming::fallSlewFive() {
  return this->fallSlewFive_;
}


double lefiTiming::fallSlewSix() {
  return this->fallSlewSix_;
}


double lefiTiming::fallSlewSeven() {
  return this->fallSlewSeven_;
}


int lefiTiming::hasRiseRS() {
  return this->hasRiseRS_;
}


double lefiTiming::riseRSOne() {
  return this->riseRSOne_;
}


double lefiTiming::riseRSTwo() {
  return this->riseRSTwo_;
}


int lefiTiming::hasRiseCS() {
  return this->hasRiseCS_;
}


double lefiTiming::riseCSOne() {
  return this->riseCSOne_;
}


double lefiTiming::riseCSTwo() {
  return this->riseCSTwo_;
}


int lefiTiming::hasFallRS() {
  return this->hasFallRS_;
}


double lefiTiming::fallRSOne() {
  return this->fallRSOne_;
}


double lefiTiming::fallRSTwo() {
  return this->fallRSTwo_;
}


int lefiTiming::hasFallCS() {
  return this->hasFallCS_;
}


double lefiTiming::fallCSOne() {
  return this->fallCSOne_;
}


double lefiTiming::fallCSTwo() {
  return this->fallCSTwo_;
}


int lefiTiming::hasUnateness() {
  return this->hasUnateness_;
}


const char* lefiTiming::unateness() {
  return this->unateness_;
}


int lefiTiming::hasFallAtt1() {
  return this->hasFallAtt1_;
}


double lefiTiming::fallAtt1One() {
  return this->fallAtt1One_;
}


double lefiTiming::fallAtt1Two() {
  return this->fallAtt1Two_;
}


int lefiTiming::hasRiseAtt1() {
  return this->hasRiseAtt1_;
}


double lefiTiming::riseAtt1One() {
  return this->riseAtt1One_;
}


double lefiTiming::riseAtt1Two() {
  return this->riseAtt1Two_;
}


int lefiTiming::hasFallTo() {
  return this->hasFallTo_;
}


double lefiTiming::fallToOne() {
  return this->fallToOne_;
}


double lefiTiming::fallToTwo() {
  return this->fallToTwo_;
}


int lefiTiming::hasRiseTo() {
  return this->hasRiseTo_;
}


double lefiTiming::riseToOne() {
  return this->riseToOne_;
}


double lefiTiming::riseToTwo() {
  return this->riseToTwo_;
}


int lefiTiming::hasStableTiming() {
  return this->hasStableTiming_;
}


double lefiTiming::stableSetup() {
  return this->stableSetup_;
}


double lefiTiming::stableHold() {
  return this->stableHold_;
}


const char* lefiTiming::stableRiseFall() {
  return this->stableRiseFall_;
}


const char* lefiTiming::SDFonePinTriggerType() {
  return this->SDFtriggerType_;
}


const char* lefiTiming::SDFtwoPinTriggerType() {
  return this->SDFtriggerType_;
}


const char* lefiTiming::SDFfromTrigger() {
  return this->SDFfromTrigger_;
}


const char* lefiTiming::SDFtoTrigger() {
  return this->SDFtoTrigger_;
}


double lefiTiming::SDFtriggerOne() {
  return this->SDFtriggerTableOne_;
}


double lefiTiming::SDFtriggerTwo() {
  return this->SDFtriggerTableTwo_;
}


double lefiTiming::SDFtriggerThree() {
  return this->SDFtriggerTableThree_;
}


const char* lefiTiming::SDFcondStart() {
  return this->SDFcondStart_;
}


const char* lefiTiming::SDFcondEnd() {
  return this->SDFcondEnd_;
}


const char* lefiTiming::SDFcond() {
  return this->SDFcond_;
}

} // namespace LefParser
