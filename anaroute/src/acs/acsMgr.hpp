/**
 * @file   acsMgr.hpp
 * @brief  Access points manager
 * @author Keren Zhu
 * @date   02/05/2020
 *
 **/

#ifndef ANAROUTE_ACS_MGR_HPP_
#define ANAROUTE_ACS_MGR_HPP_

#include "src/global/global.hpp"
#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

/// @brief Manage the generation of the access points
class AcsMgr {
  struct CandidateGridPt {
   public:
    explicit CandidateGridPt(Int_t x, Int_t y, Int_t z, UInt_t boxIdx_)
      : pt(x, y, z), boxIdx(boxIdx_) {}
    Point3d<Int_t>  pt; ///< The 3D point for representing the grid indices
    UInt_t          boxIdx; ///< The index of the box this point is in. pin.box(pt.z(), boxIdx.y())
  };

  struct CandidateAcsPt
  {
      explicit CandidateAcsPt() = default;
      explicit CandidateAcsPt(const AcsPt & acspt) : acs(acspt), overlapAreaOD(0) {}
      
      AcsPt acs;
      Int_t overlapAreaOD = 0;
      bool operator<(const CandidateAcsPt &rhs) const
      {
          return overlapAreaOD < rhs.overlapAreaOD;
      }
  };
 public:
  explicit AcsMgr(CirDB & c)
    : _cir(c) {}
  /// @brief compute the access points and push the results into the circuit db
  void computeAcs();
  /// @brief compute the access points for one pin
  /// @param the pin index
  void computePinAcs();
  void computeBoxAcs(const Box<Int_t>& box, const Int_t layerIdx, Vector_t<Point3d<Int_t>>& vAcs);
  /// @brief generate AcsPt from GridPt
  void generateCandAcsPt(const CandidateGridPt &gridPt, Vector_t<CandidateAcsPt> &candAcsPts);
  /// @brief generate vertical candidate AcsPts
  void generateVerticalCandAcsPts(const CandidateGridPt &gridPt, Vector_t<CandidateAcsPt> &candAcsPts)
  {}
  /// @brief generate horizontal candidate AcsPts
  void generateHorizontalCandAcsPts(const CandidateGridPt &gridPt, Vector_t<CandidateAcsPt> &candAcsPts);
  /// @brief compute the rectangle shape of the extension metal
  Box<Int_t> computeExtensionRect(const CandidateAcsPt &candAcsPt);

 private:
  CirDB&  _cir;
  UInt_t _curPinIdx = 0;
};

PROJECT_NAMESPACE_END

#endif //ANAROUTE_ACS_MGR_HPP_
