//
// Created by Keren on 04/17/2018
//

#ifndef ZKUTIL_VECTOR2D_H_
#define ZKUTIL_VECTOR2D_H_

#include <vector>
#include <initializer_list>
#include "src/global/namespace.hpp"
#include "src/global/type.hpp"
#include "src/global/define.hpp"
#include "src/geo/point.hpp"

PROJECT_NAMESPACE_START

/// ================================================================================ 
/// Vector map class
/// A templete for 2D vector using flat vector
/// This is a Y-dominate implementation, that is elements in the
/// same column (with the same Y) are in a piece of consecutive memory.
/// Thus, when iterate through this 2D vector using a two-level for loop,
/// the following pattern is better from the memory/cache hit prespective
/// for (x ...)
/// {
///     for (y ....)
///     {
///         ....
///     }
/// }
///
/// Update: 10/26/2018 add option to yMajor
/// ================================================================================ 
template <typename T>
class Vector2D 
{
    public:
        /// Class for initializer_list type
        enum class InitListType 
        {
            XMajor,
            yMajor
        };

    public:
        explicit Vector2D() = default;
        explicit Vector2D(UInt_t xSize, UInt_t ySize) : _vec(xSize * ySize), _xSize(xSize), _ySize(ySize) {}
        explicit Vector2D(UInt_t xSize, UInt_t ySize, const T &v) : _vec(xSize * ySize, v), _xSize(xSize), _ySize(ySize) {}
        explicit Vector2D(UInt_t xSize, UInt_t ySize, InitListType t, std::initializer_list<T> l);

        void                                      clear()                                                { _vec.clear(); _xSize = _ySize = 0; }
        void                                      resize(UInt_t xSize, UInt_t ySize)               { _vec.resize(xSize * ySize); _xSize = xSize; _ySize = ySize; }
        void                                      resize(UInt_t xSize, UInt_t ySize, const T &v)   { _vec.resize(xSize * ySize, v); _xSize = xSize; _ySize = ySize; }
        void                                      setType(InitListType type) { _type = type; }
        
        UInt_t                                 xSize() const                                          { return _xSize; }
        UInt_t                                 ySize() const                                          { return _ySize; }
        UInt_t                                 size() const                                           { return _vec.size(); }
        InitListType                              type() const                                           { return _type; }

        const T &                                 at(UInt_t x, UInt_t y) const                     { return this->atIdx(xyToIndex(x, y)); }
        T &                                       at(UInt_t x, UInt_t y)                           { return this->atIdx(xyToIndex(x, y)); }
        const T &                                 at(Point<UInt_t> xy) const                             { return this->at(xyToIndex(xy)); }
        T &                                       at(Point<UInt_t> xy)                                   { return this->at(xyToIndex(xy)); }
        const T &                                 atIdx(UInt_t idx) const                             { return _vec.at(idx); }
        T &                                       atIdx(UInt_t idx)                                   { return _vec.at(idx); }

        /// Iterator
        typename std::vector<T>::iterator         begin()                                                { return _vec.begin(); }
        typename std::vector<T>::const_iterator   begin() const                                          { return _vec.begin(); }
        typename std::vector<T>::iterator         end()                                                  { return _vec.end(); }
        typename std::vector<T>::const_iterator   end() const                                            { return _vec.end(); }

        /// Conversion
        UInt_t                                 xyToIndex(UInt_t x, UInt_t y) const;
        UInt_t                                 xyToIndex(Point<UInt_t> xy) const                      { return xyToIndex(xy.x(), xy.y()); }
        UInt_t                                 idxToX(UInt_t idx) const;
        UInt_t                                 idxToY(UInt_t idx) const;
        Point<UInt_t>                             idxToXY(UInt_t idx) const                           { return Point<UInt_t>(idxToX(idx), idxToY(idx)); }

    private:
        std::vector<T> _vec;
        UInt_t      _xSize = 0;
        UInt_t      _ySize = 0;
        InitListType   _type = InitListType::XMajor;
};

template<typename T>
inline UInt_t Vector2D<T>::xyToIndex(UInt_t x, UInt_t y) const 
{
    if (_type == InitListType::XMajor)
    {
        return _ySize * x + y;
    }
    else 
    {
        /// Y major
        return _xSize *y + x;
    }
}

template<typename T>
inline UInt_t Vector2D<T>::idxToX(UInt_t idx) const 
{
    if (_type == InitListType::XMajor)
    {
        return idx / _ySize;
    }
    else
    {
        return idx % _xSize;
    }
}

template<typename T>
inline UInt_t Vector2D<T>::idxToY(UInt_t idx) const 
{
    if (_type == InitListType::XMajor)
    {
        return idx % _ySize;
    }
    else
    {
        return idx / _xSize;
    }
}
/// Ctor using initializer_list
template <typename T>
inline Vector2D<T>::Vector2D(UInt_t xSize, UInt_t ySize, InitListType t, std::initializer_list<T> l)
    : _vec(xSize * ySize), _xSize(xSize), _ySize(ySize) 
        {
        Assert(l.size == size());
        if (t == InitListType::XMajor) 
        {
           /// Data in Vector2D is in X-major manner
           /// Just copy the initializer_list to the _vec
            std::copy(l.begin, l.end(), _vec.begin());
        }
        else 
        { // t == InitiListType::YMajor
            for (UInt_t idx = 0; idx < size(); ++idx) 
            {
                UInt_t x = idx % _xSize;
                UInt_t y = idx / _xSize;
                at(x, y) = *(l.begin() + idx);
            }
        }
}

PROJECT_NAMESPACE_END

#endif //ZKUTIL_VECTOR2D_H_
