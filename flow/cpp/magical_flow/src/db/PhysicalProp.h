/**
 * @file PhysicalProp.h
 * @brief The physical properties
 * @author Keren Zhu
 * @date 06/19/2019
 */

#ifndef MAGICAL_FLOW_PHYSICAL_PROP_H_
#define MAGICAL_FLOW_PHYSICAL_PROP_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class MAGICAL_PROJECT::MosProp
/// @brief the properties of the general mosfets.
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
    protected:
        IntType _length = -1; ///< l. unit: e-12
        IntType _width = -1; ///< w. unit: e-12
};

/// @class MAGICAL_FLOW::NchProp
/// @brief property for the nch transistor
class NchProp : public MosProp
{
    public:
        explicit NchProp() = default; 
        /// @brief multiplier
        /// @return multiplier
        IntType mult() const { return _mult; }
        /// @brief set number of multiplier
        /// @param multiplier
        void setMult(IntType mult) { _mult = mult; }
        /// @brief whether _mult is set
        /// @return whether _mult is set
        bool multValid() const { return _mult != -1; }
        /// @brief get number of fingers
        /// @return the number of fingers
        IntType numFingers() const { return _numFingers; }
        /// @brief set the number of fingers
        /// @param the number of fingers
        void setNumFingers(IntType numFingers) { _numFingers = numFingers; }
        /// @brief whether _numFingers is set
        /// @return whether _numFingers is set
        bool numFingersValid() const { return _numFingers != -1; }
        /// @brief get sd. unit: e-12
        /// @return sd. unit: e-12
        IntType sd() const { return _sd; }
        /// @brief set sd. unit: e-12
        /// @param sd. unit: e-12
        void setSd(IntType sd) { _sd = sd; }
        /// @brief whether sd is set
        /// @return whether sd is set
        bool sdValid() const { return _sd != -1; }
        /// @brief get ad. unit: e-15
        /// @return ad. unit: e-15
        IntType ad() const { return _ad; }
        /// @brief set ad. unit: e-15
        /// @param ad. unit: e-15
        void setAd(IntType ad) { _ad = ad; }
        /// @brief whether ad is set
        /// @return whether ad is set
        bool adValid() const { return _ad != -1; }
        /// @brief get as. unit: e-15
        /// @return as. unit: e-15
        IntType as() const { return _as; }
        /// @brief set as. unit: e-15
        /// @param as. unit: e-15
        void setAs(IntType as) { _as = as; }
        /// @brief whether as is set
        /// @return whether as is set
        bool asValid() const { return _as != -1; }
        /// @brief get pd. unit: e-12
        /// @return pd. unit: e-12
        IntType pd() const { return _pd; }
        /// @brief set pd. unit: e-12
        /// @param pd. unit: e-12
        void setPd(IntType pd) { _pd = pd; }
        /// @brief whether pd is set
        /// @return whether pd is set
        bool pdValid() const { return _pd != -1; }
        /// @brief get ps. unit: e-12
        /// @return ps. unit: e-12
        IntType ps() const { return _ps; }
        /// @brief set ps. unit: e-12
        /// @param ps. unit: e-12
        void setPs(IntType ps) { _ps = ps; }
        /// @brief whether ps is set
        /// @return whether ps is set
        bool psValid() const { return _ps != -1; }
        /// @brief get nrd. unit: e-12
        /// @return nrd. unit: e-12
        IntType nrd() const { return _nrd; }
        /// @brief set nrd. unit: e-12
        /// @param nrd. unit: e-12
        void setNrd(IntType nrd) { _nrd = nrd; }
        /// @brief whether nrd is set
        /// @return whether nrd is set
        bool nrdValid() const { return _nrd != -1; }
        /// @brief get nrs. unit: e-12
        /// @return nrs. unit: e-12
        IntType nrs() const { return _nrs; }
        /// @brief set nrs. unit: e-12
        /// @param nrs. unit: e-12
        void setNrs(IntType nrs) { _nrs = nrs; }
        /// @brief whether nrs is set
        /// @return whether nrs is set
        bool nrsValid() const { return _nrs != -1; }
        /// @brief get sa. unit: e-12
        /// @return sa. unit: e-12
        IntType sa() const { return _sa; }
        /// @brief set sa. unit: e-12
        /// @param sa. unit: e-12
        void setSa(IntType sa) { _sa = sa; }
        /// @brief whether sa is set
        /// @return whether sa is set
        bool saValid() const { return _sa != -1; }
        /// @brief get sb. unit: e-12
        /// @return sb. unit: e-12
        IntType sb() const { return _sb; }
        /// @brief set sb. unit: e-12
        /// @param sb. unit: e-12
        void setSb(IntType sb) { _sb = sb; }
        /// @brief whether sb is set
        /// @return whether sb is set
        bool sbValid() const { return _sb != -1; }
    protected:
        IntType _mult = -1; ///< m. 
        IntType _numFingers = -1; ///< nf.
        IntType _sd = -1; ///< sd. unit: e-12
        IntType _ad = -1; ///< ad. unit: e-15
        IntType _as = -1; ///< as. unit: e-15
        IntType _pd = -1; ///< pd. unit: e-12
        IntType _ps = -1; ///< ps. unit: e-12
        IntType _nrd = -1; ///< nrd. unit: e-12
        IntType _nrs = -1; ///< nrs. unit: e-12
        IntType _sa = -1; ///< sa. unit: e-12
        IntType _sb = -1; ///< sb. unit: e-12
};

/// @class MAGICAL_FLOW::Nch25ud18MacProp
/// @brief the properties for nch_25ud18_mac instance
class Nch25ud18macProp : public NchProp
{
    public:
        /// @brief default constructor
        explicit Nch25ud18macProp() = default;
        /// @brief get sa1. unit: e-12
        /// @return sa1. unit: e-12
        IntType sa1() const { return _sa1; }
        /// @brief set sa1. unit: e-12
        /// @param sa1. unit: e-12
        void setSa1(IntType sa1) { _sa1 = sa1; }
        /// @brief whether sa1 is set
        /// @return whether sa1 is set
        bool sa1Valid() const { return _sa1 != -1; }
        /// @brief get sa2. unit: e-12
        /// @return sa2. unit: e-12
        IntType sa2() const { return _sa2; }
        /// @brief set sa2. unit: e-12
        /// @param sa2. unit: e-12
        void setSa2(IntType sa2) { _sa2 = sa2; }
        /// @brief whether sa2 is set
        /// @return whether sa2 is set
        bool sa2Valid() const { return _sa2 != -1; }
        /// @brief get sa3. unit: e-12
        /// @return sa3. unit: e-12
        IntType sa3() const { return _sa3; }
        /// @brief set sa3. unit: e-12
        /// @param sa3. unit: e-12
        void setSa3(IntType sa3) { _sa3 = sa3; }
        /// @brief whether sa3 is set
        /// @return whether sa3 is set
        bool sa3Valid() const { return _sa3 != -1; }
        /// @brief get sb1. unit: e-12
        /// @return sb1. unit: e-12
        IntType sb1() const { return _sb1; }
        /// @brief set sb1. unit: e-12
        /// @param sb1. unit: e-12
        void setSb1(IntType sb1) { _sb1 = sb1; }
        /// @brief whether sb1 is set
        /// @return whether sb1 is set
        bool sb1Valid() const { return _sb1 != -1; }
        /// @brief get sb2. unit: e-12
        /// @return sb2. unit: e-12
        IntType sb2() const { return _sb2; }
        /// @brief set sb2. unit: e-12
        /// @param sb2. unit: e-12
        void setSb2(IntType sb2) { _sb2 = sb2; }
        /// @brief whether sb2 is set
        /// @return whether sb2 is set
        bool sb2Valid() const { return _sb2 != -1; }
        /// @brief get sb3. unit: e-12
        /// @return sb3. unit: e-12
        IntType sb3() const { return _sb3; }
        /// @brief set sb3. unit: e-12
        /// @param sb3. unit: e-12
        void setSb3(IntType sb3) { _sb3 = sb3; }
        /// @brief whether sb3 is set
        /// @return whether sb3 is set
        bool sb3Valid() const { return _sb3 != -1; }
    protected:
        IntType _sa1 = -1; ///< sa1. unit: e-12
        IntType _sa2 = -1; ///< sa2. unit: e-12
        IntType _sa3 = -1; ///< sa3. unit: e-12
        IntType _sb1 = -1; ///< sb1. unit: e-12
        IntType _sb2 = -1; ///< sb2. unit: e-12
        IntType _sb3 = -1; ///< sb3. unit: e-12
};

/// @class MAGICAL_FLOW::NchProp
/// @brief property for the nch transistor
class PchProp : public MosProp
{
    public:
        explicit PchProp() = default; 
        /// @brief multiplier
        /// @return multiplier
        IntType mult() const { return _mult; }
        /// @brief set number of multiplier
        /// @param multiplier
        void setMult(IntType mult) { _mult = mult; }
        /// @brief whether _mult is set
        /// @return whether _mult is set
        bool multValid() const { return _mult != -1; }
        /// @brief get number of fingers
        /// @return the number of fingers
        IntType numFingers() const { return _numFingers; }
        /// @brief set the number of fingers
        /// @param the number of fingers
        void setNumFingers(IntType numFingers) { _numFingers = numFingers; }
        /// @brief whether _numFingers is set
        /// @return whether _numFingers is set
        bool numFingersValid() const { return _numFingers != -1; }
        /// @brief get sd. unit: e-12
        /// @return sd. unit: e-12
        IntType sd() const { return _sd; }
        /// @brief set sd. unit: e-12
        /// @param sd. unit: e-12
        void setSd(IntType sd) { _sd = sd; }
        /// @brief whether sd is set
        /// @return whether sd is set
        bool sdValid() const { return _sd != -1; }
        /// @brief get ad. unit: e-15
        /// @return ad. unit: e-15
        IntType ad() const { return _ad; }
        /// @brief set ad. unit: e-15
        /// @param ad. unit: e-15
        void setAd(IntType ad) { _ad = ad; }
        /// @brief whether ad is set
        /// @return whether ad is set
        bool adValid() const { return _ad != -1; }
        /// @brief get as. unit: e-15
        /// @return as. unit: e-15
        IntType as() const { return _as; }
        /// @brief set as. unit: e-15
        /// @param as. unit: e-15
        void setAs(IntType as) { _as = as; }
        /// @brief whether as is set
        /// @return whether as is set
        bool asValid() const { return _as != -1; }
        /// @brief get pd. unit: e-12
        /// @return pd. unit: e-12
        IntType pd() const { return _pd; }
        /// @brief set pd. unit: e-12
        /// @param pd. unit: e-12
        void setPd(IntType pd) { _pd = pd; }
        /// @brief whether pd is set
        /// @return whether pd is set
        bool pdValid() const { return _pd != -1; }
        /// @brief get ps. unit: e-12
        /// @return ps. unit: e-12
        IntType ps() const { return _ps; }
        /// @brief set ps. unit: e-12
        /// @param ps. unit: e-12
        void setPs(IntType ps) { _ps = ps; }
        /// @brief whether ps is set
        /// @return whether ps is set
        bool psValid() const { return _ps != -1; }
        /// @brief get nrd. unit: e-12
        /// @return nrd. unit: e-12
        IntType nrd() const { return _nrd; }
        /// @brief set nrd. unit: e-12
        /// @param nrd. unit: e-12
        void setNrd(IntType nrd) { _nrd = nrd; }
        /// @brief whether nrd is set
        /// @return whether nrd is set
        bool nrdValid() const { return _nrd != -1; }
        /// @brief get nrs. unit: e-12
        /// @return nrs. unit: e-12
        IntType nrs() const { return _nrs; }
        /// @brief set nrs. unit: e-12
        /// @param nrs. unit: e-12
        void setNrs(IntType nrs) { _nrs = nrs; }
        /// @brief whether nrs is set
        /// @return whether nrs is set
        bool nrsValid() const { return _nrs != -1; }
        /// @brief get sa. unit: e-12
        /// @return sa. unit: e-12
        IntType sa() const { return _sa; }
        /// @brief set sa. unit: e-12
        /// @param sa. unit: e-12
        void setSa(IntType sa) { _sa = sa; }
        /// @brief whether sa is set
        /// @return whether sa is set
        bool saValid() const { return _sa != -1; }
        /// @brief get sb. unit: e-12
        /// @return sb. unit: e-12
        IntType sb() const { return _sb; }
        /// @brief set sb. unit: e-12
        /// @param sb. unit: e-12
        void setSb(IntType sb) { _sb = sb; }
        /// @brief whether sb is set
        /// @return whether sb is set
        bool sbValid() const { return _sb != -1; }
    protected:
        IntType _mult = -1; ///< m. 
        IntType _numFingers = -1; ///< nf.
        IntType _sd = -1; ///< sd. unit: e-12
        IntType _ad = -1; ///< ad. unit: e-15
        IntType _as = -1; ///< as. unit: e-15
        IntType _pd = -1; ///< pd. unit: e-12
        IntType _ps = -1; ///< ps. unit: e-12
        IntType _nrd = -1; ///< nrd. unit: e-12
        IntType _nrs = -1; ///< nrs. unit: e-12
        IntType _sa = -1; ///< sa. unit: e-12
        IntType _sb = -1; ///< sb. unit: e-12
};

/// @class MAGICAL_FLOW::RppolywoMProp
/// @brief the property for rppolywo_m resistors
class RppolywoMProp
{
    public:
        explicit RppolywoMProp() = default;
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
        /// @brief get _multi.
        /// @return _multi.
        IntType multi() const { return _multi; }
        /// @brief set _multi.
        /// @param _multi.
        void setMulti(IntType multi) { _multi = multi; }
        /// @brief whether multi is set
        /// @return whether multi is set
        bool multiValid() const { return _multi != -1; }
        /// @brief get _m.
        /// @return _m.
        IntType m() const { return _m; }
        /// @brief set _m.
        /// @param _m.
        void setM(IntType m) { _m = m; }
        /// @brief whether m is set
        /// @return whether m is set
        bool mValid() const { return _m != -1; }
    protected:
        IntType _lr = -1; ///< length? unit: e-12
        IntType _wr = -1; ///< width? unit: e-12
        IntType _multi = -1; ///< multipliers? 
        IntType _m = -1; ///< m ?

};

/// @class MAGICAL_FLOW::CfmomProp
/// @brief the property for cfmom capacitance
class CfmomProp
{
    public:
        /// @brief default constructor
        explicit CfmomProp() = default; 
        /// @brief get _nr. unit: e-12
        /// @return _nr. unit: e-12
        IntType nr() const { return _nr; }
        /// @brief set _nr. unit: e-12
        /// @param _nr. unit: e-12
        void setNr(IntType nr) { _nr = nr; }
        /// @brief whether nr is set
        /// @return whether nr is set
        bool nrValid() const { return _nr != -1; }
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
        /// @brief get _s. unit: e-12
        /// @return _s. unit: e-12
        IntType s() const { return _s; }
        /// @brief set _s. unit: e-12
        /// @param _s. unit: e-12
        void setS(IntType s) { _s = s; }
        /// @brief whether s is set
        /// @return whether s is set
        bool sValid() const { return _s != -1; }
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
        bool multiValid() const { return _multi != -1; }
        /// @brief get _ftip.
        /// @return _ftip.
        IntType ftip() const { return _ftip; }
        /// @brief set _ftip.
        /// @param _ftip.
        void setFtip(IntType ftip) { _ftip = ftip; }
        /// @brief whether ftip is set
        /// @return whether ftip is set
        bool ftipValid() const { return _ftip != -1; }
    protected:
        IntType _nr = -1; ///< nr? 
        IntType _lr = -1; ///< lr. unit: e-12
        IntType _w = -1; ///< w. unit: e-12
        IntType _s = -1; ///< s. unit: e-12
        IntType _stm = -1; ///< stm
        IntType _spm = -1; ///< spm
        IntType _multi = -1; ///< mutli
        IntType _ftip = -1; ///< ftip. unit: e-12
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
        /// @brief get a res rppolywom property
        /// @param the index
        /// @return a rppolywom property
        RppolywoMProp & rppolywom(IndexType idx) { return _rppolywomArray.at(idx); }
        /// @brief allocate a new res rppolywom property
        /// @return the index
        IndexType allocateRppolywom() { _rppolywomArray.emplace_back(RppolywoMProp()); return _rppolywomArray.size() - 1; }
        /// @brief get a cfmom property
        /// @param the index
        /// @return the cfmom property
        CfmomProp & cfmom(IndexType idx) { return _cfmomArray.at(idx); }
        /// @brief allocate a new cfmom property
        /// @return the index
        IndexType allocateCfmom() { _cfmomArray.emplace_back(CfmomProp()); return _cfmomArray.size() - 1; }
    private:
        std::vector<NchProp> _nchArray; ///< for nch
        std::vector<Nch25ud18macProp> _nch25ud18macArray; ///< for nch25ud18mac
        std::vector<PchProp> _pchArray; ///< for pch
        std::vector<RppolywoMProp> _rppolywomArray; ///< for rppolym
        std::vector<CfmomProp> _cfmomArray; ///< for cfmom
};
PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_PHYSICAL_PROP_H_
