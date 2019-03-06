//
// Created by anoynmous on 09/06/2018
//

#ifndef AROUTER_LEF_LIB_COMPONENTS_H_
#define AROUTER_LEF_LIB_COMPONENTS_H_

#include "global/global.h"
#include "util/Vector2D.h"
#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN


/* Fixed Via
/ Vias can be fixed vias and genrated vias. 
/ A fixed via is defined using rectangles or polygons.
/ A generated via is defined using viarule parameter and was derived from a VIARULE GENERATE statement
/ All vias consist of shapes on three layers: a cut layer and two routing (or masterslice) layers
*/
class LefFixedVia
{
    public:
        explicit LefFixedVia() = default;

        // Getters
        const std::string &                  name() const           { return _name; }
        bool                                 isDefault() const      { return _isDefault; }
        bool                                 hasResistance() const  { return _hasResistance; }
        IntType                              resistance() const     { return _resistance; }
        IndexType                            accessLayerIdx() const { return _accessLayer; }
        IndexType                            bottomLayerIdx() const { return _bottomLayerIdx; }
        IndexType                            cutLayerIdx() const    { return _cutLayerIdx; }
        IndexType                            topLayerIdx() const    { return _topLayerIdx; }
        const Box<LocType> &                 bottomRect() const     { return _bottomRect; }
        Box<LocType> &                       bottomRect()           { return _bottomRect; }
        IndexType                            numCutRect()           { return _numCutRect; }
        const std::vector<Box<LocType>> &    cutRectArray() const   { return _cutRectArray; }
        std::vector<Box<LocType>> &          cutRectArray()         { return _cutRectArray; }
        const Box<LocType> &                 topRect() const        { return _topRect; }
        Box<LocType> &                       topRect()              { return _topRect; }
        const Box<LocType> &                 boundingBox() const    { return _boundingBox; }
        Box<LocType> &                       boundingBox()          { return _boundingBox; }
        const Box<LocType> &                 cutBBox() const        { return _cutBBox; }
        Box<LocType> &                       cutBBox()              { return _cutBBox; }


        // Setters
        void setName(const std::string & name)                      { _name = name; }
        void setIsDefault(bool isDefault)                           { _isDefault = isDefault; }
        void setHasResistance(bool hasResistance)                   { _hasResistance = hasResistance; }
        void setResistance(IntType resistance)                      { _resistance = resistance; }
        void setAccessLayerIdx(IndexType accessLayerIdx)            { _accessLayer = accessLayerIdx; }
        void setBottomLayerIdx(IndexType bottomLayerIdx)            { _bottomLayerIdx = bottomLayerIdx; }
        void setCutLayerIdx(IndexType cutLayerIdx)                  { _cutLayerIdx = cutLayerIdx; }
        void setTopLayerIdx(IndexType topLayerIdx)                  { _topLayerIdx = topLayerIdx; }
        void setNumCutRect(IndexType numCutRect)                    { _numCutRect = numCutRect; }

        // Misc.
        // Calculate the bounding box for this via
        void findBoundingBox()
        {
            _boundingBox = Box<LocType>(INT_TYPE_MAX, INT_TYPE_MAX, INT_TYPE_MIN, INT_TYPE_MIN);
            _boundingBox.unionBox(_bottomRect);     _boundingBox.unionBox(_topRect);
            _cutBBox = Box<LocType>(INT_TYPE_MAX, INT_TYPE_MAX, INT_TYPE_MIN, INT_TYPE_MIN);
            for (const auto &cutBox : _cutRectArray) { _boundingBox.unionBox(cutBox); _cutBBox.unionBox(cutBox); }
        }

        /// @brief Brief offset everything to align things. 
        void offSetShape()
        {
            if (_cutRectArray.size() == 1)
            {
                XY<LocType> offset = _cutRectArray.front().center();
                offset.setX(-offset.x());
                offset.setY(-offset.y());
                _topRect.offsetBy(offset);
                _bottomRect.offsetBy(offset);
                _cutRectArray.front().offsetBy(offset);
            }
        }

    private:
        std::string _name = "";
        bool _isDefault = false; ///< if this is the default via to be used
        bool _hasResistance = false; ///< if this via has overriding resistance value specified
        IntType _resistance = 0;

        IndexType _accessLayer = 0; ///< the bottom layer of routing/masterslice layer idx. eg. 0, then probably this via connecting PO and M1

        IndexType _bottomLayerIdx = 0; ///< the bottom layer idx (of all layers)
        IndexType _cutLayerIdx = 0; ///< the cut ...
        IndexType _topLayerIdx = 0; ///< the top ...

        Box<LocType> _bottomRect; ///< the metal shape on the bottom layer, has converted to the database integer units
        IndexType _numCutRect = 0; ///< Num of cut rectangles this VIA has
        std::vector<Box<LocType>> _cutRectArray; ///< ... cut layer
        Box<LocType> _topRect; ///< ... top layer

        Box<LocType> _boundingBox; ///< Bounding box of the via
        Box<LocType> _cutBBox; ///< Bounding box for the CUT layer

};

/// @brief Lef Pin
class LefPin
{
    public:
        bool hasForeign = false;
        
        bool hasLEQ = false;
        bool hasDirection = false;
        bool hasUse = false;
        bool hasShape = false;
        bool hasMustJoin = false; // false
        bool hasOutMargin = false; // false
        bool hasOutResistance = false; // false
        bool hasInMargin = false; // false
        bool hasPower = false; // false
        bool hasLeakage = false; // false
        bool hasMaxLoad = false; // false
        bool hasMaxDelay = false; // false
        bool hasCapacitance = false; // false
        bool hasResistance = false; // false
        bool hasPullDownRes = false; // false
        bool hasTieOffr = false; // false
        bool hasVHI = false; // false
        bool hasVLO = false; // false
        bool hasRiseVoltage = false; // false
        bool hasFallVoltage = false; // false
        bool hasRiseThresh = false; // false
        /// TODO: Add others 
        
        std::string name = "";
        IntType numPorts = 0;
        // Only consider rect for now ...
        std::pair<std::string, Box<RealType>> rects; // first = layerStr, second = box of location

        std::string direction = "";
        std::string use = "";
        std::string shape = "";

};

/// @brief Lef Cell Macro
/// @brief Include macro pins
class LefMacro
{
    public:
        explicit LefMacro() = default;
        bool hasClass = false;
        bool hasGenerator = false;
        bool hasGenerate = false;
        bool hasPower = false;
        bool hasOrigin = false;
        bool hasEEQ = false;
        bool hasLEQ = false;
        bool hasSource = false;
        bool hasXSymmetry = false;
        bool hasYSymmetry = false;
        bool has90Symmetry = false;
        bool hasSiteName = false;
        bool hasSitePattern = false;
        bool hasSize = false;
        bool hasForeign = false;
        std::vector<bool> hasForeignOrigin;
        std::vector<bool> hasForeignOrient;
        std::vector<bool> hasForeignPoint;
        bool hasClockType = false;
        bool isBuffer = false;
        bool isInverter = false;

        /// Currently not using properties or other things. Even using, use other data structures
        /*
        IntType numSitePattern = 0;
        IntType numProperties = 0;
        std::vector<std::string> propName;
        std::vector<std::string> propValue;
        std::vector<RealType> propNum;
        std::vector<char> propType;
        std::vector<bool> propIsName;
        std::vector<bool> propIsString;
        */

        std::string name = "";
        std::string macroClass = "";
        std::string generator = "";
        //std::string eeq = "";
        //std::string leq = "";
        //std::string source = "";
        //std::string clockType = "";
        RealType originX = 0.0;
        RealType originY = 0.0;
        //RealType power = 0.0;
        /// generate not considered
        /// sitePattern not considered
        std::string siteName = "";
        RealType sizeX = 0.0;
        RealType sizeY = 0.0;
        IntType numForeigns = 0;
        std::vector<std::string> foreignOrientStr;
        std::vector<RealType> foreignX;
        std::vector<RealType> foreignY;
        std::vector<std::string> foreignName;
};

PROJECT_NAMESPACE_END

#endif ///AROUTER_LEF_LIB_COMPONENTS_H_
