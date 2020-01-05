/**
 * @file   dbCir.hpp
 * @brief  The Top-Level Database - Circuit
 * @author Hao Chen
 * @date   09/09/2019
 *
 **/

#ifndef _DB_CIRCUIT_HPP_
#define _DB_CIRCUIT_HPP_

#include "src/global/global.hpp"
#include "dbLef.hpp"
#include "dbTechfile.hpp"
#include "dbBlk.hpp"
#include "dbPin.hpp"
#include "dbNet.hpp"
#include "src/geo/spatial.hpp"

PROJECT_NAMESPACE_START

class CirDB {
  friend class Parser;
  friend class Ispd08Reader;
  friend class TechfileReader;
  friend class GdsReader;
 public:
  CirDB()
    : _name(""), _xl(0), _yl(0), _xh(0), _yh(0) {}
  ~CirDB() {}
  //////////////////////////////////
  //  Getter                      //
  //////////////////////////////////
  const String_t&   name()  const { return _name; }
  
  // Lef
  LefDB&            lef()         { return _lef; }
  const LefDB&      lef()   const { return _lef; }
 
  // Techfile
  TechfileDB&       tech()                                 { return _tech; }
  const TechfileDB& tech()                           const { return _tech; }
  UInt_t            layerIdx2MaskIdx(const UInt_t i) const;

  // Boundary
  Int_t             xl()     const { return _xl; }
  Int_t             yl()     const { return _yl; }
  Int_t             xh()     const { return _xh; }
  Int_t             yh()     const { return _yh; }
  Int_t             width()  const { return _xh - _xl; }
  Int_t             height() const { return _yh - _yl; }

  // Pin
  UInt_t            numPins()                             const  { return _vPins.size(); }
  UInt_t            numLayerPins(const UInt_t i)          const  { return _vvPinIndices[i].size(); }
  UInt_t            str2PinIdx(const String_t& n)         const  { return _mStr2PinIdx.at(n); }
  Pin&              pin(const String_t& n)                       { return _vPins[str2PinIdx(n)]; }
  const Pin&        pin(const String_t& n)                const  { return _vPins[str2PinIdx(n)]; }
  Pin&              pin(const UInt_t i)                          { return _vPins[i]; }
  const Pin&        pin(const UInt_t i)                   const  { return _vPins[i]; }
  Pin&              pin(const UInt_t i, const UInt_t j)          { return _vPins[_vvPinIndices[i][j]]; }
  const Pin&        pin(const UInt_t i, const UInt_t j)   const  { return _vPins[_vvPinIndices[i][j]]; }

  // Net
  UInt_t            numNets()                     const { return _vNets.size(); }
  UInt_t            str2NetIdx(const String_t& n) const { return _mStr2NetIdx.at(n); }
  Net&              net(const UInt_t i)                 { return _vNets[i]; }
  Net&              net(const String_t& n)              { return _vNets[str2NetIdx(n)]; }
  const Net&        net(const UInt_t i)           const { return _vNets[i]; }
  const Net&        net(const String_t& n)        const { return _vNets[str2NetIdx(n)]; }
  bool              hasNet(const String_t& n)     const { return _mStr2NetIdx.find(n) != _mStr2NetIdx.end(); }

  // Blk
  UInt_t            numBlks()                             const { return _vBlks.size(); }
  UInt_t            numLayerBlks(const UInt_t i)          const { return _vvBlkIndices[i].size(); }
  Blk&              blk(const UInt_t i)                         { return _vBlks[i]; }
  const Blk&        blk(const UInt_t i)                   const { return _vBlks[i]; }
  Blk&              blk(const UInt_t i, const UInt_t j)         { return _vBlks[_vvBlkIndices[i][j]]; }
  const Blk&        blk(const UInt_t i, const UInt_t j)   const { return _vBlks[_vvBlkIndices[i][j]]; }


  //////////////////////////////////
  //  Funcs                       //
  //////////////////////////////////
  void setXL(const Int_t x);
  void setYL(const Int_t y);
  void setXH(const Int_t x);
  void setYH(const Int_t y);
  
  void addPin(const Pin& p);
  void addBlk(const UInt_t i, const Blk& b);
  void addNet(const Net& n);
  void resizeVVPinIndices(const UInt_t i);
  void resizeVVBlkIndices(const UInt_t i);
  
  // Spatial
  const Vector_t<SpatialMap<Int_t, UInt_t>>& vSpatialPins()        const { return _vSpatialPins; }
  const Vector_t<SpatialMap<Int_t, UInt_t>>& vSpatialBlks()        const { return _vSpatialBlks; }
  const Vector_t<SpatialMap<Int_t, UInt_t>>& vSpatialRoutedWires() const { return _vSpatialRoutedWires; }
  const Vector_t<Spatial<Int_t>>&            vSpatialNetGuides(const UInt_t netIdx) const { return _vvSpatialNetGuides[netIdx]; }
  void buildSpatial();
  void buildSpatialPins();
  void buildSpatialBlks();
  void buildSpatialNetGuides();
  void initSpatialRoutedWires();
  void addSpatialRoutedWire(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  void addSpatialRoutedWire(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& box);
  void addSpatialRoutedVia(const UInt_t netIdx, const UInt_t viaIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool removeSpatialRoutedWire(const UInt_t netIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool removeSpatialRoutedWire(const UInt_t netIdx, const UInt_t layerIdx, const Box<Int_t>& box);
  bool removeSpatialRoutedVia(const UInt_t netIdx, const UInt_t viaIdx, const Point3d<Int_t>& u, const Point3d<Int_t>& v);
  bool querySpatialPin(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vPinIndices);
  bool querySpatialPin(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vPinIndices);
  bool querySpatialBlk(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vBlkIndices);
  bool querySpatialBlk(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vBlkIndices);
  bool existSpatialPin(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr);
  bool existSpatialPin(const UInt_t layerIdx, const Box<Int_t>& box);
  bool existSpatialBlk(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr);
  bool existSpatialBlk(const UInt_t layerIdx, const Box<Int_t>& box);
  bool querySpatialRoutedWire(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, Vector_t<UInt_t>& vNetIndices);
  bool querySpatialRoutedWire(const UInt_t layerIdx, const Box<Int_t>& box, Vector_t<UInt_t>& vNetIndices);
  bool existSpatialRoutedWire(const UInt_t layerIdx, const Point<Int_t>& bl,const Point<Int_t>& tr);
  bool existSpatialRoutedWire(const UInt_t layerIdx, const Box<Int_t>& box);
  bool existSpatialRoutedWireNet(const UInt_t layerIdx, const Point<Int_t>& bl, const Point<Int_t>& tr, const UInt_t netIdx);
  bool existSpatialRoutedWireNet(const UInt_t layerIdx, const Box<Int_t>& box, const UInt_t netIdx);
 
  // fix
  void markBlks();
  void addBlk2ConnectedPin();

  // for debug
  void printInfo() const;
  void visualize() const;
 
 private:
  String_t                       _name;
  LefDB                          _lef;
  TechfileDB                     _tech;

  Int_t                          _xl;
  Int_t                          _yl;
  Int_t                          _xh;
  Int_t                          _yh;
  
  Vector_t<Pin>                  _vPins;
  Vector_t<Net>                  _vNets;
  Vector_t<Blk>                  _vBlks;
  Vector_t<Vector_t<UInt_t>>     _vvPinIndices; // pins in each layer
  Vector_t<Vector_t<UInt_t>>     _vvBlkIndices; // blocks in each layer
 
  UMap_t<String_t, UInt_t>       _mStr2NetIdx;
  UMap_t<String_t, UInt_t>       _mStr2PinIdx;
 
  Vector_t<SpatialMap<Int_t, UInt_t>>  _vSpatialPins;
  Vector_t<SpatialMap<Int_t, UInt_t>>  _vSpatialBlks;
  Vector_t<SpatialMap<Int_t, UInt_t>>  _vSpatialRoutedWires;

  Vector_t<Vector_t<Spatial<Int_t>>>   _vvSpatialNetGuides;


  //////////////////////////////////
  //  Private Setter              //
  //////////////////////////////////
};

////////////////////////////////////////
//   Iterators                        //
////////////////////////////////////////
// layers
#define Cir_ForEachLayerIdx(cir, i) \
  Lef_ForEachLayerIdx(cir.lef(), i)

#define Cir_ForEachLayerC(cir, cpPair_, i) \
  Lef_ForEachLayerC(cir.lef(), cpPair_, i)

#define Cir_ForEachImplantLayerC(cir, cpImplayer_, i) \
  Lef_ForEachImplantLayerC(lef, cpImpLayer_, i)

#define Cir_ForEachMastersliceLayerC(cir, cpMasLayer_, i) \
  Lef_ForEachMastersliceLayerC(lef, cpMasLayer_, i)

#define Cir_ForEachCutLayerC(cir, cpCutLayer_, i) \
  Lef_ForEachCutLayerC(lef, cpCutLayer_, i)

#define Cir_ForEachRoutingLayerC(cir, cpRoutingLayer_, i) \
  Lef_ForEachRoutingLayerC(lef, cpRoutingLayer_, i)

#define Cir_ForEachOverlapLayerC(cir, cpOverlapLayer_, i) \
  Lef_ForEachOverlapLayerC(lef, cpOverlapLayer_, i)

// pins
#define Cir_ForEachPin(cir, pPin_, i) \
  for (i = 0; i < cir.numPins() and (pPin_ = &cir.pin(i)); ++i)
// const pins
#define Cir_ForEachPinC(cir, cpPin_, i) \
  for (i = 0; i < cir.numPins() and (cpPin_ = &cir.pin(i)); ++i)
// layer pins
#define Cir_ForEachLayerPin(cir, layerIdx, pPin_, i) \
  for (i = 0; i < cir.numLayerPins(layerIdx) and (pPin_ = &cir.pin(layerIdx, i)); ++i)
// const layer pins
#define Cir_ForEachLayerPinC(cir, layerIdx, cpPin_, i) \
  for (i = 0; i < cir.numLayerPins(layerIdx) and (cpPin_ = &cir.pin(layerIdx, i)); ++i)

// nets
#define Cir_ForEachNet(cir, pNet_, i) \
  for (i = 0; i < cir.numNets() and (pNet_ = &cir.net(i)); ++i)
// const nets
#define Cir_ForEachNetC(cir, cpNet_, i) \
  for (i = 0; i < cir.numNets() and (cpNet_ = &cir.net(i)); ++i)

// blks
#define Cir_ForEachBlk(cir, pBlk_, i) \
  for (i = 0; i < cir.numBlks() and (pBlk_ = &cir.blk(i)); ++i)
// const blks
#define Cir_ForEachBlkC(cir, cpBlk_, i) \
  for (i = 0; i < cir.numBlks() and (cpBlk_ = &cir.blk(i)); ++i)
// layer blks
#define Cir_ForEachLayerBlk(cir, layerIdx, pBlk_, i) \
  for (i = 0; i < cir.numLayerBlks(layerIdx) and (pBlk = &cir.blk(layerIdx, i)); ++i)
// const layer blks
#define Cir_ForEachLayerBlkC(cir, layerIdx, cpBlk_, i) \
  for (i = 0; i < cir.numLayerBlks(layerIdx) and (cpBlk = &cir.blk(layerIdx, i)); ++i)

PROJECT_NAMESPACE_END

#endif /// _DB_CIRCUIT_HPP_
