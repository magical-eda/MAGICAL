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
 * @file lefiNonDefault.hpp
 */

#ifndef lefiNonDefault_h
#define lefiNonDefault_h

#include <stdio.h>
//#include <limbo/parsers/lef/bison/lefiKRDefs.hpp>
#include <limbo/parsers/lef/bison/lefiVia.hpp>
#include <limbo/parsers/lef/bison/lefiMisc.hpp>

namespace LefParser {

class lefiNonDefault {
public:
  lefiNonDefault();
  void Init();

  void Destroy();
  ~lefiNonDefault();

  void setName(const char* name);
  void addLayer(const char* name);
  void addWidth(double num);
  void addWireExtension(double num);
  void addSpacing(double num);
  void addSpacingRule(lefiSpacing const& s);
  void addResistance(double num);
  void addCapacitance(double num);
  void addEdgeCap(double num);
  void addViaRule(lefiVia const& v);
  void addDiagWidth(double num);                    // 5.6
  void end();
  void clear();
  void addProp(const char* name, const char* value, const char type);
  void addNumProp(const char* name, const double d,
                  const char* value, const char type);
  void setHardspacing();                            // 5.6
  void addUseVia(const char* name);                 // 5.6
  void addUseViaRule(const char* name);             // 5.6
  void addMinCuts(const char* name, int numCuts);   // 5.6

  const char* name() const;
  int   hasHardspacing() const;                     // 5.6

  int numProps() const;
  const char*  propName(int index) const;
  const char*  propValue(int index) const;
  double propNumber(int index) const;
  char   propType(int index) const;
  int    propIsNumber(int index) const;
  int    propIsString(int index) const;
  
  // A non default rule can have one or more layers.
  // The layer information is kept in an array.
  int numLayers() const ;
  const char* layerName(int index) const ;
  int hasLayerWidth(int index) const;
  double layerWidth(int index) const ;
  int hasLayerSpacing(int index) const ;
  double layerSpacing(int index) const ;
  int hasLayerWireExtension(int index) const ;
  double layerWireExtension(int index) const ;
  int hasLayerResistance(int index) const;       // obsolete in 5.6
  double layerResistance(int index) const;       // obsolete in 5.6
  int hasLayerCapacitance(int index) const;      // obsolete in 5.6
  double layerCapacitance(int index) const;      // obsolete in 5.6
  int hasLayerEdgeCap(int index) const;          // obsolete in 5.6
  double layerEdgeCap(int index) const;          // obsolete in 5.6
  int hasLayerDiagWidth(int index) const;        // 5.6
  double layerDiagWidth(int index) const;        // 5.6

  // A non default rule can have one or more vias.
  // These routines return the via info.
  int numVias() const ;
  lefiVia* viaRule(int index) const ;

  // A non default rule can have one or more spacing rules.
  // These routines return the that info.
  int numSpacingRules() const ;
  lefiSpacing* spacingRule(int index) const ;

  int numUseVia() const;                         // 5.6
  const char* viaName(int index) const;          // 5.6
  int numUseViaRule() const;                     // 5.6
  const char* viaRuleName(int index) const;      // 5.6
  int numMinCuts() const;                        // 5.6
  const char* cutLayerName(int index) const;     // 5.6
  int numCuts(int index) const;                  // 5.6

  // Debug print
  void print(FILE* f) const;

protected:
  void* oldViaCbk_;     // save the users' functions.
  void* oldSpacingCbk_;     // save the users' functions.
  void* oldSpacingBeginCbk_;
  void* oldSpacingEndCbk_;
  void* oldUserData_;

  int nameSize_;
  char* name_;

  // Layer information
  int numLayers_;
  int layersAllocated_;
  char** layerName_;
  double* width_;
  double* spacing_;
  double* wireExtension_;
  char* hasWidth_;
  char* hasSpacing_;
  char* hasWireExtension_;

  // 5.4
  double* resistance_;
  double* capacitance_;
  double* edgeCap_;
  char*   hasResistance_;
  char*   hasCapacitance_;
  char*   hasEdgeCap_;

  double* diagWidth_;              // 5.6
  char*   hasDiagWidth_;           // 5.6

  int numVias_;
  int allocatedVias_;
  lefiVia** viaRules_;

  int numSpacing_;
  int allocatedSpacing_;
  lefiSpacing** spacingRules_;

  int    hardSpacing_;             // 5.6
  int    numUseVias_;              // 5.6
  int    allocatedUseVias_;        // 5.6
  char** useViaName_;              // 5.6
  int    numUseViaRules_;          // 5.6
  int    allocatedUseViaRules_;    // 5.6
  char** useViaRuleName_;          // 5.6
  int    numMinCuts_;              // 5.6
  int    allocatedMinCuts_;        // 5.6
  char** cutLayerName_;            // 5.6
  int*   numCuts_;                 // 5.6

  int numProps_;
  int propsAllocated_;
  char**  names_;
  char**  values_;
  double* dvalues_;
  char*   types_;
};

} // namespace LefParser

#endif
