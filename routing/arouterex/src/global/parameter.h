//
// Created by anoynmous on 05/13/2018
//

#ifndef AROUTER_PARAMETER_H_
#define AROUTER_PARAMETER_H_

#include "type.h"

PROJECT_NAMESPACE_BEGIN

constexpr IntType PIN_NEIGHBOR_OCCUPTY_SIZE = 1; ///< How large should the track around the pin be reserved
constexpr bool READ_GR_BRANCH = true;///< Whether read branch/2-pin-net-spliting from the global router

constexpr IntType RESISTANCE_PRECISION = 1000000; ///< Resistance rouding precision


/*------------------------------------------------*/ 
/* Global routing Setting parameters           */
/*------------------------------------------------*/ 
constexpr RealType GLOBAL_CAPACITY_RATIO = 0.5; ///< Global routing capacity ratio
constexpr IntType GLOBAL_GRID_MAX_NUM = 200; ///< Maximum global routing gird number in one dimension
constexpr IntType GLOBAL_GRID_DEFAULT_TRACK = 5; ///< Default grid width/height in number of tracks at M1
constexpr bool OVERRIDE_GLOBAL_ROUTING_SETTING = true; ///< Whether override global routing settings

constexpr bool SKIP_GLOBAL_ROUTING = true; ///< Whether skip global routing

/*------------------------------------------------*/ 
/* Global routing negotation parameters           */
/*------------------------------------------------*/ 
constexpr IntType GLOBAL_ROUTING_SEARCH_RANGE = 30; ///< Search range for global routing
constexpr IntType VIA_DISTANCE = 1; ///< Default multiplier for via cost: VIA_DISTANCE * TILE_WIDTH = via distance
constexpr IntType DR_VIA_DISTANCE = VIA_DISTANCE *20;
constexpr CostType WRONG_WAY_PENALTY = 100; ///< Default penalty for wrong way routing
constexpr CostType GLOBAL_ROUTING_SOLUTION_VIOLATION_PENALTY = 500; ///< Default global routing solution violation penalty
constexpr IntType GRID_CELL_HIS_COST_PIN = 20; ///< Initial grid cell history cost multiplier to pin count
constexpr IntType HIS_COST_INCREMENT = 10; ///< ripup history cost 

/*------------------------------------------------*/ 
/* Detailed routing negotation parameters           */
/*------------------------------------------------*/ 

constexpr IntType DETAILED_ASTAR_SEARCH_RANGE_EXTENSION = 2000; ///< How large the search range of A* should be extended over bbox
PROJECT_NAMESPACE_END
#endif ///AROUTER_PARAMETER_H_
