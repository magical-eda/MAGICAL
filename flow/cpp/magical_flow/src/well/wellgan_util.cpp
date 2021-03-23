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

  _bbox = Box<LocType>(LOC_TYPE_MAX, LOC_TYPE_MAX, LOC_TYPE_MIN, LOC_TYPE_MIN);

  auto addOdShapes = [&](auto &vec, const auto &layout, const auto &offset, bool needFlip)
  {
    auto boundary = layout.boundary();
    boundary.offsetBy(offset);
    LocType midAxis = (boundary.xLo() + boundary.yLo())/2;
    for (IndexType rectIdx = 0; rectIdx < layout.numRects(_odLayerId); ++rectIdx)
    {
      Box<LocType> rect = layout.rectConst(_odLayerId, rectIdx).rect();
      rect.offsetBy(offset);
      if (needFlip)
      {
        rect.flipX(midAxis);
      }
      vec.emplace_back(rect);
    }
    // Also set the layout offset
    _bbox.unionBox(boundary);
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
