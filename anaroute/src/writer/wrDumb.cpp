/**
 * @file   wrDumb.cpp
 * @brief  Output file writer - Dumb
 * @author Hao Chen
 * @date   12/11/2019
 *
 **/

#include "wrDumb.hpp"

PROJECT_NAMESPACE_START

void DumbWriter::write(const String_t& placementFilename, const String_t& outputFilename) {
  
  GdsDB gdsDB;
  
  if (!readPlacementGds(placementFilename, gdsDB)) {
    fprintf(stderr, "LayoutWriter::%s ERROR: Write GDS Layout Failed!\n", __func__);
    return;
  }
  
  const Int_t gds_unit = std::round(1e-6 / gdsDB.precision());
  const Int_t db_unit = _cir.lef().units().databaseNumber();
  const Float_t scale =  (Float_t)gds_unit / db_unit;

  // find boundaries
  String_t topCellName = this->topCellName(gdsDB);
  auto flatCell = gdsDB.extractCell(topCellName);
  Box<Int_t> bbox;
  for (const auto& obj : flatCell.objects()) {
    GdsParser::GdsDB::GdsObjectHelpers()(obj.first, obj.second, ExtractBBoxAction(bbox));
  }

  FILE* fout = fopen(outputFilename.c_str(), "w");
  fprintf(fout, "%d %d %d %d\n", bbox.xl(),
                                 bbox.yl(),
                                 bbox.xh(),
                                 bbox.yh());
  UInt_t i, j, k, pinIdx;
  const Net* cpNet;
  Cir_ForEachNetC(_cir, cpNet, i) {
    Box<Int_t> tarBox;
    Int_t maxArea = MIN_INT;
    Int_t maxLayerIdx = MIN_INT;
    Net_ForEachPinIdx((*cpNet), pinIdx, j) {
      const Pin& pin = _cir.pin(pinIdx);
      const Int_t layerIdx = pin.maxLayerIdx();
      if (layerIdx > maxLayerIdx) {
        maxLayerIdx = layerIdx;
        tarBox.setBounds(0, 0, 0, 0);
        maxArea = MIN_INT;
      }
      const Box<Int_t>* cpBox;
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, k) {
        if (cpBox->area() > maxArea) {
          maxArea = cpBox->area();
          tarBox = *cpBox;
        }
      }
    }
    const auto& vWires = cpNet->vWires();
    for (const auto& pair : vWires) {
      if (pair.second > maxLayerIdx) {
        maxLayerIdx = pair.second;
        tarBox = pair.first;
        maxArea = pair.first.area();
      }
      else if (pair.second == maxLayerIdx and pair.first.area() > maxArea) {
        tarBox = pair.first;
        maxArea = pair.first.area();
      }
    }
    const Int_t routingLayer = _cir.lef().layerPair(maxLayerIdx).second + 1;
    fprintf(fout, "%s %d %d %d %d %d\n", cpNet->name().c_str(),
                                         routingLayer,
                                         (Int_t)(tarBox.xl() * scale),
                                         (Int_t)(tarBox.yl() * scale),
                                         (Int_t)(tarBox.xh() * scale),
                                         (Int_t)(tarBox.yh() * scale));
  }
  fclose(fout);
}

/////////////////////////////////////////
//    Private functions                //
/////////////////////////////////////////
bool DumbWriter::readPlacementGds(const String_t& filename, GdsDB& gdsDB) {
  GdsParser::GdsDB::GdsReader gdsr(gdsDB);
  if (!gdsr(filename)) {
    fprintf(stderr, "LayoutWriter::%s ERROR: Cannot open placement file %s!\n", __func__, filename.c_str());
    return false;
  }

  return true;
}

String_t DumbWriter::topCellName(const GdsDB& gdsDB) {
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


PROJECT_NAMESPACE_END
