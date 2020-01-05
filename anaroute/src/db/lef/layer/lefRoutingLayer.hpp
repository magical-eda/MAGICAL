/**
 * @file   lefRoutingLayer.hpp
 * @brief  Technology configuration - Lef Routing layer
 * @author Hao Chen
 * @date   09/11/2019
 *
 **/


#ifndef _DB_LEF_LAYER_ROUTING_HPP_
#define _DB_LEF_LAYER_ROUTING_HPP_

#include "lefLayerBase.hpp"

PROJECT_NAMESPACE_START


class LefSpacingTable {
 public:
  Vector_t<Int_t> vParallelRunLength; // v[0] = runlength0, v[1] = runlength1 ...
  Vector_t<Pair_t<Int_t, Vector_t<Int_t>>> table; // table[0] = {width, {spacing 0, spacing1...}}
};

class LefEolSpacing {
 public:
  LefEolSpacing(const Int_t s, const Int_t d, const Int_t w)
    : _eolSpacing(s), _eolWidth(d), _eolWithin(w) {}
  ~LefEolSpacing() {}
  
  Int_t eolSpacing() const { return _eolSpacing; }
  Int_t eolWidth()   const { return _eolWidth; }
  Int_t eolWithin()  const { return _eolWithin; }
 private:
  Int_t _eolSpacing = 0;
  Int_t _eolWidth = 0;
  Int_t _eolWithin = 0;
};

class LefRoutingLayer {
  friend class LefReader;
  
 public:
  enum class RouteDir : Byte_t {
    HORIZONTAL = 0,
    VERTICAL   = 1,
    BOTH       = 2,
    POLY       = 3,
    FORBID     = 4
    //DIAG45
    //DIAG135
  };

  LefRoutingLayer()
    : _name(""), _routeDirStr(""), _routeDir(RouteDir::FORBID),
      _minArea(0), _minWidth(0), _maxWidth(0), _defaultWidth(0),
      _pitch(0), _pitchX(0), _pitchY(0),
      _offset(0), _offsetX(0), _offsetY(0) {}
  ~LefRoutingLayer() {}

  ////////////////////////////////////////
  //   Getter                           //
  ////////////////////////////////////////
  const String_t&         name()                      const { return _name; }
  const String_t&         routeDirStr()               const { return _routeDirStr; }
  RouteDir                routeDir()                  const { return _routeDir; }
  Int_t                   minArea()                   const { return _minArea; }
  Int_t                   minWidth()                  const { return _minWidth; }
  Int_t                   maxWidth()                  const { return _maxWidth; }
  Int_t                   defaultWidth()              const { return _defaultWidth; }
  Int_t                   pitch()                     const { return _pitch; }
  Int_t                   pitchX()                    const { return _pitchX; }
  Int_t                   pitchY()                    const { return _pitchY; }
  Int_t                   offset()                    const { return _offset; }
  Int_t                   offsetX()                   const { return _offsetX; }
  Int_t                   offsetY()                   const { return _offsetY; }
  Int_t                   spacing(const UInt_t i)     const { return _vSpacings[i]; }
  UInt_t                  numSpacings()               const { return _vSpacings.size(); }
  UInt_t                  numEolSpacings()            const { return _vEolSpacings.size(); }
  Int_t                   eolSpacing(const UInt_t i)  const { return _vEolSpacings[i].eolSpacing(); }
  Int_t                   eolWidth(const UInt_t i)    const { return _vEolSpacings[i].eolWidth(); }
  Int_t                   eolWithin(const UInt_t i)   const { return _vEolSpacings[i].eolWithin(); }
  const LefSpacingTable&  spacingTable()              const { return _spacingTable; }

  // for debug
  void logInfo() const;

 private:
  //////////////////////////////////////
  //    Members                       //
  //////////////////////////////////////
  String_t                  _name;
  String_t                  _routeDirStr;
  RouteDir                  _routeDir;
  Int_t                     _minArea;
  Int_t                     _minWidth;
  Int_t                     _maxWidth;
  Int_t                     _defaultWidth;
  Int_t                     _pitch;
  Int_t                     _pitchX;
  Int_t                     _pitchY;
  Int_t                     _offset;
  Int_t                     _offsetX;
  Int_t                     _offsetY;
  Vector_t<Int_t>           _vSpacings;
  Vector_t<LefEolSpacing>   _vEolSpacings;
  LefSpacingTable           _spacingTable;

  //////////////////////////////////////
  //    Setter                        //
  //////////////////////////////////////
  void setName(const String_t& n);
  void setRouteDir(const String_t& d);
  void setMinArea(const Int_t a);
  void setMinWidth(const Int_t w);
  void setMaxWidth(const Int_t w);
  void setDefaultWidth(const Int_t w);
  void setPitch(const Int_t p);
  void setPitchX(const Int_t p);
  void setPitchY(const Int_t p);
  void setOffset(const Int_t o);
  void setOffsetX(const Int_t o);
  void setOffsetY(const Int_t o);
  
  void addSpacing(const Int_t s);
  void addEolSpacing(const Int_t s, const Int_t d, const Int_t w);
  void addParallelRunLength(const Int_t p);
  void addSpacingTableWidth(const Int_t w);
  void addSpacingTableWidthSpacing(const UInt_t idx, const Int_t s);
};

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_LAYER_ROUTING_HPP_
