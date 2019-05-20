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

#include <limbo/parsers/lef/bison/lefiMisc.hpp>
#include <string.h>
#include <stdlib.h>
//#include "lex.h"
#include <limbo/parsers/lef/bison/lefiDebug.hpp>
//#include "FEF_stringhash.h"
#include <limbo/parsers/lef/bison/lefiUtil.hpp>
#include <limbo/parsers/lef/bison/LefDataBase.h> // be careful about cross reference

namespace LefParser {


lefiGeomRect::lefiGeomRect()
{
	xl = yl = xh = yh = 0;
}
void lefiGeomRect::copy(lefiGeomRect const& rhs)
{
	this->xl = rhs.xl; 
	this->yl = rhs.yl;
	this->xh = rhs.xh; 
	this->yh = rhs.yh;
}
#if 0
lefiGeomRect::~lefiGeomRect()
{
}
#endif 
lefiGeomRectIter::lefiGeomRectIter()
{
	xl = yl = xh = yh = 0;
	xStart = yStart = xStep = yStep = 0;
}
void lefiGeomRectIter::copy(lefiGeomRectIter const& rhs)
{
	this->xl = rhs.xl; 
	this->yl = rhs.yl;
	this->xh = rhs.xh; 
	this->yh = rhs.yh;
	this->xStart = rhs.xStart;
	this->yStart = rhs.yStart;
	this->xStep = rhs.xStep;
	this->yStep = rhs.yStep;
}
#if 0
lefiGeomRectIter::~lefiGeomRectIter()
{
}
#endif 
lefiGeomPath::lefiGeomPath()
{
	numPoints = 0;
	x = 0;
	y = 0;
}
void lefiGeomPath::copy(lefiGeomPath const& rhs)
{
	this->numPoints = rhs.numPoints;
	this->x = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	this->y = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	for (int i = 0; i < rhs.numPoints; ++i)
	{
		this->x[i] = rhs.x[i];
		this->y[i] = rhs.y[i];
	}
}
#if 0
lefiGeomPath::~lefiGeomPath()
{
	if (this->numPoints != 0)
	{
		lefFree((double*)this->x);
		lefFree((double*)this->y);
	}
}
#endif 
lefiGeomPathIter::lefiGeomPathIter()
{
	numPoints = 0;
	x = 0;
	y = 0;
	xStart = yStart = xStep = yStep = 0;
}
void lefiGeomPathIter::copy(lefiGeomPathIter const& rhs)
{
	this->numPoints = rhs.numPoints;
	this->x = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	this->y = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	for (int i = 0; i < rhs.numPoints; ++i)
	{
		this->x[i] = rhs.x[i];
		this->y[i] = rhs.y[i];
	}
	this->xStart = rhs.xStart;
	this->yStart = rhs.yStart;
	this->xStep = rhs.xStep;
	this->yStep = rhs.yStep;
}
#if 0
lefiGeomPathIter::~lefiGeomPathIter()
{
	if (this->numPoints != 0)
	{
		lefFree((double*)this->x);
		lefFree((double*)this->y);
	}
}
#endif 
lefiGeomPolygon::lefiGeomPolygon()
{
	numPoints = 0;
	x = 0;
	y = 0;
}
void lefiGeomPolygon::copy(lefiGeomPolygon const& rhs)
{
	this->numPoints = rhs.numPoints;
	this->x = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	this->y = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	for (int i = 0; i < rhs.numPoints; ++i)
	{
		this->x[i] = rhs.x[i];
		this->y[i] = rhs.y[i];
	}
}
#if 0
lefiGeomPolygon::~lefiGeomPolygon()
{
	if (this->numPoints != 0)
	{
		lefFree((double*)this->x);
		lefFree((double*)this->y);
	}
}
#endif 
lefiGeomPolygonIter::lefiGeomPolygonIter()
{
	numPoints = 0;
	x = 0;
	y = 0;
	xStart = yStart = xStep = yStep = 0;
}
void lefiGeomPolygonIter::copy(lefiGeomPolygonIter const& rhs)
{
	this->numPoints = rhs.numPoints;
	this->x = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	this->y = (double*)lefMalloc(sizeof(double)*rhs.numPoints);
	for (int i = 0; i < rhs.numPoints; ++i)
	{
		this->x[i] = rhs.x[i];
		this->y[i] = rhs.y[i];
	}
	this->xStart = rhs.xStart;
	this->yStart = rhs.yStart;
	this->xStep = rhs.xStep;
	this->yStep = rhs.yStep;
}
#if 0
lefiGeomPolygonIter::~lefiGeomPolygonIter()
{
	if (this->numPoints != 0)
	{
		lefFree((double*)this->x);
		lefFree((double*)this->y);
	}
}
#endif 
lefiGeomVia::lefiGeomVia()
{
	name = 0;
	x = y = 0;
}
void lefiGeomVia::copy(lefiGeomVia const& rhs)
{
	this->name = (char*)lefMalloc(sizeof(char)*(strlen(rhs.name)+1));
	strcpy(this->name, CASE(rhs.name));
	this->x = rhs.x;
	this->y = rhs.y;
}
#if 0
lefiGeomVia::~lefiGeomVia()
{
	if (this->name) lefFree((double*)this->name);
}
#endif 
lefiGeomViaIter::lefiGeomViaIter()
{
	name = 0;
	x = y = 0;
	xStart = yStart = xStep = yStep = 0;
}
void lefiGeomViaIter::copy(lefiGeomViaIter const& rhs)
{
	this->name = (char*)lefMalloc(sizeof(char)*(strlen(rhs.name)+1));
	strcpy(this->name, CASE(rhs.name));
	this->x = rhs.x;
	this->y = rhs.y;
	this->xStart = rhs.xStart;
	this->yStart = rhs.yStart;
	this->xStep = rhs.xStep;
	this->yStep = rhs.yStep;
}
#if 0
lefiGeomViaIter::~lefiGeomViaIter()
{
	if (this->name) lefFree((double*)this->name);
}
#endif 
////////////////////////////////////////////
////////////////////////////////////////////
//
//  lefiGeometries
//
////////////////////////////////////////////
////////////////////////////////////////////


lefiGeometries::lefiGeometries() {
  this->lefiGeometries::Init();
}
lefiGeometries::lefiGeometries(lefiGeometries const& rhs)
{
	this->Init();
	this->copy(rhs);
}

void lefiGeometries::Init() {
  this->itemsAllocated_ = 2;
  this->numItems_ = 0;
  this->itemType_ = (enum lefiGeomEnum*)lefMalloc(sizeof(enum lefiGeomEnum)*2);
  this->items_ = (void**)lefMalloc(sizeof(void*)*2);
  this->numPoints_ = 0;
  this->pointsAllocated_ = 0;
  this->x_ = 0;
  this->y_ = 0;
  this->xStart_ = -1;
  this->yStart_ = -1;
  this->xStep_ = -1;
  this->yStep_ = -1;
}

void lefiGeometries::copy(lefiGeometries const& rhs)
{

	this->addStepPattern(rhs.xStart_, rhs.yStart_, rhs.xStep_, rhs.yStep_);

	for (int i = 0; i < rhs.numPoints_; ++i)
	{
		if (i == 0)
			this->startList(rhs.x_[i], rhs.y_[i]);
		else 
			this->addToList(rhs.x_[i], rhs.y_[i]);
	}

	for (int i = 0; i < rhs.numItems(); ++i) 
	{
		switch (rhs.lefiGeometries::itemType(i)) 
		{
			case lefiGeomLayerE:
				this->addLayer(rhs.getLayer(i));
				break;
			case lefiGeomLayerExceptPgNetE:
				if (rhs.hasLayerExceptPgNet(i))
					this->addLayerExceptPgNet();
				break;
			case lefiGeomLayerMinSpacingE:
				this->addLayerMinSpacing(rhs.getLayerMinSpacing(i));
				break;
			case lefiGeomLayerRuleWidthE:
				this->addLayerRuleWidth(rhs.getLayerRuleWidth(i));
				break;
			case lefiGeomWidthE:
				this->addWidth(rhs.getWidth(i));
				break;
			case lefiGeomPathE:
			{
				lefiGeomPath* p = (lefiGeomPath*)lefMalloc(sizeof(lefiGeomPath)); p->copy(*rhs.getPath(i));
				this->lefiGeometries::add((void*)p, lefiGeomPathE);
				break;
			}
			case lefiGeomPathIterE:
			{
				lefiGeomPathIter* p = (lefiGeomPathIter*)lefMalloc(sizeof(lefiGeomPathIter)); p->copy(*rhs.getPathIter(i));
				this->lefiGeometries::add((void*)p, lefiGeomPathIterE);
				break;
			}
			case lefiGeomRectE:
			{
				lefiGeomRect* p = (lefiGeomRect*)lefMalloc(sizeof(lefiGeomRect)); p->copy(*rhs.getRect(i));
				this->lefiGeometries::add((void*)p, lefiGeomRectE);
				break;
			}
			case lefiGeomRectIterE:
			{
				lefiGeomRectIter* p = (lefiGeomRectIter*)lefMalloc(sizeof(lefiGeomRectIter)); p->copy(*rhs.getRectIter(i));
				this->lefiGeometries::add((void*)p, lefiGeomRectIterE);
				break;
			}
			case lefiGeomPolygonE:
			{
				lefiGeomPolygon* p = (lefiGeomPolygon*)lefMalloc(sizeof(lefiGeomPolygon)); p->copy(*rhs.getPolygon(i));
				this->lefiGeometries::add((void*)p, lefiGeomPolygonE);
				break;
			}
			case lefiGeomPolygonIterE:
			{
				lefiGeomPolygonIter* p = (lefiGeomPolygonIter*)lefMalloc(sizeof(lefiGeomPolygonIter)); p->copy(*rhs.getPolygonIter(i));
				this->lefiGeometries::add((void*)p, lefiGeomPolygonIterE);
				break;
			}
			case lefiGeomViaE:
			{
				lefiGeomVia* p = (lefiGeomVia*)lefMalloc(sizeof(lefiGeomVia)); p->copy(*rhs.getVia(i));
				this->lefiGeometries::add((void*)p, lefiGeomViaE);
				break;
			}
			case lefiGeomViaIterE:
			{
				lefiGeomViaIter* p = (lefiGeomViaIter*)lefMalloc(sizeof(lefiGeomViaIter)); p->copy(*rhs.getViaIter(i));
				this->lefiGeometries::add((void*)p, lefiGeomViaIterE);
				break;
			}
			case lefiGeomClassE:
			{
				this->addClass(rhs.getClass(i));
				break;
			}
			default: lefiError("ERROR (LEFPARS-1375): unknown geometry type");
					 break;
		}
	}
}

void lefiGeometries::Destroy() {
  this->lefiGeometries::clear();
  lefFree((char*)(this->items_));
  lefFree((char*)(this->itemType_));
  if (this->x_)
    { lefFree((double*)(this->x_)); lefFree((double*)(this->y_)); }
  this->pointsAllocated_ = 0;
}


lefiGeometries::~lefiGeometries() {
  this->lefiGeometries::Destroy();
}


void lefiGeometries::clear() {
   int i;

   for (i = 0; i < this->numItems_; i++) {
     if (this->itemType_[i] == lefiGeomViaE)
       lefFree(((struct lefiGeomVia*)(this->items_[i]))->name);
     if (this->itemType_[i] == lefiGeomViaIterE)
       lefFree(((struct lefiGeomViaIter*)(this->items_[i]))->name);
     if (this->itemType_[i] == lefiGeomPathE) {
       ((struct lefiGeomPath*)(this->items_[i]))->numPoints = 0;
       lefFree((double*)((struct lefiGeomPath*)this->items_[i])->x);
       lefFree((double*)((struct lefiGeomPath*)this->items_[i])->y);
     }
     if (this->itemType_[i] == lefiGeomPathIterE) {
       ((struct lefiGeomPathIter*)(this->items_[i]))->numPoints = 0;
       lefFree((double*)((struct lefiGeomPathIter*)this->items_[i])->x);
       lefFree((double*)((struct lefiGeomPathIter*)this->items_[i])->y);
     }
     if (this->itemType_[i] == lefiGeomPolygonE) {
       ((struct lefiGeomPolygon*)(this->items_[i]))->numPoints = 0;
       lefFree((double*)((struct lefiGeomPolygon*)this->items_[i])->x);
       lefFree((double*)((struct lefiGeomPolygon*)this->items_[i])->y);
     }
     if (this->itemType_[i] == lefiGeomPolygonIterE) {
       ((struct lefiGeomPolygonIter*)(this->items_[i]))->numPoints = 0;
       lefFree((double*)((struct lefiGeomPolygonIter*)this->items_[i])->x);
       lefFree((double*)((struct lefiGeomPolygonIter*)this->items_[i])->y);
     }
     lefFree((char*)(this->items_[i]));
   }
   this->numItems_ = 0;
}


void lefiGeometries::clearPolyItems() {
  lefFree((char*)(this->items_));
  lefFree((char*)(this->itemType_));
  if (this->x_)
    { lefFree((double*)(this->x_)); lefFree((double*)(this->y_)); }
  this->numPoints_ = 0;
  this->pointsAllocated_ = 0;
  this->numItems_ = 0;
}


void lefiGeometries::add(void* v, enum lefiGeomEnum e) {
  if (this->numItems_ == this->itemsAllocated_) {
    int i;
    void** newi;
    enum lefiGeomEnum* newe;
    if (this->itemsAllocated_ == 0)   // 9/12/2002 - for C version
       this->itemsAllocated_ = 1;
    else
       this->itemsAllocated_ *= 2;
    newe = (enum lefiGeomEnum*)lefMalloc(sizeof(enum lefiGeomEnum)*this->itemsAllocated_);
    newi= (void**)lefMalloc(sizeof(void*)*this->itemsAllocated_);
    for (i = 0; i < this->numItems_; i++) {
      newe[i] = this->itemType_[i];
      newi[i] = this->items_[i];
    }
    lefFree((char*)(this->items_));
    lefFree((char*)(this->itemType_));
    this->items_ = newi;
    this->itemType_ = newe;
  }
  this->items_[this->numItems_] = v;
  this->itemType_[this->numItems_] = e;
  this->numItems_ += 1;
}


void lefiGeometries::addLayer(const char* name) {
  char* c = (char*)lefMalloc(strlen(name)+1);
  strcpy(c, CASE(name));
  this->lefiGeometries::add((void*)c, lefiGeomLayerE);
}


// 5.7
void lefiGeometries::addLayerExceptPgNet() {
  int* d = (int*)lefMalloc(sizeof(int));
  *d = 1;
  this->lefiGeometries::add((void*)d, lefiGeomLayerExceptPgNetE);
}


void lefiGeometries::addLayerMinSpacing(double spacing) {
  double* d = (double*)lefMalloc(sizeof(double));
  *d = spacing;
  this->lefiGeometries::add((void*)d, lefiGeomLayerMinSpacingE);
}


void lefiGeometries::addLayerRuleWidth(double width) {
  double* d = (double*)lefMalloc(sizeof(double));
  *d = width;
  this->lefiGeometries::add((void*)d, lefiGeomLayerRuleWidthE);
}


void lefiGeometries::addClass(const char* name) {
  char* c = (char*)lefMalloc(strlen(name)+1);
  strcpy(c, CASE(name));
  this->lefiGeometries::add((void*)c, lefiGeomClassE);
}


void lefiGeometries::addWidth(double w) {
  double* d = (double*)lefMalloc(sizeof(double));
  *d = w;
  this->lefiGeometries::add((void*)d, lefiGeomWidthE);
}


void lefiGeometries::addPath() {
  int i;
  int lim;
  struct lefiGeomPath* p = (struct lefiGeomPath*)lefMalloc(
                 sizeof(struct lefiGeomPath));
  lim = p->numPoints = this->numPoints_;
  if (lim > 0) {
     p->x = (double*)lefMalloc(sizeof(double)*lim);
     p->y = (double*)lefMalloc(sizeof(double)*lim);
     for (i = 0; i < lim; i++) {
       p->x[i] = this->x_[i];
       p->y[i] = this->y_[i];
     }
  } else {
     p->x = 0;
     p->y = 0;
  }
  this->lefiGeometries::add((void*)p, lefiGeomPathE);
}


void lefiGeometries::addPathIter() {
  int i;
  int lim;
  struct lefiGeomPathIter* p = (struct lefiGeomPathIter*)lefMalloc(
                 sizeof(struct lefiGeomPathIter));
  lim = p->numPoints = this->numPoints_;
  if (lim > 0) {
     p->x = (double*)lefMalloc(sizeof(double)*lim);
     p->y = (double*)lefMalloc(sizeof(double)*lim);
     for (i = 0; i < lim; i++) {
       p->x[i] = this->x_[i];
       p->y[i] = this->y_[i];
     }
  } else {
     p->x = 0;
     p->y = 0;
  }
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomPathIterE);
}


/* pcr 481783 & 560504 */
void lefiGeometries::addRect(double xl, double yl, double xh, double yh) {
  struct lefiGeomRect* p = (struct lefiGeomRect*)lefMalloc(
                 sizeof(struct lefiGeomRect));
  p->xl = xl;
  p->yl = yl;
  p->xh = xh;
  p->yh = yh;
  this->lefiGeometries::add((void*)p, lefiGeomRectE);
}


void lefiGeometries::addRectIter(double xl, double yl,
                                 double xh, double yh) {
  struct lefiGeomRectIter* p = (struct lefiGeomRectIter*)lefMalloc(
                 sizeof(struct lefiGeomRectIter));
  p->xl = xl;
  p->yl = yl;
  p->xh = xh;
  p->yh = yh;
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomRectIterE);
}

void lefiGeometries::addPolygon() {
  int i;
  int lim;
  struct lefiGeomPolygon* p = (struct lefiGeomPolygon*)lefMalloc(
                 sizeof(struct lefiGeomPolygon));
  lim = p->numPoints = this->numPoints_;
  if (lim > 0) {
     p->x = (double*)lefMalloc(sizeof(double)*lim);
     p->y = (double*)lefMalloc(sizeof(double)*lim);
     for (i = 0; i < lim; i++) {
       p->x[i] = this->x_[i];
       p->y[i] = this->y_[i];
     }
  } else {
     p->x = 0;
     p->y = 0;
  }
  this->lefiGeometries::add((void*)p, lefiGeomPolygonE);
}

void lefiGeometries::addPolygonIter() {
  int i;
  int lim;
  struct lefiGeomPolygonIter* p = (struct lefiGeomPolygonIter*)lefMalloc(
                 sizeof(struct lefiGeomPolygonIter));
  lim = p->numPoints = this->numPoints_;
  if (lim > 0) {
     p->x = (double*)lefMalloc(sizeof(double)*lim);
     p->y = (double*)lefMalloc(sizeof(double)*lim);
     for (i = 0; i < lim; i++) {
       p->x[i] = this->x_[i];
       p->y[i] = this->y_[i];
     }
  } else {
     p->x = 0;
     p->y = 0;
  }
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomPolygonIterE);
}


void lefiGeometries::addVia(double x, double y, const char* name) {
  struct lefiGeomVia* p = (struct lefiGeomVia*)lefMalloc(
                 sizeof(struct lefiGeomVia));
  char* c = (char*)lefMalloc(strlen(name)+1);
  strcpy(c, CASE(name));
  p->x = x;
  p->y = y;
  p->name = c;
  this->lefiGeometries::add((void*)p, lefiGeomViaE);
}


void lefiGeometries::addViaIter(double x, double y, const char* name) {
  struct lefiGeomViaIter* p = (struct lefiGeomViaIter*)lefMalloc(
                 sizeof(struct lefiGeomViaIter));
  char* c = (char*)lefMalloc(strlen(name)+1);
  strcpy(c, CASE(name));
  p->x = x;
  p->y = y;
  p->name = c;
  p->xStart = this->xStart_;
  p->yStart = this->yStart_;
  p->xStep = this->xStep_;
  p->yStep = this->yStep_;
  this->lefiGeometries::add((void*)p, lefiGeomViaIterE);
}


void lefiGeometries::addStepPattern(double xStart, double yStart,
                                    double xStep, double yStep) {
  this->xStart_ = xStart;
  this->yStart_ = yStart;
  this->xStep_ = xStep;
  this->yStep_ = yStep;
}


void lefiGeometries::startList(double x, double y) {
  if (!this->x_) {
    this->numPoints_ = 0;
    this->pointsAllocated_ = 16;
    this->x_ = (double*)lefMalloc(sizeof(double)*16);
    this->y_ = (double*)lefMalloc(sizeof(double)*16);
  } else {  // reset the numPonits to 0
    this->numPoints_ = 0;
  }
  this->lefiGeometries::addToList(x, y);
}


void lefiGeometries::addToList(double x, double  y) {
  if (this->numPoints_ == this->pointsAllocated_) {
    int i;
    double* nx;
    double* ny;
    if (this->pointsAllocated_ == 0)
       this->pointsAllocated_ = 2;
    else
       this->pointsAllocated_ *= 2;
    nx = (double*)lefMalloc(sizeof(double)*this->pointsAllocated_);
    ny = (double*)lefMalloc(sizeof(double)*this->pointsAllocated_);
    for (i = 0; i < this->numPoints_; i++) {
      nx[i] = this->x_[i];
      ny[i] = this->y_[i];
    }
    lefFree((char*)(this->x_));
    lefFree((char*)(this->y_));
    this->x_ = nx;
    this->y_ = ny;
  }
  this->x_[this->numPoints_] = x;
  this->y_[this->numPoints_] = y;
  this->numPoints_ += 1;
}


int lefiGeometries::numItems() const {
  return this->numItems_;
}


enum lefiGeomEnum lefiGeometries::itemType(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1360): The index number %d given for the geometry item is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return lefiGeomUnknown;
  }
  return this->itemType_[index];
}


struct lefiGeomRect* lefiGeometries::getRect(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1361): The index number %d given for the geometry RECTANGLE is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomRect*)(this->items_[index]);
}


struct lefiGeomRectIter* lefiGeometries::getRectIter(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1362): The index number %d given for the geometry RECTANGLE ITERATE is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomRectIter*)(this->items_[index]);
}


struct lefiGeomPath* lefiGeometries::getPath(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1363): The index number %d given for the geometry PATH is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomPath*)(this->items_[index]);
}


struct lefiGeomPathIter* lefiGeometries::getPathIter(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1364): The index number %d given for the geometry PATH ITERATE is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomPathIter*)(this->items_[index]);
}


char* lefiGeometries::getLayer(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1365): The index number %d given for the geometry LAYER is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (char*)(this->items_[index]);
}


// 5.7
int lefiGeometries::hasLayerExceptPgNet(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1366): The index number %d given for the geometry LAYER EXCEPT PG NET is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return *((int*)(this->items_[index]));
}


double lefiGeometries::getLayerMinSpacing(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1367): The index number %d given for the geometry LAYER MINSPACING is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return *((double*)(this->items_[index]));
}


double lefiGeometries::getLayerRuleWidth(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1368): The index number %d given for the geometry LAYER RULE WIDTH is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return *((double*)(this->items_[index]));
}


double lefiGeometries::getWidth(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1369): The index number %d given for the geometry WIDTH is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return *((double*)(this->items_[index]));
}


struct lefiGeomPolygon* lefiGeometries::getPolygon(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1370): The index number %d given for the geometry POLYGON is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomPolygon*)(this->items_[index]);
}


struct lefiGeomPolygonIter* lefiGeometries::getPolygonIter(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1371): The index number %d given for the geometry POLYGON ITERATE is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomPolygonIter*)(this->items_[index]);
}


char* lefiGeometries::getClass(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1372): The index number %d given for the geometry CLASS is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (char*)(this->items_[index]);
}


struct lefiGeomVia* lefiGeometries::getVia(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1373): The index number %d given for the geometry VIA is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomVia*)(this->items_[index]);
}


struct lefiGeomViaIter* lefiGeometries::getViaIter(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numItems_) {
    sprintf (msg, "ERROR (LEFPARS-1374): The index number %d given for the geometry VIA ITERATE is invalid.\nValid index is from 0 to %d", index, this->numItems_);
    lefiError (msg);
    return 0;
  }
  return (struct lefiGeomViaIter*)(this->items_[index]);
}


void lefiGeometries::print(FILE* f) const {
  int i;
  int l;
  struct lefiGeomRect* rect;
  struct lefiGeomRectIter* rectiter;
  struct lefiGeomPath* path;
  struct lefiGeomPathIter* pathiter;
  struct lefiGeomPolygon* polygon;
  struct lefiGeomPolygonIter* polygoniter;

  for (i = 0; i < this->numItems_; i++) {
    switch (this->lefiGeometries::itemType(i)) {

    case lefiGeomLayerE:
         fprintf(f, "Layer %s\n", this->lefiGeometries::getLayer(i));
         break;

    case lefiGeomLayerExceptPgNetE:
         if (this->lefiGeometries::getLayerMinSpacing(i))
                 fprintf(f, "EXCEPTPGNET \n");
         break;

    case lefiGeomLayerMinSpacingE:
         fprintf(f, "Spacing %g\n",
                 this->lefiGeometries::getLayerMinSpacing(i));
         break;

    case lefiGeomLayerRuleWidthE:
         fprintf(f, "DesignRuleWidth %g\n",
                 this->lefiGeometries::getLayerRuleWidth(i));
         break;

    case lefiGeomWidthE:
         fprintf(f, "Width %g\n", this->lefiGeometries::getWidth(i));
         break;

    case lefiGeomPathE:
         path = this->lefiGeometries::getPath(i);
         fprintf(f, "Path");
         for (l = 0; l < path->numPoints; l++)
           fprintf(f, " %g,%g", path->x[l], path->y[l]);
         fprintf(f, "\n");
         break;

    case lefiGeomPathIterE:
         pathiter = this->lefiGeometries::getPathIter(i);
         fprintf(f, "Path iter  start %g,%g  step %g,%g\n",
         pathiter->xStart, pathiter->yStart,
         pathiter->xStep, pathiter->yStep);
         for (l = 0; l < pathiter->numPoints; l++)
           fprintf(f, " %g,%g", pathiter->x[l], pathiter->y[l]);
         fprintf(f, "\n");
         break;

    case lefiGeomRectE:
         rect = this->lefiGeometries::getRect(i);
         fprintf(f, "Rect %g,%g  %g,%g\n", rect->xl, rect->yl,
         rect->xh, rect->yh);
         break;

    case lefiGeomRectIterE:
         rectiter = this->lefiGeometries::getRectIter(i);
         fprintf(f, "Rect iter  start %g,%g  step %g,%g\n",
         rectiter->xStart, rectiter->yStart,
         rectiter->xStep, rectiter->yStep);
         fprintf(f, "     %g,%g  %g,%g\n", rectiter->xl, rectiter->yl,
         rectiter->xh, rectiter->yh);
         break;

    case lefiGeomPolygonE:
         polygon = this->lefiGeometries::getPolygon(i);
         fprintf(f, "Polygon");
         for (l = 0; l < polygon->numPoints; l++)
           fprintf(f, " %g,%g", polygon->x[l], polygon->y[l]);
         fprintf(f, "\n");
         break;

    case lefiGeomPolygonIterE:
         polygoniter = this->lefiGeometries::getPolygonIter(i);
         fprintf(f, "Polygon iter  start %g,%g  step %g,%g\n",
         polygoniter->xStart, polygoniter->yStart,
         polygoniter->xStep, polygoniter->yStep);
         for (l = 0; l < polygoniter->numPoints; l++)
           fprintf(f, " %g,%g", polygoniter->x[l], polygoniter->y[l]);
         fprintf(f, "\n");
         break;

    case lefiGeomViaE:
         fprintf(f, "Via \n");
         break;

    case lefiGeomViaIterE:
         fprintf(f, "Via iter \n");
         break;

    case lefiGeomClassE:
         fprintf(f, "Classtype %s\n", (char*)this->items_[i]);
         break;

    default: lefiError("ERROR (LEFPARS-1375): unknown geometry type");
          fprintf(f, "Unknown geometry type %d\n",
          (int)(this->lefiGeometries::itemType(i)));
          break;
    }
  }
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiSpacing
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiSpacing::lefiSpacing() {
  this->lefiSpacing::Init();
}


void lefiSpacing::Init() {
  this->name1_ = (char*)lefMalloc(16);
  this->name2_ = (char*)lefMalloc(16);
  this->name1Size_ = 16;
  this->name2Size_ = 16;
  this->distance_ = 0;
  this->hasStack_ = 0;
}


void lefiSpacing::Destroy() {
  if (this->name1_)
     lefFree(this->name1_);
  if (this->name2_)
     lefFree(this->name2_);
}


lefiSpacing::~lefiSpacing() {
  this->lefiSpacing::Destroy();
}


lefiSpacing* lefiSpacing::clone() const {
  lefiSpacing* sp = (lefiSpacing*)lefMalloc(sizeof(lefiSpacing));
  sp->name1Size_ = strlen(this->name1_)+1;
  sp->name1_ = (char*)lefMalloc(sp->name1Size_);
  strcpy(sp->name1_, this->name1_);
  sp->name2Size_ = strlen(this->name2_)+1;
  sp->name2_ = (char*)lefMalloc(sp->name2Size_);
  strcpy(sp->name2_, this->name2_);
  sp->distance_ = this->distance_;
  sp->hasStack_ = this->hasStack_;
  return sp;
}


void lefiSpacing::set(const char* name1, const char* name2, double d,
     int hasStack) {
  int len = strlen(name1) + 1;
  if (len > this->name1Size_) {
    lefFree(this->name1_);
    this->name1_ = (char*)lefMalloc(len);
    this->name1Size_ = len;
  }
  len = strlen(name2) + 1;
  if (len > this->name2Size_) {
    lefFree(this->name2_);
    this->name2_ = (char*)lefMalloc(len);
    this->name2Size_ = len;
  }
  strcpy(this->name1_, CASE(name1));
  strcpy(this->name2_, CASE(name2));
  this->distance_ = d;
  this->hasStack_ = hasStack;
}


const char* lefiSpacing::name1() const {
  return this->name1_;
}


const char* lefiSpacing::name2() const {
  return this->name2_;
}


double lefiSpacing::distance() const {
  return this->distance_;
}


int lefiSpacing::hasStack() const {
  return this->hasStack_;
}



void lefiSpacing::print(FILE* f) const {
  fprintf(f, "SPACING %s %s %g", this->lefiSpacing::name1(),
      this->lefiSpacing::name2(), this->lefiSpacing::distance());

  if (this->lefiSpacing::hasStack())
    fprintf(f, "  STACK");

  fprintf(f, "\n");
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiIRDrop
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiIRDrop::lefiIRDrop() {
  this->lefiIRDrop::Init();
}


void lefiIRDrop::Init() {
  this->nameSize_ = 16;
  this->value1Size_ = 16;
  this->value2Size_ = 16;
  this->name_ = (char*)lefMalloc(16);
  this->numValues_ = 0;
  this->valuesAllocated_ = 2;
  this->value1_ = (double*)lefMalloc(sizeof(double)*2);
  this->value2_ = (double*)lefMalloc(sizeof(double)*2);
}


void lefiIRDrop::Destroy() {
  lefFree(this->name_);
  this->lefiIRDrop::clear();
  lefFree((char*)(this->value1_)); 
  lefFree((char*)(this->value2_)); 
}


lefiIRDrop::~lefiIRDrop() {
  this->lefiIRDrop::Destroy();
}


void lefiIRDrop::clear() {
  this->numValues_ = 0;
}


void lefiIRDrop::setTableName(const char* name) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
  this->lefiIRDrop::clear();
/*
  *(this->value1_) = '\0';
  *(this->value2_) = '\0';
*/
}


void lefiIRDrop::setValues(double value1, double value2) {
  if (this->numValues_ == this->valuesAllocated_) {
    int i;
    double* v1;
    double* v2;
    if (this->valuesAllocated_ == 0) {
      this->valuesAllocated_ = 2;
      v1 = (double*)lefMalloc(sizeof(double)*this->valuesAllocated_);
      v2 = (double*)lefMalloc(sizeof(double)*this->valuesAllocated_);
    }
    else {
      this->valuesAllocated_ *= 2;
      v1 = (double*)lefMalloc(sizeof(double)*this->valuesAllocated_);
      v2 = (double*)lefMalloc(sizeof(double)*this->valuesAllocated_);
      for (i = 0; i < this->numValues_; i++) {
        v1[i] = this->value1_[i];
        v2[i] = this->value2_[i];
      }
      lefFree((char*)(this->value1_));
      lefFree((char*)(this->value2_));
    }
    this->value1_ = v1;
    this->value2_ = v2;
  }
  this->value1_[this->numValues_] = value1;
  this->value2_[this->numValues_] = value2;
  this->numValues_ += 1;
}


const char* lefiIRDrop::name() const {
  return this->name_;
}


int lefiIRDrop::numValues() const {
  return this->numValues_;
}


double lefiIRDrop::value1(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numValues_) {
    sprintf (msg, "ERROR (LEFPARS-1376): The index number %d given for the IRDROP is invalid.\nValid index is from 0 to %d", index, this->numValues_);
    lefiError (msg);
    return 0;
  }
  return this->value1_[index];
}


double lefiIRDrop::value2(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numValues_) {
    sprintf (msg, "ERROR (LEFPARS-1376): The index number %d given for the IRDROP is invalid.\nValid index is from 0 to %d", index, this->numValues_);
    lefiError (msg);
    return 0;
  }
  return this->value2_[index];
}


void lefiIRDrop::print(FILE* f) const {
  int i;
  fprintf(f, "IRDROP %s ", this->lefiIRDrop::name());
  for (i = 0; i < this->lefiIRDrop::numValues(); i++) {
     fprintf(f, "%g %g ", this->lefiIRDrop::value1(i),
             this->lefiIRDrop::value2(i));
  }
  fprintf(f, "\n");
  fprintf(f, "END IRDrop\n");
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefitMinFeature
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////



lefiMinFeature::lefiMinFeature() {
  this->lefiMinFeature::Init();
}



void lefiMinFeature::Init() {
  // nothing to do
}



void lefiMinFeature::Destroy() {
  // nothing to do
}



lefiMinFeature::~lefiMinFeature() {
  this->lefiMinFeature::Destroy();
}



void lefiMinFeature::set(double one, double two) {
  this->one_ = one;
  this->two_ = two;
}



double lefiMinFeature::one() const {
  return this->one_;
}



double lefiMinFeature::two() const {
  return this->two_;
}



void lefiMinFeature::print(FILE* f) const {
  fprintf(f, "MINfEATURE %g %g\n", this->lefiMinFeature::one(),
     this->lefiMinFeature::two());
}



////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiSite
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiSite::lefiSite() {
  this->lefiSite::Init();
}


void lefiSite::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);
  this->numRowPattern_ = 0;
  this->rowPatternAllocated_ = 0;
  this->siteNames_ = 0;
  this->siteOrients_ = 0;
}


void lefiSite::Destroy() {
  int i;

  lefFree(this->name_);
  if (this->numRowPattern_) {
     for (i = 0; i < this->numRowPattern_; i++)
        lefFree(this->siteNames_[i]);
     lefFree(this->siteNames_);
     lefFree(this->siteOrients_);
     this->numRowPattern_ = 0;
  }
}


lefiSite::~lefiSite() {
  this->lefiSite::Destroy();
}


void lefiSite::setName(const char* name) {
  int i;
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));
  this->hasClass_ = 0;
  *(this->siteClass_) = 0;
  this->hasSize_ = 0;
  this->symmetry_ = 0;
  if (this->numRowPattern_) {
     for (i = 0; i < this->numRowPattern_; i++)
        lefFree(this->siteNames_[i]);
     this->numRowPattern_ = 0;
  }
}


void lefiSite::setClass(const char* cls) {
  strcpy(this->siteClass_, cls);
  this->hasClass_ = 1;
}


void lefiSite::setSize(double x, double y) {
  this->hasSize_ = 1;
  this->sizeX_ = x;
  this->sizeY_ = y;
}


void lefiSite::setXSymmetry() {
  this->symmetry_ |= 1;
}


void lefiSite::setYSymmetry() {
  this->symmetry_ |= 2;
}


void lefiSite::set90Symmetry() {
  this->symmetry_ |= 4;
}


void lefiSite::addRowPattern(const char* name, int orient) {
  if (this->numRowPattern_ == this->rowPatternAllocated_) {
    int    i;
    char** sn;
    int*   so;

    this->rowPatternAllocated_ = (this->rowPatternAllocated_ == 0) ? 2 :
          this->rowPatternAllocated_ * 2;
    sn = (char**)lefMalloc(sizeof(char*)*this->rowPatternAllocated_); 
    so = (int*)lefMalloc(sizeof(int)*this->rowPatternAllocated_); 
    for (i = 0; i < this->numRowPattern_; i++){
       sn[i] = this->siteNames_[i];
       so[i] = this->siteOrients_[i];
    }
    if (this->siteNames_) {
       lefFree((char*)(this->siteNames_)); 
       lefFree((char*)(this->siteOrients_)); 
    }
    this->siteNames_ = sn;
    this->siteOrients_ = so;
  }  
  this->siteNames_[this->numRowPattern_] = strdup(name);
  this->siteOrients_[this->numRowPattern_] = orient;
  this->numRowPattern_ += 1;
}


const char* lefiSite::name() const {
  return this->name_;
}


int lefiSite::hasClass() const {
  return this->hasClass_;
}


const char* lefiSite::siteClass() const {
  return this->siteClass_;
}


double lefiSite::sizeX() const {
  return this->sizeX_;
}


double lefiSite::sizeY() const {
  return this->sizeY_;
}


int lefiSite::hasSize() const {
  return this->hasSize_;
}


int lefiSite::hasXSymmetry() const {
  return (this->symmetry_ & 1) ? 1 : 0 ;
}


int lefiSite::hasYSymmetry() const {
  return (this->symmetry_ & 2) ? 1 : 0 ;
}


int lefiSite::has90Symmetry() const {
  return (this->symmetry_ & 4) ? 1 : 0 ;
}


int lefiSite::hasRowPattern() const {
  return (this->numRowPattern_) ? 1 : 0 ;
}


int lefiSite::numSites() const {
  return (this->numRowPattern_);
}


char* lefiSite::siteName(int index) const {
  return (this->siteNames_[index]);
}


int lefiSite::siteOrient(int index) const {
  return (this->siteOrients_[index]);
}


char* lefiSite::siteOrientStr(int index) const {
  return (lefiOrientStr(this->siteOrients_[index]));
}


void lefiSite::print(FILE* f) const {
  fprintf(f, "SITE %s", this->lefiSite::name());

  if (this->lefiSite::hasClass())
    fprintf(f, " CLASS %s", this->lefiSite::siteClass());

  if (this->lefiSite::hasSize())
    fprintf(f, " SIZE %g %g", this->lefiSite::sizeX(),
       this->lefiSite::sizeY());

  if (this->lefiSite::hasXSymmetry())
    fprintf(f, " SYMMETRY X");

  if (this->lefiSite::hasYSymmetry())
    fprintf(f, " SYMMETRY Y");

  if (this->lefiSite::has90Symmetry())
    fprintf(f, " SYMMETRY R90");

  fprintf(f, "\n");
}




////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiSitePattern
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiSitePattern::lefiSitePattern() {
  this->lefiSitePattern::Init();
}


void lefiSitePattern::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);
}


void lefiSitePattern::Destroy() {
  lefFree(this->name_);
}


lefiSitePattern::~lefiSitePattern() {
  this->lefiSitePattern::Destroy();
}


void lefiSitePattern::set(const char* name, double x, double y, int orient,
      double xStart, double yStart, double xStep, double yStep) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));

  this->x_ = x;
  this->y_ = y;
  this->xStep_ = xStep;
  this->yStep_ = yStep;
  this->xStart_ = xStart;
  this->yStart_ = yStart;
  this->orient_ = orient;
}


const char* lefiSitePattern::name() const {
  return this->name_;
}


int lefiSitePattern::orient() const {
  return this->orient_;
}


const char* lefiSitePattern::orientStr() const {
  return (lefiOrientStr(this->orient_));
}


double lefiSitePattern::x() const {
  return this->x_;
}


double lefiSitePattern::y() const {
  return this->y_;
}


int lefiSitePattern::hasStepPattern() const {
  if (this->xStart_ == -1 && this->yStart_ == -1 &&
      this->xStep_ == -1 && this->yStep_ == -1)
     return 0;
  return 1;
}


double lefiSitePattern::xStart() const {
  return this->xStart_;
}


double lefiSitePattern::yStart() const {
  return this->yStart_;
}


double lefiSitePattern::xStep() const {
  return this->xStep_;
}


double lefiSitePattern::yStep() const {
  return this->yStep_;
}


void lefiSitePattern::print(FILE* f) const {
  fprintf(f, "  SITE Pattern %s  %g,%g %s\n",
    this->lefiSitePattern::name(),
    this->lefiSitePattern::x(),
    this->lefiSitePattern::y(),
    this->lefiSitePattern::orientStr());
  fprintf(f, "    %g,%g step %g,%g\n", 
    this->lefiSitePattern::xStart(),
    this->lefiSitePattern::yStart(),
    this->lefiSitePattern::xStep(),
    this->lefiSitePattern::yStep());
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiTrackPattern
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiTrackPattern::lefiTrackPattern() {
  this->lefiTrackPattern::Init();
}


void lefiTrackPattern::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);
  this->start_ = 0;
  this->numTracks_ = 0;
  this->space_ = 0;
  this->numLayers_ = 0;
  this->layerAllocated_ = 2;
  this->layerNames_ = (char**) lefMalloc(sizeof(char*)*2);
  this->lefiTrackPattern::clear();
}


void lefiTrackPattern::Destroy() {
  if (this->name_) lefFree(this->name_);
  this->lefiTrackPattern::clear();
  this->name_ = 0;
  this->start_ = 0;
  this->numTracks_ = 0;
  this->space_ = 0;
  lefFree((char*)(this->layerNames_));
}


void lefiTrackPattern::clear() {
  int i;
  for (i = 0; i < this->numLayers_; i++) {
    lefFree(this->layerNames_[i]);
  }
}


lefiTrackPattern::~lefiTrackPattern() {
  this->lefiTrackPattern::Destroy();
}


void lefiTrackPattern::set(const char* name, double start, int numTracks,
      double space) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));

  this->start_ = start;
  this->numTracks_ = numTracks;
  this->space_ = space;
}


void lefiTrackPattern::addLayer(const char* name) {
  int len;
  if (this->numLayers_ == this->layerAllocated_) {
    int i;
    char** nn;

    if (this->layerAllocated_ == 0)
      this->layerAllocated_ = 2;
    else
      this->layerAllocated_ *= 2;
    nn = (char**) lefMalloc(sizeof(char*)*this->layerAllocated_);
    for (i = 0; i < this->numLayers_; i++)
      nn[i] = this->layerNames_[i];
    lefFree((char*)(this->layerNames_));
    this->layerNames_ = nn;
  }
  len = strlen(name) + 1;
  this->layerNames_[this->numLayers_] = (char*)lefMalloc(len);
  strcpy(this->layerNames_[this->numLayers_], CASE(name));
  this->numLayers_ += 1;
}


const char* lefiTrackPattern::name() const {
  return this->name_;
}


double lefiTrackPattern::start() const {
  return this->start_;
}


int lefiTrackPattern::numTracks() const {
  return this->numTracks_;
}


double lefiTrackPattern::space() const {
  return this->space_;
}


int lefiTrackPattern::numLayers() const {
  return this->numLayers_;
}


const char* lefiTrackPattern::layerName(int index) const {
  char msg[160];
  if (index < 0 || index >= this->numLayers_) {
    sprintf (msg, "ERROR (LEFPARS-1377): The index number %d given for the TRACK PATTERN  is invalid.\nValid index is from 0 to %d", index, this->numLayers_);
    lefiError (msg);
    return 0;
  }
  return this->layerNames_[index];
}


void lefiTrackPattern::print(FILE* f) const {
  int i;
  fprintf(f, "  TRACK Pattern %s  %g DO %d STEP %g\n",
    this->lefiTrackPattern::name(),
    this->lefiTrackPattern::start(),
    this->lefiTrackPattern::numTracks(),
    this->lefiTrackPattern::space());
  if (this->lefiTrackPattern::numLayers() > 0) {
    fprintf(f, "    LAYER "); 
    for (i = 0; i < this->lefiTrackPattern::numLayers(); i++) 
      fprintf(f, "%s ", this->lefiTrackPattern::layerName(i));
    fprintf(f, "\n");
  }
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiGcellPattern
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiGcellPattern::lefiGcellPattern() {
  this->lefiGcellPattern::Init();
}


void lefiGcellPattern::Init() {
  this->nameSize_ = 16;
  this->name_ = (char*)lefMalloc(16);
  this->start_ = 0;
  this->numCRs_ = 0;
  this->space_ = 0;
}


void lefiGcellPattern::Destroy() {
  if (this->name_) lefFree(this->name_);
  this->name_ = 0;
  this->start_ = 0;
  this->numCRs_ = 0;
  this->space_ = 0;
}


lefiGcellPattern::~lefiGcellPattern() {
  this->lefiGcellPattern::Destroy();
}


void lefiGcellPattern::set(const char* name, double start, int numCRs,
      double space) {
  int len = strlen(name) + 1;
  if (len > this->nameSize_) {
    lefFree(this->name_);
    this->name_ = (char*)lefMalloc(len);
    this->nameSize_ = len;
  }
  strcpy(this->name_, CASE(name));

  this->start_ = start;
  this->numCRs_ = numCRs;
  this->space_ = space;
}


const char* lefiGcellPattern::name() const {
  return this->name_;
}


double lefiGcellPattern::start() const {
  return this->start_;
}


int lefiGcellPattern::numCRs() const {
  return this->numCRs_;
}


double lefiGcellPattern::space() const {
  return this->space_;
}


void lefiGcellPattern::print(FILE* f) const {
  fprintf(f, "  TRACK Pattern %s  %g DO %d STEP %g\n",
    this->lefiGcellPattern::name(),
    this->lefiGcellPattern::start(),
    this->lefiGcellPattern::numCRs(),
    this->lefiGcellPattern::space());
}

#if 0
////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiAlias_itr
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiAlias_itr::lefiAlias_itr() {
  this->lefiAlias_itr::Init();
}


void lefiAlias_itr::Init() {
  this->num_ = -1;
}
 

void lefiAlias_itr::Destroy() {
}


lefiAlias_itr::~lefiAlias_itr() {
  this->lefiAlias_itr::Destroy();
}

 
int lefiAlias_itr::Next() {
  while (1) {
    this->num_ += 1;
    if (this->num_ <= -1 || this->num_ >= aliasLimit()) return 0;
    if (aliasKey(this->num_)) break;
  }
  return 1;
}


const char* lefiAlias_itr::Key() {
  if (this->num_ <= -1 || this->num_ >= aliasLimit()) return 0;
  return aliasKey(this->num_);
}


const char* lefiAlias_itr::Data() {
  if (this->num_ <= -1 || this->num_ >= aliasLimit()) return 0;
  return aliasData(this->num_);
}
#endif 
 
////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiUseMinSpacing
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiUseMinSpacing::lefiUseMinSpacing() {
  this->lefiUseMinSpacing::Init();
}


void lefiUseMinSpacing::Init() {
  this->name_ = 0;
  this->value_ = 0;
}


void lefiUseMinSpacing::Destroy() {
  if (this->name_)
     lefFree(this->name_);
}


lefiUseMinSpacing::~lefiUseMinSpacing() {
  this->lefiUseMinSpacing::Destroy();
}


void lefiUseMinSpacing::set(const char* name, int value) {

  this->lefiUseMinSpacing::Destroy();  // lefFree previous name, if there is any
  this->name_ = (char*)lefMalloc(strlen(name)+1);
  strcpy(this->name_, CASE(name));
  this->value_ = value;
}


const char* lefiUseMinSpacing::name() const {
  return this->name_;
}


int lefiUseMinSpacing::value() const {
  return this->value_;
}


void lefiUseMinSpacing::print(FILE* f) const {
  fprintf(f, "USEMINSPACING %s %d\n",
          this->lefiUseMinSpacing::name(),
          this->lefiUseMinSpacing::value());
}


////////////////////////////////////////////////////
////////////////////////////////////////////////////
//
//    lefiMaxStackVia
//
////////////////////////////////////////////////////
////////////////////////////////////////////////////


lefiMaxStackVia::lefiMaxStackVia() {
  this->lefiMaxStackVia::Init();
}


void lefiMaxStackVia::Init() {
  this->bottomLayer_ = 0;
  this->topLayer_ = 0;
  this->hasRange_ = 0;
  this->value_ = 0;
#if 0
  this->hasRange_ = 0;
  this->value_ = 0;
  if (this->bottomLayer_)          // This is for C version, since C will
     lefFree(this->bottomLayer_);  // call this function before calling
  if (this->topLayer_)             // setMaxStackViaRange when more than 1 lef
     lefFree(this->topLayer_);     // files are parse. C++ skips this function
  this->bottomLayer_ = 0;
  this->topLayer_ = 0;
#endif 
}


void lefiMaxStackVia::Destroy() {
  if (this->bottomLayer_)
     lefFree(this->bottomLayer_);
  if (this->topLayer_)
     lefFree(this->topLayer_);
  this->bottomLayer_ = 0;
  this->topLayer_ = 0;
  this->hasRange_ = 0;
  this->value_ = 0;
}


lefiMaxStackVia::~lefiMaxStackVia() {
  this->lefiMaxStackVia::Destroy();
}


void lefiMaxStackVia::setMaxStackVia(int value) {
  this->value_ = value;
}


void lefiMaxStackVia::setMaxStackViaRange(const char* bottomLayer,
                                          const char* topLayer) {
  this->hasRange_ = 1;
  if (this->bottomLayer_)          // May be lefrReset is called and 
     lefFree(this->bottomLayer_);  // bottomLayer_ and/or topLayer_ have
  if (this->topLayer_)             // value malloc on them
     lefFree(this->topLayer_);
  this->bottomLayer_ = (char*)lefMalloc(strlen(bottomLayer) + 1);
  strcpy(this->bottomLayer_, CASE(bottomLayer));
  this->topLayer_ = (char*)lefMalloc(strlen(topLayer) + 1);
  strcpy(this->topLayer_, CASE(topLayer));
  //this->bottomLayer_ = strdup(bottomLayer);
  //this->topLayer_    = strdup(topLayer);
}


int lefiMaxStackVia::maxStackVia() const {
  return this->value_;
}


int lefiMaxStackVia::hasMaxStackViaRange() const {
  return this->hasRange_ ? 1 : 0;
}


const char* lefiMaxStackVia::maxStackViaBottomLayer() const {
  return this->bottomLayer_;
}


const char* lefiMaxStackVia::maxStackViaTopLayer() const {
  return this->topLayer_;
}


void lefiMaxStackVia::print(FILE* f) const {
  fprintf(f, "MAXVIASTACK %d", this->lefiMaxStackVia::maxStackVia());
  if (this->lefiMaxStackVia::hasMaxStackViaRange())
     fprintf(f, " RANGE %s %s", this->lefiMaxStackVia::maxStackViaBottomLayer(),
                this->lefiMaxStackVia::maxStackViaTopLayer());
  fprintf(f, "\n");
}

} // namespace LefParser
