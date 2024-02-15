/**
 * @file   lefVia.hpp
 * @brief  Technology configuration - Lef Via
 * @author Hao Chen, Keren Zhu
 * @date   09/16/2019
 *
 **/

#ifndef _DB_LEF_VIA_HPP_
#define _DB_LEF_VIA_HPP_

#include "src/global/global.hpp"
#include "src/geo/box.hpp"
#include "src/util/Vector2D.h"
#include "lefViaImpl.hpp"

PROJECT_NAMESPACE_START

class ViaGenerationNotImplementedException : public std::exception 
{
    public:
    virtual const char* what() const throw()
    {
        return "Generation for the given LefViaRule type has not been implemented";
    }
};

class ViaGenerationInfeasibleException : public std::exception
{
    public:
    virtual const char* what() const throw()
    {
        return "Infeasible VIA generation from ViaRule";
    }
};

class ViaGenerationInfeasibleMaxWidthException : public ViaGenerationInfeasibleException
{
    virtual const char* what() const throw()
    {
        return "Infeasible VIA generation from ViaRule: exceed max metal width";
    }
};

class LefDB;

class LefVia {
  friend class LefReader;
  friend class LefViaTable;
  friend class LefViaImplementor;
 public:
  LefVia()
    : _name(""), _bDefault(false), _resistance(0),
      _layerIndices{0, 0 , 0}, _layerNames{"", "", ""} {}
  ~LefVia() {}
  LefVia(const LefVia& lefVia)
  {
    _name = lefVia.name();
    _bDefault = lefVia.bDefault();
    _resistance = lefVia.resistance();
    _layerIndices[0] = lefVia._layerIndices[0]; 
    _layerIndices[1] = lefVia._layerIndices[1];
    _layerIndices[2] = lefVia._layerIndices[2];
    _layerNames[0] = lefVia._layerNames[0];
    _layerNames[1] = lefVia._layerNames[1];
    _layerNames[2] = lefVia._layerNames[2];
    _vBoxes[0] = lefVia._vBoxes[0];
    _vBoxes[1] = lefVia._vBoxes[1];
    _vBoxes[2] = lefVia._vBoxes[2];
    _valid = lefVia._valid;
    _cutBBox = lefVia._cutBBox;

  }

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
  bool bValid() const { return _valid; }
  void computeCutBBox()
  {
    _cutBBox = _vBoxes[1][0];
    for (const auto & cut : _vBoxes[1])
    {
      _cutBBox.setXL(std::min(cut.xl(), _cutBBox.xl()));
      _cutBBox.setXH(std::max(cut.xh(), _cutBBox.xh()));
      _cutBBox.setYL(std::min(cut.yl(), _cutBBox.yl()));
      _cutBBox.setYH(std::max(cut.yh(), _cutBBox.yh()));
    }
  }



                                         
  // for debug
  void logInfo() const;
 protected:                                                        
  String_t                             _name;
  bool                                 _bDefault; // true: fixed via, false: generated via
  Int_t                                _resistance;
  std::array<UInt_t, 3>                _layerIndices; // 0 -> botLayer 1 -> cutLayer 2 -> topLayer
  std::array<String_t, 3>              _layerNames;   // 0 -> botLayer 1 -> cutLayer 2 -> topLayer 
  std::array<Vector_t<Box<Int_t>>, 3>  _vBoxes;       // 0 -> botLayer 1 -> cutLayer 2 -> topLayer 
  bool                                 _valid = false;
  Box<Int_t>                           _cutBBox= Box<Int_t>(0, 0, 0, 0);  

 public:
  /////////////////////////////////
  //    Setter                   //
  /////////////////////////////////
  void setName(const String_t& n);
  void setDefault(bool bDefault) { _bDefault = bDefault; }
  void setResistance(const Int_t r);
  void setLayerIdx(const UInt_t i, const UInt_t v);
  void setLayerName(const UInt_t i, const String_t& n);
  void addBox(const UInt_t i, const Box<Int_t>& b);
  void setValid(bool valid) { _valid = valid; }
};

class LefViaPrototype 
{
  public:
    LefViaPrototype()  {}
    template<typename ViaRuleType>
    void config(const LefVia &lefVia, const ViaRuleType &viaRule);
    const LefVia &lefVia() const { return _lefVia; }
    LefVia &lefVia() { return _lefVia; }
    /// @brief configure the width and height for the via metals
    /// @param first: target width for bottom metal. -1 if don't care
    /// @param second: target height for bottom metal. -1 if don't care
    /// @param third: target width for top metal. -1 if don't care
    /// @param fourth: target height for top metal. -1 if don't care
    /// @return true: if configuration success. false: failed. the original shapes should not change
    bool configureMetalWidthHeight(Int_t botWidth, Int_t botHeight, Int_t topWidth, Int_t topHeight)
    {
     return  _implementor->configureMetalWidthHeight(botWidth, botHeight, topWidth, topHeight);
    }
    /// @param Orientation 2D type representing the routing direction
    /// @return if configuration success
    bool configureMetalWithHeightWithDirection(Orient2D_t botOrient, Orient2D_t topOrient)
    {
      return _implementor->configureMetalWithHeightWithDirection(botOrient, topOrient);
    }
    /// @brief configure the via into default mode
    void configureDefault()
    {
      _implementor->configureDefault();
    }
  protected:
    LefVia _lefVia;
    std::shared_ptr<LefViaImplementor> _implementor = nullptr;
};

/// @brief the table for LefVia
class LefViaTable
{
    protected:
        static constexpr UInt_t MAX_GENERATE_ROW = 4; ///< The max number of rows to generate
        static constexpr UInt_t MIN_GENERATE_ROW = 1; ///< The min number of rows to generate
        static constexpr UInt_t MAX_GENERATE_COLUMN = 4; ///< The max number of columns to generate
        static constexpr UInt_t MIN_GENERATE_COLUMN = 1; ///< The min number of columns to generate
    public:
        LefViaTable() 
        {
            _table.clear();
        }
        /// @brief constructor with number of layers
        /// @param the number of layers need to consider
        explicit LefViaTable(UInt_t numLayers) 
        {
            _table.resize(numLayers, 
                    Vector2D<LefViaPrototype>(MAX_GENERATE_ROW - MIN_GENERATE_ROW + 1,
                                     MAX_GENERATE_COLUMN - MIN_GENERATE_COLUMN +1)
                    ); // size: # of layers * # of rows * # of columns
        }
        /// @brief genearte the vias based on viarules
        /// @param the LEF tech database
        void generateVias(LefDB &lef);
        LefVia & via(UInt_t lowerMetalLayer, UInt_t row, UInt_t col)
        {
          return _table.at(lowerMetalLayer).at(row - MIN_GENERATE_ROW, col - MIN_GENERATE_COLUMN).lefVia();
        }
        const LefVia & via(UInt_t lowerMetalLayer, UInt_t row, UInt_t col) const
        {
          return _table.at(lowerMetalLayer).at(row - MIN_GENERATE_ROW, col - MIN_GENERATE_COLUMN).lefVia();
        }
        LefViaPrototype & viaProtoType(UInt_t lowerMetalLayer, UInt_t row, UInt_t col)
        {
          return _table.at(lowerMetalLayer).at(row - MIN_GENERATE_ROW, col - MIN_GENERATE_COLUMN);
        }
    private:
        Vector_t<Vector2D<LefViaPrototype>> _table; ///< _table[layer][# of rows][# of columns] = via
};

PROJECT_NAMESPACE_END

#endif /// _DB_LEF_VIA_HPP_
