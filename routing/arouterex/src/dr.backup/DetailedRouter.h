//
// Created by Keren on 2018/08/20
//

#ifndef AROUTER_DETAILEDTOUTER_H_BACK
#define AROUTER_DETAILEDTOUTER_H_BACK

#include "DRBase.h"
#include "AStarDRRouter.h"
#include "DetailedRoutingAStarSequential.h"

/// ================================================================================ 
/// DetailedRouter.h
///
/// Detailed router
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

class DetailedRouter : public DRBase
{
    public:
        explicit DetailedRouter(Database &db) : DRBase(db), _graphConstructor(db.drDB()), _astar(_graphConstructor, db), _fixedViaSelector(db) {}

        void testSearchGraph();
        void testRouting();

        ///////////////
        /// Init
        ///////////////
        void addPinAccessFromAllLayers(); /// make the grid cell of the pin can access to all layers
        /// pre-occupied pins to make sure pin access
        void occupyTracksAroundPins(IntType blockSize);

        /////////////////////////
        /// routing
        /////////////////////////
        /// one by one, most straightforward routing flow
        bool directRouteFlow();
        bool solveAStarSequential();

        /////////////////////////
        /// Routing subroutine
        /////////////////////////
        bool routeNet(IndexType netIdx);


        /////////////////////////
        /// Routing resource
        /////////////////////////
        void relaxPinNeighborLayer(IndexType netIdx);
    private:
        SearchGraphConstructor _graphConstructor;
        AStarDRRouter _astar;
        FixedViaSelector _fixedViaSelector;
};

PROJECT_NAMESPACE_END

#endif /// AROUTER_DETAILEDTOUTER_H_
