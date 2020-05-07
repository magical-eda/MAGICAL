/**
 * @file   wrLayout.cpp
 * @brief  Output file writer - final layout
 * @author Hao Chen
 * @date   11/21/2019
 *
 **/
using namespace std;
#include "wrLayout.hpp"
#include "wrGds.hpp"

PROJECT_NAMESPACE_START

void LayoutWriter::writeGds(const String_t& placementFilename, const String_t& outputFilename, const bool bFlatten) {
  GdsDB gdsDB;

  if (!readPlacementGds(placementFilename, gdsDB)) {
    fprintf(stderr, "LayoutWriter::%s ERROR: Write GDS Layout Failed!\n", __func__);
    return;
  }
  const Int_t gds_unit = std::round(1e-6 / gdsDB.precision());
  const Int_t db_unit = _cir.lef().units().databaseNumber();
  const Float_t scale =  (Float_t)gds_unit / db_unit;

  String_t topCellName = this->topCellName(gdsDB);
  GdsCell& gdsCell = *gdsDB.getCell(topCellName);

  addRoutingLayout(gdsCell, scale);

  if (bFlatten) { 
    auto flatCell = gdsDB.extractCell(topCellName);
    flatCell.setName(flatCell.name() + "_flat");
    gdsDB.cells().clear();
    gdsDB.addCell(flatCell);
  }

  GdsParser::GdsDB::GdsWriter gw(gdsDB);
  gw(outputFilename.c_str());
}


/////////////////////////////////////////
//    Private functions                //
/////////////////////////////////////////
bool LayoutWriter::readPlacementGds(const String_t& filename, GdsDB& gdsDB) {
  GdsParser::GdsDB::GdsReader gdsr(gdsDB);
  if (!gdsr(filename)) {
    fprintf(stderr, "LayoutWriter::%s ERROR: Cannot open placement file %s!\n", __func__, filename.c_str());
    return false;
  }

  return true;
}

void LayoutWriter::addRoutingLayout(GdsCell& gdsCell, const Float_t scale) {
  UInt_t i;
  const Net* cpNet;
  Cir_ForEachNet(_cir, cpNet, i) {
    const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vWires = cpNet->vWires();
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vWires) {
      const Box<Int_t>& box = pair.first;
      const Int_t layerIdx = pair.second;
      const Int_t maskIdx = _cir.layerIdx2MaskIdx(pair.second);
      if (layerIdx > (Int_t)_cir.lef().routingLayerIdx2LayerIdx(5))
        addBox2Cell(gdsCell, maskIdx, box, scale, 40);
      else
        addBox2Cell(gdsCell, maskIdx, box, scale, 0);
    }
    if (cpNet->bIOPort()) {
      const Pin& pin = _cir.pin(cpNet->pinIdx(0));
      const Int_t layerIdx = pin.minLayerIdx();
      const Box<Int_t>& box = pin.box(layerIdx, 0);
      const Int_t maskIdx = _cir.layerIdx2MaskIdx(layerIdx);
      addTxt2Cell(gdsCell, maskIdx, box.center(), cpNet->name(), 20, scale);
    }
  }
}

String_t LayoutWriter::topCellName(const GdsDB& gdsDB) {
  UMap_t<String_t, bool> cellNameFoundMap;
  const auto& cells = gdsDB.cells();
  for (Int_t i = cells.size() - 1; i >= 0; --i) {
    cellNameFoundMap[cells.at(i).name()] = false;
  }
  for (const auto& cell : cells) {
    for (const auto& obj : cell.objects()) {
      String_t name;
      GdsParser::GdsDB::GdsObjectHelpers()(obj.first, obj.second, GetSRefNameAction(name));
      if (name != "") {
        cellNameFoundMap[name] = true;
      }
    }
  }
  for (const auto& pair : cellNameFoundMap) {
    if (pair.first != "" and !pair.second) {
      return pair.first;
    }
  }
  return "";
}

void LayoutWriter::addBox2Cell(GdsCell& gdsCell, const Int_t maskIdx, const Box<Int_t>& box, const Float_t scale, const Int_t dataType) {
  Vector_t<GdsPoint> vPts;
  auto scalePoint = [&] (GdsPoint& pt) {
    Int_t x = std::round(pt.x() * scale);
    Int_t y = std::round(pt.y() * scale);
    pt = GdsPoint(x, y);
  }; 

  GdsPoint pt0(box.xl(), box.yl());
  GdsPoint pt1(box.xl(), box.yh());
  GdsPoint pt2(box.xh(), box.yh());
  GdsPoint pt3(box.xh(), box.yl());
  GdsPoint pt4(box.xl(), box.yl());

  scalePoint(pt0);
  scalePoint(pt1);
  scalePoint(pt2);
  scalePoint(pt3);
  scalePoint(pt4);

  vPts.emplace_back(pt0);
  vPts.emplace_back(pt1);
  vPts.emplace_back(pt2);
  vPts.emplace_back(pt3);
  vPts.emplace_back(pt4);

  gdsCell.addPolygon(maskIdx, dataType, vPts);

}

void LayoutWriter::addTxt2Cell(GdsCell& gdsCell, const Int_t maskIdx, const Point<Int_t>& pt,  const String_t& txt, const Int_t size, const Float_t scale) {

  auto scalePoint = [&] (GdsPoint& pt) {
    Int_t x = std::round(pt.x() * scale);
    Int_t y = std::round(pt.y() * scale);
    pt = GdsPoint(x, y);
  }; 

  GdsPoint gdsPt(pt.x(), pt.y());
  scalePoint(gdsPt);

  gdsCell.addText(100 + maskIdx,
                  std::numeric_limits<Int_t>::max(),
                  0,
                  txt,
                  gdsPt,
                  std::numeric_limits<Int_t>::max(),
                  5, 0, 0.2, 0);
}


PROJECT_NAMESPACE_END

