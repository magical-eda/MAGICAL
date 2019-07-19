#include "RouteGuide.h"
#include "Database.h"

#ifdef DEBUG_INPUT
#ifdef USE_CAIRO
#include "util/CairoDraw.h"
#endif
#endif

PROJECT_NAMESPACE_BEGIN
#ifdef DEBUG_INPUT
#ifdef USE_CAIRO
void RouteGuide::drawRouteGuide(IndexType netIdx, const std::string &filename) const
{
    CairoDraw cd(CairoDraw::ImageType::PNG);
    cd.openImage(filename, _numX, _numY, 0, 0);
    cd.setCanvasBox(0, 0, _numX, _numY);
    for (IndexType x = 0; x < _numX; ++x)
    {
        for (IndexType y = 0; y < _numY; ++y)
        {
            cd.setGrayScale(_guides.at(netIdx).at(x, y));
            cd.drawRectangle(x, y, x+1, y+1, true);
        }
    }
    cd.exportImage();
    DBG("%s PNG saved to %s \n", __FUNCTION__, filename.c_str());
}
#endif
#endif

void RouteGuide::initAverageProb(Database &db)
{
    if (!_enable)
    {
        return;
    }
    IntType totalWeight = 0;
    for (IndexType netIdx = 0; netIdx <  _hasGuide.size(); ++netIdx)
    {
        if (_hasGuide.at(netIdx) && db.grDB().net(netIdx).weight() > 0)
        {
            totalWeight += db.grDB().net(netIdx).weight();
        }
        else
        {
            _guides.at(netIdx).resize(_numX, _numY, 0);
        }
    }
    if (totalWeight == 0)
    {
         return;
    }
    for (IndexType x = 0; x < _numX; ++x)
    {
        for (IndexType y = 0; y < _numY; ++y)
        {
            IntType sum = 0;
            for (IndexType netIdx = 0; netIdx < _guides.size(); ++netIdx)
            {
                sum += _guides.at(netIdx).at(x, y) * db.grDB().nets().at(netIdx).weight();
            }
            _avgProb.at(x, y) = sum / totalWeight;
        }
    }
}
PROJECT_NAMESPACE_END
