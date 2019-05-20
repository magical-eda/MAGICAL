//
// Created by Keren on 04/27/2018
//

#ifndef AROUTER_DEFINE_H_
#define AROUTER_DEFINE_H_

//#define NODEBUG

//#define AROUTER_GLOBAL_ROUTING_EXTRACTION /// use the extraction main
//#define AROUTER_GLOBAL_ROUTING_ONLY /// only the global routing
//#define AROUTER_GLOBAL_DETAILED_ROUTING /// both global and detailed routing
//#define AROUTER_LEF_PRINT /// Only print the layer information from the technology LEF
//#define AROUTER_DRAFT_TEST /// Testing modules

//#define _USE_LAWS                 // Layer Assignment for Weighted Sum Delay
//#define _USE_LAMP               // Layer Assignment for Maximum Path Delay


//#define _USE_GUROBI               // whether use GUROBI to solve ILP/LP
//#define _USE_CPLEX              // whether use CPLEX to solve ILP/LP
//#define _USE_CBC                // whether use CBC to solve ILP/LP
//#define _USE_LEMON              // whether use lemon to solve min-cost flow
//#define _USE_BOOST

//#define DEBUG_GR_  /// if debugging the global routing
//#define DEBUG_INPUT /// if debug input parser
//#define DEBUG_GR2DR /// if debug parsering global routing to detailed routing
//#define DEBUG_ASTAR /// if debug astar
//#define DEBUG_SEARCH_GRAPH_ /// if debug search graph construction
//#define DEBUG_DR_ /// if is debugging detailed routing
//#define DEBUG_GDS_OUTPUT // If output gds for debugging
//#define DEBUG_SHAPE_MAP /// If debugging the shape map
//#define DEBUG_DRAW_PIN /// if draw the pins in the layout

//#define TILE_MIDDLE_ /// if true,  x - lower_left_x + tile_width/2
#define TRIM_OVERLAP_PIN /// remove overlapped pins
#define USE_XYZ_NODEIDEN /// use XY instead of flat idx in searchgraph

#endif /// AROUTER_DEFINE_H
