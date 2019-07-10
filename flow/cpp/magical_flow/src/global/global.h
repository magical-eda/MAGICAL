/**
 * @file global.h
 * @brief The wrapper for the global-included headers
 * @author Keren Zhu
 * @date 06/14/2019
 */

#ifndef MAGICAL_FLOW_GLOBAL_H_
#define MAGICAL_FLOW_GLOBAL_H_

// Add all global/common header files here

#include "type.h"
#include "constant.h"
#include "parameter.h"
#include "util/MsgPrinter.h"
#include "util/Assert.h"
#include "util/kLibBase.h"

PROJECT_NAMESPACE_BEGIN

// Function aliases
static const auto &INF = MsgPrinter::inf;
static const auto &WRN = MsgPrinter::wrn;
static const auto &ERR = MsgPrinter::err;
static const auto &DBG = MsgPrinter::dbg;

PROJECT_NAMESPACE_END

#include "util/Box.h"

PROJECT_NAMESPACE_BEGIN

// Some convenient functions
namespace MfUtil
{
    /// @brief convert the coordinate under orient and offset for N/R0 orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> northCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        return coord + offset;
    }
    /// @brief convert the coordinate under orient and offset for S/R180 orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> southCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.x();
        x = bbox.xLen() - x;
        x = x + offset.x();
        LocType y = coord.y();
        y = bbox.yLen() - y;
        y = y + offset.y();
        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under orient and offset for W/R90 orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> westCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.y();
        x = bbox.yLen() - x;
        x = x + offset.x();

        LocType y = coord.x();
        y = y + offset.y();

        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under orient and offset for E/R270 orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> eastCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.y();
        x = x + offset.x();

        LocType y = coord.x();
        y = bbox.xLen() - y;
        y = y + offset.y();

        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under orient and offset for FN/MY orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> flipNorthCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.x();
        x = bbox.xLen() - x;
        x = x + offset.x();

        LocType y = coord.y();
        y = y + offset.y();

        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under orient and offset for FS/MX orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> flipSouthCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.x();
        x = x + offset.x();

        LocType y = coord.y();
        y = bbox.yLen() - y;
        y = y + offset.y();

        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under orient and offset for FW/MX90 orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> flipWestCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.y();
        x = x + offset.x();

        LocType y = coord.x();
        y = y + offset.y();

        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under orient and offset for FE/MY90 orientation
    /// @param first: the coordinate before conversion
    /// @param second: the offset coordinate (lower-left point of the bounding box)
    /// @param third: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> flipEastCoordinate(const XY<LocType> coord, const XY<LocType> &offset, const Box<LocType> &bbox)
    {
        LocType x = coord.y();
        x = bbox.yLen() - x;
        x = x + offset.x();

        LocType y = coord.x();
        y = bbox.xLen() - y;
        y = y + offset.y();

        return XY<LocType>(x, y);
    }
    /// @brief convert the coordinate under certain orient and offset
    /// @param first: the coordinate before conversion
    /// @param second: the orientation
    /// @param third: the offset coordinate (lower-left point of the bounding box)
    /// @param fourth: the bounding box
    /// @return the coordinate after conversion
    inline XY<LocType> orientConv(const XY<LocType> coord, OriType orient = OriType::N, const XY<LocType> &offset = XY<LocType>(0, 0), const Box<LocType> &bbox = Box<LocType>(0,0,0,0))
    {
        if (orient == OriType::N)
        {
            return northCoordinate(coord, offset, bbox);
        }
        else if (orient == OriType::S)
        {
            return southCoordinate(coord, offset, bbox);
        }
        else if (orient == OriType::W)
        {
            return westCoordinate(coord, offset, bbox);
        }
        else if (orient == OriType::E)
        {
            return eastCoordinate(coord, offset, bbox);
        }
        else if (orient == OriType::FN)
        {
            return flipNorthCoordinate(coord, offset, bbox);
        }
        else if (orient == OriType::FS)
        {
            return flipSouthCoordinate(coord, offset, bbox);
        }
        else if (orient == OriType::FW)
        {
            return flipWestCoordinate(coord, offset, bbox);
        }
        else
        {
            // FE
            return flipEastCoordinate(coord, offset, bbox);
        }
    }

    /// @brief Determine whether the implementation type is a device or subckt
    /// @param the ImplType for CktGraph
    /// @return true: device. false: subckt
    inline bool isImplTypeDevice(ImplType implType)
    {
        if (implType == ImplType::PCELL_Cap)
        {
            return true;
        }
        else if (implType == ImplType::PCELL_Nch)
        {
            return true;
        }
//        else if (implType == ImplType::PCELL_Nch25ud18mac)
//        {
//            return true;
//        }
        else if (implType == ImplType::PCELL_Pch)
        {
            return true;
        }
        else if (implType == ImplType::PCELL_Res)
        {
            return true;
        }
        else if (implType == ImplType::UNSET)
        {
            return false;
        }
        else
        {
            AssertMsg(false, "UTIL::isDevice: unknown ImplType \n");
            return true;
        }
    }
}

PROJECT_NAMESPACE_END

#endif // MAGICAL_FLOW_GLOBAL_H_
