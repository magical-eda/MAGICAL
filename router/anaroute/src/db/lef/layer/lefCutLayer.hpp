/**
 * @file   lefCutLayer.hpp
 * @brief  Technology configuration - Lef Cut layer
 * @author Hao Chen
 * @date   09/12/2019
 *
 **/


#ifndef _DB_LEF_LAYER_CUT_HPP_
#define _DB_LEF_LAYER_CUT_HPP_

#include "lefLayerBase.hpp"

PROJECT_NAMESPACE_START

class LefCutLayer {
  friend class LefReader;
 
 public:
  LefCutLayer()
    : _name(""), _minWidth(0), _spacing(0), _sameNetSpacing(0),
      _parallelLength(0), _parallelWithin(0), _parallelOverlapSpacing(0), _exceptCutWithin(0){}
  ~LefCutLayer() {}

  ////////////////////////////////////////
  //   Getter                           //
  ////////////////////////////////////////
  const String_t& name()                    const { return _name; }
  Int_t           minWidth()                const { return _minWidth; }
  Int_t           spacing()                 const { return _spacing; }
  Int_t           sameNetSpacing()          const { return _sameNetSpacing; }
  Int_t           parallelLength()          const { return _parallelLength; }
  Int_t           parallelOverlapSpacing()  const { return _parallelOverlapSpacing; }
  Int_t           exceptCutWithin()         const { return _exceptCutWithin; }

  // for debug
  void logInfo() const;

 private:
  String_t  _name;
  Int_t     _minWidth;
  Int_t     _spacing;
  Int_t     _sameNetSpacing;
  /// @brief PROPERTY LEF58_ENCLOSUREEDGE
  Int_t     _parallelLength;
  Int_t     _parallelWithin;
  Int_t     _parallelOverlapSpacing;
  Int_t     _exceptCutWithin;
 
  ////////////////////////////////////////
  //   Setter                           //
  ////////////////////////////////////////
  void setName(const String_t& n);
  void setMinWidth(const Int_t w);
  void setSpacing(const Int_t s);
  void setSameNetSpacing(const Int_t s);
  void setParallelLength(const Int_t l);
  void setParallelOverlapSpacing(const Int_t s);
  void setExceotCutWithin(const Int_t w);

};

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_LAYER_CUT_HPP_
