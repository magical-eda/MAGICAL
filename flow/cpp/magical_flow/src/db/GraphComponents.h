/**
 * @file GraphComponents.h
 * @brief Abstract components for the graph
 * @author Keren Zhu
 * @date 06/17/2019
 */

#ifndef MAGICAL_FLOW_GRAPH_COMPONENTS_H_
#define MAGICAL_FLOW_GRAPH_COMPONENTS_H_

#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

/// @class MAGICAL_FLOW::GdsData
/// @brief for July integration. Store the GDSII filename and bounding box
class GdsData
{
    public:
        explicit GdsData() = default;
        /// @brief get the bounding box
        /// @return the reference to the bounding box
        Box<LocType> & bbox() { return _bbox; }
        /// @brief set the bounding box
        /// @param xlo ylo xhi yhi
        void setBBox(LocType xLo, LocType yLo, LocType xHi, LocType yHi) { _bbox = Box<LocType>(xLo, yLo, xHi, yHi); }
        /// @brief get the gds filename
        /// @return gds filename
        std::string gdsFile() { return _gdsFile; }
        /// @breif set gds filename
        /// @param gds filename
        void setGdsFile(const std::string &filename) { _gdsFile = filename; }
    private:
        std::string _gdsFile = ""; ///< Filename for GDSII layout
        Box<LocType> _bbox; ///< The bounding box of the layout
};

/// @class MAGICAL_FLOW::CktNode
/// @brief the abstracted node concepts for representing the physical components circuits
class CktNode 
{
    public:
        /// @brief default constructor
        explicit CktNode() = default; 
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the graph index of the subgraph this node representing
        /// @return the subgraph graph index
        IndexType subgraphIdx() const { return _graphIdx; }
        /// @brief get the array of pin indices this node has (at the current level of graph)
        /// @return the array of pin indices
        std::vector<IndexType> & pinIdxArray() { return _pinIdxArray; }
        /// @brief get the array of pin indices this node has (at the current level of graph)
        /// @return the array of pin indices
        const std::vector<IndexType> & pinIdxArray() const { return _pinIdxArray; }
        /// @brief get the number of pins this CktNode contains
        /// @return the number of pins this CktNode contains
        IndexType numPins() const { return _pinIdxArray.size(); }
        /// @brief get the index n-th pin of this node
        /// @return the index of n-th pin of this node
        IndexType pinIdx(IndexType nth) const { return _pinIdxArray.at(nth); }
        /// @brief get the reference name
        /// @return the reference name of the node
        const std::string & refName() const { return _refName; }
        /// @brief get the name of the node
        /// @return the name of node
        const std::string & name() const { return _name; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the graph index of the subgraph this node representing
        /// @param the subgraph index
        void setSubgraphIdx(IndexType subgraphIdx) { _graphIdx = subgraphIdx; }
        /// @brief set if this node has been physical implemented
        /// @param if this node has been physical implemented
        void setIsImpl(bool isImpl)  {_implPhy = isImpl; }
        /// @brief set the reference name of this node
        /// @param the reference name of the node
        void setRefName(const std::string &refName) { _refName = refName; }
        /// @brief set the name of this node
        /// @param the name of the node
        void setName(const std::string &name) { _name = name; }
        /*------------------------------*/ 
        /* Attributes                   */
        /*------------------------------*/ 
        /// @brief get the coordinate offset of this node
        /// @return the offset of this node
        //const XY<LocType> & offset() const { return _offset; }
        /// @brief get the coordinate offset of this node
        /// @return the offset of this node
        XY<LocType> & offset() { return _offset; }
        /// @brief get the orientation of this node
        /// @return the orientation of this node
        //const OriType & orient() const { return _orient; }
        /// @brief get the orientation of this node
        /// @return the orientation of this node
        OriType & orient() { return _orient; }
        /// @brief set the orientation of this node
        /// @param the orientation of this node
        void setOrient(OriType ori) { _orient = ori; }
        /// @brief get whether this node has been physically implemented
        /// @return if this node has been physically implemented
        bool isImpl() const { return _implPhy; }
        /// @brief get the implementation type
        /// @return the implementation type of this node
        ImplType implType() const { return _implType; }
        /// @brief set the implementation type
        /// @param the implementation type of this node
        void setImplType(ImplType impl) { _implType = impl; }
        /*------------------------------*/ 
        /* Vector operation             */
        /*------------------------------*/ 
        /// @brief append a pinIdx to the pinIdxArray
        /// @param a pinIdx
        void appendPinIdx(IndexType pinIdx) { _pinIdxArray.emplace_back(pinIdx); }
        /*------------------------------*/ 
        /* Graph Properties             */
        /*------------------------------*/ 
        /// @brief if this node is a leaf node
        /// @return whether this node is a leaf node in the graph. If not, it represents a subgraph
        bool isLeaf() const { return _graphIdx == INDEX_TYPE_MAX; }
    private:
        IndexType _graphIdx = INDEX_TYPE_MAX; ///< The index of the sub graph this node corresponding to. If INDEX_TYPE_MAX, then this is a leaf node
        std::vector<IndexType> _pinIdxArray; ///< The pins this node containing
        XY<LocType> _offset = XY<LocType>(0, 0); ///< The offset of the location
        OriType _orient = OriType::N; ///< The orientation of this node
        bool _implPhy = false; ///< Whether this node has been implemented physically
        ImplType _implType = ImplType::UNSET; ///< what is the implementation type of the node 
        std::string _refName = ""; ///< The reference name of this node
        std::string _name = ""; ///< The name of this node
};

/// @class MAGICAL_FLOW::Net
/// @brief the abstracted net concepts for representing the connectivity of the circuits
class Net
{
    public:
        /// @brief default constructor
        explicit Net() = default; 
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the array of pin indices that the net connecting
        /// @return the array of pin indices that the net connecting
        std::vector<IndexType> & pinIdxArray() { return _pinIdxArray; }
        /// @brief get the number of pins this net is connecting
        /// @return the number of pins this net is connecting
        IndexType numPins() const { return _pinIdxArray.size(); }
        /// @brief get the n-th pin index of this net
        /// @return the n-th pin index of this net
        IndexType pinIdx(IndexType nth) const { return _pinIdxArray.at(nth); }
        /// @brief get the name of the net
        /// @return the name of the net
        const std::string & name() const { return _name; }
        /// @brief get the index of io
        /// @return index of the net io
        IndexType ioPos() const { return _ioPos; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the name for the net
        /// @param the name for the net
        void setName(const std::string &name) { _name = name; }
        /// @brief set pos of io
        /// @param the index pos of io
        void setIoPos(IndexType ioPos) { _ioPos = ioPos; }
        /*------------------------------*/ 
        /* Attributes                   */
        /*------------------------------*/ 
        /// @brief check if the net is io
        /// @param return true if net is io
        bool isIo() { return _ioPos != INDEX_TYPE_MAX; }
        /*------------------------------*/ 
        /* Vector operation             */
        /*------------------------------*/ 
        /// @brief append a pinIdx to the pinIdxArray
        /// @param a pinIdx
        void appendPinIdx(IndexType pinIdx) { _pinIdxArray.emplace_back(pinIdx); }
        /*------------------------------*/ 
        /* Integration                  */
        /*------------------------------*/ 
        /// @brief set the pin shape of this net for external accessing
        /// @param first: xlo
        /// @param second: ylo
        /// @param third: xhi
        /// @param fourth: yhi
        void setIoShape(LocType xLo, LocType yLo, LocType xHi, LocType yHi) { _ioShape = Box<LocType>(xLo, yLo, xHi, yHi); }
        /// @brief get the pin shape of this net for external accessing
        /// @return the pin shape as rectangle
        Box<LocType> & ioShape() { return _ioShape; }
        /// @brief get io shape layer. Metal layer
        /// @return IO shape layer, metal layer
        IndexType ioLayer() const { return _ioLayer; }
        /// @brief set IO shape layer. Metal layer
        /// @param metal layer
        void setIoLayer(IndexType ioLayer) { _ioLayer = ioLayer; }

    private:
        std::vector<IndexType> _pinIdxArray; ///< The indices of pins this nets connecting to
        std::string _name = ""; ///< The name of this net
        IndexType _ioPos = INDEX_TYPE_MAX; ///< The index of net if it is IO.
        /*------------------------------*/ 
        /* Integration                  */
        /*------------------------------*/ 
        Box<LocType> _ioShape; ///< The shape for pin for accessing from external
        IndexType _ioLayer = INDEX_TYPE_MAX; ///< Metal layers

};

/// @class MAGICAL_FLOW::Pin
/// @brief the abstracted pin concepts for representing the pin components
class Pin
{
    public:
        /// @brief default constructor
        explicit Pin() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief node index this pin 
        /// @return the node index that this pin belonging to
        IndexType nodeIdx() const { return _nodeIdx; }
        /// @brief internal net index of this pin
        /// @return the internal net index of the node this pin corresponding to
        IndexType intNetIdx() const { return _intNetIdx; }
        /// @brief get one net index that connected to this pin
        /// @param the n-th net of this pin
        /// @return the net index
        IndexType netIdx() const { return _netIdx; }
        /// @brief whether this pin is connected to a net
        /// @return whether this pin is connected to a net
        bool isConnected() const { return _netIdx != INDEX_TYPE_MAX; }
        /// @brief get the number of elements in the _layoutIdx
        /// @return the number of rectangles in Layout
        IndexType numLayoutRects() const { return _layoutRectIdx.size(); }
        /// @brief get one layout rectangle index of this pin
        /// @param the index in the member variable vector
        /// @return one layout rectangle index of this pin
        IndexType layoutRectIdx(IndexType idx) const { return _layoutRectIdx.at(idx); }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the node index
        /// @param the node index this pin belonging to
        void setNodeIdx(IndexType nodeIdx) { _nodeIdx = nodeIdx; }
        /// @brief set the internal pin index
        /// @param set the internal pin index this pin corresponding to in the node it belonging to
        void setIntNetIdx(IndexType intNetIdx) { _intNetIdx = intNetIdx; }
        /// @brief set the net index
        /// @param the index of the net this pin connecting to
        void setNetIdx(IndexType netIdx) { _netIdx = netIdx; }
        /*------------------------------*/ 
        /* Vector operations            */
        /*------------------------------*/ 
        /// @brief add a new rectangle index in _layoutRectIdx
        /// @param An index of rectangle in Layout
        /// @return the index of the new rectangle
        IndexType addLayoutRectIdx(IndexType rectIdx) { _layoutRectIdx.emplace_back(rectIdx); return _layoutRectIdx.size() - 1; }
    private:
        IndexType _nodeIdx = INDEX_TYPE_MAX; ///< The node index of the pin
        IndexType _intNetIdx = INDEX_TYPE_MAX; ///< The corresponding internal pin index in the internal node
        IndexType _netIdx = INDEX_TYPE_MAX; ///< The nets this pin corresponding to
        std::vector<IndexType> _layoutRectIdx; ///< The corresponding indices of rectangles in the Layout
};

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_GRAPH_COMPONENTS_H_
