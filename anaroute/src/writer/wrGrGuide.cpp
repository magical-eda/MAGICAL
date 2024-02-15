/**
 * @file   wrGrGuide.cpp
 * @brief  Output file writer - Global routing guide
 * @author Hao Chen
 * @date   10/18/2019
 *
 **/

#include "wrGrGuide.hpp"
#include "wrGds.hpp"

PROJECT_NAMESPACE_START

void GrGuideWriter::write(const String_t& filename) const {
  if (filename.empty())
    return;
  
  FILE* fout = fopen(filename.c_str(), "w");
  
  if (!fout) {
    fprintf(stderr, "GrGuideWriter::%s ERROR: Cannot write file %s!\n", __func__, filename.c_str());
    return;
  }

  UInt_t i;
  const Net* cpNet;
  Cir_ForEachNetC(_cir, cpNet, i) {
    fprintf(fout, "%s %u\n", cpNet->name().c_str(), cpNet->numGuides());
    const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vGuides = cpNet->vGuides();
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vGuides) {
      const Box<Int_t>& box = pair.first;
      //fprintf(fout, "%d %d %d %d %d\n", pair.second, box.xl(), box.yl(), box.xh(), box.yh());
      const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(pair.second);
      assert(layerPair.first == LefLayerType::ROUTING);
      fprintf(fout, "%d %d %d %d %d\n", layerPair.second + 1, box.xl(), box.yl(), box.xh(), box.yh());
    }
  }

  fclose(fout);
}

void GrGuideWriter::writeGds(const String_t& netName, const String_t& filename) const {
  if (filename.empty())
    return;

  FILE* fout = fopen(filename.c_str(), "w");

  if (!fout) {
    fprintf(stderr, "GrGuideWriter::%s ERROR: Cannot write file %s\n", __func__, filename.c_str());
    return;
  }

  GdsWriter gw(filename);
  gw.initWriter();
  gw.createLib("TOP", 2000, 1e-6/2000);
  gw.writeCellBgn("INTERCONNECTION");

  // write pin locs
  const Net& net = _cir.net(netName);
  UInt_t i, j, pinIdx, layerIdx;
  const Box<Int_t>* cpBox;
  Net_ForEachPinIdx(net, pinIdx, i) {
    const Pin& pin = _cir.pin(pinIdx);
    Pin_ForEachLayerIdx(pin, layerIdx) {
      Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, j) {
        gw.writeRectangle(*cpBox, layerIdx, 0);
      }
    }
  }

  // write blocks
  const Blk* cpBlk;
  Cir_ForEachBlkC(_cir, cpBlk, i) {
    const Box<Int_t>& box = cpBlk->box();
    gw.writeRectangle(box, cpBlk->layerIdx() + 100, 0);
  }


  // write guides
  const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vGuides = net.vGuides();
  for (const Pair_t<Box<Int_t>, Int_t>& pair : vGuides) {
    const Box<Int_t>& box = pair.first;
    const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(pair.second);
    assert(layerPair.first == LefLayerType::ROUTING);
    gw.writeRectangle(box, layerPair.second + 1001, 0);
  }

  gw.writeCellEnd();
  gw.endLib();

}

void GrGuideWriter::writeGds(const String_t& filename) const {
  if (filename.empty())
    return;

  FILE* fout = fopen(filename.c_str(), "w");

  if (!fout) {
    fprintf(stderr, "GrGuideWriter::%s ERROR: Cannot write file %s\n", __func__, filename.c_str());
    return;
  }

  GdsWriter gw(filename);
  gw.initWriter();
  gw.createLib("TOP", 2000, 1e-6/2000);
  gw.writeCellBgn("INTERCONNECTION");
  
  // write pin locs
  UInt_t i, j, k, pinIdx, layerIdx;
  const Net* cpNet;
  const Box<Int_t>* cpBox;
  Cir_ForEachNet(_cir, cpNet, i) {
    Net_ForEachPinIdx((*cpNet), pinIdx, j) {
      const Pin& pin = _cir.pin(pinIdx);
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBoxC(pin, layerIdx, cpBox, k) {
          gw.writeRectangle(*cpBox, layerIdx + 100 * (i + 1), 0);
        }
      }
    }
    // write guides
    const Vector_t<Pair_t<Box<Int_t>, Int_t>>& vGuides = cpNet->vGuides();
    for (const Pair_t<Box<Int_t>, Int_t>& pair : vGuides) {
      const Box<Int_t>& box = pair.first;
      const Pair_t<LefLayerType, UInt_t>& layerPair = _cir.lef().layerPair(pair.second);
      assert(layerPair.first == LefLayerType::ROUTING);
      gw.writeRectangle(box, pair.second + 100 * (i + 1), 0);
    }
  }

  // write blocks
  const Blk* cpBlk;
  Cir_ForEachBlkC(_cir, cpBlk, i) {
    const Box<Int_t>& box = cpBlk->box();
    gw.writeRectangle(box, cpBlk->layerIdx() + 100000, 0);
  }
  

  gw.writeCellEnd();
  gw.endLib();

}

PROJECT_NAMESPACE_END

