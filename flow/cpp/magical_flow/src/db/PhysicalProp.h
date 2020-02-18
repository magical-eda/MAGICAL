/**
 * @file PhysicalProp.h
 * @brief The physical properties
 * @author Keren Zhu, Mingjie Liu
 * @date 06/19/2019
 */

#ifndef MAGICAL_FLOW_PHYSICAL_PROP_H_
#define MAGICAL_FLOW_PHYSICAL_PROP_H_

#include "global/global.h"
#include <string>

PROJECT_NAMESPACE_BEGIN

/// @class MAGICAL_PROJECT::MosProp
/// @brief the properties of the general mosfets.
/// Current deprecated support for multi
class MosProp
{
    public:
        /// @brief default constructor
        explicit MosProp() = default;
        /// @brief get the length for the MOSFET
        /// @return the length of the transistor. unit: e-12
        IntType length() const { return _length; }
        /// @brief get the width for the MOSFET
        /// @return the width of the transistor. unit: e-12
        IntType width() const { return _width; }
        /// @brief set the length for the MOSFET
        /// @param the length of the transistor. unit: e-12
        void setLength(IntType length) { _length = length; }
        /// @brief set the width for the MOSFET
        /// @param the width of the transistor. unit: e-12
        void setWidth(IntType width) { _width = width; }
        /// @brief whether _length is set
        /// @return whether _length is set
        bool lengthValid() const { return _length != -1; }
        /// @brief whether _width is set
        /// @return whether _width is set
        bool widthValid() const { return _width != -1; }
        IntType mult() const { return _mult; }
        /// @brief set number of multiplier
        /// @param multiplier
        void setMult(IntType mult) { _mult = mult; }
        /// @brief whether _mult is set
        /// @return whether _mult is set
        IntType numFingers() const { return _numFingers; }
        /// @brief set the number of fingers
        /// @param the number of fingers
        void setNumFingers(IntType numFingers) { _numFingers = numFingers; }
        /// @brief whether _numFingers is set
        /// @return whether _numFingers is set
        std::string attr() const { return _attributes; }
        /// @brief set _attribute string
        /// @param the attribute string
        void setAttr(std::string attributes) { _attributes = attributes; }
        /// @brief set _attribute string
        /// @param the attribute string
        std::string pinConType() const { return _pinConType; }
        /// @brief get pinConType string
        /// @param the type string: 'GS', 'DG', 'DS' for self connection
        void setPinConType(std::string type) { _pinConType = type; }
        /// @brief set pinConType string
        /// @param the type string: 'GS', 'DG', 'DS' for self connection
        void appendBulkCon(IndexType pin) { _bulkCon.emplace_back(pin); }
        /// @brief append to bulkCon 
        /// @param the pinType connected to bulk: 0:D, 1:G, 2:S 
        /// Currently only valid for PMOS
        IndexType numBulkCon() { return _bulkCon.size(); }
        /// @brief append to bulkCon 
        /// @param the pinType connected to bulk: 0:D, 1:G, 2:S 
        /// Currently only valid for PMOS
        IndexType bulkCon(IndexType id) { return _bulkCon.at(id); }
        /// @brief return the bulkCon at Index
        /// @param the Index
        /// Currently only valid for PMOS
    protected:
        IntType _length = -1; ///< l. unit: e-12
        IntType _width = -1; ///< w. unit: e-12
        IntType _mult = 1; ///<mult. 
        IntType _numFingers = 1; ///<numFinger.
        std::string _attributes=""; ///<attributes. This is reference of the device to match devgen.
        std::string _pinConType=""; ///<pinConType. Self connection type.
        std::vector<IndexType> _bulkCon; ///<bulkConnections.
};

/// @class MAGICAL_FLOW::NchProp
/// @brief property for the nch transistor
/// Current deprecated support for multi
class NchProp : public MosProp
{
    public:
        explicit NchProp() = default; 
        /// @brief multiplier
        /// @return multiplier
};

// @class MAGICAL_FLOW::PchProp
/// @brief property for the pch transistor
/// Current deprecated support for multi
class PchProp : public MosProp
{
    public:
        explicit PchProp() = default; 
        /// @brief multiplier
        /// @return multiplier
};

/// @class MAGICAL_FLOW::RppolywoMProp
/// @brief the property for resistors
/// Current deprecated support for multi
class ResProp
{
    public:
        explicit ResProp() = default;
        /// @brief get _lr. unit: e-12
        /// @return _lr. unit: e-12
        IntType lr() const { return _lr; }
        /// @brief set _lr. unit: e-12
        /// @param _lr. unit: e-12
        void setLr(IntType lr) { _lr = lr; }
        /// @brief whether lr is set
        /// @return whether lr is set
        bool lrValid() const { return _lr != -1; }
        /// @brief get _wr. unit: e-12
        /// @return _wr. unit: e-12
        IntType wr() const { return _wr; }
        /// @brief set _wr. unit: e-12
        /// @param _wr. unit: e-12
        void setWr(IntType wr) { _wr = wr; }
        /// @brief whether wr is set
        /// @return whether wr is set
        bool wrValid() const { return _wr != -1; }
        /// @brief whether resister is series
        /// @return whether resister is series
        bool series() const { return _series; }
        /// @brief set _series
        void setSeries(bool series) { _series = series; }
        /// @brief whether resister is parallel
        /// @return whether resister is parallel
        bool parallel() const { return _parallel; }
        /// @brief set _parallel
        void setParallel(bool parallel) { _parallel = parallel; }
        /// @brief get _segNum
        /// @return number of segments in s/p
        IntType segNum() const { return _segNum; }
        /// @brief set _parallel
        void setSegNum(IntType segNum) { _segNum = segNum; }
        /// @brief get _segSpace
        /// @param _segSpace. unit: e-12
        IntType segSpace() const { return _segSpace; } 
        /// @brief set _segSpace. unit: e-12
        /// @param _segSpace. unit: e-12
        void setSegSpace(IntType segSpace) { _segSpace = segSpace; } 
        /// @brief get _attribute string
        /// @param the attribute string
        std::string attr() const { return _attributes; }
        /// @brief set _attribute string
        /// @param the attribute string
        void setAttr(std::string attributes) { _attributes = attributes; }
   protected:
        IntType _lr = -1; ///< length. unit: e-12
        IntType _wr = -1; ///< width. unit: e-12
        bool _series = false; ///< series. 
        bool _parallel = false; ///< parallel
        IntType _segNum = 1; ///< number of segments
        IntType _segSpace = -1; ///< space between segments. unit: e-12
        std::string _attributes = ""; ///< attribute string.
};

/// @class MAGICAL_FLOW::CapProp
/// @brief the property for capacitors
/// Current deprecated support for multi
class CapProp
{
    public:
        /// @brief default constructor
        explicit CapProp() = default; 
        /// @brief get _numFingers
        /// @return _numFingers
        IntType numFingers() const { return _numFingers; }
        /// @brief set _numFingers. 
        /// @param _numFingers
        void setNumFingers(IntType numFingers) { _numFingers = numFingers; }
        /// @brief get _lr. unit: e-12
        /// @return _lr. unit: e-12
        IntType lr() const { return _lr; }
        /// @brief set _lr. unit: e-12
        /// @param _lr. unit: e-12
        void setLr(IntType lr) { _lr = lr; }
        /// @brief whether lr is set
        /// @return whether lr is set
        bool lrValid() const { return _lr != -1; }
        /// @brief get _w. unit: e-12
        /// @return _w. unit: e-12
        IntType w() const { return _w; }
        /// @brief set _w. unit: e-12
        /// @param _w. unit: e-12
        void setW(IntType w) { _w = w; }
        /// @brief whether w is set
        /// @return whether w is set
        bool wValid() const { return _w != -1; }
        /// @brief get _spacing. unit: e-12
        /// @return _spacing. Spacing between fingers. unit: e-12
        IntType spacing() const { return _spacing; }
        /// @brief set _spacing. unit: e-12
        /// @param _spacing. unit: e-12
        void setSpacing(IntType spacing) { _spacing = spacing; }
        /// @brief whether _spacing is set
        /// @return whether _spacing is set
        bool spacingValid() const { return _spacing != -1; }
        /// @brief get _stm.
        /// @return _stm.
        IntType stm() const { return _stm; }
        /// @brief set _stm.
        /// @param _stm.
        void setStm(IntType stm) { _stm = stm; }
        /// @brief whether stm is set
        /// @return whether stm is set
        bool stmValid() const { return _stm != -1; }
        /// @brief get _spm.
        /// @return _spm.
        IntType spm() const { return _spm; }
        /// @brief set _spm.
        /// @param _spm.
        void setSpm(IntType spm) { _spm = spm; }
        /// @brief whether spm is set
        /// @return whether spm is set
        bool spmValid() const { return _spm != -1; }
        /// @brief get _multi.
        /// @return _multi.
        IntType multi() const { return _multi; }
        /// @brief set _multi.
        /// @param _multi.
        void setMulti(IntType multi) { _multi = multi; }
        /// @brief whether multi is set
        /// @return whether multi is set
        IntType ftip() const { return _ftip; }
        /// @brief set _ftip.
        /// @param _ftip.
        void setFtip(IntType ftip) { _ftip = ftip; }
        /// @brief whether ftip is set
        /// @return whether ftip is set
        bool ftipValid() const { return _ftip != -1; }
        /// @brief get _attribute string
        /// @param the attribute string
        std::string attr() const { return _attributes; }
        /// @brief set _attribute string
        /// @param the attribute string
        void setAttr(std::string attributes) { _attributes = attributes; }
    protected:
        IntType _numFingers = 1; ///< number of fingers.
        IntType _lr = -1; ///< lr. unit: e-12
        IntType _w = -1; ///< w. unit: e-12
        IntType _spacing = -1; ///< s. unit: e-12
        IntType _stm = -1; ///< stm
        IntType _spm = -1; ///< spm
        IntType _multi = -1; ///< multi
        IntType _ftip = -1; ///< ftip. unit: e-12
        std::string _attributes = ""; ///< attributes string.
};


class PhyPropDB
{
    public:
        /// @brief default constructor
        explicit PhyPropDB() = default; 
        /// @brief get a nch property
        /// @param the index
        /// @return a nch property
        NchProp & nch(IndexType idx) { return _nchArray.at(idx); }
        /// @brief allocate a new nch property
        /// @return the index of the property
        IndexType allocateNch() { _nchArray.emplace_back(NchProp()); return _nchArray.size() - 1; }
        /// @brief get a pch property
        /// @param the index
        /// @return a pch property
        PchProp & pch(IndexType idx) { return _pchArray.at(idx); }
        /// @brief allocate a new pch property
        /// @return the index of the pch property
        IndexType allocatePch() { _pchArray.emplace_back(PchProp());  return _pchArray.size() - 1; }
        /// @brief get a resister property
        /// @param the index
        /// @return a resister property
        ResProp & resister(IndexType idx) { return _resArray.at(idx); }
        /// @brief allocate a new resister property
        /// @return the index
        IndexType allocateRes() { _resArray.emplace_back(ResProp()); return _resArray.size() - 1; }
        /// @brief get a capacitor property
        /// @param the index
        /// @return the capacitor property
        CapProp & capacitor(IndexType idx) { return _capArray.at(idx); }
        /// @brief allocate a new capacitore property
        /// @return the index
        IndexType allocateCap() { _capArray.emplace_back(CapProp()); return _capArray.size() - 1; }
    private:
        std::vector<NchProp> _nchArray; ///< for nch
        std::vector<PchProp> _pchArray; ///< for pch
        std::vector<ResProp> _resArray; ///< for rppolym
        std::vector<CapProp> _capArray; ///< for cfmom
};


PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_PHYSICAL_PROP_H_
