/**
 * @file   dbPin.hpp
 * @brief  Circuit Element - Pin
 * @author Hao Chen
 * @date   09/20/2019
 *
 **/

#ifndef _DB_PIN_HPP_
#define _DB_PIN_HPP_

#include "src/global/global.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START

class Pin {
  friend class Parser;
  friend class CirDB;
  friend class Ispd08Reader;
 public:
  Pin()
    : _name(""), _netName(""), _netIdx(MAX_UINT), _idx(MAX_UINT),
      _minLayerIdx(MAX_UINT), _maxLayerIdx(0) {}
  ~Pin() {}

  ////////////////////////////////////////
  //   Getter                           //
  ////////////////////////////////////////
  const String_t&                         name()                                        const { return _name; }
  const String_t&                         netName()                                     const { return _netName; }
  UInt_t                                  netIdx()                                      const { return _netIdx; }
  UInt_t                                  idx()                                         const { return _idx; }
  UInt_t                                  minLayerIdx()                                 const { return _minLayerIdx; }
  UInt_t                                  maxLayerIdx()                                 const { return _maxLayerIdx; }
  UInt_t                                  numBoxes(const UInt_t layerIdx)               const { return _vvBoxes[layerIdx].size(); }
  Box<Int_t>&                             box(const UInt_t layerIdx, const UInt_t j)          { return _vvBoxes[layerIdx][j]; }
  const Box<Int_t>&                       box(const UInt_t layerIdx, const UInt_t j)    const { return _vvBoxes[layerIdx][j]; }
  const Vector_t<Box<Int_t>>&             vBoxes(const UInt_t layerIdx)                 const { return _vvBoxes[layerIdx]; }
  const Vector_t<Vector_t<Box<Int_t>>>&   vvBoxes()                                     const { return _vvBoxes; }

  ////////////////////////////////////////
  //   Setter                           //
  ////////////////////////////////////////
  void setName(const String_t& n);
  void setNetName(const String_t& n);
  void setNetIdx(const UInt_t i);
  void setIdx(const UInt_t i);
  void setMinLayerIdx(const UInt_t i);
  void setMaxLayerIdx(const UInt_t i);
  void addBox(const UInt_t layerIdx, const Box<Int_t>& box);
  void setLayerBoxes(const UInt_t layerIdx, const Vector_t<Box<Int_t>>& vBoxes);
  void resizeLayerBoxes(const UInt_t i);
  
  // for debug
  void printInfo() const;

 private:
  String_t                        _name;
  String_t                        _netName;
  UInt_t                          _netIdx;
  UInt_t                          _idx;
  UInt_t                          _minLayerIdx;
  UInt_t                          _maxLayerIdx;
  Vector_t<Vector_t<Box<Int_t>>>  _vvBoxes; // Boxes in multiple layers
  
};

////////////////////////////////////////
//   Iterators                        //
////////////////////////////////////////
// shapes never located on cut layers
#define Pin_ForEachLayerIdx(pin, i) \
  for (i = pin.minLayerIdx(); i <= pin.maxLayerIdx(); i += 2)
// box
#define Pin_ForEachLayerBox(pin, layerIdx, pBox_, i) \
  for (i = 0; i < pin.numBoxes(layerIdx) and (pBox_ = &pin.box(layerIdx, i)); ++i)
// const box
#define Pin_ForEachLayerBoxC(pin, layerIdx, cpBox_, i) \
  for (i = 0; i < pin.numBoxes(layerIdx) and (cpBox_ = &pin.box(layerIdx, i)); ++i)

PROJECT_NAMESPACE_END

#endif /// _DB_PIN_HPP_
