/**
 * @file   drSymmetry.hpp
 * @brief  Degree of symmetric detection
 * @author Hao Chen
 * @date   02/26/2020
 *
 **/

#ifndef _DR_SYMMETRY_HPP_
#define _DR_SYMMETRY_HPP_

#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class DrSymmetry {
 public:
  DrSymmetry(CirDB& cir)
    : _cir(cir) {}
  ~DrSymmetry() {}

  void solve(const bool bUseSymFile);

  void    bestMatching(const Net& net1, const Net& net2, Int_t& bestSymAxisX, Float_t& maxDegSymPre);
  Float_t degSymPre(const Net& net1, const Net& net2, const Int_t symAxisX);
  Float_t degSelfSymPre(const Net& net, const Int_t symAxisX);

 private:
  CirDB& _cir;

  // helper
  void addPinShapes(const Net& net, Vector_t<Vector_t<Box<Int_t>>>& vvBoxes);
  bool bExistTotallySymPin(const Pin& pin, const Int_t symAxisX, const Vector_t<Vector_t<Box<Int_t>>>& vvSymBoxes);
};

PROJECT_NAMESPACE_END

#endif /// _DR_SYMMETRY_HPP_
