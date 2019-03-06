#include "DRBase.h"

PROJECT_NAMESPACE_BEGIN


void DRBase::checkPinOverlap()
{
    std::unordered_map<XYZ<LocType, IndexType>, std::vector<IndexType>> pinDict;

    for (IndexType netIdx = 0; netIdx < drDB().netArray().size(); ++netIdx)
    {
        const auto &net = drDB().netArray().at(netIdx);
        for (IndexType pinIdx = 0; pinIdx < net.pinArray().size(); ++pinIdx)
        {
            const auto &pin = net.pinArray().at(pinIdx);
            XYZ<LocType, IndexType> xyz = XYZ<LocType, IndexType>(pin.loc(), pin.layer());
            auto pinIter = pinDict.find(xyz);
            if (pinIter == pinDict.end())
            {
                pinDict[xyz] = std::vector<IndexType>();
                pinDict[xyz].emplace_back(netIdx);
            }
            else
            {
                pinIter->second.emplace_back(netIdx);
                std::string overlaps = "";
                for (IndexType netOverlap : pinIter->second)
                {
                    overlaps = overlaps + std::to_string(netOverlap) + " ";
                }
                INF("%s: %s Net: %s\n", __PRETTY_FUNCTION__, xyz.toStr().c_str(), overlaps.c_str());
            }
        }
    }
}
PROJECT_NAMESPACE_END
