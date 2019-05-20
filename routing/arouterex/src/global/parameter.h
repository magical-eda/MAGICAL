//
// Created by Keren on 05/13/2018
//

#ifndef AROUTER_PARAMETER_H_
#define AROUTER_PARAMETER_H_

#include "type.h"

PROJECT_NAMESPACE_BEGIN

constexpr IntType PIN_NEIGHBOR_OCCUPTY_SIZE = 1; ///< How large should the track around the pin be reserved
constexpr bool READ_GR_BRANCH = true;///< Whether read branch/2-pin-net-spliting from the global router

constexpr IntType RESISTANCE_PRECISION = 1000000; ///< Resistance rouding precision

constexpr IntType HIGHEST_LAYER = 4;

/*------------------------------------------------*/ 
/* Global routing Setting parameters           */
/*------------------------------------------------*/ 
constexpr RealType GLOBAL_CAPACITY_RATIO = 0.5; ///< Global routing capacity ratio
constexpr IntType GLOBAL_GRID_MAX_NUM = 200; ///< Maximum global routing gird number in one dimension
constexpr IntType GLOBAL_GRID_DEFAULT_TRACK = 4; ///< Default grid width/height in number of tracks at M1
constexpr bool OVERRIDE_GLOBAL_ROUTING_SETTING = true; ///< Whether override global routing settings

constexpr bool SKIP_GLOBAL_ROUTING = false; ///< Whether skip global routing
constexpr bool GR_2D = true; ///< Whether the global routing is 2D model or 3D model

/*------------------------------------------------*/ 
/* Global routing negotation parameters           */
/*------------------------------------------------*/ 
constexpr IntType GLOBAL_ROUTING_SEARCH_RANGE = 30; ///< Search range for global routing
constexpr IntType VIA_DISTANCE = 1; ///< Default multiplier for via cost: VIA_DISTANCE * TILE_WIDTH = via distance
constexpr IntType DR_VIA_DISTANCE = VIA_DISTANCE *2000;
constexpr CostType WRONG_WAY_PENALTY = 100; ///< Default penalty for wrong way routing
constexpr CostType GLOBAL_ROUTING_SOLUTION_VIOLATION_PENALTY = 500; ///< Default global routing solution violation penalty
constexpr IntType GRID_CELL_HIS_COST_PIN = 20; ///< Initial grid cell history cost multiplier to pin count
constexpr IntType HIS_COST_INCREMENT = 100; ///< ripup history cost 
constexpr IntType GUIDE_COST_MULTI = 100; ///< The cost multiplier for guide violation cost
constexpr CostType GR_CELL_PENALTY_COST = 100000; ///< The penalty cost for grid cell (embeded in history cost when doing calculation)

/*------------------------------------------------*/ 
/* Detailed routing negotation parameters           */
/*------------------------------------------------*/ 
constexpr IntType DETAILED_ASTAR_SEARCH_RANGE_EXTENSION = 10000; ///< How large the search range of A* should be extended over bbox
constexpr IntType DETAILED_GUIDE_MULTI = 2000; ///< THe multiplier to guide cost
/*------------------------------------------------*/ 
/* Post processing parameters                     */
/*------------------------------------------------*/ 
constexpr LocType POWER_MESH_NET_NODE_SEARCH_RANGE = 500; ///< The search range for adding net nodes for searching star
constexpr IndexType POWER_MESH_CLOSEST_PAIR_RANGE = 4; ///< The closest distance between two new candidatte terminal pairs (in unit of routeGuide distance)
PROJECT_NAMESPACE_END
#endif ///AROUTER_PARAMETER_H_
