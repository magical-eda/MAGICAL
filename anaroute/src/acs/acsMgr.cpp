/**
 * @file   acsMgr.cpp
 * @brief  Access points manager
 * @author Keren Zhu
 * @date   02/05/2020
 *
 **/

#include "acsMgr.hpp"

PROJECT_NAMESPACE_START

void AcsMgr::computeAcs() {
  fprintf(stdout, "AcsMgr::%s Start Access Points Generation\n", __func__);
  for (UInt_t pinIdx = 0; pinIdx < _cir.numPins(); ++pinIdx) {
    _curPinIdx = pinIdx;
    computePinAcs();
  }
}


void AcsMgr::computePinAcs() {
  auto& pin = _cir.pin(_curPinIdx);
  Vector_t<CandidateGridPt> candGridPts; // The candidates for pin access
  for (UInt_t layerIdx = pin.minLayerIdx(); layerIdx <= pin.maxLayerIdx(); ++layerIdx) {
    for (UInt_t i = 0; i < pin.numBoxes(layerIdx); ++i) {
      const auto& box = pin.box(layerIdx, i);
      Vector_t<Point3d<Int_t>> vAcs;
      computeBoxAcs(box, layerIdx, vAcs);
      for (const auto& p : vAcs) {
        candGridPts.emplace_back(CandidateGridPt(p.x(), p.y(), p.z(), i));
      }
    }
  }
  Vector_t<CandidateAcsPt> candAcsPts;
  for (const auto &candGridPt : candGridPts)
  {
    generateCandAcsPt(candGridPt, candAcsPts);
  }
  for (const auto &candAcsPt : candAcsPts)
  {
    pin.addAcsPt(candAcsPt.acs);
  }
}

void AcsMgr::computeBoxAcs(const Box<Int_t>& box, const Int_t layerIdx, Vector_t<Point3d<Int_t>>& vAcs) {
  const Int_t lowerGridIdxX = (box.xl() - _cir.gridOffsetX() + _cir.gridStep() - 1) / _cir.gridStep(); // round up 
  const Int_t lowerGridIdxY = (box.yl() - _cir.gridOffsetY() + _cir.gridStep() - 1) / _cir.gridStep(); // round up
  const Int_t higherGridIdxX = (box.xh() - _cir.gridOffsetX()) / _cir.gridStep(); // round down
  const Int_t higherGridIdxY = (box.yh() - _cir.gridOffsetY()) / _cir.gridStep(); // round down
  for (Int_t x = lowerGridIdxX; x <= higherGridIdxX; ++x) {
    for (Int_t y = lowerGridIdxY; y <= higherGridIdxY; ++y) {
      vAcs.emplace_back(x * _cir.gridStep() + _cir.gridOffsetX(),
                        y * _cir.gridStep() + _cir.gridOffsetY(),
                        layerIdx);
    }
  }
}

void AcsMgr::generateCandAcsPt(const CandidateGridPt &gridPt, Vector_t<CandidateAcsPt> &candAcsPts)
{
  generateVerticalCandAcsPts(gridPt, candAcsPts);
  generateHorizontalCandAcsPts(gridPt, candAcsPts);
}

void AcsMgr::generateHorizontalCandAcsPts(const CandidateGridPt &gridPt, Vector_t<CandidateAcsPt> &candAcsPts)
{
  static constexpr Int_t maxAllowedCands = 1;
  UInt_t originCandSize = candAcsPts.size();
  auto calculateOverlap = [&](CandidateAcsPt &candAcsPt)
  {
    auto rect = computeExtensionRect(candAcsPt);
    Int_t overlapArea = _cir.overlapAreaWithOD(rect);
    candAcsPt.overlapAreaOD = overlapArea;
  };
  // Insert different directions
  // East
  candAcsPts.emplace_back(CandidateAcsPt(AcsPt(gridPt.pt, AcsPt::DirType::EAST)));
  calculateOverlap(candAcsPts.back());
  // WEST
  candAcsPts.emplace_back(CandidateAcsPt(AcsPt(gridPt.pt, AcsPt::DirType::WEST)));
  calculateOverlap(candAcsPts.back());
  // NORTH
  candAcsPts.emplace_back(CandidateAcsPt(AcsPt(gridPt.pt, AcsPt::DirType::NORTH)));
  calculateOverlap(candAcsPts.back());
  // SOUTH
  candAcsPts.emplace_back(CandidateAcsPt(AcsPt(gridPt.pt, AcsPt::DirType::SOUTH)));
  calculateOverlap(candAcsPts.back());
  // Sort the new generated candidates with increasing overlap areaWEST
  std::sort(candAcsPts.begin() + originCandSize, candAcsPts.end());
  Int_t numZeros = 0;
  for (UInt_t idx = 0; idx < 4; ++idx)
  {
    if (candAcsPts.at(idx + originCandSize).overlapAreaOD == 0)
    {
      numZeros++;
    }
    else
    {
      break;
    }
  }
  // Erase the unwanted ones
  candAcsPts.erase(candAcsPts.begin() + originCandSize + std::max(maxAllowedCands, numZeros), candAcsPts.end());
}

Box<Int_t> AcsMgr::computeExtensionRect(const CandidateAcsPt &acsPt)
{
  Int_t step = _cir.gridStep(); 
  Int_t width = _cir.lef().routingLayer(_cir.lef().layerPair(acsPt.acs.gridPt().z()).second).minWidth();
  Int_t eolExtension = width;
  Point<Int_t> origin = Point<Int_t>(acsPt.acs.gridPt().x(), acsPt.acs.gridPt().y()); 
  // The below is east
  Box<Int_t> rect = Box<Int_t>(
      origin.x() - eolExtension,
      origin.y() - width / 2,
      origin.x() + step + eolExtension,
      origin.y() + width / 2
      );
  if (acsPt.acs.dir() == AcsPt::DirType::NORTH)
  {
    // counterclockwise rorate 90
    rect.rotate90(origin.x(), origin.y(), false);
  }
  else if (acsPt.acs.dir() == AcsPt::DirType::SOUTH)
  {
    // clockwise rorate 90
    rect.rotate90(origin.x(), origin.y(), true);
  }
  else if (acsPt.acs.dir() == AcsPt::DirType::EAST)
  {
  }
  else if (acsPt.acs.dir() == AcsPt::DirType::WEST)
  {
    rect.flipX(origin.x());
  }
  else
  {
    assert(0);
  }
  return rect;
}
PROJECT_NAMESPACE_END
