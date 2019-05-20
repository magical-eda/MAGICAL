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
 * @file lefiMisc.hpp
 */

#ifndef lefiMisc_h
#define lefiMisc_h

#include <stdio.h>
//#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>

namespace LefParser {

// The different types of items in a geometry list.

struct lefiGeomRect 
{
	lefiGeomRect();
	void copy(lefiGeomRect const& rhs);
	//~lefiGeomRect();
	double xl;
	double yl;
	double xh;
	double yh;
};

struct lefiGeomRectIter 
{
	lefiGeomRectIter();
	void copy(lefiGeomRectIter const& rhs);
	//~lefiGeomRectIter();
	double xl;
	double yl;
	double xh;
	double yh;
	double xStart;
	double yStart;
	double xStep;
	double yStep;
};

struct lefiGeomPath 
{
	lefiGeomPath();
	void copy(lefiGeomPath const& rhs);
	//~lefiGeomPath();
	int numPoints;
	double* x;
	double* y;
};

struct lefiGeomPathIter 
{
	lefiGeomPathIter();
	void copy(lefiGeomPathIter const& rhs);
	//~lefiGeomPathIter();
	int numPoints;
	double* x;
	double* y;
	double xStart;
	double yStart;
	double xStep;
	double yStep;
};

struct lefiGeomPolygon 
{
	lefiGeomPolygon();
	void copy(lefiGeomPolygon const& rhs);
	//~lefiGeomPolygon();
	int numPoints;
	double* x;
	double* y;
};

struct lefiGeomPolygonIter 
{
	lefiGeomPolygonIter();
	void copy(lefiGeomPolygonIter const& rhs);
	//~lefiGeomPolygonIter();
	int numPoints;
	double* x;
	double* y;
	double xStart;
	double yStart;
	double xStep;
	double yStep;
};

enum lefiGeomEnum {
  lefiGeomUnknown = 0,
  lefiGeomLayerE,
  lefiGeomLayerExceptPgNetE,
  lefiGeomLayerMinSpacingE,
  lefiGeomLayerRuleWidthE,
  lefiGeomWidthE,
  lefiGeomPathE,
  lefiGeomPathIterE,
  lefiGeomRectE,
  lefiGeomRectIterE,
  lefiGeomPolygonE,
  lefiGeomPolygonIterE,
  lefiGeomViaE,
  lefiGeomViaIterE,
  lefiGeomClassE,
  lefiGeomEnd
};

struct lefiGeomVia 
{
	lefiGeomVia();
	void copy(lefiGeomVia const& rhs);
	//~lefiGeomVia();
	char* name;
	double x;
	double y;
};

struct lefiGeomViaIter 
{
	lefiGeomViaIter();
	void copy(lefiGeomViaIter const& rhs);
	//~lefiGeomViaIter();
	char* name;
	double x;
	double y;
	double xStart;
	double yStart;
	double xStep;
	double yStep;
};


class lefiGeometries {
public:
  lefiGeometries();
  lefiGeometries(lefiGeometries const& rhs); // copy constructor is necessary 
  void Init();
  void copy(lefiGeometries const& rhs);

  void Destroy();
  ~lefiGeometries();

  void clear();
  void clearPolyItems();
  void add(void* v, enum lefiGeomEnum e);
  void addLayer(const char* name);
  void addLayerExceptPgNet();                     // 5.7
  void addLayerMinSpacing(double spacing);
  void addLayerRuleWidth(double width);
  void addClass(const char* name);
  void addWidth(double w);
  void addPath();
  void addPathIter();
/*  pcr 481783 & 560504
*/
  void addRect(double xl, double yl, double xh, double yh);
  void addRectIter(double xl, double yl, double xh, double yh);
/*
  void addRect(char* tmpStr1, char* tmpStr2, char* tmpStr3, char* tmpStr4);
  void addRectIter(char* tmpStr1, char* tmpStr2, char* tmpStr3, char* tmpStr4);
*/
  void addPolygon();
  void addPolygonIter();
  void addVia(double x, double y, const char* name);
  void addViaIter(double x, double y, const char* name);
  void addStepPattern(double xStart, double yStart,
                      double xStep, double yStep);
  void startList(double x, double y);
  void addToList(double x, double y);

  int numItems() const;
  enum lefiGeomEnum itemType(int index) const;
  struct lefiGeomRect* getRect(int index) const;
  struct lefiGeomRectIter* getRectIter(int index) const;
  struct lefiGeomPath* getPath(int index) const;
  struct lefiGeomPathIter* getPathIter(int index) const;
  int    hasLayerExceptPgNet(int index) const ;     // 5.7
  char*  getLayer(int index) const;
  double getLayerMinSpacing(int index) const;
  double getLayerRuleWidth(int index) const;
  double getWidth(int index) const;
  struct lefiGeomPolygon* getPolygon(int index) const;
  struct lefiGeomPolygonIter* getPolygonIter(int index) const;
  char*  getClass(int index) const;
  struct lefiGeomVia* getVia(int index) const;
  struct lefiGeomViaIter* getViaIter(int index) const;

  void print(FILE* f) const;

protected:
  int numItems_;
  int itemsAllocated_;
  enum lefiGeomEnum* itemType_;
  void** items_;

  int numPoints_;
  int pointsAllocated_;
  double* x_;
  double* y_;

  double xStart_;
  double yStart_;
  double xStep_;
  double yStep_;
};

class lefiSpacing {
public:
  lefiSpacing();
  void Init();

  void Destroy();
  ~lefiSpacing();

  lefiSpacing* clone() const;

  void set(const char* name1, const char* name2, double num, int hasStack);

  int hasStack() const;

  const char* name1() const;
  const char* name2() const;
  double distance() const;

  // Debug print
  void print(FILE* f) const;

protected:
  int    name1Size_;
  int    name2Size_;
  char*  name1_;
  char*  name2_;
  double distance_;
  int    hasStack_;
};



class lefiIRDrop {
public:
  lefiIRDrop();
  void Init();

  void Destroy();
  ~lefiIRDrop();

  void clear();
  void setTableName(const char* name);
  void setValues(double name1, double name2);

  const char* name() const;
  double value1(int index) const;
  double value2(int index) const;

  int numValues() const;

  // Debug print
  void print(FILE* f) const;

protected:
  int     nameSize_;
  int     value1Size_;
  int     value2Size_;
  int     numValues_;
  int     valuesAllocated_;
  char*   name_;
  double* value1_;
  double* value2_;
};


class lefiMinFeature {
public:
  lefiMinFeature();
  void Init();

  void Destroy();
  ~lefiMinFeature();

  void set(double one, double two);

  double one() const;
  double two() const;

  // Debug print
  void print(FILE* f) const;

protected:
  double one_;
  double two_;
};


class lefiSite {
public:
  lefiSite();
  void Init();

  void Destroy();
  ~lefiSite();

  void setName(const char* name);
  void setClass(const char* cls);
  void setSize(double x, double y);
  void setXSymmetry();
  void setYSymmetry();
  void set90Symmetry();
  void addRowPattern(const char* name, int orient);

  const char* name() const;
  int hasClass() const;
  const char* siteClass() const;
  double sizeX() const;
  double sizeY() const;
  int hasSize() const;
  int hasXSymmetry() const;
  int hasYSymmetry() const;
  int has90Symmetry() const;
  int hasRowPattern() const;                 // 5.6
  int numSites() const;                      // 5.6
  char* siteName(int index) const;           // 5.6
  int   siteOrient(int index) const;         // 5.6
  char* siteOrientStr(int index) const;      // 5.6

  // Debug print
  void print(FILE* f) const;

protected:
  int    nameSize_;
  char*  name_;
  int    hasClass_;
  char   siteClass_[8];
  double sizeX_;
  double sizeY_;
  int    hasSize_;
  int    symmetry_;   // bit 0-x   bit 1-y   bit 2-90

  int    numRowPattern_;         // 5.6 ROWPATTERN
  int    rowPatternAllocated_;
  char** siteNames_;
  int*   siteOrients_;
};


class lefiSitePattern {
public:
  lefiSitePattern();
  void Init();

  void Destroy();
  ~lefiSitePattern();

  void set(const char* name, double x, double y, int orient,
	   double xStart, double yStart, double xStep, double yStep);

  const  char* name() const;
  int    orient() const;
  const  char* orientStr() const;
  double x() const;
  double y() const;
  int    hasStepPattern() const;    // 5.6
  double xStart() const;
  double yStart() const;
  double xStep() const;
  double yStep() const;

  // Debug print
  void print(FILE* f) const;

protected:
  int    nameSize_;
  char*  name_;
  int    orient_;
  double x_;
  double y_;
  double xStart_;
  double yStart_;
  double xStep_;
  double yStep_;
};


class lefiTrackPattern {
public:
  lefiTrackPattern();
  void Init();

  void Destroy();
  ~lefiTrackPattern();

  void clear();
  void set(const char* name, double start, int numTracks, double space);
  void addLayer(const char* name);

  const char* name() const;
  double start() const;
  int numTracks() const;
  double space() const;

  int numLayers() const;
  const char* layerName(int index) const;

  // Debug print
  void print(FILE* f) const;

protected:
  int    nameSize_;
  char*  name_;
  double start_;
  int    numTracks_;
  double space_;

  int    numLayers_;
  int    layerAllocated_;
  char** layerNames_;
};


class lefiGcellPattern {
public:
  lefiGcellPattern();
  void Init();

  void Destroy();
  ~lefiGcellPattern();

  void set(const char* name, double start, int numCRs, double space);

  const char* name() const;
  double start() const;
  int numCRs() const;
  double space() const;

  // Debug print
  void print(FILE* f) const;

protected:
  int    nameSize_;
  char*  name_;
  double start_;
  int    numCRs_;
  double space_;
};


#if 0
class lefiAlias_itr {
public:
  lefiAlias_itr();
  void Init();

  void Destroy();
  ~lefiAlias_itr();

  int Next();
  const char* Key();
  const char* Data();

protected:
  int num_;
};
#endif 

class lefiUseMinSpacing {
public:
  lefiUseMinSpacing();
  void Init();

  void Destroy();
  ~lefiUseMinSpacing();

  void set(const char* name, int value);

  const char* name() const;
  int   value() const;

  // Debug print
  void print(FILE* f) const;

protected:
  char* name_;
  int   value_;
};


// 5.5 for Maximum Stacked-via rule
class lefiMaxStackVia {
public:
  lefiMaxStackVia();
  void Init();

  void Destroy();
  ~lefiMaxStackVia();

  void clear();
  void setMaxStackVia(int value);
  void setMaxStackViaRange(const char* bottomLayer, const char* topLayer);

  int maxStackVia() const;
  int hasMaxStackViaRange() const;
  const char* maxStackViaBottomLayer() const;
  const char* maxStackViaTopLayer() const;

  // Debug print
  void print(FILE* f) const;

protected:
  int   value_;
  int   hasRange_;
  char* bottomLayer_;
  char* topLayer_;
};

} // namespace LefParser 

#endif

