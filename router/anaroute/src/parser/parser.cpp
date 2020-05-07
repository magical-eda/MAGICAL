/**
 * @file   parser.cpp
 * @brief  Parser manager for database
 * @author Hao Chen
 * @date   10/27/2019
 *
 **/

#include "parser.hpp"

PROJECT_NAMESPACE_START

void Parser::parseLef(const String_t& filename) {
  fprintf(stdout, "Parsing LEF file %s\n", filename.c_str());
  _lefr.parse(filename);
}

void Parser::parseTechfile(const String_t& filename) {
  fprintf(stdout, "Parsing Tech file %s\n", filename.c_str());
  _techr.parse(filename);
}

void Parser::parseIspd08(const String_t& filename) {
  fprintf(stdout, "Parsing netlist file %s\n", filename.c_str());
  _ispd08r.parse(filename);
}

void Parser::parseGds(const String_t& filename) {
  fprintf(stdout, "Parsing placement GDS layout file %s\n", filename.c_str());
  _gdsr.parse(filename);
}

void Parser::parseSymNet(const String_t& filename) {
  fprintf(stdout, "Parsing symnet file %s\n", filename.c_str());
  _symNetr.parse(filename);
}

void Parser::parseIOPin(const String_t& filename) {
  fprintf(stdout, "Parsing IO pin file %s\n", filename.c_str());
  _ioPinr.parse(filename);
}


// patch for placement bugs .... orz
void Parser::correctPinNBlkLoc() {
  UInt_t i, j, layerIdx;
  // pins
  Pin* pPin;
  Cir_ForEachPin(_cir, pPin, i) {
    Pin_ForEachLayerIdx((*pPin), layerIdx) {
      //Pin_ForEachLayerBox((*pPin), layerIdx, pBox, j) {
      for (j = 0; j < pPin->numBoxes(layerIdx); ++j) {
        Box<Int_t>* pBox = &pPin->_vvBoxes[layerIdx][j];
        assert(abs(pBox->xl()) % 10 == 0 or abs(pBox->xl()) % 10 == 8);
        assert(abs(pBox->yl()) % 10 == 0 or abs(pBox->yl()) % 10 == 8);
        assert(abs(pBox->xh()) % 10 == 0 or abs(pBox->xh()) % 10 == 8);
        assert(abs(pBox->yh()) % 10 == 0 or abs(pBox->yh()) % 10 == 8);
        if (abs(pBox->xl()) % 10 == 8) {
          if (pBox->xl() > 0)
            pBox->setXL(pBox->xl() + 2);
          else
            pBox->setXL(pBox->xl() - 2);
        }
        if (abs(pBox->yl()) % 10 == 8) {
          if (pBox->yl() > 0)
            pBox->setYL(pBox->yl() + 2);
          else
            pBox->setYL(pBox->yl() - 2);
        }
        if (abs(pBox->xh()) % 10 == 8) {
          if (pBox->xh() > 0)
            pBox->setXH(pBox->xh() + 2);
          else
            pBox->setXH(pBox->xh() - 2);
        }
        if (abs(pBox->yh()) % 10 == 8) {
          if (pBox->yh() > 0)
            pBox->setYH(pBox->yh() + 2);
          else
            pBox->setYH(pBox->yh() - 2);
        }
      }
    }
  }

  // blks
  Blk* pBlk;
  Cir_ForEachBlk(_cir, pBlk, i) {
    Box<Int_t>* pBox = &pBlk->_box;
    assert(abs(pBox->xl()) % 10 == 0 or abs(pBox->xl()) % 10 == 8);
    assert(abs(pBox->yl()) % 10 == 0 or abs(pBox->yl()) % 10 == 8);
    assert(abs(pBox->xh()) % 10 == 0 or abs(pBox->xh()) % 10 == 8);
    assert(abs(pBox->yh()) % 10 == 0 or abs(pBox->yh()) % 10 == 8);
    if (abs(pBox->xl()) % 10 == 8) {
      if (pBox->xl() > 0)
        pBox->setXL(pBox->xl() + 2);
      else
        pBox->setXL(pBox->xl() - 2);
    }
    if (abs(pBox->yl()) % 10 == 8) {
      if (pBox->yl() > 0)
        pBox->setYL(pBox->yl() + 2);
      else
        pBox->setYL(pBox->yl() - 2);
    }
    if (abs(pBox->xh()) % 10 == 8) {
      if (pBox->xh() > 0)
        pBox->setXH(pBox->xh() + 2);
      else
        pBox->setXH(pBox->xh() - 2);
    }
    if (abs(pBox->yh()) % 10 == 8) {
      if (pBox->yh() > 0)
        pBox->setYH(pBox->yh() + 2);
      else
        pBox->setYH(pBox->yh() - 2);
    }
  }
}

PROJECT_NAMESPACE_END
