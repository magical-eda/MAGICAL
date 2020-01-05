/**
 * @file   grGridMap3d.hpp
 * @brief  Global Routing Structure - 3D GridMap
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/

#ifndef _GR_GRID_MAP_3D_HPP_
#define _GR_GRID_MAP_3D_HPP_

#include "grGridCell.hpp"
#include "grGridEdge.hpp"

PROJECT_NAMESPACE_START

class GrGridMap3d {
 public:
  GrGridMap3d() {}
  ~GrGridMap3d() {}

  /////////////////////////////////////////
  //    Getters                          //
  /////////////////////////////////////////
  // gridcells
  UInt_t              numGridCellsX()                                                           const { return _vvvGridCells[0].size(); }
  UInt_t              numGridCellsY()                                                           const { return _vvvGridCells[0][0].size(); }
  UInt_t              numGridCellsZ()                                                           const { return _vvvGridCells.size(); }
  GrGridCell&         gridCell(const UInt_t z, const UInt_t x, const UInt_t y)                        { return _vvvGridCells[z][x][y]; }
  const GrGridCell&   gridCell(const UInt_t z, const UInt_t x, const UInt_t y)                  const { return _vvvGridCells[z][x][y]; }
  
  // gridedges
  UInt_t              numGridEdgesX(const UInt_t t)                                             const { return _vvvGridEdges[t][0].size(); }
  UInt_t              numGridEdgesY(const UInt_t t)                                             const { return _vvvGridEdges[t][0][0].size(); }
  UInt_t              numGridEdgesZ(const UInt_t t)                                             const { return _vvvGridEdges[t].size(); }
  GrGridEdge&         gridEdge(const UInt_t i, const UInt_t z, const UInt_t x, const UInt_t y)        { return _vvvGridEdges[i][z][x][y]; }
  const GrGridEdge&   gridEdge(const UInt_t i, const UInt_t z, const UInt_t x, const UInt_t y)  const { return _vvvGridEdges[i][z][x][y]; }
  GrGridEdge&         gridEdgeHor(const UInt_t z, const UInt_t x, const UInt_t y)                     { return _vvvGridEdges[0][z][x][y]; }
  const GrGridEdge&   gridEdgeHor(const UInt_t z, const UInt_t x, const UInt_t y)               const { return _vvvGridEdges[0][z][x][y]; }
  GrGridEdge&         gridEdgeVer(const UInt_t z, const UInt_t x, const UInt_t y)                     { return _vvvGridEdges[1][z][x][y]; }
  const GrGridEdge&   gridEdgeVer(const UInt_t z, const UInt_t x, const UInt_t y)               const { return _vvvGridEdges[1][z][x][y]; }
  GrGridEdge&         gridEdgeVia(const UInt_t z, const UInt_t x, const UInt_t y)                     { return _vvvGridEdges[2][z][x][y]; }
  const GrGridEdge&   gridEdgeVia(const UInt_t z, const UInt_t x, const UInt_t y)               const { return _vvvGridEdges[2][z][x][y]; }

  /////////////////////////////////////////
  //    Setters                          //
  /////////////////////////////////////////
  void setNumZ(const UInt_t i) {
    assert(i >= 1);
    _vvvGridCells.resize(i);
    for (UInt_t t = 0; t < 3; ++t) {
      _vvvGridEdges[t].resize(i);
    }
  }
  void setNumX(const UInt_t i) {
    assert(i >= 1);
    for (auto& vv : _vvvGridCells)
      vv.resize(i);
    for (auto& vv : _vvvGridEdges[0])
      vv.resize(i - 1);
    for (auto& vv : _vvvGridEdges[1])
      vv.resize(i);
    for (auto& vv : _vvvGridEdges[2])
      vv.resize(i);
  }
  void setNumY(const UInt_t i) {
    assert(i >= 1);
    for (auto& vv : _vvvGridCells)
      for (auto& v : vv)
        v.resize(i);
    for (auto& vv : _vvvGridEdges[0])
      for (auto& v : vv)
        v.resize(i);
    for (auto& vv : _vvvGridEdges[1])
      for (auto& v : vv)
        v.resize(i - 1);
    for (auto& vv : _vvvGridEdges[2])
      for (auto& v : vv)
        v.resize(i);
  }
  void setGridCell(const UInt_t z, const UInt_t x, const UInt_t y, const GrGridCell& c) {
    _vvvGridCells[z][x][y] = c;
  }
  void setGridEdge(const UInt_t i, const UInt_t z, const UInt_t x, const UInt_t y, const GrGridEdge& e) {
    _vvvGridEdges[i][z][x][y] = e;
  }
 
 private:
  Vector_t<Vector_t<Vector_t<GrGridCell>>> _vvvGridCells;
  Vector_t<Vector_t<Vector_t<GrGridEdge>>> _vvvGridEdges[3]; // 0: hor, 1: ver, 2: via
};

PROJECT_NAMESPACE_END

#endif /// _GR_GRID_MAP_3D_HPP_
