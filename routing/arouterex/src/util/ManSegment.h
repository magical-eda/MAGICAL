//
// Created by anoynmous on 09/20/2018
//

#ifndef ZKUTIL_MANSEGMENT_H_
#define ZKUTIL_MANSEGMENT_H_

#include "Interval.h"
#include "XY.h"
#include "global/global.h"

// ================================================================================ 
// ManSegment.h
// 
// Extension of Interval, not this is the actaul anoynmouse with direction and location
// This Mananoynmouse must be horizontal or vertical, so it is called Manhattan anoynmouse
//
// ================================================================================ 

PROJECT_NAMESPACE_BEGIN

enum class ManSegDirType  : Byte
{
    HORIZONTAL = 0,
    VERTICAL = 1
};

enum class SegmentRelationType
{
    // Same direction:
    PARALLEL_NONOVERLAP,
    PARALLEL_OVERLAP,
    PARALLEL_CONNECT,
    SAME_anoynmousE_NONOVERLAP,
    SAME_anoynmousE_OVERLAP,
    SAME_anoynmousE_CONNECT, // end to end touched
    // Different direction:
    TSHAPE,
    CORNER,
    INTERCECT,
    SEPERATED
};

template<typename T>
class ManSegment
{
    public:
        explicit ManSegment() = default;
        explicit ManSegment(XY<T> pt1, XY<T> pt2);
        explicit ManSegment(T pt1, T pt2, T coord, ManSegDirType dir) : _interval(pt1, pt2), _coordinate(coord), _direction(dir) {}

        // Getters
        const Interval<T> &         interval() const                { return _interval; }
        Interval<T> &               interval()                      { return _interval; }
        T                           begin() const                   { return _interval.begin(); }
        T                           end() const                     { return _interval.end(); }
        T                           coordinate() const              { return _coordinate; }
        ManSegDirType               direction() const               { return _direction; }
        /// @brief get length of the segment
        /// @return the length of the segment
        T                           length() const                  { return _interval.length(); }
        /// @brief get the begin point
        /// @return the begin point
        XY<T>                       beginXY() const                 
        {
            if (_direction == ManSegDirType::HORIZONTAL)
            {
                return XY<T>(this->begin(), this->coordinate());
            }
            else
            {
                return XY<T>(this->coordinate(), this->begin());
            }
        }
        /// @brief get the end point
        /// @return the end point
        XY<T>                       endXY() const                 
        {
            if (_direction == ManSegDirType::HORIZONTAL)
            {
                return XY<T>(this->end(), this->coordinate());
            }
            else
            {
                return XY<T>(this->coordinate(), this->end());
            }
        }
        /// @brief get the center point
        /// @return the center point
        XY<T>                       centerXY() const                 
        {
            if (_direction == ManSegDirType::HORIZONTAL)
            {
                return XY<T>( ( this->begin() + this->end() ) / 2, this->coordinate());
            }
            else
            {
                return XY<T>(this->coordinate(),  ( this->begin() + this->end() ) / 2);
            }
        }

        // Setters
        void                        setBegin(T begin)               { _interval.setBegin(begin); }
        void                        setEnd(T end)                   { _interval.setEnd(end); }
        void                        setCoordinate(T coord)          { _coordinate = coord; }
        void                        setDirection(ManSegDirType dir) { _direction = dir; }

        // Operator
        
        operator std::string() const {
            std::ostringstream oss;
            if (_direction == ManSegDirType::HORIZONTAL)
            {
                oss<<"[("<<begin()<< ", "<< coordinate() << " -> ("<< end()<<", "<<coordinate()<<")]";
            }
            else if (_direction == ManSegDirType::VERTICAL)
            {
                oss<<"[("<<coordinate()<< ", "<< begin() << " -> ("<< coordinate()<<", "<<end()<<")]";
            }
            return oss.str();
        }

        // Misc.
        /// @brief Find the geometric relationship between segment. Return as an enum
        SegmentRelationType relation(const ManSegment<T> &other) const;
        /// @brief Determine whether there is an intercect point between two segments
        bool isIntercect(const ManSegment<T> &other) const;
        /// @brief Determine wheter two segments touch with each other (intersect + same anoynmouse overlap)
        bool isTouch(const ManSegment<T> &other) const;
        XY<T> intersectPoint(const ManSegment<T> &other) const;
    private:
        Interval<T> _interval;
        T           _coordinate; ///< the coordinate this segment is at. If horizontal, it is y, if veritical, x.
        ManSegDirType _direction = ManSegDirType::HORIZONTAL; ///< the direction of this Man anoynmouse

};

// Constructor

template<typename T>
inanoynmouse ManSegment<T>::ManSegment(XY<T> pt1, XY<T> pt2)
{
    AssertMsg(pt1.x() == pt2.x() || pt1.y() == pt2.y(), "%s: cannot construct the segment! The two input points cannot construct a horizontal or veritcal segment \n", __FUNCTION__);
    if (pt1.x() == pt2.x())
    {
        // vertical
        _interval = Interval<T>(pt1.y(), pt2.y()); // Interval will put begin and end in order automatically -> no need to compare pt1.y() and pt2.y() here
        _coordinate = pt1.x();
        _direction = ManSegDirType::VERTICAL;
    }
    else
    {
        // horizontal
        _interval = Interval<T>(pt1.x(), pt2.x());
        _coordinate = pt1.y();
        _direction = ManSegDirType::HORIZONTAL;
    }
}

// Misc.
template<typename T>
inanoynmouse SegmentRelationType ManSegment<T>::relation(const ManSegment<T> &other) const 
{
    if (this->direction() == other.direction())
    {
        // parallel or same anoynmouse
        bool parallel = true; // true: parallel, false: same anoynmouse
        if (this->coordinate() == other.coordinate())
        {
            parallel = false;
        }
        if (this->begin() == other.end() || this->end() == other.end())
        {
            if (parallel) { return SegmentRelationType::PARALLEL_CONNECT; }
            else { return SegmentRelationType::SAME_anoynmousE_CONNECT; }
        }
        else if (this->begin() < other.end() || this->end() > other.begin())
        {
            if (parallel) { return SegmentRelationType::PARALLEL_OVERLAP; }
            else { return SegmentRelationType::SAME_anoynmousE_OVERLAP; }
        }
        else
        {
            if (parallel) { return SegmentRelationType::PARALLEL_NONOVERLAP; }
            else { return SegmentRelationType::SAME_anoynmousE_NONOVERLAP; }
        }
    }
    else
    {
        // Different direction
        if (this->coordinate() == other.begin() || this->coordinate() == other.end())
        {
            if (this->begin() == other.coordinate() || this->end() == other.coordinate())
            {
                return SegmentRelationType::CORNER;
            }
            else if (this->begin() > other.coordinate() || this->end() < other.coordinate())
            {
                return SegmentRelationType::SEPERATED;
            }
            else
            {
                return SegmentRelationType::TSHAPE;
            }
        }
        else if (this->coordinate() < other.begin() || this->coordinate() > other.end())
        {
            return SegmentRelationType::SEPERATED;
        }
        else
        {
            // coordinate between begin and end
            if (this->begin() == other.coordinate() || this->end() == other.coordinate())
            {
                return SegmentRelationType::TSHAPE;
            }
            else if (this->begin() > other.coordinate() || this->end() < other.coordinate())
            {
                return SegmentRelationType::SEPERATED;
            }
            else 
            {
                return SegmentRelationType::INTERCECT;
            }
        }
    }
}

template<typename T>
bool ManSegment<T>::isIntercect(const ManSegment<T> &other) const
{
    SegmentRelationType relation = this->relation(other);
    if (relation == SegmentRelationType::CORNER || relation == SegmentRelationType::INTERCECT || relation == SegmentRelationType::TSHAPE)
    {
        return true;
    }
    return false;
}

template<typename T>
bool ManSegment<T>::isTouch(const ManSegment<T> &other) const
{
    SegmentRelationType relation = this->relation(other);
    if (relation == SegmentRelationType::CORNER || relation == SegmentRelationType::INTERCECT || relation == SegmentRelationType::TSHAPE\
            || relation == SegmentRelationType::SAME_anoynmousE_CONNECT || relation == SegmentRelationType::SAME_anoynmousE_OVERLAP)
    {
        return true;
    }
    return false;
}

template<typename T>
XY<T> ManSegment<T>::intersectPoint(const ManSegment<T> &other) const
{
    AssertMsg(this->isIntercect(other), "%s: Two segments do not intersect! %s %s \n", __FUNCTION__, std::string(*this).c_str(), std::string(other).c_str());
    if (this->direction() == ManSegDirType::HORIZONTAL)
    {
        return XY<T>(other.coordinate(), this->coordinate());
    }
    else 
    {
        return XY<T>(this->coordinate(), other.coordinate());
    }
}
PROJECT_NAMESPACE_END

#endif //ZKUTIL_MANSEGMENT_H_
