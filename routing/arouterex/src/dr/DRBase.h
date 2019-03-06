/**
 * @file DRBase.h
 * @brief Base class for detailed routing algorithm
 * @author anoynmous anoynmous
 * @date 10/26/2018
 */

#ifndef AROUTER_DRBASE_H_
#define AROUTER_DRBASE_H_

#include "dr/ShapeMapBase.h"
#include <memory> // std::unique_ptr

PROJECT_NAMESPACE_BEGIN

/// @class AROUTER::DRBase
/// @brief base class for the detailed routing supporting functions
class DRBase
{
    public:
        /*------------------------------*/ 
        /* Constructors                 */
        /*------------------------------*/ 
        /// @brief constructor
        /// @param the database for the router
        explicit DRBase(Database &db) : _db(db) {}
        /*------------------------------*/ 
        /* Initialization of the shapes */
        /*------------------------------*/ 
        /// @brief add pins and terminals to the shapeMap
        void addPinsAndTerminalsToShapeMap();
        /// @brief add blockages to the shapeMap
        void addBlockagesToShapeMap();
        /*------------------------------*/ 
        /* Ripup                        */
        /*------------------------------*/ 
        /// @brief remove a shape in both the db and shapeMap
        /// @param the shapeID want to remove
        /// @return if successful
        bool removeShape(ShapeID shapeID);
        /// @brief rip up a whole net
        /// @param the net index
        /// @return if successful
        bool ripupNet(IndexType netIdx);
        /*------------------------------*/ 
        /* Construct the shapes         */
        /*------------------------------*/ 
        /// @brief add a wire and grow the tree from the parent (new node is a child)
        /// @param netIdx: the index of the net. parentNodeIdx: the index of the parent node. loc: the location and the layer of the new wire. wire: the specification of the wire
        /// @return the index of the new node
        IndexType addWireNodeFromParent(IndexType netIdx, IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire);
        /// @brief add a wire and grow the tree from the parent (new node is a child)
        /// @param netIdx: the index of the net. parentNodeIdx: the index of the parent node. loc: the location and the layer of the new wire. via: the specification of the via
        /// @return the index of the new node
        IndexType addViaNodeFromParent(IndexType netIdx, IndexType parentNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via);
        /// @brief add a wire and grow the tree from the child (new node is a parent)
        /// @param netIdx: the index of the net. childNodeIdx: the index of the child node. loc: the location and the layer of the new wire. wire: the specification of the wire
        /// @return the index of the new node
        IndexType addWireNodeFromChild(IndexType netIdx, IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeWire &wire);
        /// @brief add a wire and grow the tree from the child (new node is a parent)
        /// @param netIdx: the index of the net. childNodeIdx: the index of the child node. loc: the location and the layer of the new wire. via: the specification of the via
        /// @return the index of the new node
        IndexType addViaNodeFromChild(IndexType netIdx, IndexType childNodeIdx, const NetNodeLoc &loc, const NetNodeVIA &via);
        /// @brief connect two nodes and create the shape between them with a wire
        /// @param netIdx: the index of the net. parentNodeIdx: the index of the parent node. childNodeIdx: the index of the child node. wire: the specification of the wire
        /// @return if successful
        void connectWire(IndexType netIdx, IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeWire &wire);
        /// @brief connect two nodes and create the shape between them with a wire
        /// @param netIdx: the index of the net. parentNodeIdx: the index of the parent node. childNodeIdx: the index of the child node. wire: the specification of the wire
        /// @return if successful
        void connectVia(IndexType netIdx, IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeVIA &via);
        /// @brief insert a new node between a wire
        /// @param netIdx: the index of the net. parentNodeIdx: the index of the parent node, childNodeIdx: the index of the child ndoe. loc: the location and the layer of the new node
        /// @return the index of the new node
        IndexType insertNodeInWire(IndexType netIdx, IndexType parentNodeIdx, IndexType childNodeIdx, const NetNodeLoc &loc);
        /// @brief insert fake node from a parent
        IndexType addFakeNodeFromParent(IndexType netIdx, IndexType parentNodeIdx, const NetNodeLoc &loc);
        /*------------------------------*/ 
        /* Adjust node                  */
        /*------------------------------*/ 
        /// @brief set a net node to be physical pin
        /// @param netIdx: the index of the net. nodeIdx: the index of the node
        void setNodePhysicalPin(IndexType netIdx, IndexType nodeIdx) { _db.drDB().net(netIdx).node(nodeIdx).setType(NetNodeType::PHYSICAL); }
        /// @brief set the source node of a net
        /// @param netIdx: the index of the net. loc: the location and the layer of the source node
        void initNetSourceNode(IndexType netIdx, NetNodeLoc loc);
        /*------------------------------*/ 
        /* Debug                        */
        /*------------------------------*/ 
        /// @brief draw the global routing guide for a net as gds
        /// @param filename: the filename for the output gds file. netIdx: the index of the net want to draw
        void drawGdsGlobalRoutingGuide(const std::string &filename, IndexType netIdx) const;
    private:
        /*------------------------------*/ 
        /* Adjust the tree structure    */
        /*------------------------------*/ 
        /// @brief remove empty net node. The node should have no parent, sibanoynmousg or child
        /// @param the indices of the node
        void removeEmptyNetNode(IndexType netIdx, IndexType nodeIdx);
    protected:
        Database &_db; ///< The Database for the whole router
        std::shared_ptr<ShapeMap> _shapeMapPtr = nullptr; ///< The shape map for query the shapes of routing
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_DRBASE_H_
