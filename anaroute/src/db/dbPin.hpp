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
#include "src/geo/point3d.hpp"
#include "src/geo/box.hpp"

PROJECT_NAMESPACE_START


/// @brief The access point for each pins
class AcsPt
{
    public:
        /// @brief the direction enum type for the access point
        enum class DirType : std::uint8_t
        {
            NORTH = 0,
            SOUTH = 1,
            EAST  = 2,
            WEST  = 3,
            UP    = 4,
            DOWN  = 5
        };
        static std::string dirType2Str(DirType dir)
        {
          switch (dir)
          {
            case DirType::NORTH : return "NORTH";
            case DirType::SOUTH : return "SOUTH";
            case DirType::EAST : return "EAST";
            case DirType::WEST : return "WEST";
            case DirType::UP : return "UP";
            case DirType::DOWN : return "DOWN";
            default: return "INVALID";
          }
        }
    public:
        explicit AcsPt() = default;
        explicit AcsPt(const Point3d<Int_t> &gridPt, DirType dir)
          : _gridPt(gridPt), _dir(dir) {}
        /// @brief set the grid point
        void setGridPt(const Point3d<Int_t> &gridPt) { _gridPt = gridPt; }
        void setGridPt(Int_t x, Int_t y, Int_t z) { _gridPt = Point3d<Int_t>(x, y, z); }
        void setX(Int_t x) { _gridPt.setX(x); }
        void setY(Int_t y) { _gridPt.setY(y); }
        void setZ(Int_t z) { _gridPt.setZ(z); }
        void setDir(DirType dir) { _dir = dir; }
        DirType dir() const { return _dir; }
        const Point3d<Int_t> & gridPt() const { return _gridPt; }
        std::string str() const 
        {
          std::stringstream ss;
          ss << _gridPt << " " << dirType2Str(_dir);
          return ss.str();
        }
    private:
        Point3d<Int_t> _gridPt; ///< The grid index for the access point
        DirType _dir; ///< The direction of this access point
};


class Pin {
  friend class Parser;
  friend class CirDB;
  friend class Ispd08Reader;
 public:
  Pin(const String_t& name = "", const String_t& netName = "",
      const UInt_t netIdx = MAX_UINT, const UInt_t idx = MAX_UINT)
    : _name(name), _netName(netName), _netIdx(netIdx), _idx(idx),
      _minLayerIdx(MAX_UINT), _maxLayerIdx(0)
  {}
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
  UInt_t                                  numAcsPts()                                   const { return _vAcsPts.size(); }
  AcsPt&                                  acsPt(const UInt_t i)                               { return _vAcsPts[i]; }
  const AcsPt&                            acsPt(const UInt_t i)                         const { return _vAcsPts[i]; }
  Vector_t<AcsPt>&                        AcsPts()                                            { return _vAcsPts; }
  const Vector_t<AcsPt>&                  AcsPts()                                      const { return _vAcsPts; }

  // for power
  bool                                    bPower()                                      const { return _bPower; }
  bool                                    bStripe()                                     const { return _bStripe; }

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
  void addAcsPt(const AcsPt& p);
 
  // for power
  void setPower(const bool b = true);
  void setStripe(const bool b = true);

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
  Vector_t<AcsPt>                 _vAcsPts;  ///< Access points

  // for power
  bool                            _bPower = false;
  bool                            _bStripe = false;
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
// acsPt
#define Pin_ForEachAcsPt(pin, pAcsPt_, i) \
  for (i = 0; i < pin.numAcsPts() and (pAcsPt_ = &pin.acsPt(i)); ++i)
// const acsPt
#define Pin_ForEachAcsPtC(pin, cpAcsPt_, i) \
  for (i = 0; i < pin.numAcsPts() and (cpAcsPt_ = &pin.acsPt(i)); ++i)

PROJECT_NAMESPACE_END

#endif /// _DB_PIN_HPP_
