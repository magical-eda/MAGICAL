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
        /// @brief get if the node should be flipped
        /// @return the flip vert flag
        bool flipVertFlag() const { return _flipVertFlag; }

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
        /// @brief set the coordinate offset of this node
        /// @set the offset of this node
        void setOffset(LocType x, LocType y) { _offset = XY<LocType>(x, y); }
        /// @brief set the flip flag
        /// @set the flip flag
        void setFlipVertFlag(bool flag) { _flipVertFlag = flag; }

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
        bool _flipVertFlag = false; ///< Whether this node should be fliped due to symmetry constraint
        ImplType _implType = ImplType::UNSET; ///< what is the implementation type of the node 
        std::string _refName = ""; ///< The reference name of this node
        std::string _name = ""; ///< The name of this node
};

/// @brief the IO pin shape configuration
struct IoPinConfigure
{
    explicit IoPinConfigure() = default;
    Box<LocType> shape;  ///< The shape for pin for accessing from external
    IndexType layer = INDEX_TYPE_MAX; ///< Metal layer
    IntType isPowerStripe = 0;

};

/// @class MAGICAL_FLOW::Net
/// @brief the abstracted net concepts for representing the connectivity of the circuits
class Net
{
    public:
        /// @brief default constructor
        explicit Net() 
        {
            _ioInterfaces.resize(1);
            Assert(_ioInterfaces.at(0).layer == INDEX_TYPE_MAX);
            Assert(_ioInterfaces.at(0).isPowerStripe == 0);
        }
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the array of pin indices that the net connecting
        /// @return the array of pin indices that the net connecting
        std::vector<IndexType> & pinIdxArray() { return _pinIdxArray; }
        /// @brief get the number of pins this net is connecting
        /// @return the number of pins this net is connecting
        IndexType numPins() const { return _pinIdxArray.size(); }
        /// @brief get the number of substrate pins this CktNode contains
        /// @return the number of substrate pins this CktNode contains
        IndexType numSubs() const { return _subIdxArray.size(); }
        /// @brief get the n-th pin index of this net
        /// @return the n-th pin index of this net
        IndexType pinIdx(IndexType nth) const { return _pinIdxArray.at(nth); }
        /// @brief get the name of the net
        /// @return the name of the net
        const std::string & name() const { return _name; }
        /// @brief get the index of io
        /// @return index of the net io
        IndexType ioPos() const { return _ioPos; }
        /// @brief get whether this net is power nets
        bool isPower() const { return _isVss or _isVdd; }
        /// @brief get whether this net is vdd net
        bool isVdd() const { return _isVdd; }
        /// @brief get whether this net is vss net
        bool isVss() const { return _isVss; }
        /// @brief get whether this net is digital net
        bool isDigital() const { return _isDigital; }
        /// @brief get whether this net is analog net
        bool isAnalog() const { return _isAnalog; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the name for the net
        /// @param the name for the net
        void setName(const std::string &name) { _name = name; }
        /// @brief set pos of io
        /// @param the index pos of io
        void setIoPos(IndexType ioPos) { _ioPos = ioPos; }
        /// @brief mark this net as VDD
        void markVddFlag() { Assert(!_isVss); _isVdd = true; }
        /// @brief remove the VDD flag from this net
        void revokeVddFlag() { _isVdd = false; }
        /// @brief mark this net as VSS
        void markVssFlag() { Assert(!_isVdd); _isVss = true; }
        /// @brief remove the VSS flag from this net
        void revokeVssFlag() { _isVss = false; }
        /// @brief mark this net as digital
        void markDigitalFlag() { _isDigital = true; _isAnalog = false; }
        /// @brief mark this net as analog
        void markAnalogFlag() { _isDigital = false; _isAnalog = true; }
        /*------------------------------*/ 
        /* Attributes                   */
        /*------------------------------*/ 
        /// @brief check if the net is io
        /// @param return true if net is io
        bool isIo() { return _ioPos != INDEX_TYPE_MAX; }
        /// @brief return true if net is a substrate net
        /// @return true if a substrate net
        bool isSub() { return _subIdxArray.empty(); }
        /*------------------------------*/ 
        /* Vector operation             */
        /*------------------------------*/ 
        /// @brief append a pinIdx to the pinIdxArray
        /// @param a pinIdx
        void appendPinIdx(IndexType pinIdx) { _pinIdxArray.emplace_back(pinIdx); }
        /// @brief append a pinIdx to the subIdxArray
        /// @param a pinIdx
        void appendSubIdx(IndexType pinIdx) { _subIdxArray.emplace_back(pinIdx); }
        /*------------------------------*/ 
        /* Integration                  */
        /*------------------------------*/ 
        /// @brief set the pin shape of this net for external accessing
        /// @param first: xlo
        /// @param second: ylo
        /// @param third: xhi
        /// @param fourth: yhi
        void setIoShape(LocType xLo, LocType yLo, LocType xHi, LocType yHi) { _ioInterfaces.at(0).shape = Box<LocType>(xLo, yLo, xHi, yHi); }
        /// @brief get the pin shape of this net for external accessing
        /// @return the pin shape as rectangle
        Box<LocType> & ioShape() { return _ioInterfaces.at(0).shape; }
        /// @brief get io shape layer. Metal layer
        /// @return IO shape layer, metal layer
        IndexType ioLayer() const { return _ioInterfaces.at(0).layer; }
        /// @brief set IO shape layer. Metal layer
        /// @param metal layer
        void setIoLayer(IndexType ioLayer) { _ioInterfaces.at(0).layer = ioLayer; }
        /// @brief get the number of io pins
        /// @return the number of io pins
        IndexType numIoPins() const { return _ioInterfaces.size(); }
        /// @brief add a io pin
        /// @param first: io pin shape xLo 
        /// @param second: io pin shape yLo 
        /// @param third: io pin shape xHi 
        /// @param fourth: io pin shape yHi 
        /// @param fifth: metal layer 
        void addIoPin(LocType xLo, LocType yLo, LocType xHi, LocType yHi, IndexType metalLayer)
        {
            if (_ioInterfaces.at(0).layer == INDEX_TYPE_MAX)
            {
                _ioInterfaces[0].shape = Box<LocType>(xLo, yLo, xHi, yHi);
                _ioInterfaces[0].layer = metalLayer;
            }
            else
            {
                _ioInterfaces.emplace_back(IoPinConfigure());
                _ioInterfaces.back().shape = Box<LocType>(xLo, yLo, xHi, yHi);
                _ioInterfaces.back().layer = metalLayer;
            }
        }
        /// @brief get whether a io shape is power stripe
        /// @param the index of the io interface
        bool isIoPowerStripe(IndexType ioIdx) const { return _ioInterfaces.at(ioIdx).isPowerStripe == 1; }
        /// @brief mark a io shape as power stripe
        /// @param the index of the io interface
        void markIoPowerStripe(IndexType ioIdx) { _ioInterfaces.at(ioIdx).isPowerStripe = 1; }
        /// @brief mark the last io shape as power stripe
        void markLastIoPowerStripe() { _ioInterfaces.back().isPowerStripe = 1; }
        /// @brief get the io pin shape
        /// @param the index 
        /// @return the shape
        Box<LocType> & ioPinShape(IndexType idx) { return _ioInterfaces.at(idx).shape; }
        /// @brief get the io pin metal layer
        /// @param the index
        /// @return the metal layer
        IndexType ioPinMetalLayer(IndexType idx) { return _ioInterfaces.at(idx).layer; }
        /// @brief flip io shape according to vertical axis
        /// @param symmetry vertical axis x=axis
        void flipVert(LocType axis) 
        { 
            for (auto &io : _ioInterfaces)
            {
                auto & ioshape = io.shape;
                LocType xLo = ioshape.xLo();    
                ioshape.setXLo(2 * axis - ioshape.xHi());
                ioshape.setXHi(2 * axis - xLo);
            }
        }

    private:
        std::vector<IndexType> _pinIdxArray; ///< The indices of pins this nets connecting to (includes sub pins)
        std::vector<IndexType> _subIdxArray; ///< The indices of device substrate pins this nets connecting to
        std::string _name = ""; ///< The name of this net
        IndexType _ioPos = INDEX_TYPE_MAX; ///< The index of net if it is IO.
        bool _isVdd = false; ///< Whether this net is a VDD net
        bool _isVss = false; ///< Whether this net is a VSS net
        bool _isDigital = false; ///< Whether this net is digital
        bool _isAnalog =  false; ///< whether this net is analog
        /*------------------------------*/ 
        /* For higher hierarchy         */
        /*------------------------------*/ 
        std::vector<IoPinConfigure> _ioInterfaces; ///< The shape for pin for accessing from external
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
        /// @return the internal net index of the node this pin corresponding to in the sub graph
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
        /// @brief get if the pin type
        /// @return return the pin type
        PinType pinType() const { return _pinType; }
        /// @brief get if pin is valid
        /// @return return true if pin is valid
        bool valid() const { return _valid; }
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
        /// @brief set the pin type
        /// @param a pinType
        void setPinType(PinType pinType) { _pinType = pinType; }
        /// @brief set if pin is valid
        /// @param boolean
        void setValid(bool valid) { _valid = valid ; }
        /*------------------------------*/ 
        /* Vector operations            */
        /*------------------------------*/ 
        /// @brief add a new rectangle index in _layoutRectIdx
        /// @param An index of rectangle in Layout
        /// @return the index of the new rectangle
        IndexType addLayoutRectIdx(IndexType rectIdx) { _layoutRectIdx.emplace_back(rectIdx); return _layoutRectIdx.size() - 1; }
    private:
        PinType _pinType = PinType::UNSET; ///< The pin is a substrate pin psub/nwell
        IndexType _nodeIdx = INDEX_TYPE_MAX; ///< The node index of the pin
        IndexType _intNetIdx = INDEX_TYPE_MAX; ///< The corresponding internal pin index in the internal node
        IndexType _netIdx = INDEX_TYPE_MAX; ///< The nets this pin corresponding to
        std::vector<IndexType> _layoutRectIdx; ///< The corresponding indices of rectangles in the Layout
        bool _valid = true; ///< If the pin is valid and should be routed to net.
};

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_GRAPH_COMPONENTS_H_
