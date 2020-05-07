/**
 * @file   grGridMap2d.hpp
 * @brief  Global Routing Structure - 2D GridMap
 * @author Hao Chen
 * @date   10/07/2019
 *
 **/

#ifndef _GR_GRID_MAP_2D_HPP_
#define _GR_GRID_MAP_2D_HPP_

#include "grGridCell.hpp"
#include "grGridEdge.hpp"

PROJECT_NAMESPACE_START

class GrGridMap2d {
 public:
  GrGridMap2d() {}
  ~GrGridMap2d() {}

  /////////////////////////////////////////
  //    Getters                          //
  /////////////////////////////////////////
  // gridcells
  UInt_t              numGridCellsX()                                           const  { return _vvGridCells.size(); }
  UInt_t              numGridCellsY()                                           const  { return _vvGridCells[0].size(); }
  GrGridCell&         gridCell(const UInt_t i, const UInt_t j)                         { return _vvGridCells[i][j]; }
  const GrGridCell&   gridCell(const UInt_t i, const UInt_t j)                  const  { return _vvGridCells[i][j]; }
  
  // gridedges
  UInt_t              numGridEdgesX(const UInt_t t)                             const  { return _vvGridEdges[t].size(); }
  UInt_t              numGridEdgesY(const UInt_t t)                             const  { return _vvGridEdges[t][0].size(); }
  GrGridEdge&         gridEdge(const bool bVer, const UInt_t i, const UInt_t j)        { return _vvGridEdges[bVer][i][j]; }
  const GrGridEdge&   gridEdge(const bool bVer, const UInt_t i, const UInt_t j) const  { return _vvGridEdges[bVer][i][j]; }
  GrGridEdge&         gridEdgeHor(const UInt_t i, const UInt_t j)                      { return _vvGridEdges[0][i][j]; }
  const GrGridEdge&   gridEdgeHor(const UInt_t i, const UInt_t j)               const  { return _vvGridEdges[0][i][j]; }
  GrGridEdge&         gridEdgeVer(const UInt_t i, const UInt_t j)                      { return _vvGridEdges[1][i][j]; }
  const GrGridEdge&   gridEdgeVer(const UInt_t i, const UInt_t j)               const  { return _vvGridEdges[1][i][j]; }

  /////////////////////////////////////////
  //    Setters                          //
  /////////////////////////////////////////
  void setNumX(const UInt_t i) {
    assert(i >= 1);
    _vvGridCells.resize(i);
    _vvGridEdges[0].resize(i - 1);
    _vvGridEdges[1].resize(i);
  }
  void setNumY(const UInt_t i) {
    assert(i >= 1);
    for (auto& v : _vvGridCells)
      v.resize(i);
    for (auto& v : _vvGridEdges[0])
      v.resize(i);
    for (auto& v : _vvGridEdges[1])
      v.resize(i - 1);
  }
  void setGridCell(const UInt_t i, const UInt_t j, const GrGridCell& c) {
    _vvGridCells[i][j] = c;
  }
  void setGridEdge(const bool bVer, const UInt_t i, const UInt_t j, const GrGridEdge& e) {
    _vvGridEdges[bVer][i][j] = e;
  }

 private:
  Vector_t<Vector_t<GrGridCell>> _vvGridCells;
  Vector_t<Vector_t<GrGridEdge>> _vvGridEdges[2]; // Horizontal _vvGridEdges[0][lower x][y]
                                                  // Vertical   _vvGridEdges[1][x][lower y]

};

PROJECT_NAMESPACE_END

#endif /// _GR_GRID_MAP_2D_HPP_
