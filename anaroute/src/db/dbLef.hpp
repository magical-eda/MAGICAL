/**
 * @file   dbLef.hpp
 * @brief  Technology configuration from Lef file
 * @author Hao Chen
 * @date   09/09/2019
 *
 **/

#ifndef _DB_LEF_HPP_
#define _DB_LEF_HPP_

#include <limbo/parsers/lef/adapt/LefDataBase.h> // for class lefiXXX

#include "src/global/global.hpp"
#include "lef/lefUnits.hpp"
#include "lef/lefLayers.hpp"
#include "lef/lefSite.hpp"
#include "lef/lefVia.hpp"
#include "lef/lefViaRule.hpp"

PROJECT_NAMESPACE_START

class LefDB {
  friend class LefReader;
 public:
  LefDB()
    : _version(0), _versionStr(""), _dividerChar(""), _manufacturingGrid(0),
    _clearanceMeasure(""), _busbitChars("") {}
  ~LefDB() {}

  ////////////////////////////////////////
  //   Basics                           //
  ////////////////////////////////////////
  Float_t                           version()           const { return _version; }
  const String_t&                   versionStr()        const { return _versionStr; }
  const String_t&                   dividerChar()       const { return _dividerChar; }
  Float_t                           manufacturingGrid() const { return _manufacturingGrid; }
  const String_t&                   clearanceMeasure()  const { return _clearanceMeasure; }
  const String_t&                   busbitChars()       const { return _busbitChars; }

  ////////////////////////////////////////
  //   Units                            //
  ////////////////////////////////////////
  const LefUnits&                  units()              const { return _units; }

  ////////////////////////////////////////
  //   Sites                            //
  ////////////////////////////////////////
  UInt_t                           numSites()           const { return _vSites.size(); }
  const LefSite&                   site(const UInt_t i) const { return _vSites[i]; }
  const Vector_t<LefSite>&         vSites()             const { return _vSites; }    

  ////////////////////////////////////////
  //   Layers                           //
  ////////////////////////////////////////
  // All
  UInt_t                                   numLayers()                                    const { return _vAllLayers.size(); }
  UInt_t                                   str2LayerIdx(const String_t& n)                const { return _mStr2AllLayerIdx.at(n); }
  UInt_t                                   implantLayerIdx2LayerIdx(const UInt_t i)       const { return _vImplantLayerIdx2AllLayerIdx[i]; }
  UInt_t                                   mastersliceLayerIdx2LayerIdx(const UInt_t i)   const { return _vMastersliceLayerIdx2AllLayerIdx[i]; }
  UInt_t                                   cutLayerIdx2LayerIdx(const UInt_t i)           const { return _vCutLayerIdx2AllLayerIdx[i]; }
  UInt_t                                   routingLayerIdx2LayerIdx(const UInt_t i)       const { return _vRoutingLayerIdx2AllLayerIdx[i]; }
  UInt_t                                   overlapLayerIdx2LayerIdx(const UInt_t i)       const { return _vOverlapLayerIdx2AllLayerIdx[i]; }
  const Pair_t<LefLayerType, UInt_t>&      layerPair(const UInt_t i)                      const { return _vAllLayers[i]; }
  const Pair_t<LefLayerType, UInt_t>&      str2Layer(const String_t& n)                   const { return _vAllLayers[str2LayerIdx(n)]; }
  bool                                     bImplantLayer(const UInt_t i)                  const { return _vAllLayers[i].first == LefLayerType::IMPLANT; }
  bool                                     bMastersliceLayer(const UInt_t i)              const { return _vAllLayers[i].first == LefLayerType::MASTERSLICE; }
  bool                                     bCutLayer(const UInt_t i)                      const { return _vAllLayers[i].first == LefLayerType::CUT; }
  bool                                     bRoutingLayer(const UInt_t i)                  const { return _vAllLayers[i].first == LefLayerType::ROUTING; }
  bool                                     bOverlapLayer(const UInt_t i)                  const { return _vAllLayers[i].first == LefLayerType::OVERLAP; }
  // Implant Layers                                                                    
  const LefImplantLayer&                   implantLayer(const UInt_t i)                   const { return _vImplantLayers[i]; }
  const Vector_t<LefImplantLayer>&         vImplantLayers()                               const { return _vImplantLayers; }
  UInt_t                                   numImplantLayers()                             const { return _vImplantLayers.size(); }
  UInt_t                                   str2ImplantLayerIdx(const String_t& n)         const { return _mStr2ImplantLayerIdx.at(n); }
  const LefImplantLayer&                   str2ImplantLayer(const String_t& n)            const { return _vImplantLayers[str2ImplantLayerIdx(n)]; }
  // Masterslice Layers
  const LefMastersliceLayer&               mastersliceLayer(const UInt_t i)               const { return _vMastersliceLayers[i]; }
  const Vector_t<LefMastersliceLayer>&     vMastersliceLayers()                           const { return _vMastersliceLayers; }
  UInt_t                                   numMastersliceLayers()                         const { return _vMastersliceLayers.size(); }
  UInt_t                                   str2MastersliceLayerIdx(const String_t& n)     const { return _mStr2MastersliceLayerIdx.at(n); }
  const LefMastersliceLayer&               str2MastersliceLayer(const String_t& n)        const { return _vMastersliceLayers[str2MastersliceLayerIdx(n)]; }
  // Cut Layers
  const LefCutLayer&                       cutLayer(const UInt_t i)                       const { return _vCutLayers[i]; }
  const Vector_t<LefCutLayer>&             vCutLayers()                                   const { return _vCutLayers; }
  UInt_t                                   numCutLayers()                                 const { return _vCutLayers.size(); }
  UInt_t                                   str2CutLayerIdx(const String_t& n)             const { return _mStr2CutLayerIdx.at(n); }
  const LefCutLayer&                       str2CutLayer(const String_t& n)                const { return _vCutLayers[str2CutLayerIdx(n)]; }
  // Routing layers                                                          
  const LefRoutingLayer&                   routingLayer(const UInt_t i)                   const { return _vRoutingLayers[i]; }
  const Vector_t<LefRoutingLayer>&         vRoutingLayers()                               const { return _vRoutingLayers; }
  UInt_t                                   numRoutingLayers()                             const { return _vRoutingLayers.size(); }
  UInt_t                                   str2RoutinglayerIdx(const String_t& n)         const { return _mStr2RoutingLayerIdx.at(n); }
  const LefRoutingLayer&                   str2Routinglayer(const String_t& n)            const { return _vRoutingLayers[str2RoutinglayerIdx(n)]; }
  // Overlap layers                                                          
  const LefOverlapLayer&                   overlapLayer(const UInt_t i)                   const { return _vOverlapLayers[i]; }
  const Vector_t<LefOverlapLayer>&         vOverlapLayers()                               const { return _vOverlapLayers; }
  UInt_t                                   numOverlapLayers()                             const { return _vOverlapLayers.size(); }
  UInt_t                                   str2OverlaplayerIdx(const String_t& n)         const { return _mStr2OverlapLayerIdx.at(n); }
  const LefOverlapLayer&                   str2Overlaplayer(const String_t& n)            const { return _vOverlapLayers[str2OverlaplayerIdx(n)]; }
  ////////////////////////////////////////
  //   Vias                             //
  ////////////////////////////////////////
  const LefVia&                   via(const UInt_t i)                     const { return _vVias[i]; }
  const Vector_t<LefVia>&         vVias()                                 const { return _vVias; }
  UInt_t                          numVias()                               const { return _vVias.size(); }
  UInt_t                          numLayerVias(const UInt_t l)            const { return _vvViaIndices[l].size(); }
  UInt_t                          viaIdx(const UInt_t l, const UInt_t i)  const { return _vvViaIndices[l][i]; }
  
  
  ////////////////////////////////////////
  //   Via Rules                        //
  ////////////////////////////////////////
  const Vector_t<LefViaRuleTemplate1>& vViaRuleTemplate1() const { return _vViaRuleTemplate1; } 
  Vector_t<LefViaRuleTemplate1>& vViaRuleTemplate1() { return _vViaRuleTemplate1; } 
  void constructViaTableFromViaRule() {
    _viaTable = LefViaTable(_vRoutingLayers.size() + _vMastersliceLayers.size());
    _viaTable.generateVias(*this);
  }
  // lowerLayerIdx: M1 -> 2
  LefVia         via(const Int_t botLayerIdx, const Int_t row, const Int_t col)
  {
    auto metalIdx = layerPair(botLayerIdx).second;
    auto &prototype = _viaTable.viaProtoType(metalIdx, row, col);
    prototype.configureDefault();
    return prototype.lefVia();
  }
  LefVia         via(const Int_t botLayerIdx, const Int_t row, const Int_t col, Orient2D_t bot, Orient2D_t top)
  {
    auto metalIdx = layerPair(botLayerIdx).second;
    auto &prototype = _viaTable.viaProtoType(metalIdx, row, col);
    prototype.configureMetalWithHeightWithDirection(bot, top);
    return prototype.lefVia();
  }

  LefVia via(const Int_t botLayerIdx, const Int_t row, const Int_t col, Int_t botWidth, Int_t botHeight, Int_t topWidth, Int_t topHeight)
  {
    auto metalIdx = layerPair(botLayerIdx).second;
    auto &prototype = _viaTable.viaProtoType(metalIdx, row, col);
    prototype.configureMetalWidthHeight(botWidth, botHeight, topWidth, topHeight);
    return prototype.lefVia();
  }

  ////////////////////////////////////////
  //   Spacings                         //
  ////////////////////////////////////////
  Int_t prlSpacing(const Int_t layerIdx, const Int_t wireWidth, const Int_t prl = 0);

  // for debug
  void logInfo() const;

 private:
  // According to the Cadence Lef API
  Float_t                         _version;
  String_t                        _versionStr;
  String_t                        _dividerChar;
  Float_t                         _manufacturingGrid;
  String_t                        _clearanceMeasure;
  String_t                        _busbitChars;

  // unit
  LefUnits                        _units;

  // site
  Vector_t<LefSite>              _vSites;
  UMap_t<String_t, UInt_t>       _mStr2SiteIdx;

  // Layer
  Vector_t<Pair_t<LefLayerType, UInt_t>>  _vAllLayers; // first: type, second: idx in corresponding vector. ex. {ROUTING, 2} -> vRoutingLayers[2];
  Vector_t<LefImplantLayer>               _vImplantLayers;
  Vector_t<LefMastersliceLayer>           _vMastersliceLayers;
  Vector_t<LefCutLayer>                   _vCutLayers;
  Vector_t<LefRoutingLayer>               _vRoutingLayers;
  Vector_t<LefOverlapLayer>               _vOverlapLayers;
  UMap_t<String_t, UInt_t>                _mStr2AllLayerIdx;
  UMap_t<String_t, UInt_t>                _mStr2ImplantLayerIdx;
  UMap_t<String_t, UInt_t>                _mStr2MastersliceLayerIdx;
  UMap_t<String_t, UInt_t>                _mStr2CutLayerIdx;
  UMap_t<String_t, UInt_t>                _mStr2RoutingLayerIdx;
  UMap_t<String_t, UInt_t>                _mStr2OverlapLayerIdx;
  Vector_t<UInt_t>                        _vImplantLayerIdx2AllLayerIdx;
  Vector_t<UInt_t>                        _vMastersliceLayerIdx2AllLayerIdx;
  Vector_t<UInt_t>                        _vCutLayerIdx2AllLayerIdx;
  Vector_t<UInt_t>                        _vRoutingLayerIdx2AllLayerIdx;
  Vector_t<UInt_t>                        _vOverlapLayerIdx2AllLayerIdx;

  // Via
  Vector_t<LefVia>              _vVias;
  Vector_t<Vector_t<UInt_t>>    _vvViaIndices;
  UMap_t<String_t, UInt_t>      _mStr2ViaIdx;
  LefViaTable _viaTable;

  // Viarule
  Vector_t<LefViaRuleTemplate1> _vViaRuleTemplate1; ///< ViaRule template 1.  No special Via property. enclosure and width in metals. rect and spacing in via Based on tsmc40 lef.

  ////////////////////////////////////////
  //   Setter                           //
  ////////////////////////////////////////
  void setVersionStr(const String_t& v);
  void setVersion(const Float_t v);
  void setDividerChar(const String_t& v);
  void setManufacturingGrid(const Float_t v);
  void setClearanceMeasure(const String_t& v);
  void setBusbitChars(const String_t& v);

  void addSite(const LefSite& s);

  void addImplantLayer(const LefImplantLayer& l);
  void addMastersliceLayer(const LefMastersliceLayer& l);
  void addCutLayer(const LefCutLayer& l);
  void addRoutingLayer(const LefRoutingLayer& l);
  void addOverlapLayer(const LefOverlapLayer& l);

  void addVia(const LefVia& v);
};

////////////////////////////////////////
//   Iterators                        //
////////////////////////////////////////
// sites
#define Lef_ForEachSiteC(lef, cpSite_, i) \
  for (i = 0; i < lef.numSites() and (cpSite_ = &lef.site(i)); ++i)

// layers
#define Lef_ForEachLayerIdx(lef, i) \
  for (i = 0; i < lef.numLayers(); ++i)

#define Lef_ForEachLayerC(lef, cpPair_, i) \
  for (i = 0; i < lef.numLayers() and (cpPair_ = &lef.layerPair(i)); ++i)

#define Lef_ForEachImplantLayerC(lef, cpImpLayer_, i) \
  for (i = 0; i < lef.numImplantLayers() and (cpImpLayer_ = &lef.implantLayer(i)); ++i)

#define Lef_ForEachMastersliceLayerC(lef, cpMasLayer_, i) \
  for (i = 0; i < lef.numMastersliceLayers() and (cpMasLayer_ = & lef.mastersliceLayer(i)); ++i)

#define Lef_ForEachCutLayerC(lef, cpCutLayer_, i) \
  for (i = 0; i < lef.numCutLayers() and (cpCutLayer_ = &lef.cutLayer(i)); ++i)

#define Lef_ForEachRoutingLayerC(lef, cpRoutingLayer_, i) \
  for (i = 0; i < lef.numRoutingLayers() and (cpRoutingLayer_ = &lef.routingLayer(i)); ++i)

#define Lef_ForEachOverlapLayerC(lef, cpOverlapLayer_, i) \
  for (i = 0; i < lef.numOverlapLayers() and (cpOverlapLayer_ = &lef.overlapLayer(i)); ++i)

// vias
#define Lef_ForEachViaC(lef, cpVia_, i) \
  for (i = 0; i < lef.numVias() and (cpVia_ = &lef.via(i)); ++i)
#define Lef_ForEachLayerViaIdx(lef, layerIdx, viaIdx, i) \
  for (i = 0; i < lef.numLayerVias(layerIdx) and (viaIdx = lef.viaIdx(layerIdx, i)); ++i)
#define Lef_ForEachLayerViaC(lef, layerIdx, cpVia_, i) \
  for (i = 0; i < lef.numLayerVias(layerIdx) and (cpVia_ = lef.via(lef.viaIdx(layerIdx, i))); ++i)

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_HPP_
