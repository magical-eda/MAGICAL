/**
 * @file Pin.h
 * @brief Pin or steiner points for the Nets
 * @author Keren Zhu
 * @date 08/15/2018
 */

#ifndef AROUTER_PIN_H_
#define AROUTER_PIN_H_

#include "global/global.h"
#include "util/Polygon2Rect.h"

PROJECT_NAMESPACE_BEGIN


/// @class AROUTER::PinShape
/// @brief Pin shapes
class PinPolygon
{
    public:
        explicit PinPolygon() = default; 
        /*------------------------------*/ 
        /* Constructing the polygon     */
        /*------------------------------*/ 
        /// @brief add a corner point
        /// @param a point to as the corner point of the rectangle
        void addPoint(XY<LocType> pt) { _pts.emplace_back(pt); } 
        /// @brief init the polygon with points already added
        /// @return if successful
        bool init();
    private:
        /// @brief split the polygon to a rectangle
        /// @return if the pts can be successfully splitted into rectangles
        bool splitPolygon();
        /// @brief calculate bounding box
        /// @return if the bounding box calculation is successful
        bool calculateBBox();
        /// @brief pick a good point(?) in the shape of pin
        /// @return if a point is successfully picked
        bool pickLoc();
    public:
        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        /// @brief get the rectangles of the pin
        /// @return the splitted polygon as a list of rectangles
        const std::vector<Box<LocType>> & rectangles() const { return _rect; }
        /// @brief get the rectangles of the pin
        /// @return the splitted polygon as a list of rectangles
        std::vector<Box<LocType>> & rectangles() { return _rect; }
        /// @brief whether this pin has pin shape
        /// @return whether the pin has pin shape
        bool hasPin() const { return _pts.size() > 0; }
        /// @brief get the location of point if think the pin as a point
        /// @return the location of the pin as a point
        XY<LocType> loc() const { return _defaultLoc; }
        IndexType defaultShapeIdx() const { return _defaultLocShapeIdx; }
        const std::vector<bool> &includeTermFlag() const { return _includeTermFlag; }
        std::vector<bool> &includeTermFlag() { return _includeTermFlag; }
    private:
        std::vector<XY<LocType>> _pts; ///< The corner points of the shape
        std::vector<Box<LocType>> _rect; ///< The rectangles corresponding to the corner points
        XY<LocType> _defaultLoc; ///< The one point if think the pin shape as a point
        Box<LocType> _bbox; ///< Bounding box of the pin shape polygon
        IndexType _defaultLocShapeIdx = 0; ///< The default location is at this shape
        std::vector<bool> _includeTermFlag; ///< Whether this shape is included in part of terminal, if yes then no need to redundantly include it into the routing
};

inline bool PinPolygon::init()
{
    /// Directly return if no points
    if (_pts.size() == 0) { return true; }
    /// Split the polygon into rectangles
    if (!splitPolygon()) 
    { 
        ERR("Pin: Split polygon into rectangles fails! Check the input design \n"); 
        for (auto pt : _pts)
        {
            ERR("pt: %s \n", pt.toStr().c_str());
        }
        return false; 
    }
    if (_rect.size() == 0 ) { ERR("Pin: the points are not forming a rectilinear polygon \n"); return false; }
    for (IndexType shapeIdx = 0; shapeIdx < _rect.size(); ++shapeIdx)
    {
        _includeTermFlag.emplace_back(false);
    }
    /// Calculate bounding box
    if (!calculateBBox()) {WRN("Pin: Calculating pin bounding box fails! \n"); return false; }
    /// Find a default location of the polygon
    if (!pickLoc()) { WRN("Pin Pick location in the pin shape fails! \n"); return false; }
    return true;
}

inline bool PinPolygon::splitPolygon()
{
    if (_pts.size() == 0)
    {
        return true;
    }
    return ::klib::convertPolygon2Rects(_pts, _rect);
}

class Pin
{
    public:
        explicit Pin() = default;
        explicit Pin(XY<LocType> loc, IndexType layer) : _loc(loc), _layer(layer) {}

        /// @brief init the data structure
        bool init();

        /*------------------------------*/ 
        /* Getters                      */
        /*------------------------------*/ 
        IndexType                           layer() const                           { return _layer; }
        /// @brief get _gridIdx
        /// @return the indices of grid cell of this pin
        const XY<IndexType> &               gridIdx() const                         { return _gridIdx; }
        /// @brief get _gridIdx
        /// @return the indices of grid cell of this pin
        XY<IndexType> &                     gridIdx()                               { return _gridIdx; }
        /// @brief get _isSteiner
        /// @return is this pin a steiner point
        bool                                isSteiner() const                       { return _isSteiner; }

        /// @brief get the polygon rectangles
        /// @return the list of rectangles as the splitied polygon
        const std::vector<Box<LocType>> &   shapeRects() const                      { return _pinShape.rectangles(); }
        /// @brief get the polygon rectangles
        /// @return the list of rectangles as the splitied polygon
        std::vector<Box<LocType>> &         shapeRects()                            { return _pinShape.rectangles(); }
        /// @brief whether this shape is included as a terminal shape
        /// @return if this shape is included as a terminal shape
        bool                                isIncludeTerm(IndexType shapeIdx) const                   { return _pinShape.includeTermFlag().at(shapeIdx); }
        /// @brief get the default shape index, the location should be in this shape 
        /// @return the default shape index
        IndexType defaultShapeIdx() const { return _pinShape.defaultShapeIdx(); }

        /*------------------------------*/ 
        /* Setters                      */
        /*------------------------------*/ 
        void                                setLoc(const XY<LocType> &loc)        { _loc = loc; }
        void                                setLayer(IndexType layer)             { _layer = layer; }
        /// @brief set _gridIdx
        /// @param the indices of grid cell this pin is in
        void                                setGridIdx(const XY<IndexType> &grid) { _gridIdx = grid; }
        /// @brief set _isSteiner
        /// @param whether this pin is a steiner point
        void                                setIsSteiner(bool isSteiner)          { _isSteiner = isSteiner; }

        /// @brief add one point to the pin shape polygon
        /// @param a point of polygon
        void                                addPinShapePoint(XY<LocType> pt)      { _pinShape.addPoint(pt); }
        /// @brief flag this pin as part of terminal shapes, then don't need to be added to the shape map as a individual pin
        /// @param includeTermFlag: whether this pin shape is flagged as part of terminal shapes
        void                                setIncludeTermFlag(IndexType shapeIdx, bool includeTermFlag) { _pinShape.includeTermFlag().at(shapeIdx) = includeTermFlag; }

        /*------------------------------*/ 
        /* Location query               */
        /*------------------------------*/ 
        /// @brief return the x of the pin if think the pin is a point
        /// @return if think this pin as a point, the x coordinate
        LocType ptX() const;
        /// @brief return the y of the pin if think the pin is a point
        /// @return if think this pin as a point, the y coordinate
        LocType ptY() const;
        /// @brief get the default location point of the pin, should distinguishing the pin shape
        /// @return if think this pin as a point, the location
        XY<LocType> loc() const;
        /*------------------------------*/ 
        /* Print                        */
        /*------------------------------*/ 
        std::string toStr() const
        {
            std::ostringstream oss;
            oss<<"("<<this->ptX()<<","<<this->ptY()<<","<<layer()<<")";
            return oss.str();
        }
    private:
        XY<LocType> _loc; ///< location
        IndexType _layer = 0; ///< layer
        bool _isSteiner = false; ///< is this pin a steiner point? If false: this is a physical pin
        XY<IndexType> _gridIdx; ///< the indices of grid cell this pin laying in
        PinPolygon _pinShape; ///< the pin shape (now only 1 polygon)
};

inline LocType Pin::ptX() const
{
    if (!_pinShape.hasPin())
    {
        return _loc.x();
    }
    return _pinShape.loc().x(); 
}

inline LocType Pin::ptY() const
{
    if (!_pinShape.hasPin())
    {
        return _loc.y();
    }
    return _pinShape.loc().y(); 
}

inline XY<LocType> Pin::loc() const
{
    if (!_pinShape.hasPin())
    {
        return _loc;
    }
    return _pinShape.loc();
}

inline bool Pin::init()
{
    /// Split the polygon into rectangles
    return _pinShape.init();
}
PROJECT_NAMESPACE_END


#endif /// AROUTER_PIN_H_
