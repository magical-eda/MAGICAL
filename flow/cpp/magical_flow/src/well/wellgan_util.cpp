#include "wellgan_util.hpp"

PROJECT_NAMESPACE_BEGIN

void DataWellGAN::construct(IndexType cktIdx)
{
  /*
   * In WellGAN, the images are constructed into three channel
   * R: OD inside NW
   * G: OD outside NW
   * B: NW
   *
   * Since here we only use for inference, we don't need B channel.
   * Ouside and inside are basically used to distinguish PMOS vs NMOS
   * Therefore, in the below, we extract two channel of shapes
   * R: 
   */
  _odOtherShapes.clear();
  _odPchShapes.clear();

  auto addOdShapes = [&](auto &vec, const auto &layout, const auto &offset, bool needFlip)
  {
    const auto boundary = layout.boundary();
    LocType midAxis = (boundary.xLo() + boundary.yLo())/2;
    for (IndexType rectIdx = 0; rectIdx < layout.numRects(_odLayerId); ++rectIdx)
    {
      Box<LocType> rect = layout.rectConst(_odLayerId, rectIdx).rect();
      rect.flipX(midAxis);
      if (needFlip)
      {
        rect.offsetBy(offset);
      }
      vec.emplace_back(rect);
    }
    // Also set the layout offset
    _layoutOffset.setX(std::min(_layoutOffset.x(), boundary.xLo()));
    _layoutOffset.setY(std::min(_layoutOffset.y(), boundary.yLo()));
  };

  const CktGraph &ckt = _ddb.subCkt(cktIdx);
  for (const CktNode &node : ckt.nodeArray())
  {
    const CktGraph &nodeCkt = _ddb.subCkt(node.subgraphIdx());
    if (nodeCkt.implType() == ImplType::PCELL_Pch)
    {
      addOdShapes(_odPchShapes, nodeCkt.layoutConst(), node.offsetConst(), node.flipVertFlag());
    }
    else
    {
      addOdShapes(_odOtherShapes, nodeCkt.layoutConst(), node.offsetConst(), node.flipVertFlag());
    }
  }
}

PROJECT_NAMESPACE_END
