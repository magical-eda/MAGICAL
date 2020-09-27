/**
 * @file Layout.h
 * @brief Data structure for maintain the layout in the flow
 * @author Keren Zhu, Mingjie Liu
 * @date 07/03/2019
 */

#ifndef MAGICAL_FLOW_LAYOUT_H_
#define MAGICAL_FLOW_LAYOUT_H_

#include <utility> // std::forward
#include <limits> // std::numeric_limits
#include "global/global.h"

PROJECT_NAMESPACE_BEGIN

class LayoutObject
{
#if 0
    public:
        explicit LayoutObject() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the index of the layer
        /// @return the index of the layer
        IndexType layer() const { return _layer; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the layer of the object
        /// @param the index of the layer
        void setLayer(IndexType layer) { _layer = layer; }
    protected:
        IndexType _layer = INDEX_TYPE_MAX; ///< The layer of the layout object
#endif
};

/// @class MAGICAL_FLOW::TextLayout
/// @brief the text object in the layout
class TextLayout : public LayoutObject
{
    public:
        /// @brief default constructor
        explicit TextLayout() = default;
        /// @brief constructor
        /// @param first: string for text
        /// @param second: coordinate for the text
        explicit TextLayout(const std::string &text, const XY<LocType> &coord) : _text(text), _coord(coord) {}
        /// @brief constructor
        /// @param first: string for text
        /// @param second: x coordinate
        /// @param third: y coordinate
        explicit TextLayout(const std::string &text, LocType x, LocType y) : _text(text), _coord(XY<LocType>(x, y)) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the text of this layout object
        /// @return the text of this layout object
        std::string & text() { return _text; }
        /// @brief get the coordinate of the text object
        /// @return the reference to the text object
        XY<LocType> & coord() { return _coord; }
        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        /// @brief set the text of this object
        /// @param the text of this object
        void setText(const std::string &text) { _text = text; } 
    private:
        std::string _text = ""; ///< The text string of this object
        XY<LocType> _coord; ///< The coordinate of this text object
};

/// @class MAGICAL_FLOW::RectLayout
/// @brief the rectangle object in the layout
class RectLayout : public LayoutObject
{
    public:
        /// @brief default constructor
        explicit RectLayout() = default;
        /// @brief construtor
        /// @param a box object representing the rectangle geometry
        explicit RectLayout(const Box<LocType> &rect) : _rect(rect), _datatype(0) {}
        /// @brief constructor
        /// @param the lower left coordinate
        /// @param the upper right coordinate
        explicit RectLayout(const XY<LocType> &lo, const XY<LocType> &ur) : _rect(Box<LocType>(lo, ur)), _datatype(0) {}
        /// @brief constructor
        /// @param the x coordinate of the lower left point
        /// @param the y coordinate of the lower left point
        /// @param the x coordinate of the upper right point
        /// @param the y coordinate of the upper right pointLayout
        explicit RectLayout(LocType xLo, LocType yLo, LocType xHi, LocType yHi) : _rect(Box<LocType>(xLo, yLo, xHi, yHi)), _datatype(0) {}
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the rectangle of the object
        /// @return the rectangle shape of the object
        Box<LocType> & rect() { return _rect; }
        const Box<LocType> & rect() const { return _rect; }
        IndexType datatype() { return _datatype; }
        /// @brief set the datatype of the shape, default is 0
        void setDatatype(IndexType datatype) { _datatype = datatype; }
    private:
        Box<LocType> _rect; ///< The shape of this rectangle
        IndexType _datatype;
};

/// @class MAGICAL_FLOW::LayoutLayer
/// @brief Data structure for one layer of the layout
class LayoutLayer
{
    public:
        explicit LayoutLayer() = default;
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the text vector
        /// @return the text vector
        const std::vector<TextLayout> & textList() const { return _texts; }
        /// @brief get the text vector
        /// @return the text vector
        std::vector<TextLayout> & textList() { return _texts; }
        /// @brief get the rectangle vector
        /// @return the rectangle vector
        const std::vector<RectLayout> & rectList() const { return _rects; }
        /// @brief get the rectangle vector
        /// @return the rectangle vector
        std::vector<RectLayout> & rectList() { return _rects; }
        /// @brief get one text object
        /// @param the index of the text object
        TextLayout & text(IndexType textIdx) { return _texts.at(textIdx); }
        /// @brief get one rectangle object
        /// @param the index of the rectangle object
        RectLayout & rect(IndexType rectIdx) { return _rects.at(rectIdx); }
        /*------------------------------*/ 
        /* Add items                    */
        /*------------------------------*/ 
        /// @brief insert text object
        /// @param the TextLayout want to insert
        /// @return the index of the object inserted
        IndexType insertText(const TextLayout &text) { _texts.emplace_back(text); return _texts.size() - 1; }
        /// @brief insert text object
        /// @param paramters forward to TextLayout constructors
        /// @return the index of the object inserted
        template<typename... T>
        IndexType insertText(T&&... params) { _texts.emplace_back(TextLayout(std::forward<T>(params)...)); return _texts.size() - 1; }
        /// @brief insert rectangle object
        /// @param the RectLayout want to insert
        /// @return the index of the object inserted
        IndexType insertRect(const RectLayout &rect) { _rects.emplace_back(rect); return _rects.size() - 1; }
        /// @brief insert rectangle object
        /// @param paramters forward to Rectangle constructors
        /// @return the index of the object inserted
        template<typename... T>
        IndexType insertRect(T&&... params) { _rects.emplace_back(RectLayout(std::forward<T>(params)...)); return _rects.size() - 1; }
#if 0
        /// @brief insert text object
        /// @param first: string for text
        /// @param second: coordinate for the text
        void insertText(const std::string &text, const XY<LocType> &coord) { _texts.emplace_back(TextLayout(text, coord)); }
        /// @brief insert text object
        /// @param first: string for text
        /// @param second: x coordinate
        /// @param third: y coordinate
        void insertText(const std::string &text, LocType x, LocType y) { _texts.emplace_back(TextLayout(text, x, y)); }
        /// @brief insert rectangle object
        /// @param the RectLayout object want to insert
        void insertRect(const RectLayout &rect) { _rects.emplace_back(rect); }
        /// @brief insert rectangle object
        /// @param a box object representing the rectangle geometry
        void insertRect(const Box<LocType> &rect) { _rects.emplace_back(RectLayout(rect)); }
        /// @brief insert rectangle object
        /// @param the lower left coordinate
        /// @param the upper right coordinate
        void insertRect(const XY<LocType> &lo, const XY<LocType> &ur) { _rects.emplace_back(RectLayout(lo, ur)); }
        /// @brief insert rectangle object
#endif
    private:
        std::vector<TextLayout> _texts; ///< vector of text objects
        std::vector<RectLayout> _rects; ///< vector of rectangle objects
};

/// @class MAGICAL_FLOW::Layout
/// @brief The data structure to maintain the layout for MAGICAL
class Layout
{
    public:
        /// @brief default constructor
        explicit Layout() { this->init(RESERVED_LAYERS_NUMBER); }
        /// @brief clear the layout
        void clear() {  
            AssertMsg(_numLayers >= 0, "%s: ensure the number of layers are set \n", __FUNCTION__); _layers.clear(); _layers.resize(_numLayers); 
            _boundary.setXLo(std::numeric_limits<LocType>::max());
            _boundary.setYLo(std::numeric_limits<LocType>::max());
            _boundary.setXHi(std::numeric_limits<LocType>::min());
            _boundary.setYHi(std::numeric_limits<LocType>::min());
            }
        /// @brief initialize the object with number of layers
        /// @param the number of layers
        void init(IndexType numLayers) { 
            _numLayers = numLayers; 
            this->clear(); 
        }
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get one text layout object
        /// @param first: the index of layer
        /// @param second: the index of the text in that layer
        /// @return the requested text layout object
        TextLayout & text(IndexType layerIdx, IndexType textIdx) { return _layers.at(layerIdx).text(textIdx); }
        /// @brief get one rect layout object
        /// @param first: the index of layer
        /// @param second: the index of the text in that layer
        /// @return the requested rect layout object
        RectLayout & rect(IndexType layerIdx, IndexType rectIdx) { return _layers.at(layerIdx).rect(rectIdx); }
        /// @brief get the number of layers
        /// @return the number of layers
        IndexType numLayers() const { return _numLayers; }
        /// @brief get the number of texts in one layer
        /// @param the index of one layer
        /// @return the number of texts in the layer
        IndexType numTexts(IndexType layerIdx) const { return _layers.at(layerIdx).textList().size(); }
        /// @brief get the number of rectangles in one layer
        /// @param the index of one layer
        /// @return the number of rectangles in the layer
        IndexType numRects(IndexType layerIdx) const { return _layers.at(layerIdx).rectList().size(); }
        /// @brief get the boundary box of layout
        /// @return boundary box
        Box<LocType> boundary() const { return _boundary; }
        /*------------------------------*/ 
        /* Add items                    */
        /*------------------------------*/ 
        /// @brief insert a text 
        /// @param first: layer index
        /// @param second: a text object
        /// @return the index of the object inserted
        IndexType insertText(IndexType layerIdx, const TextLayout &text) { return _layers.at(layerIdx).insertText(text); }
        /// @brief insert a text
        /// @param first: layer index
        /// @param second: the text in the layout
        /// @param third: the coordinate of the layout object
        /// @return the index of the object inserted
        IndexType insertText(IndexType layerIdx, const std::string &text, const XY<LocType> &coord) { return  _layers.at(layerIdx).insertText(text, coord); }
        /// @brief insert a text
        /// @param first: layer index
        /// @param second: the text in the layout
        /// @param third: x coordinate
        /// @param fourth: y coordinate
        /// @return the index of the object inserted
        IndexType insertText(IndexType layerIdx, const std::string &text, LocType x, LocType y) { return  _layers.at(layerIdx).insertText(text, x, y); }
        /// @brief insert a rectangle
        /// @param first: layer index
        /// @param second: a rectangle object
        /// @return the index of the object inserted
        IndexType insertRect(IndexType layerIdx, const RectLayout &rect) { 
            _boundary.unionBox(rect.rect());
            return _layers.at(layerIdx).insertRect(rect); 
            }
        /// @brief insert a rectangle
        /// @param first: layer index
        /// @param second: a box object to represent the geometry of the rectanle
        /// @return the index of the object inserted
        IndexType insertRect(IndexType layerIdx, const Box<LocType> &box) { 
            _boundary.unionBox(box);
            return _layers.at(layerIdx).insertRect(box); 
            }
        /// @brief insert a rectangle
        /// @param first: layer index
        /// @param second: the lower left coordinate of the rectangle
        /// @param third: the upper right coordinate of the rectangle
        /// @return the index of the object inserted
        IndexType insertRect(IndexType layerIdx, const XY<LocType> &lo, const XY<LocType> &ur) { 
            _boundary.unionBox(Box<LocType>(lo, ur));
            return _layers.at(layerIdx).insertRect(lo, ur); 
            } 
        /// @brief insert a rectangle
        /// @param first: layer index
        /// @param second: the x coordinate of the lower-left point
        /// @param third: the y coordinate of the lower-left point
        /// @param second: the x coordinate of the upper-right point
        /// @param third: the y coordinate of the upper-right point
        /// @return the index of the object inserted
        IndexType insertRect(IndexType layerIdx, LocType xLo, LocType yLo, LocType xHi, LocType yHi) { 
            _boundary.unionBox(Box<LocType>(xLo, yLo, xHi, yHi));
            return _layers.at(layerIdx).insertRect(xLo, yLo, xHi, yHi); 
            } 
        /// @brief insert a Layout
        /// @param first: layout to be inserted
        /// @param second: x_offset
        /// @param third: y_offset
        /// @param fourth: boolean if to flip vertically
        void insertLayout(Layout & layout, LocType x_offset, LocType y_offset, bool flipVertFlag);
        /// @brief set the datatype of a rectangle
        /// @param first: layer index
        /// @param second: the rect index in the layer
        /// @param third: the datatype of the object
        void setRectDatatype(IndexType layerIdx, IndexType rectIdx, IndexType datatype) {_layers.at(layerIdx).rect(rectIdx).setDatatype(datatype); }
        /// @brief set the boundary box of layout
        /// @param boundary box
        void setBoundary(LocType xLo, LocType yLo, LocType xHi, LocType yHi) { _boundary.set(xLo, yLo, xHi, yHi); }

    private:
        std::vector<LayoutLayer> _layers; ///< _text[idx of layer] = vector of text objects
        Box<LocType> _boundary;
        IntType _numLayers = -1; ///< The number of layers
};

PROJECT_NAMESPACE_END

#endif // MAGICAL_FLOW_LAYOUT_H_
