//
// Created by anoynmous on 09/06/2018
//


#ifndef AROUTER_LEF_TECH_COMPONENTS_H_
#define AROUTER_LEF_TECH_COMPONENTS_H_

#include "global/global.h"
#include "util/Vector2D.h"

// LefTechComponents.h
//
// The components for technology reading from LEF file
//

PROJECT_NAMESPACE_BEGIN

class LefUnit
{
    public: 
        explicit LefUnit() = default;
        bool hasDatabase = false;
        bool hasCapacitance = false;
        bool hasResistance = false;
        bool hasTime = false;
        bool hasPower = false;
        bool hasCurrent = false;
        bool hasVoltage = false;
        bool hasFrequency = false;

        std::string datarbaseName = "";
        RealType databaseNumber = 0.0;
        RealType capacitance = 0.0;
        RealType resistance = 0.0;
        RealType time = 0.0;
        RealType power = 0.0;
        RealType current = 0.0;
        RealType voltage = 0.0;
        RealType frequency = 0.0;

};

/// @brief Definitation of properties
/// @brief PROPERTYDEFINIZTIONS statements from LEF
/// TODO: this will be called once or multiple times?
class LefPropDef
{
    public:
        explicit LefPropDef() = default;       
        
        // Have to have
        std::string propType = "";
        std::string propName = "";

        char dataType; // No data, ascill, int2, int4 etc. 
        
        // Optional
        bool hasNumber = false;
        RealType number = 0.0;

        bool hasRange = false;
        RealType left = 0.0;
        RealType right = 0.0;

        bool hasString = false;
        std::string propString = "";

        bool hasNameMapString = false; /// TODO: What is that?
};

/// @brief Define a placement site in the design
/// @brief LEF SITE definitions can be used in DEF ROW statements
class LefSite
{
    public:
        explicit LefSite() = default;
        std::string name = "";
        bool hasClass = false;
        RealType sizeX = 0.0;
        RealType sizeY = 0.0;
        bool hasSize = false;
        bool hasXSymmetry  = false;
        bool hasYSymmetry = false;
        bool has90Symmetry = false;
        bool hasRowPattern = false;
        IntType numSites = 0;
        std::vector<std::string> siteName;
        std::vector<IntType> siteOrient;
        std::vector<std::string> siteOrientStr;
};

/// @brief Layer CUT
/// @brief Layer could be cut, masterslice, implant, or routing.
/// @brief This is cut. For ususally usage, this is for via layer specification
class LefLayerCut
{
    public:
        explicit LefLayerCut() = default;
        std::string name = "";
        RealType width = 0.0;
        //RealType area = 0.0;
        RealType spacing = -1.0;
        /// TODO: Adjacent
        RealType spacingParallelOverLap = -1.0; ///< Spacing if two cuts are parallelly overlapped
        RealType spacingSameNet = -1.0; ///< Spacing if two cuts are from same net
};

/// @brief Layer routing
/// @brief Layer could be cut, masterslice, implant, or routing.
/// @brief This is routing. For ususally usage, this is for metal layer specification

class EOLSpacing
{
    public:
        explicit EOLSpacing(RealType spacing, RealType endOfanoynmouse, RealType within) : _spacing(spacing), _endOfanoynmouse(endOfanoynmouse), _within(within) {}
        RealType _spacing = 0.0;
        RealType _endOfanoynmouse = 0.0;
        RealType _within = 0.0;
};

class LefLayerRouting
{
    public:
        explicit LefLayerRouting() = default;
        bool hasPitch = false;
        bool hasXYPitch = false;
        bool hasOffset = false;
        bool hasXYOffset = false;
        bool hasWidth = false;
        bool hasArea = false;
        //bool hasDiagPitch = false;
        //bool hasXYDiagPitch = false;
        //bool hasDiagWidth = false;
        //bool hasDiagSpacing = false;
        bool hasSpacingNumber = false;
        //std::vector<bool> hasSpacingName;
        //std::vector<bool> hasSpacingLayerStack;
        //std::vector<bool> hasSpacingAdjacent;
        //std::vector<bool> hasSpacingCenterToCenter;
        //std::vector<bool> hasSpacingRange;
        //std::vector<bool> hasSpacingRangeUseLengthThreshold;
        //std::vector<bool> hasSpacingRangeInfluence;
        //std::vector<bool> hasSpacingArea;
        std::vector<bool> hasEndOfanoynmouse;
        bool hasDirection = false;
        bool hasMinArea = false;

        /*
        IntType numProps = 0;
        std::vector<std::string> propName;
        std::vector<std::string> propValue;
        std::vector<RealType> propNumber;
        std::vector<char> propType;
        std::vector<bool> propIsNumber;
        std::vector<bool> propIsString;
         */
        IntType numSpacing = 0;

        std::string name = "";
        //std::string type;
        RealType pitch = 0.0;
        RealType pitchX = 0.0;
        RealType pitchY = 0.0;
        RealType offset = 0.0;
        RealType offsetX = 0.0;
        RealType offsetY = 0.0;
        RealType width = 0.0;
        RealType area = 0.0;
        RealType minArea = 0.0;
        // Just spacing
        std::vector<RealType> spacing;
        // End of anoynmouse spacing
        std::vector<EOLSpacing> eolSpacing;
    

        std::string direction = "";

        bool hasMaxWidth = false;
        bool hasMinWidth = false;

        RealType maxWidth = 0.0;
        RealType minWidth = 0.0;

        IntType numSpacingTable = 0;
        std::vector<RealType> spacingTableParallelRunLength;
        std::vector<RealType> spacingTableWidth;
        Vector2D<RealType> spacingTableSpacing;

};


/// Layer implant
/// Layer could be cut, masterslice, implant, or routing.
/// For ususally usage, this is for implant layer specification
/// Each layer is defined by assigning it a name and simple spacing and width rules. These spacing and width rules only affect the legal cell placments
/// These rules ineract with the library methodology, detailed placement and filler cell support
/// 
/// Ignore for now
class LefLayerImplant
{
};

/// Layer Overlap
/// Usually the top overlap layer above AP, probably not useful for routing?
///
/// Ignore for now
class LefLayerOverlap
{
};

/// Layer Masterslice
/// Masterslice (nonrouting) layers in the design.
/// Masterslice layers are typically polysilicon layers and are only needed if the cell MACROs have pins on the polysilicon layer
/// Should be on the bottom?

class LefLayerMasterslice
{
    public:
        explicit LefLayerMasterslice() = default;
        std::string name;
};

PROJECT_NAMESPACE_END
#endif /// AROUTER_LEF_TECH_COMPONENTS_H_
