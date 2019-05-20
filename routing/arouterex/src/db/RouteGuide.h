/**
 * @file RouteGuide.h
 * @brief The routing guide read
 * @author Keren Zhu
 * @date 02/11/2019
 */

#ifndef AROUTER_ROUTE_GUIDE_H_
#define AROUTER_ROUTE_GUIDE_H_

#include <cmath> // std::pow
#include "global/global.h"
#include "util/Vector2D.h"

PROJECT_NAMESPACE_BEGIN

class Database;

/// @class PROJECT_NAMESPACE::RouteGuide
/// @brief routing guide for nets
class RouteGuide
{
    public:
        explicit RouteGuide() = default;
        explicit RouteGuide(IndexType numX, IndexType numY, IntType maxRange, IndexType numNets)
            : _numX(numX), _numY(numY), _maxRange(maxRange)
        {
            _guides.resize(numNets);
            _hasGuide.resize(numNets, false);
            for (auto &map : _guides)
            {
                map.resize(numX, numY, 0);
            }
            _avgProb.resize(numX, numY);
        }
        /*------------------------------*/ 
        /* Setter                       */
        /*------------------------------*/ 
        /// @brief a guide value
        /// @param first: the routing probability of a net at a grid cell
        /// @param second: net index
        /// @param thrid: x-th grid cell
        /// @param fourth: y-th grid cell
        void setGuide(IntType value, IndexType netIdx, IndexType x, IndexType y) { _guides.at(netIdx).at(x, y) = value; }
        /// @brief set whether the guide mode is enabled
        void setEnable(bool enable) { _enable = enable; }
        /// @brief set whether a net has guide
        /// @param first: the index of the net
        /// @param second: whether the net has guide
        void setNetHasGuide(IndexType netIdx, bool hasGuide) { _hasGuide.at(netIdx) = hasGuide; }
        /*------------------------------*/ 
        /* Getter                       */
        /*------------------------------*/ 
        /// @brief get whether the net has guide
        /// @param the idnex of the net
        /// @return whether the net has guide
        bool hasGuide(IndexType netIdx) const { return _hasGuide.at(netIdx); }
        /// @brief get the raw guidance probability
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return raw int probability
        IntType probability(IndexType netIdx, IndexType x, IndexType y) const { return _guides.at(netIdx).at(x, y); }
        /// @brief get the float guidance probability
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return probability as float of 0-1.0
        RealType probabilityFloat(IndexType netIdx, IndexType x, IndexType y) const { return static_cast<RealType>(_guides.at(netIdx).at(x, y)) / static_cast<RealType>(_maxRange); }
        /// @brief get the cost 
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return the cost as inverse probability
        IntType cost(IndexType netIdx, IndexType x, IndexType y) const 
        { 
            if (!_enable) { return 0; }
            IntType violateCost = this->violateCost(netIdx, x, y);
            IntType congestCost = this->congestCost(netIdx, x, y);
            //DBG("violate %d , congest %d \n", violateCost, congestCost);
            return violateCost + congestCost;
            //return ( _maxRange - _guides.at(netIdx).at(x, y) ) * GUIDE_COST_MULTI  ;
            //+ std::max((_avgProb.at(x, y) - _guides.at(netIdx).at(x, y) ), 0) * GUIDE_COST_MULTI; 
        }
        /// @brief just the guidance following, can be negative
        IntType guideConfi(IndexType netIdx, IndexType x, IndexType y) const;
        /// @brief whether the guide is condfident that the net should routed in some grid
        /// @param first: index of the net
        /// @param second: index of the grid
        bool confident(IndexType netIdx, XY<IndexType> grid) const { return _guides.at(netIdx).at(grid) > _maxRange / 5; }
        /// @brief whether this object has been initialized
        /// @return whether this object has been initialized
        bool isInit() const { return _guides.size() != 0; }
        /// @brief get the num of x cells
        /// @return number of x cells
        IndexType numX() const { return _numX; }
        /// @brief get the num of y cells
        /// @return number of y cells
        IndexType numY() const { return _numY; }
        /// @brief get the max probability value range
        /// @return the range of probability value
        IntType maxRange() const { return _maxRange; }
        /// @brief init average probability
        void initAverageProb(Database &db);
#ifdef DEBUG_INPUT
        /// @brief draw a guide of one net
        /// @param first: net index
        /// @param second: save to figure name
        void drawRouteGuide(IndexType netIdx, const std::string &filename) const;
#endif
    private:
        /// @brief the cost for violating the guidance
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return the cost of violating the guidance
        IntType violateCost(IndexType netIdx, IndexType x, IndexType y) const;
        /// @brief the cost for taking the usage of other nets' guidance
        /// @param first: net index
        /// @param second: x-th grid cell
        /// @param third: y-th grid cell
        /// @return the cost of taking the usage of other nets' guidance
        IntType congestCost(IndexType netIdx, IndexType x, IndexType y) const;
        
    private:
        std::vector<Vector2D<IntType>> _guides; ///< Guide for nets, saved as probability
        IndexType _numX = INDEX_TYPE_MAX; ///< Number of x grids in guide
        IndexType _numY = INDEX_TYPE_MAX; ///< number of y grids in guide
        IntType _maxRange = INT_TYPE_MAX; ///< Range of probability
        Vector2D<IntType> _avgProb; ///< Average _guide value
        bool _enable = true; ///< Whether the route guide is enabled
        std::vector<bool> _hasGuide; /// Whether each net has guide
};

inline IntType RouteGuide::guideConfi(IndexType netIdx, IndexType x, IndexType y) const
{
    if (!_enable)
    {
        return 0;
    }
    /// Sigmoid 99.97226 +  (9.870109 - 99.97226) / (1 + (x / 0.385189) ^ 4.73)
    /// Ratio 0 - 100 99.97226 + (9.870109 - 99.97226) / (1 + (x / 38.5189)^4.73)
    if (!_hasGuide.at(netIdx))
    {
        return 0;
    }
    IntType ratio = _guides.at(netIdx).at(x, y) * 100 / _maxRange;
    return 99.76226 - 90 / ( 1 + std::pow((ratio / 38.5198), 4.73));
}

inline IntType RouteGuide::violateCost(IndexType netIdx, IndexType x, IndexType y) const 
{
    /// Ratio 0-100. -100 + 718.2 / 2 ^ (ratio/ 37)
    if (!_hasGuide.at(netIdx))
    {
        return 0;
    }
    IntType ratio = _guides.at(netIdx).at(x, y) * 100 / _maxRange;
    return -100 + 718.2 / std::pow(2, (ratio / 37));
}

inline IntType RouteGuide::congestCost(IndexType netIdx, IndexType x, IndexType y) const 
{
    if (_hasGuide.at(netIdx))
    {
        return std::max((_avgProb.at(x, y) - _guides.at(netIdx).at(x, y)), 0) * 3 / _maxRange ;
    }
    else
    {
        return (_avgProb.at(x, y) ) * 100 / _maxRange;
    }
}

PROJECT_NAMESPACE_END

#endif //AROUTER_ROUTE_GUIDE_H_
