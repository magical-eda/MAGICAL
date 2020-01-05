/**
 * @file   lefVia.hpp
 * @brief  Technology configuration - Lef Via
 * @author Hao Chen
 * @date   09/16/2019
 *
 **/

#ifndef _DB_LEF_VIA_HPP_
#define _DB_LEF_VIA_HPP_

#include "src/global/global.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START

class LefVia {
  friend class LefReader;
 public:
  LefVia()
    : _name(""), _bDefault(false), _resistance(0),
      _layerIndices{0, 0 , 0}, _layerNames{"", "", ""} {}
  ~LefVia() {}

  /////////////////////////////////
  //    Getter                   //
  /////////////////////////////////
  const String_t&               name()                  const { return _name; }
  bool                          bDefault()              const { return _bDefault; }
  Int_t                         resistance()            const { return _resistance; }
  UInt_t                        botLayerIdx()           const { return _layerIndices[0]; }
  UInt_t                        cutLayerIdx()           const { return _layerIndices[1]; }
  UInt_t                        topLayerIdx()           const { return _layerIndices[2]; }
  const String_t&               botLayerName()          const { return _layerNames[0]; } 
  const String_t&               cutLayerName()          const { return _layerNames[1]; } 
  const String_t&               topLayerName()          const { return _layerNames[2]; }
  
  // Boxes
  UInt_t                        numBotBoxes()           const { return _vBoxes[0].size(); }
  UInt_t                        numCutBoxes()           const { return _vBoxes[1].size(); }
  UInt_t                        numTopBoxes()           const { return _vBoxes[2].size(); }
  const Box<Int_t>&             botBox(const UInt_t i)  const { return _vBoxes[0][i]; }
  const Box<Int_t>&             cutBox(const UInt_t i)  const { return _vBoxes[1][i]; }
  const Box<Int_t>&             topBox(const UInt_t i)  const { return _vBoxes[2][i]; }
  const Vector_t<Box<Int_t>>&   vBotBoxes()             const { return _vBoxes[0]; }
  const Vector_t<Box<Int_t>>&   vCutBoxes()             const { return _vBoxes[1]; }
  const Vector_t<Box<Int_t>>&   vTopBoxes()             const { return _vBoxes[2]; }
                                         
  // for debug
  void logInfo() const;
 private:                                                        
  String_t             _name;
  bool                 _bDefault; // true: fixed via, false: generated via
  Int_t                _resistance;
  UInt_t               _layerIndices[3]; // 0 -> botLayer 1 -> cutLayer 2 -> topLayer
  String_t             _layerNames[3];   // 0 -> botLayer 1 -> cutLayer 2 -> topLayer 
  Vector_t<Box<Int_t>> _vBoxes[3];       // 0 -> botLayer 1 -> cutLayer 2 -> topLayer 
  /////////////////////////////////
  //    Setter                   //
  /////////////////////////////////
  void setName(const String_t& n);
  void setDefault();
  void setResistance(const Int_t r);
  void setLayerIdx(const UInt_t i, const UInt_t v);
  void setLayerName(const UInt_t i, const String_t& n);
  void addBox(const UInt_t i, const Box<Int_t>& b);
};

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_VIA_HPP_
