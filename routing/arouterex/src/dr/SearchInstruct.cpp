#include "SearchInstruct.h"
#include "db/Database.h"

PROJECT_NAMESPACE_BEGIN


bool SearchInstruct::init(Database &db)
{
    for (const auto &layer : db.techDB().layerTypeVec())
    {
        if (layer.first == 0)
        {
            // Cut
            this->readCutLayer(db.techDB().cutLayers().at(layer.second), db);
        }
        else if (layer.first == 1)
        {
            // routing
            this->readMetalLayer(db.techDB().routingLayers().at(layer.second), db);
            LocType minWidth = db.techDB().micro2DBU(db.techDB().routingLayers().at(layer.second).width);
            _metalMinWidth.emplace_back(minWidth);
            // Set routing direction
            auto &routingLayer = db.techDB().routingLayers().at(layer.second);
            RouteDirType routeType;
            if (routingLayer.hasDirection)
            {
                if (routingLayer.direction == "HORIZONTAL") { routeType = RouteDirType::HORIZONTAL; }
                else if (routingLayer.direction == "VERTICAL") { routeType = RouteDirType::VERTICAL; }
                else 
                { 
                    ERR("%s: Unknown routing direction %s of layer %s \n", __FUNCTION__, routingLayer.direction.c_str(), routingLayer.name.c_str());
                    Assert(0); 
                    routeType = RouteDirType::FORBID;
                }
            }
            else
            {
                routeType = RouteDirType::BOTH;
            }
            _routingDir.emplace_back(routeType);
        }
        else if (layer.first == 2)
        {
            // Masterslice
            this->readMasterSliceLayer(db.techDB().mastersliceLayers().at(layer.second), db);
            LocType minWidth = db.techDB().micro2DBU(db.techDB().routingLayers().at(0).width);
            _metalMinWidth.emplace_back(minWidth);
            _routingDir.emplace_back(RouteDirType::FORBID);
        }
        else
        {
        }
    }

    // Post processing
    initConvervativeCutSpacings();

    // Grid alignment
    _gridAlign = ::klib::round2Int(db.techDB().units().databaseNumber * db.techDB().manufacturingGird());

    // Routing direction
    //_routingDir = db.drDB().routeDirArray();
    return true;
}

/*------------------------------*/ 
/* Init and processing          */
/*------------------------------*/ 
void SearchInstruct::readCutLayer(const LefLayerCut &cut, Database &db)
{
    LocType spacing = db.techDB().micro2DBU(cut.spacing);
    LocType spacingSameNet = db.techDB().micro2DBU(cut.spacingSameNet);
    LocType spacingParallelOverLap = db.techDB().micro2DBU(cut.spacingParallelOverLap);

    /// Override the requirement is not defined
    if (spacingSameNet <= 0)
    {
        spacingSameNet = spacing;
    }
    if (spacingParallelOverLap <= 0)
    {
        spacingParallelOverLap = spacing;
    }

    /// Add to the class
    _cutSpacings.emplace_back(spacing);
    _cutSameNetSapcings.emplace_back(spacingSameNet);
    _cutParallelOverlapSpacings.emplace_back(spacingParallelOverLap);
}

void SearchInstruct::readMetalLayer(const LefLayerRouting &routing, Database &db)
{
    /// Skip AP layer by hardcoding
    if (routing.spacingTableSpacing.size() == 0)
    {
        if (routing.hasSpacingNumber)
        {
            if (routing.numSpacing > 0)
            {
                Assert(routing.numSpacing == 1);
                LocType spacing = db.techDB().micro2DBU(routing.spacing.at(0));
                _metalMinSpacings.emplace_back(spacing);
                _metalSpacingTables.emplace_back(SpacingTable());
                _metalSpacingTables.back().parallelRunLengthVec().emplace_back(0);
                _metalSpacingTables.back().widthVec().emplace_back(0);
                _metalSpacingTables.back().spacingVec2D().resize(1, 1, spacing);
            }
        }
        else
        {
            ERR("%s: don't know this layer spacing \n", __FUNCTION__);
            Assert(false);
        }
        return;
    }
    if (routing.hasMinArea)
    {
        _metalMinAreas.emplace_back(db.techDB().micro2DBU(routing.minArea));
    }
    else
    {
        _metalMinAreas.emplace_back(db.techDB().micro2DBU(0));
    }
    LocType minSpacing = db.techDB().micro2DBU(routing.spacingTableSpacing.at(0, 0));
    _metalMinSpacings.emplace_back(minSpacing);

    /// Spacing table
    _metalSpacingTables.emplace_back(SpacingTable());
    auto &table = _metalSpacingTables.back();

    if (routing.spacingTableParallelRunLength.size() == 0)
    {
        // Just add min spacing
        table.parallelRunLengthVec().emplace_back(0);
        table.widthVec().emplace_back(0);
        table.spacingVec2D().resize(1, 1);
        table.spacingVec2D().at(0, 0) = minSpacing;
    }
    else
    {
        // Has spacing table rules
        for (RealType paraLen : routing.spacingTableParallelRunLength)
        {
            table.parallelRunLengthVec().emplace_back(db.techDB().micro2DBU(paraLen));
        }

        for (RealType width : routing.spacingTableWidth)
        {
            table.widthVec().emplace_back(db.techDB().micro2DBU(width));
        }

        table.spacingVec2D().resize(routing.spacingTableWidth.size(), routing.spacingTableParallelRunLength.size(), 0);

        for (IndexType x = 0; x < routing.spacingTableParallelRunLength.size(); ++x)
        {
            for (IndexType y = 0; y < routing.spacingTableWidth.size(); ++y)
            {
                RealType spacing = routing.spacingTableSpacing.at(x, y);
                table.spacingVec2D().at(y, x) = db.techDB().micro2DBU(spacing);
            }
        }

    }
    
    /// min area
    /// TODO
    _metalMinAreas.emplace_back(0);
}

void SearchInstruct::readMasterSliceLayer(const LefLayerMasterslice &master, Database &db)
{
    /// TODO:
    /// Here just override with metal 1
    this->readMetalLayer(db.techDB().routingLayers().at(0), db);
}

PROJECT_NAMESPACE_END
