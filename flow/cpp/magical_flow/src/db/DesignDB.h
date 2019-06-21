/**
 * @file DesignDB.h
 * @brief Database for the hierarchical design
 * @author Keren Zhu
 * @date 06/17/2019
 */

#ifndef MAGICAL_FLOW_DESIGN_DB_H_
#define MAGICAL_FLOW_DESIGN_DB_H_

#include "GraphComponents.h"
#include "CktGraph.h"

PROJECT_NAMESPACE_BEGIN

/// @class MAGICAL_FLOW::DesignDB
/// @brief the database class for the hierarchical flow
class DesignDB
{
    private:
        std::vector<CktGraph> _cktTrees; ///< The hierarchical tree of the circuits. Each circuit is represented as a graph.
};

PROJECT_NAMESPACE_END

#endif //MAGICAL_FLOW_DESIGN_DB_H_
