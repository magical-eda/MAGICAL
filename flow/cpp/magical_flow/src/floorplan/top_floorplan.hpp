/**
 * @file top_floorplan.h
 * @brief The floorplan for top-level system integration
 * @author Keren Zhu
 * @date 10/05/2020
 */

#pragma once

#include "db/DesignDB.h"
#include "util/linear_programming.h"
#include "util/Box.h"
#include "constraint_graph.hpp"

PROJECT_NAMESPACE_BEGIN

/// @brief the top-level problem
class TopFloorplanProblem
{
    friend class IlpTopFloorplanProblem;
    public:
        explicit TopFloorplanProblem() = default;
        /// @brief intialize a problem from CktGraph
        void initProblem(const DesignDB& dDb, const CktGraph &ckt, const std::string& symnetFile);
        /// @brief set resource per length
        void setPinResourceLen(IntType pinResourceLen) { _resourcePerLen = pinResourceLen; }
    private:
        enum class FpPinType
        {
            ASYM = 0,
            SYM_PRI = 1, // The primary one in the sym pair
            SYM_SCE = 2, // The secondary one in the sym pair
            OTHER // Don't need to care
        };
        enum class FpPinAssignType
        {
            LEFT = 0,
            RIGHT = 1,
            UNDEFINED = 2
        };
        struct PinIdx
        {
            FpPinType pinType; 
            IndexType idx;
            IndexType cellIdx;
            std::string name = "";
            FpPinAssignType assign = FpPinAssignType::UNDEFINED;
        };
        struct FpNet
        {
            std::vector<IndexType> pins;
        };
        /* Problem   */
        std::vector<PinIdx> _pinIdx; ///< To relate the pin in CktGraph to the variables
        std::vector<Box<LocType>> _cellBBox; ///< The original cell layout bounding boxes
        IntType _resourcePerLen = 10000; ///< To quanitfy the pin resource per length
        std::vector<FpNet> _nets; ///< The nets
        IndexType _numAsymPins = 0; ///< The number of asym pins need to assign
        IndexType _numSymPriPins = 0; ///< The number of primary sym pins need to assign
        IndexType _numSymSecPins = 0; ///< The number of primary sym pins need to assign

};

/// @brief the ilp for solving the TopFloorplanProblem
class IlpTopFloorplanProblem
{
    typedef ::klib::lp::LpModel lp_solver_type;
    typedef ::klib::lp::LpTrait lp_trait;
    typedef lp_trait::variable_type lp_variable_type;
    typedef lp_trait::expr_type lp_expr_type;
    public:
        explicit IlpTopFloorplanProblem(const TopFloorplanProblem &problem)
            : _problem(problem) {}
        /// @brief solve the problem
        /// @return true: successful.
        bool solve();
        /* Write out the result */
        void writeOut();
    private:
        /// @brief sweep line to generate the vertical constraint graph
        void verticalSweepLine();
        /* ILP Variable */
        void addVariables();
        /// @brief get the crossing variable
        /// @param first: index of the net
        /// @param second: index of the first pin in _problem.nets.pins
        /// @param third: index of the second pin in _problem.nets.pins
        const lp_variable_type & crossVariable(IndexType netIdx, IndexType firstPinIdx, IndexType secondPinIdx) const
        {
            // Calculate the index of the pair
            // Assume the order is (0,1) (0,2) (1,2)
            if (firstPinIdx > secondPinIdx)
            {
                std::swap(firstPinIdx, secondPinIdx);
            }
            auto numPins = _problem._nets.at(netIdx).pins.size();
            IndexType offset = 0;
            for (IndexType i = 0; i < firstPinIdx; ++i)
            {
                offset += (numPins - i - 1);
            }
            IndexType idx = offset + (secondPinIdx - firstPinIdx - 1);
            return _crossVars.at(netIdx).at(idx);
        }
        /* ILP Constraints */
        void addConstr();
        void addYLoConstr();
        void addPinResrouceConstr();
        void addCrossConstr();
        void addBoundaryConstr();
        /* ILP Objective Function */
        void configObjFunc();
        /* ILP solve */
        bool solveIlp();
        /* For debugging */
        void printVariableValue();
        
    private:
        std::vector<constraint_graph::ConstraintEdge> _verConstrGraph; ///< The vertical constraint graph
        const TopFloorplanProblem &_problem;
        lp_solver_type _solver; ///< ILP model
        lp_expr_type _obj;  ///< ILP objective function
        std::vector<lp_variable_type> _symPinAssignVars; ///< The binary variables to represent how to assign the sym pins
        std::vector<lp_variable_type> _aSymAssignVars; ///< The binary variables to represent how to assign the asym pins
        std::vector<lp_variable_type> _extraResourcesVars; ///< The variables to represent the extra resources of each module
        std::vector<lp_variable_type> _yLoVars; ///< The lower y variables for each module
        std::vector<std::vector<lp_variable_type>> _crossVars; ///< The variables to represent whether there is a crossing between pair of pins
        lp_variable_type _yHiVar; ///< The variable representing the upper boundary of the floorplan
};

PROJECT_NAMESPACE_END
