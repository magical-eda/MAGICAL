/*! @file db/Bias.h
    @brief A vector of Mosfet Bias.
    @author Mingjie Liu
    @date 12/11/2018
*/ 
#ifndef __BIAS_H__
#define __BIAS_H__

#include "global/type.h"
#include "db/Netlist.h"
#include "db/MosPair.h"

PROJECT_NAMESPACE_BEGIN
/*! @class Bias
    @brief A vector of Mosfet.

    This class stores a group of Mosfet Id 
    that are bias circuits.
*/
class Bias
{
public:
/*! @brief Default Constructor. */
    explicit Bias() = default;
/*! @brief Constructor for Bias

    Sequence of Ids does not matter.
    pattern is set according to input.

    @param netId Gate netId.
    @param netlist Netlist class object.
*/
    explicit Bias(IndexType netId, const Netlist & netlist)
        :_netId(netId), _netlist(netlist)
    {
        init();
    }

    // Getters
/*! @brief Get entire bias group. */
    const std::vector<IndexType> &  bias() const                    { return _bias; }
/*! @brief Get the driver group. */
    const std::vector<IndexType> &  driver() const                  { return _driver; }

    bool                            valid() const                   { return _driver.size() > 0; }
    void                            init();

private:
    IndexType                       _netId;
    const Netlist &                 _netlist;
    std::vector<IndexType>          _bias;
    std::vector<IndexType>          _driver;
};

PROJECT_NAMESPACE_END

#endif
