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
 * @file lefiArray.hpp
 */

#ifndef lefiArray_h
#define lefiArray_h

#include <stdio.h>
//#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>
#include <limbo/parsers/lef/bison/lefiMisc.hpp>

namespace LefParser {

class lefiArrayFloorPlan {
public:
	lefiArrayFloorPlan();
  void Init(const char* name);
  void Destroy();
  void addSitePattern(const char* typ, lefiSitePattern* s);

  int numPatterns() const;
  lefiSitePattern* pattern(int index);
  char* typ(int index);
  const char* name();

protected:
  int numPatterns_;
  int patternsAllocated_;
  lefiSitePattern** patterns_;
  char** types_;
  char* name_;
};


class lefiArray {
public:
  lefiArray();
  void Init();

  void Destroy();
  ~lefiArray();

  void setName(const char* name);
  void addSitePattern(lefiSitePattern* s);
  void setTableSize(int tsize);
  void addDefaultCap(int minPins, double cap);
  void addCanPlace(lefiSitePattern* s);
  void addCannotOccupy(lefiSitePattern* s);
  void addTrack(lefiTrackPattern* t);
  void addGcell(lefiGcellPattern* g);
  void addFloorPlan(const char* name);
  void addSiteToFloorPlan(const char* typ, lefiSitePattern* p);
  void clear();
  void bump(void*** arr, int used, int* allocated);

  int numSitePattern();
  int numCanPlace();
  int numCannotOccupy();
  int numTrack();
  int numGcell();
  int hasDefaultCap();

  const char* name() const;
  lefiSitePattern* sitePattern(int index);
  lefiSitePattern* canPlace(int index);
  lefiSitePattern* cannotOccupy(int index);
  lefiTrackPattern* track(int index);
  lefiGcellPattern* gcell(int index);

  int tableSize();
  int numDefaultCaps();
  int defaultCapMinPins(int index);
  double defaultCap(int index);

  int numFloorPlans();
  const char* floorPlanName(int index);
  int numSites(int index);
  const char* siteType(int floorIndex, int siteIndex);
  lefiSitePattern* site(int floorIndex, int siteIndex);

  // Debug print
  void print(FILE* f) const;

protected:
  int nameSize_;
  char* name_;

  int patternsAllocated_;
  int numPatterns_;
  lefiSitePattern** pattern_;

  int canAllocated_;
  int numCan_;
  lefiSitePattern** canPlace_;

  int cannotAllocated_;
  int numCannot_;
  lefiSitePattern** cannotOccupy_;

  int tracksAllocated_;
  int numTracks_;
  lefiTrackPattern** track_;

  int gAllocated_;
  int numG_;
  lefiGcellPattern** gcell_;

  int hasDefault_;
  int tableSize_;
  int numDefault_;
  int defaultAllocated_;
  int* minPins_;
  double* caps_;

  int numFloorPlans_;
  int floorPlansAllocated_;
  lefiArrayFloorPlan** floors_;
};

} // namespace LefParser
#endif
