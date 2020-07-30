#ifndef __XY_H__
#define __XY_H__

#include "global/namespace.h"
#include "BasicTypeSelection.h"
#include "MsgPrinter.h"
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry.hpp>

PROJECT_NAMESPACE_BEGIN

/// enum type for position of segments versus points
enum class PtPosType : std::uint8_t
{
    NORTH        = 0,
    SOUTH        = 1,
    WEST         = 2,
    EAST         = 3,
    TOP          = 4,
    BOTTOM       = 5,
    NOT_NEIGHBOR = 6
};

template <typename T>
class XY
{
    typedef boost::geometry::model::point<T, 2, boost::geometry::cs::cartesian> boostPointType;
public:

    template <typename U>
    friend XY<typename MetaProg::selectBasicType<T, U>::type> operator*(U lhs, const XY<T> &rhs) { return XY<typename MetaProg::selectBasicType<T, U>::type>(lhs * rhs.x(), lhs * rhs.y()); }

    template <typename U>
    friend XY<typename MetaProg::selectBasicType<T, U>::type> operator*(const XY<T> &lhs, U rhs) { return XY<typename MetaProg::selectBasicType<T, U>::type>(lhs.x() * rhs, lhs.y() * rhs); }

    template <typename U>
    friend XY<typename MetaProg::selectBasicType<T, U>::type> operator/(const XY<T> &lhs, U rhs) { return XY<typename MetaProg::selectBasicType<T, U>::type>(lhs.x() / rhs, lhs.y() / rhs); }

public:
    explicit XY() = default;
    explicit XY(T x, T y) : _x(x), _y(y) {}
    //explicit XY(T x, T y, T z) : _x(x), _y(y) {MsgPrinter::wrn("%s, ignore z for XY(x,y,z) \n", __PRETTY_FUNCTION__);}

    template <typename U>
    explicit XY(const XY<U> &other) : _x((T)other.x()), _y((T)other.y()) {}

    // Getters
    T        x() const        { return _x; }
    T        y() const        { return _y; }
    XY<T>    left() const     { return XY<T>(_x - 1, _y); }
    XY<T>    right() const    { return XY<T>(_x + 1, _y); }
    XY<T>    bottom() const   { return XY<T>(_x, _y - 1); }
    XY<T>    top() const      { return XY<T>(_x, _y + 1); }

    // Setters
    void     setX(T x)       { _x = x; }
    void     setY(T y)       { _y = y; }
    void     setXY(T x, T y) { _x = x; _y = y; }

    bool     operator==(const XY<T> &rhs) const { return _x == rhs.x() && _y == rhs.y(); }
    bool     operator!=(const XY<T> &rhs) const { return ! (*this == rhs); }
    bool     operator<(const XY<T> &rhs) const { return _y == rhs.y() ? _x < rhs.x() : _y < rhs.y(); }

    template <typename U>
    XY<typename MetaProg::selectBasicType<T, U>::type>   operator+(const XY<U> &rhs) const  { return XY<typename MetaProg::selectBasicType<T, U>::type>(_x + rhs.x(), _y + rhs.y()); }

    template <typename U>
    XY<typename MetaProg::selectBasicType<T, U>::type>   operator-(const XY<U> &rhs) const  { return XY<typename MetaProg::selectBasicType<T, U>::type>(_x - rhs.x(), _y - rhs.y()); }

    template <typename U>
    XY<T> &                                              operator+=(const XY<U> &rhs)       { _x += rhs.x(); _y += rhs.y(); return *this; }

    template <typename U>
    XY<T> &                                              operator-=(const XY<U> &rhs)       { _x -= rhs.x(); _y -= rhs.y(); return *this; }

    template <typename U>
    XY<T> &                                              operator*=(U rhs)                  { _x *= rhs; _y *= rhs; return *this; }

    template <typename U>
    XY<T> &                                              operator/=(U rhs)                  { _x /= rhs; _y /= rhs; return *this; }

    /// Find the relative position of the rhs TO this
    PtPosType             ptPos(const XY<T> &rhs) const
    {
        if (top() == rhs)
        {
            return PtPosType::NORTH;
        }
        else if (bottom() == rhs)
        {
            return PtPosType::SOUTH;
        }
        else if (right() == rhs)
        {
            return PtPosType::EAST;
        }
        else if (left() == rhs)
        {
            return PtPosType::WEST;
        }
        else
        {
            return PtPosType::NOT_NEIGHBOR;
        }
    }
    
    boostPointType     toBoost() const
    {
        return boostPointType(_x, _y);
    }
    void fromBoost(const boostPointType &rtV)
    {
        T x = boost::geometry::get<0>(rtV);
        T y = boost::geometry::get<1>(rtV);
        setXY(x,y);
    }
    
    /// Misc.
    std::string toStr() const
    {
        std::ostringstream oss;
        oss << " ("<<_x<<","<<_y<<") ";
        return oss.str();
    }
private:
    T       _x = 0;
    T       _y = 0;
};

template<typename T>
struct XYHashFunc
{
    std::size_t operator() (const XY<T> &xy) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, xy.x());
        boost::hash_combine(seed, xy.y());
        return seed;
    };
};

PROJECT_NAMESPACE_END

/// Hash function
namespace std
{
    template<typename T> 
    struct hash<PROJECT_NAMESPACE::XY<T> >
    {
        typedef PROJECT_NAMESPACE::XY<T> argumentType;
        typedef std::size_t resultType;
        resultType operator()(argumentType const& s) const noexcept
        {
            resultType seed = 0;
            boost::hash_combine(seed, s.x());
            boost::hash_combine(seed, s.y());
            return seed;
        }
    };
}

#endif // __XY_H__
