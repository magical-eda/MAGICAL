//
// Created by Keren on 09/14/2018
//

#ifndef AROUTER_WRITEGDS_H_
#define AROUTER_WRITEGDS_H_

#include <limbo/parsers/gdsii/stream/GdsWriter.h>
#include "global/global.h"
#include "util/Box.h"

// ================================================================================ 
// WriteGds.h
//
// A wrapper for Yibo's gds writer
//
// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

/// @brief enum for the datatype
enum class GdsDataTypeType : Byte
{
    NODATA = 0,
    BITARRAY = 1,
    INT2 = 2,
    INT4 = 3,
    REAL4 = 4,
    REAL8 = 5,
    ASCII = 6
};

static IntType gdsDataTypeType2Int(GdsDataTypeType dataType)
{
    switch(dataType)
    {
        case GdsDataTypeType::NODATA : return 0; break;
        case GdsDataTypeType::BITARRAY : return 1; break;
        case GdsDataTypeType::INT2 : return 2; break;
        case GdsDataTypeType::INT4 : return 3; break;
        case GdsDataTypeType::REAL4 : return 4; break;
        case GdsDataTypeType::REAL8 : return 5; break;
        case GdsDataTypeType::ASCII : return 6; break;
        default : return 0; break;
    }
}

// see http://www.artwork.com/gdsii/gdsii/page4.htm for the end cap path types
enum class GdsPathEndCapType : Byte
{
    FLUSH  = 0,
    HALF_ROUND_EXTENSION = 1,
    HALF_WIDTH_EXTENSION = 2,
    CUSTOM = 4
};

static IntType gdsPathEndCapType2Int(GdsPathEndCapType pathEnd)
{
    switch(pathEnd)
    {
        case GdsPathEndCapType::FLUSH : return 0; break;
        case GdsPathEndCapType::HALF_ROUND_EXTENSION : return 1; break;
        case GdsPathEndCapType::HALF_WIDTH_EXTENSION : return 2; break;
        case GdsPathEndCapType::CUSTOM : return 4; break;
        default : return 0; break;
    }
}

// Check if the _gw is active, and if not, return false
#ifdef NDEBUG
#define checkActive() \
        do { \
        } while(false)
#else
#define checkActive() \
    if (!_isActive) \
    { \
        ERR("%s: Error: There is no active gdsWriter inited yet! \n", __FUNCTION__); \
        return false; \
    }
#endif

/// Define a macro to check wheter _isActive
class WriteGds
{
    public:
        explicit WriteGds(std::string filename) : _gw(filename.c_str()) {}

        //////////////////////////////
        /// Writing function routines
        //////////////////////////////

        bool initWriter();
        bool createLib(const std::string &libName, RealType dbu_uu = 0.001, RealType dbu_m = 1e-09);
        bool endLib();
        bool writeCellBgn(const std::string &cellName);
        bool writeCellEnd();

        ////////////////////////////////
        /// Write shapes, cells etc.
        ////////////////////////////////
        /// Write a cell/structure
        bool writeCellRef(std::string cellName, XY<IntType> loc, RealType mag = 1.0, RealType angle = 0.0);

        /// Write an rectangle
        bool writeRectangle(const Box<IntType> &box, IntType layer = 1, IntType dataType = 0);
        bool writeRectangle(const Box<IntType> &box, IntType layer = 1, GdsDataTypeType dataType = GdsDataTypeType::NODATA) { return writeRectangle(box, layer, gdsDataTypeType2Int(dataType)); }

        /// Write an text
        bool writeText(const std::string &text, IntType x, IntType y, IntType layer, IntType size);

        /// write an path
        bool writePath(const std::vector<IntType> x, const std::vector<IntType> y, IntType width, IntType layer = 1, IntType dataType = 0, IntType ennCapType = 0);
        bool writePath(const std::vector<IntType> x, const std::vector<IntType> y, IntType width, IntType layer = 1, GdsDataTypeType dataType = GdsDataTypeType::NODATA, GdsPathEndCapType  endCapType= GdsPathEndCapType::FLUSH) { return writePath(x, y, width, layer, gdsDataTypeType2Int(dataType), gdsPathEndCapType2Int(endCapType)); }
    private:
        GdsParser::GdsWriter _gw;
        bool _isActive = false; /// Whether GdsParser::GdsWriter is in active writing
};

PROJECT_NAMESPACE_END
#endif //AROUTER_WRITEGDS_H_
