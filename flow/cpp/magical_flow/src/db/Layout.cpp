/**
 * @file Layout.cpp
 * @brief Data structure for maintain the layout in the flow
 * @author Keren Zhu, Mingjie Liu
 * @date 12/19/2019
 */

#include "db/Layout.h"
 
PROJECT_NAMESPACE_BEGIN

void Layout::insertLayout(Layout & layout, LocType x_offset, LocType y_offset, bool flipVertFlag)
{
    for (IndexType layerIdx = 0; layerIdx < layout.numLayers(); layerIdx++)
    {
        for (IndexType recIdx = 0; recIdx < layout.numRects(layerIdx); recIdx++)
        {
            RectLayout rect = layout.rect(layerIdx, recIdx);
            LocType xlo, xhi, ylo, yhi;
            if (flipVertFlag)
            {
                LocType axis = layout.boundary().xLo() + layout.boundary().xHi();
                xlo = axis - rect.rect().xHi() + x_offset;
                xhi = axis - rect.rect().xLo() + x_offset;
            }
            else
            {
                xlo = rect.rect().xLo() + x_offset;
                xhi = rect.rect().xHi() + x_offset;
            }
            ylo = rect.rect().yLo() + y_offset;
            yhi = rect.rect().yHi() + y_offset;
            IndexType newRectIdx = insertRect(layerIdx, xlo, ylo, xhi, yhi);
            setRectDatatype(layerIdx, newRectIdx, rect.datatype());
        }
    }
}

// void RectLayout::shift(LocType x_offset, LocType y_offset)
// {
//     _rect.setXLo(_rect.xLo() + x_offset);
//     _rect.setXHi(_rect.xHi() + x_offset);
//     _rect.setYLo(_rect.yLo() + y_offset);
//     _rect.setYHi(_rect.yHi() + y_offset);
// }
// 
// void Layout::shiftAllLayers(LocType x_offset, LocType y_offset)
// {
//     for (IndexType layerIdx = 0; layerIdx < _numLayers; layerIdx++)
//     {
//         for (IndexType rectIdx = 0; rectIdx < numRects(layerIdx); rectIdx++)
//         {
//             _layers.at(layerIdx).rect(rectIdx).shift(x_offset, y_offset); 
//         }
//     }
// }
// 
PROJECT_NAMESPACE_END
