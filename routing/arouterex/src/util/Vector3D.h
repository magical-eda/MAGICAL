//
// Created by anoynmous on 04/17/2018
//

#ifndef ZKUTIL_VECTOR3D_H_
#define ZKUTIL_VECTOR3D_H_

#include <vector>
#include <initializer_list>
#include "global/namespace.h"
#include "global/type.h"
#include "util/XYZ.h"

PROJECT_NAMESPACE_BEGIN

/// ================================================================================ 
/// Vector cube class
/// A templete for 3D vector using flat vector
/// This is a Z-dominate-y-dominate-x implementation, that is elements in the
/// same column (with the same z, then the same x) are in a piece of consecutive memory.
/// Thus, when iterate through this 2D vector using a two-level for loop,
/// the following pattern is better from the memory/cache hit prespective
/// for (x ...)
/// {
///     for (y ....)
///     {
///         for (z ...) 
///         {
///             ...
///         }
///     }
/// }
/// ================================================================================ 


template <typename T>
class Vector3D 
{
        public:
        explicit Vector3D() = default;
        explicit Vector3D(IndexType xSize, IndexType ySize, IndexType zSize) : _vec(xSize * ySize * zSize), _xSize(xSize), _ySize(ySize), _zSize(zSize), _yzSize(zSize * ySize) {}
        explicit Vector3D(IndexType xSize, IndexType ySize, IndexType zSize, const T &v) : _vec(xSize * ySize * zSize, v), _xSize(xSize), _ySize(ySize), _zSize(zSize), _yzSize(zSize * ySize) {}

        void                                      clear()                                                                 { _vec.clear(); _xSize = _ySize = _zSize = _yzSize  = 0; }
        void                                      resize(IndexType xSize, IndexType ySize, IndexType zSize)               { _vec.resize(xSize * ySize * zSize); _xSize = xSize; _ySize = ySize; _zSize = zSize; _yzSize = zSize * ySize;  }
        void                                      resize(IndexType xSize, IndexType ySize, IndexType zSize, const T &v)   { _vec.resize(xSize * ySize * zSize, v); _xSize = xSize; _ySize = ySize; _zSize = zSize; _yzSize = zSize * ySize; }
        
        IndexType                                 xSize() const                                                       { return _xSize; }
        IndexType                                 ySize() const                                                       { return _ySize; }
        IndexType                                 zSize() const                                                       { return _zSize; }
        IndexType                                 size() const                                                        { return _vec.size(); }

        T &                                       at(IndexType x, IndexType y, IndexType z)                           { return _vec.at(xyzToIndex(x, y, z)); }
        const T &                                 at(IndexType x, IndexType y, IndexType z) const                     { return _vec.at(xyzToIndex(x, y, z)); }
        T &                                       at(IndexType idx)                                      { return _vec.at(idx); }
        const T &                                 at(IndexType idx) const                                { return _vec.at(idx); }

        const T &                                 at(const XYZ<IndexType> &xyz) const                    { return _vec.at(xyzToIndex(xyz)); } 
        T &                                       at(const XYZ<IndexType> &xyz)                          { return _vec.at(xyzToIndex(xyz)); }

        /// Iterator
        typename std::vector<T>::iterator         begin()                                                { return _vec.begin(); }
        typename std::vector<T>::const_iterator   begin() const                                          { return _vec.begin(); }
        typename std::vector<T>::iterator         end()                                                  { return _vec.end(); }
        typename std::vector<T>::const_iterator   end() const                                            { return _vec.end(); }

        /// Conversion
        IndexType                                 xyzToIndex(IndexType x, IndexType y, IndexType z) const { return _yzSize * x + y * _zSize + z; }
        IndexType                                 xyzToIndex(XYZ<IndexType> xyz)                    const { return _yzSize * xyz.x() + _zSize * xyz.y() + xyz.z(); }
        IndexType                                 idxToX(IndexType idx) const                            { return idx / _yzSize; }
        IndexType                                 idxToY(IndexType idx) const                            { return idx % _yzSize / _zSize; }
        IndexType                                 idxToZ(IndexType idx) const                            { return idx % _yzSize % _zSize; }
        XYZ<IndexType>                            idxToXYZ(IndexType idx) const
        {
            return XYZ<IndexType>( idxToX(idx),
                                   idxToY(idx),
                                   idxToZ(idx) );
        }

    private:
        std::vector<T> _vec;
        IndexType      _xSize = 0;
        IndexType      _ySize = 0;
        IndexType      _zSize = 0;
        IndexType      _yzSize = 0; /// zSize * ySize
};


PROJECT_NAMESPACE_END

#endif //ZKUTIL_VECTOR3D_H_
