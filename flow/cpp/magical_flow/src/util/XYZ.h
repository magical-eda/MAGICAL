#ifndef __XYZ_H__
#define __XYZ_H__

#include "global/namespace.h"
#include "MsgPrinter.h"
#include "XY.h"

PROJECT_NAMESPACE_BEGIN


inline PtPosType revertPtPos(PtPosType init)
{
    if (init == PtPosType::NORTH)
    {
        return PtPosType::SOUTH;
    }
    else if (init == PtPosType::SOUTH)
    {
        return PtPosType::NORTH;
    }
    else if (init == PtPosType::WEST)
    {
        return PtPosType::EAST;
    }
    else if (init == PtPosType::EAST)
    {
        return PtPosType::WEST;
    }
    else if (init == PtPosType::TOP)
    {
        return PtPosType::BOTTOM;
    }
    else if (init == PtPosType::BOTTOM)
    {
        return PtPosType::TOP;
    }
    return PtPosType::NOT_NEIGHBOR;
}


template <typename XYType, typename ZType = XYType>
class XYZ
{
    typedef boost::geometry::model::point<XYType, 3, boost::geometry::cs::cartesian> boostPointType;
public:
    explicit XYZ() = default;
    explicit XYZ(XYType x, XYType y, ZType z) : _xy(x, y), _z(z) {}
    explicit XYZ(XY<XYType> xy, ZType z) : _xy(xy), _z(z) {}
    explicit XYZ(XYType x, XYType y) :_xy(x,y), _z(0) { MsgPrinter::wrn("%s, filling in 0 as z for XYZ(x,y) \n", __PRETTY_FUNCTION__);}

    /// Getters
    XYType                x() const                           { return _xy.x(); }
    XYType                y() const                           { return _xy.y(); }
    ZType                 z() const                           { return _z; }
    const XY<XYType> &    xy() const                          { return _xy; } 
    XYZ<XYType, ZType>    north() const                       { return XYZ<XYType, ZType>(_xy.x(), _xy.y() + 1, _z); }
    XYZ<XYType, ZType>    south() const                       { return XYZ<XYType, ZType>(_xy.x(), _xy.y() - 1, _z); }
    XYZ<XYType, ZType>    east() const                        { return XYZ<XYType, ZType>(_xy.x() + 1, _xy.y(), _z); }
    XYZ<XYType, ZType>    west() const                        { return XYZ<XYType, ZType>(_xy.x() - 1, _xy.y(), _z); }
    XYZ<XYType, ZType>    top() const                         { return XYZ<XYType, ZType>(_xy.x(), _xy.y(), _z + 1); }
    XYZ<XYType, ZType>    bottom() const                      { return XYZ<XYType, ZType>(_xy.x(), _xy.y(), _z - 1); }

    /// Setters
    void                  setX(XYType x)                       { _xy.setX(x); }
    void                  setY(XYType y)                       { _xy.setY(y); }
    void                  setZ(ZType z)                        { _z = z; }
    void                  setXY(const XY<XYType> &xy)          { _xy = xy; }
    void                  setXY(XYType x, XYType y)            { _xy.setXY(x, y); }
    void                  setXYZ(XYType x, XYType y, ZType z)  { _xy.setXY(x, y); _z = z; }

    bool                  operator==(const XYZ<XYType, ZType> &rhs) const { return _xy == rhs.xy() && _z == rhs.z(); } 
    bool                  operator!=(const XYZ<XYType, ZType> &rhs) const { return ! (*this == rhs); }
    bool                  operator<(const XYZ<XYType, ZType> &rhs)  const  { return _z == rhs.z() ? _xy < rhs.xy() : _z < rhs.z(); }


    /// Find the relative position of the rhs TO this
    PtPosType             ptPos(const XYZ<XYType, ZType> &rhs) const
    {
        if (north() == rhs)
        {
            return PtPosType::NORTH;
        }
        else if (south() == rhs)
        {
            return PtPosType::SOUTH;
        }
        else if (east() == rhs)
        {
            return PtPosType::EAST;
        }
        else if (west() == rhs)
        {
            return PtPosType::WEST;
        }
        else if (bottom() == rhs)
        {
            return PtPosType::BOTTOM;
        }
        else if (top() == rhs)
        {
            return PtPosType::TOP;
        }
        else
        {
            return PtPosType::NOT_NEIGHBOR;
        }
    }

    boostPointType     toBoost() const
    {
        return boostPointType(_xy.x(), _xy.y(), _z);
    }

    void fromBoost(const boostPointType &rtV)
    {
        XYType x = boost::geometry::get<0>(rtV);
        XYType y = boost::geometry::get<1>(rtV);
        ZType z = boost::geometry::get<2>(rtV);
        setXYZ(x,y,z);
    }

    /// Misc.
    std::string toStr() const
    {
        return std::string(*this);
    }

    operator std::string() const
    {
        std::ostringstream oss;
        oss << " ("<<_xy.x()<<","<<_xy.y()<<","<<_z<<") ";
        return oss.str();
    }
private:
    XY<XYType>  _xy;
    ZType       _z = 0;
};

template<typename T>
struct XYZHashFunc
{
    std::size_t operator() (const XYZ<T> &xyz) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, xyz.x());
        boost::hash_combine(seed, xyz.y());
        boost::hash_combine(seed, xyz.z());
        return seed;
    };
};

PROJECT_NAMESPACE_END


/// Hash function
namespace std
{
    template<typename T, typename U> 
    struct hash<PROJECT_NAMESPACE::XYZ<T, U> >
    {
        typedef PROJECT_NAMESPACE::XYZ<T, U> argumentType;
        typedef std::size_t resultType;
        resultType operator()(argumentType const& s) const noexcept
        {
            resultType seed = 0;
            boost::hash_combine(seed, s.x());
            boost::hash_combine(seed, s.y());
            boost::hash_combine(seed, s.z());
            return seed;
        }
    };
    template<> 
    struct hash<PROJECT_NAMESPACE::XYZ<uint32_t> >
    {
        typedef PROJECT_NAMESPACE::XYZ<uint32_t> argumentType;
        typedef std::size_t resultType;
        resultType operator()(argumentType const& s) const noexcept
        {
            resultType seed = 0;
            boost::hash_combine(seed, s.x());
            boost::hash_combine(seed, s.y());
            boost::hash_combine(seed, s.z());
            return seed;
        }
    };
}

#endif // __XYZ_H__
