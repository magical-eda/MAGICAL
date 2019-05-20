/**
 * @file Interval.h
 * @brief A interval from begin() to end(). begin() is always less than end()
 * @author Keren Zhu
 * @date 05/08/2018
 */

#ifndef ZKUTIL_INTERVAL_H_
#define ZKUTIL_INTERVAL_H_

#include <algorithm> /// std::max std::min
//#include <boost/functional/hash_fwd.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry.hpp>
#include "global/namespace.h"

// ================================================================================ 
// Updated:
// 09/20/2018:
//     Now the begin and end must be in order
//     In other words, there must be operator < for type T
// 10/04/2018
//     Add hash function
//     Note that the boost::hash_combine has been changed location in the newest version
//     The one inlcude here is the old version boost
// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

template<typename T>
class Interval
{
    public:
        explicit Interval() = default;
        explicit Interval(const T &begin, const T &end)  { _begin = std::min(begin, end); _end = std::max(begin, end); }

        // Getters
        const T &      begin() const                { return _begin; }
        T &            begin()                      { return _begin; }
        const T &      end() const                  { return _end; }
        T &            end()                        { return _end; }
        /// @brief get the length of the interval
        /// @return the length of the interval
        T              length() const               { return _end - _begin; }

        // setters
        void           setBegin(const T &begin)     { _begin = std::min(begin, _end); _end = std::max(begin, _end); }
        void           setEnd(const T &end)         { _begin = std::min(_begin, end); _end = std::max(_begin, end); }

        /*------------------------------*/ 
        /* Useful operations            */
        /*------------------------------*/ 
        /// @brief determine if two interval are adjacent
        /// @param another Interval<T> want to compare
        /// @return true if two intervals are adjacent to each other, false if not
        bool adjacent(const Interval<T> &other) const;
        /// @brief operator <
        /// @param another Interval
        bool operator<(const Interval<T> &rhs) const { return this->begin() == rhs.begin()? this->end() < rhs.end() : this->begin() < rhs.end(); }
        /// @brief operator ==
        /// @pararm another Interval
        bool operator==(const Interval<T> &rhs) const { return this->begin() == rhs.begin() && this->end() == rhs.end(); }


    private:
        T   _begin;
        T   _end;
};

template<typename T>
bool Interval<T>::adjacent(const Interval<T> &other) const
{

    if (this->end() == other.begin())   { return true; }
    if (this->begin() == other.end())   { return true; }
    if (this->begin() == other.begin()) { return true; }
    if (this->end() == other.end())     { return true; }
    return false;
}

PROJECT_NAMESPACE_END


/// @brief hash function for PROJECT_NAMASPACE::Interval
namespace std
{
    template<typename T> 
    struct hash<PROJECT_NAMESPACE::Interval<T> >
    {
        typedef PROJECT_NAMESPACE::Interval<T> argumentType;
        typedef std::size_t resultType;
        resultType operator()(argumentType const& s) const noexcept
        {
            resultType seed = 0;
            boost::hash_combine(seed, hash<T>(s.begin()));
            boost::hash_combine(seed, hash<T>(s.end()));
            return seed;
        }
    };
}
#endif ///ZKUTIL_INTERVAL_H_
