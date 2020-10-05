/**
 * @file top_floorplan.h
 * @brief The floorplan for top-level system integration
 * @author Keren Zhu
 * @date 10/05/2020
 */

#pragma once

#include "db/CktGraph.h"

PROJECT_NAMESPACE_BEGIN

/// @brief the top-level problem
class TopFpProblem
{
    public:
        explicit TopFpProblem() = default;
        /// @brief intialize a problem from CktGraph
        void initProblem(const CktGraph &ckt);
    private:
};

PROJECT_NAMESPACE_END
