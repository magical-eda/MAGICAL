/**
 * @file   postMgr.hpp
 * @brief  Post DR processing - Manager
 * @author Hao Chen
 * @date   02/15/2020
 *
 **/

#ifndef _POST_MGR_HPP_
#define _POST_MGR_HPP_

#include "src/db/dbCir.hpp"
#include "src/geo/box2polygon.hpp"

PROJECT_NAMESPACE_START

class PostMgr {
 public:
  PostMgr(CirDB& c)
    : _cir(c) {}
  ~PostMgr() {}
 
  void solve();

 private:
  CirDB& _cir;

  Int_t _cnt = 0;

  void patchJogs();

  void initPolygons(Vector_t<Vector_t<Polygon<Int_t>>>& vvPolygons);
  bool patchConcaveJogs(const Vector_t<Vector_t<Polygon<Int_t>>>& vvPolygons);
  bool patchConvexJogs(const Vector_t<Vector_t<Polygon<Int_t>>>& vvPolygons);
  void addMetal2Net();

  enum class JogOrient_t {
    NE = 0,
    SE = 1,
    SW = 2,
    NW = 3,
    INVALID = 4
  };

  bool clockwise(const Point<Int_t>& p0, const Point<Int_t>& p1, const Point<Int_t>& p2, JogOrient_t& orient);
  bool counterClockwise(const Point<Int_t>& p0, const Point<Int_t>& p1, const Point<Int_t>& p2, JogOrient_t& orient);


};

PROJECT_NAMESPACE_END

#endif /// _POST_MGR_HPP_
