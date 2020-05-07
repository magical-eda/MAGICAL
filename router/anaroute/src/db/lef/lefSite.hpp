/**
 * @file   lefSite.hpp
 * @brief  Technology configuration - Lef Site
 * @author Hao Chen
 * @date   09/15/2019
 *
 **/

#ifndef _DB_LEF_SITE_HPP_
#define _DB_LEF_SITE_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

enum class LefSiteClassType : Byte_t{
  PAD     = 0,
  CORE    = 1,
  UNKNOWN = 2
};

enum class LefSiteOrient : Byte_t {
  N       = 0,
  S       = 1,
  E       = 2,
  W       = 3,
  FN      = 4,
  FS      = 5,
  FE      = 6,
  FW      = 7,
};

class LefSiteRowPattern {
  friend class LefSite;
  friend class LefReader;
 public:
  LefSiteRowPattern() {}
  ~LefSiteRowPattern() {}
 
  UInt_t            numSites()                 const { return _vSiteNames.size(); }
  const String_t&   siteName(const UInt_t i)   const { return _vSiteNames[i]; }
  const String_t&   orientStr(const UInt_t i)  const { return _vOrientStrs[i]; }
  LefSiteOrient     orient(const UInt_t i)     const { return _vOrients[i]; }

 private:
  Vector_t<String_t>       _vSiteNames;
  Vector_t<String_t>       _vOrientStrs;
  Vector_t<LefSiteOrient>  _vOrients;
  
  void addPattern(const String_t& siteName, const String_t& orientStr) {
    _vSiteNames.emplace_back(siteName);
    _vOrientStrs.emplace_back(orientStr);
    LefSiteOrient orient = LefSiteOrient::N;
    if (orientStr == "N")
      orient = LefSiteOrient::N;
    else if (orientStr == "S")
      orient = LefSiteOrient::S;
    else if (orientStr == "E")
      orient = LefSiteOrient::E;
    else if (orientStr == "W")
      orient = LefSiteOrient::W;
    else if (orientStr == "FN")
      orient = LefSiteOrient::FN;
    else if (orientStr == "FS")
      orient = LefSiteOrient::FS;
    else if (orientStr == "FE")
      orient = LefSiteOrient::FE;
    else if (orientStr == "FW")
      orient = LefSiteOrient::FW;
    _vOrients.emplace_back(orient);
  }
};

class LefSite {
  friend class LefReader;
 public:
  LefSite()
    : _name(""), _classTypeStr(""), _classType(LefSiteClassType::UNKNOWN),
      _bXSymmetry(false), _bYSymmetry(false), _bR90Symmetry(false),
      _sizeX(0), _sizeY(0) {}
  ~LefSite() {}
  /////////////////////////////////
  //    Getter                   //
  /////////////////////////////////
  const String_t&           name()              const { return _name; }
  const String_t&           classTypeStr()      const { return _classTypeStr; }
  LefSiteClassType          classType()         const { return _classType; }
  bool                      bXSymmetry()        const { return _bXSymmetry; }
  bool                      bYSymmetry()        const { return _bYSymmetry; }
  bool                      bR90Symmetry()      const { return _bR90Symmetry; }
  const LefSiteRowPattern&  rowPattern()        const { return _rowPattern; }
  Int_t                     sizeX()             const { return _sizeX; }
  Int_t                     sizeY()             const { return _sizeY; }

  // for debug
  void logInfo() const;
 private:
  String_t                _name;
  String_t                _classTypeStr;
  LefSiteClassType        _classType;
  bool                    _bXSymmetry;
  bool                    _bYSymmetry;
  bool                    _bR90Symmetry;
  LefSiteRowPattern       _rowPattern;
  Int_t                   _sizeX;
  Int_t                   _sizeY;
  
  /////////////////////////////////
  //    Setter                   //
  /////////////////////////////////
  void setName(const String_t& n);
  void setClassType(const String_t& t);
  void setXSymmetry();
  void setYSymmetry();
  void setR90Symmetry();
  void setSizeX(const Int_t s);
  void setSizeY(const Int_t s);
  void addRowPattern(const String_t& n, const String_t& o);
};

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_SITE_HPP_
