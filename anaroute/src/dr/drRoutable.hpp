/**
 * @file   drRoutable.hpp
 * @brief  Net routable construction
 * @author Hao Chen
 * @date   02/21/2020
 *
 **/

#ifndef _DR_ROUTABLE_HPP_
#define _DR_ROUTABLE_HPP_

#include "src/db/dbCir.hpp"

PROJECT_NAMESPACE_START

class DrRoutable {
 public:
  DrRoutable(CirDB& c)
    : _cir(c) {}
  ~DrRoutable() {}

  void constructRoutables();

  void constructNetRoutables(Net& net, const bool bSym, const bool bSelfSym);
  void constructNormalNetRoutables(Net& net);
  void constructSelfSymNetRoutables(Net& net);
  void constructSymNetRoutables(Net& net);
  
  // for power
  void constructNormalPowerRoutables(Net& net);
  void constructSelfSymPowerRoutables(Net& net);
  

  // for cross sym
  bool bCrossSymNet(const Net& net);
  bool bCrossSymAxisX(const Net& net);
  void constructCrossSymNetRoutables(Net& net);


  // for debug
  void printNetRoutableInfo(const Net& net);

 private:
  CirDB& _cir;

  // helper
  void  addPinShapes(const Net& net, Vector_t<Vector_t<Box<Int_t>>>& vvBoxes);
  bool  bExistTotallySymPin(const Pin& pin, const Vector_t<Vector_t<Box<Int_t>>>& vvSymBoxes);
  Int_t totallySymPinIdx(const Pin& pin, const Vector_t<Vector_t<Box<Int_t>>>& vvSymBoxes);
  bool  bPinOnLeft(const Pin& pin);
  bool  bPinOnRight(const Pin& pin);
};

PROJECT_NAMESPACE_END

#endif /// _DR_ROUTABLE_HPP_
