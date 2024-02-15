/**
 * @file   writer.cpp
 * @brief  Output file writer
 * @author Hao Chen
 * @date   11/21/2019
 *
 **/

#include "writer.hpp"

PROJECT_NAMESPACE_START

// result
void Writer::writeLayoutGds(const String_t& placementFilename, const String_t& outputFilename, const bool bFlatten) {
  fprintf(stdout, "Writing Routing GDS Layout %s\n", outputFilename.c_str());
  _layoutw.writeGds(placementFilename, outputFilename, bFlatten);
}

// guide
void Writer::writeGrGuide(const String_t& filename) const {
  fprintf(stdout, "Writing Global Routing Guides %s\n", filename.c_str());
  _grGuidew.write(filename);
}

void Writer::writeGrGuideGds(const String_t& netName, const String_t& filename) const {
  fprintf(stdout, "Writing Global Routing Guides (GDS) %s\n", filename.c_str());
  _grGuidew.writeGds(netName, filename);
}

void Writer::writeGrGuideGds(const String_t& filename) const {
  fprintf(stdout, "Writing Global Routing Guides (GDS) %s\n", filename.c_str());
  _grGuidew.writeGds(filename);
}

// ....
void Writer::writeDumb(const String_t& placementFilename, const String_t& outputFilename) {
  fprintf(stdout, "Writing Dumb File %s\n", outputFilename.c_str());
  _dumbw.write(placementFilename, outputFilename);
}

void Writer::writeIspd08(const String_t& outputFilename) {
  fprintf(stderr, "Writing Ispd08 File %s\n", outputFilename.c_str());
  FILE* fout = fopen(outputFilename.c_str(), "w");
  fprintf(fout, "num net %d\n", (Int_t)_cir.numNets());
  for (Int_t i = 0 ; i < (Int_t)_cir.numNets(); ++i) {
    const Net& net = _cir.net(i);
    fprintf(fout, "%s %d %d 1 %d\n", net.name().c_str(), i, (Int_t)net.numPins(), 70);
    for (Int_t j = 0; j < (Int_t)net.numPins(); ++j) {
      const Pin& pin = _cir.pin(net.pinIdx(j));
      UInt_t layerIdx, k;
      const Box<Int_t>* cpBox;
      Pin_ForEachLayerIdx(pin, layerIdx) {
        Pin_ForEachLayerBox(pin, layerIdx, cpBox, k) {
          fprintf(fout, "%d %d %d %d %d %d %d %d %d\n",
                  layerIdx / 2,
                  cpBox->xl() / 2, cpBox->yl() / 2,
                  cpBox->xh() / 2, cpBox->yl() / 2,
                  cpBox->xh() / 2, cpBox->yh() / 2,
                  cpBox->xl() / 2, cpBox->yh() / 2);
        }
      }
    }
    fprintf(fout, "666\n");
  }
  fclose(fout);

}

PROJECT_NAMESPACE_END
