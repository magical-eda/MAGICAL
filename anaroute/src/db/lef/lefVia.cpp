/**
 * @file   lefVia.cpp
 * @brief  Technology configuration - Lef Via
 * @author Hao Chen, Keren Zhu
 * @date   09/16/2019
 *
 **/

#include "lefVia.hpp"
#include "lefViaImpl.hpp"
#include "src/db/dbLef.hpp"
#include "src/writer/wrGds.hpp"

PROJECT_NAMESPACE_START

void LefVia::setName(const String_t& n) {
  _name = n;
}

void LefVia::setResistance(const Int_t r) {
  _resistance = r;
}

void LefVia::setLayerIdx(const UInt_t i, const UInt_t v) {
  assert(0 <= i and i <= 2);
  _layerIndices[i] = v;
}

void LefVia::setLayerName(const UInt_t i, const String_t& n) {
  assert(0 <= i and i <= 2);
  _layerNames[i] = n;
}

void LefVia::addBox(const UInt_t i, const Box<Int_t>& b) {
  assert(0 <= i and i <= 2);
    _vBoxes[i].emplace_back(b);
  //_cutBBox.coverPoint(b.bl());
  //_cutBBox.coverPoint(b.tr());
}

void LefVia::logInfo() const {
  FILE* fout = stderr;
  fprintf(fout, "VIA %s\n", _name.c_str());
  fprintf(fout, "    RESISTANCE %d\n", _resistance);
  fprintf(fout, "  LAYER %s\n", _layerNames[0].c_str());
  for (UInt_t i = 0; i < _vBoxes[0].size(); ++i) {
    Box<Int_t> box = _vBoxes[0][i];
    fprintf(fout, "    RECT %d %d %d %d\n", box.xl(), box.yl(), box.xh(), box.yh());
  }
  fprintf(fout, "  LAYER %s\n", _layerNames[1].c_str());
  for (UInt_t i = 0; i < _vBoxes[1].size(); ++i) {
    Box<Int_t> box = _vBoxes[1][i];
    fprintf(fout, "    RECT %d %d %d %d\n", box.xl(), box.yl(), box.xh(), box.yh());
  }
  fprintf(fout, "  LAYER %s\n", _layerNames[2].c_str());
  for (UInt_t i = 0; i < _vBoxes[2].size(); ++i) {
    Box<Int_t> box = _vBoxes[2][i];
    fprintf(fout, "    RECT %d %d %d %d\n", box.xl(), box.yl(), box.xh(), box.yh());
  }
  fprintf(fout, "END %s\n\n", _name.c_str());
}

template<typename ViaRuleType>
void LefViaPrototype::config(const LefVia &via, const ViaRuleType &viaRule)
{
  _lefVia = LefVia(via);
  _implementor = std::make_shared<LefViaImplementor>(&_lefVia);
  LefViaImplementorConceptTraits<ViaRuleType>::configure(viaRule, *_implementor);
}

template<typename ViaRuleType>
void generateBasedOnViaRule(LefVia &via, ViaRuleType &rule, UInt_t row, UInt_t col)
{
    throw ViaGenerationNotImplementedException();
}

template<>
void generateBasedOnViaRule(LefVia &via, LefViaRuleTemplate1 &rule, UInt_t row, UInt_t col)
{
    // Conservative approach
    Int_t lowerEnclosure = std::max(rule.enclosureOverhang1[0], rule.enclosureOverhang2[0]);
    Int_t higherEnclosure = std::max(rule.enclosureOverhang1[1], rule.enclosureOverhang2[1]);
    // Compute the x and y coordinates indenpendently
    Int_t cutXLoOffset; // The center point of the leftmost cut
    Int_t cutXLo, cutXHi; // The xLo/xHi of the bounding box of cuts
    if (col %2 == 0)
    {
        cutXLoOffset = - ((col / 2) * rule.spacingStepX - 0.5 * rule.spacingStepX);
    }
    else
    {
        cutXLoOffset = - (col / 2) * rule.spacingStepX;
    }
    cutXLo = cutXLoOffset + rule.rectXLo;
    cutXHi = - cutXLoOffset + rule.rectXHi;
    Int_t lowerXLo = cutXLo - lowerEnclosure;
    Int_t higherXLo = cutXLo - higherEnclosure;
    Int_t lowerXHi = cutXHi + lowerEnclosure;
    Int_t higherXHi = cutXHi + higherEnclosure;
    // y
    Int_t cutYLoOffset; // The center point of the lowerest cut
    Int_t cutYLo, cutYHi; // The yLo/yHi of the bounding box of cuts
    if (row %2 == 0)
    {
        cutYLoOffset = - ((row / 2) * rule.spacingStepY - 0.5 * rule.spacingStepY);
    }
    else
    {
        cutYLoOffset = - (row / 2) * rule.spacingStepY;
    }
    cutYLo = cutYLoOffset + rule.rectYLo;
    cutYHi = -cutYLoOffset + rule.rectYHi;
    Int_t lowerYLo = cutYLo - lowerEnclosure;
    Int_t higherYLo = cutYLo - higherEnclosure;
    Int_t lowerYHi = cutYHi + lowerEnclosure;
    Int_t higherYHi = cutYHi + higherEnclosure;


    // Make sure the width rule is met
    if (lowerXHi - lowerXLo < rule.widthLo[0])
    {
        auto dif = rule.widthLo[0] - (lowerXHi - lowerXLo);
        lowerXHi += dif / 2;
        lowerXLo -= dif / 2;
    }
    if (lowerYHi - lowerYLo < rule.widthLo[0])
    {
        auto dif = rule.widthLo[0] - (lowerYHi - lowerYLo);
        lowerYHi += dif / 2;
        lowerYLo -= dif / 2;
    }
    if (higherXHi - higherXLo < rule.widthLo[1])
    {
        auto dif = rule.widthLo[1] - (higherXHi - higherXLo);
        higherXHi += dif / 2;
        higherXLo -= dif / 2;
    }
    if (higherYHi - higherYLo < rule.widthLo[1])
    {
        auto dif = rule.widthLo[1] - (higherYHi - higherYLo);
        higherYHi += dif / 2;
        higherYLo -= dif / 2;
    }

    if (lowerXHi - lowerXLo > rule.widthHi[0])
    {
        throw ViaGenerationInfeasibleMaxWidthException();
    }
    if (lowerYHi - lowerYLo > rule.widthHi[0])
    {
        throw ViaGenerationInfeasibleMaxWidthException();
    }
    if (higherXHi - higherXLo > rule.widthHi[1])
    {
        throw ViaGenerationInfeasibleMaxWidthException();
    }
    if (higherYHi - higherYLo > rule.widthHi[1])
    {
        throw ViaGenerationInfeasibleMaxWidthException();
    }
    // Draw the shapes
    via.addBox(0, Box<Int_t>(lowerXLo, lowerYLo, lowerXHi, lowerYHi));
    via.addBox(2, Box<Int_t>(higherXLo, higherYLo, higherXHi, higherYHi));
    for (UInt_t rowIdx = 0; rowIdx < row; ++rowIdx)
    {
        for (UInt_t colIdx = 0; colIdx < col; ++colIdx)
        {
            via.addBox(1,
                    Box<Int_t>
                    (
                     cutXLoOffset + rule.rectXLo + colIdx * rule.spacingStepX,
                     cutYLoOffset + rule.rectYLo + rowIdx * rule.spacingStepY,
                     cutXLoOffset + rule.rectXHi + colIdx * rule.spacingStepX,
                     cutYLoOffset + rule.rectYHi + rowIdx * rule.spacingStepY
                    ));
        }
    }
    via.setValid(true);
    via.computeCutBBox();
}

void drawAllVias(const Vector_t<Vector2D<LefVia>> &table)
{
    GdsWriter gw("vias.gds");
    gw.initWriter();
    gw.createLib("TOP", 2000, 1e-6/2000);
    gw.writeCellBgn("INTERCONNECTION");

    UInt_t idx = 0;
    for (const auto & tt : table)
    {
        for (const auto &via : tt)
        {
            if (!via.bValid())
            {
                idx++;
                continue;
            }
            UInt_t bottomLayerIdx = via.botLayerIdx() + 100 * idx;
            UInt_t cutLayerIdx = via.cutLayerIdx() + 100 * idx;
            UInt_t topLayerIdx = via.topLayerIdx() + 100 *idx;
            for (UInt_t boxIdx = 0; boxIdx < via.numBotBoxes(); ++boxIdx)
            {
                const auto &box = via.botBox(boxIdx);
                gw.writeRectangle(box, bottomLayerIdx, 0);
            }
            for (UInt_t boxIdx = 0; boxIdx < via.numCutBoxes(); ++boxIdx)
            {
                const auto &box = via.cutBox(boxIdx);
                gw.writeRectangle(box, cutLayerIdx, 0);
            }
            for (UInt_t boxIdx = 0; boxIdx < via.numTopBoxes(); ++boxIdx)
            {
                const auto &box = via.topBox(boxIdx);
                gw.writeRectangle(box, topLayerIdx, 0);
            }
            idx++;
        }
    }

    gw.writeCellEnd();
    gw.endLib();
}

void LefViaTable::generateVias(LefDB &lef)
{
    // Generate based on different viarules
    // template 1
    for (auto &temp1 : lef.vViaRuleTemplate1())
    {
        auto layer0Pair = lef.str2Layer(temp1.metalLayerNames[0]);
        auto layer1Pair = lef.str2Layer(temp1.metalLayerNames[1]);
        if (layer0Pair.second > layer1Pair.second)
        {
            std::swap(layer0Pair, layer1Pair);
            temp1.swapMetalLayers();
        }
        assert(layer0Pair.first == LefLayerType::MASTERSLICE or layer0Pair.first == LefLayerType::ROUTING);
        assert(layer1Pair.first == LefLayerType::MASTERSLICE or layer1Pair.first == LefLayerType::ROUTING);
        assert(lef.str2Layer(temp1.viaLayerName).first == LefLayerType::CUT);
        if (layer0Pair.first == LefLayerType::MASTERSLICE)
        {
            continue; // Don't need to generate PO-M1
        }
        if (layer1Pair.first == LefLayerType::MASTERSLICE)
        {
            continue; // Don't need to generate PO-M1
        }
        auto lowerMetalLayerIdx = lef.str2RoutinglayerIdx(temp1.metalLayerNames[0]);
        if (lowerMetalLayerIdx >= _table.size())
        {
            continue; // Exceed what we need to consider
        }
        auto bottomLayerIdx = lef.str2LayerIdx(temp1.metalLayerNames[0]);
        const auto &bottomLayerName = temp1.metalLayerNames[0];
        auto topLayerIdx = lef.str2LayerIdx(temp1.metalLayerNames[1]);
        const auto &topLayerName = temp1.metalLayerNames[1];
        const auto &cutLayerName = temp1.viaLayerName;
        auto cutLayerIdx = lef.str2LayerIdx(cutLayerName);
        for (UInt_t row = MIN_GENERATE_ROW; row <= MAX_GENERATE_ROW; ++row)
        {
            for (UInt_t col = MIN_GENERATE_COLUMN; col <= MAX_GENERATE_COLUMN; ++col)
            {
                auto via  = LefVia();
                via.setDefault(false);
                via.setLayerIdx(0, bottomLayerIdx);
                via.setLayerName(0, bottomLayerName);
                via.setLayerIdx(1, cutLayerIdx);
                via.setLayerName(1, cutLayerName);
                via.setLayerIdx(2, topLayerIdx);
                via.setLayerName(2, topLayerName);
                std::string viaName = temp1.name +"_row_"+std::to_string(row) +"_col_"+std::to_string(col);
                via.setName(viaName);
                try 
                {
                    generateBasedOnViaRule(via, temp1, row, col);
                    this->viaProtoType(lowerMetalLayerIdx, row, col).config(via, temp1);
                }
                catch(ViaGenerationInfeasibleException e)
                {
                    ERR("Generate VIA: %s\n", e.what());
                    WRN("Generate VIA: skip the via M%d rol %d col %d \n", lowerMetalLayerIdx, row, col);
                }
                catch(std::exception e)
                {
                    throw e;
                }
                catch(...)
                {
                    assert(0);
                }

            }
        }
    }
}

PROJECT_NAMESPACE_END

