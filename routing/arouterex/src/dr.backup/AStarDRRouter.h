//
// Created by anoynmous on 2018/08/22
//

#ifndef AROUTER_ASTARROUTER_H_
#define AROUTER_ASTARROUTER_H_

#include "SearchGraphConstructor.h"
#include "AStarDRBase.h"
#include "FixedViaSelector.h"

/// ================================================================================ 
/// AStarDRRouter.h
///
/// Use Astar for detailed routing
///
/// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

/// Some information should be attached to the NodeInfo
class NodeAttachment
{
    public:
        /// fixed via of this node, if existing 
        IndexType fixedViaID = 0; ///< record in to via
        bool insertFixedVia = false;
        bool viaUp = true; ///< true this via is from lower to higher, false from: higher to: lower

        IndexType fromVia = 0; ///< the grid this node is from last via in the search

};

class AStarDRRouter : public AStarDRBase<NodeIdentifier, NodeAttachment>
{
    public: 
        explicit AStarDRRouter(SearchGraphConstructor &searchGraph, Database &db) : AStarDRBase(searchGraph.listGraph()), _searchGraph(searchGraph),  _db(db) {}

        ///////////////////////
        /// Init
        ///////////////////////
        void initFixedViaSelector(FixedViaSelector &viaSelector) { _fixedViaSelector = std::make_shared<FixedViaSelector>(viaSelector); }

        ////////////////////////
        /// Setters
        ////////////////////////
        void setManDistWeight(IntType manDistWeight) { _manDistWeight = manDistWeight; }
        void setLayerDifWeight(IntType layerDifWeight) { _layerDifWeight =layerDifWeight; }

        /////////////////////////
        /// Override functions
        /////////////////////////
        IntType distance(const NodeIdentifier &nodeFrom, const NodeIdentifier &nodeTo) const override;
        IntType heuristicCostEstimate(const NodeIdentifier &nodeIdenty) const override;
        bool isOccupied(const NodeIdentifier &nodeIdenty) const override;
        void updateNodeParent(const NodeIdentifier &from, const NodeIdentifier &to, IntType tentativeGCost) override;

        ///////////////////////////
        /// Init Augment node list
        ///////////////////////////
        
        /// Replace the origin init function, call it once at beginning
        void initAugListGraphXYZ(const std::vector<XY<IndexType>> &sizeXY);
        

        ////////////////////////
        /// Routing opeartion
        ////////////////////////
        /// Just search from source to goal, assume searchGraph is ready : test version
        void routeSingleTwoPinNet(const NodeIdentifier &from, const NodeIdentifier &to);
        /// Route two pin net : general version
        bool routeTwoPinNet(IndexType netIdx, IndexType branchIdx);
        /// Route a multi-pin nets (route all the two-pin nets in net.branches)
        bool routeAndAssignNet(IndexType netIdx);
        /// Assign routing results to net.drNode()
        void assignResultToNode(IndexType netIdx);
        /// Add existing nodes to the source
        void addExistingNodes(IndexType netIdx);

    private:
        //////////////////////////
        /// Misc. supporting
        //////////////////////////
        bool needVia(const NodeIdentifier &from, const NodeIdentifier &to) { return from.layer() != to.layer(); }
        /// Pick a fixed for the two node connection, return -1 if no possible via
        IntType pickFixedVia(const NodeIdentifier &from, const NodeIdentifier &to);
        /// @brief find the search region for the cut layer spacing checking
        /// @param the index of the layer (in cut layers)
        /// @return the enlarge distance for checking
        LocType cutLayerInquirySpacing(IndexType cutLayerIdx);
    private:
        ///////////////////////
        /// Outter data structure
        ///////////////////////
        SearchGraphConstructor &_searchGraph;
        Database &_db;
        std::shared_ptr<FixedViaSelector> _fixedViaSelector = NULL;
        //////////////////
        /// Parameters
        //////////////////
        IntType _manDistWeight = 1;
        IntType _layerDifWeight = 1;
        ///IntType bendWeight = 1; /// Unidirectional, no bend
        /// If add existing segments as sources of searching
        bool _isMultiSource = true; 

        ///////////////////
        /// Misc
        ///////////////////
        IntType _netIdx = -1; /// The index of net currently solving
};


PROJECT_NAMESPACE_END

#endif /// AROUTER_ASTARROUTER_H_
