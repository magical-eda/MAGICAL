//
// Created by Keren on 05/05/2018
//

#ifndef ZKUTIL_ASTARSOLVER_H_
#define ZKUTIL_ASTARSOLVER_H_

#include <memory>
#include <boost/graph/astar_search.hpp>
#include "util/VertexListGraph.h" 

/// ================================================================================ 
/// A* search solver
/// A wrapper of boost astar
/// The code take references from https://www.boost.org/doc/libs/1_62_0/libs/graph/example/astar-cities.cpp
/// ================================================================================ 

namespace klib 
{
/// Default location type
struct location 
{
    location() = default;
    location(IndexType xx, IndexType yy, IndexType zz) : x(xx), y(yy), z(zz) {}
    location(IndexType xx, IndexType yy) : x(xx), y(yy), z(0) {}
    IndexType x = 0;
    IndexType y = 0;
    IndexType z = 0;
};


/// Location map LUT, used to build a bridge between heursitic and VLGraph
/// Since represente Vertex by vecS, it is possible to look up it by index, otherwise use a hash map
template<typename CostType, typename Directed, typename LocType>
class LocMap 
{
        using VLGraph = VertexListGraph<CostType, Directed, boost::vecS, boost::vecS>;
        typedef typename boost::property<boost::edge_weight_t, CostType>                                                            EdgeWeightProperty;
        typedef typename boost::property<boost::vertex_name_t, IndexType>                                                           VertexIndexNameProperty;
        typedef typename boost::adjacency_list<boost::vecS, boost::vecS , Directed, VertexIndexNameProperty, EdgeWeightProperty>   Graph;
        typedef typename boost::graph_traits<Graph>::vertex_descriptor                                                              Vertex;
    public:
        LocMap(VLGraph &vlGraph) : _vlGraph(vlGraph) {}
        void         addVertex(LocType &loc) { _locationLUT.emplace_back( loc);}
        void         addVertex(LocType loc) { _locationLUT.emplace_back( loc);}
        LocType &    location(Vertex u) { return _locationLUT.at(u); }
        IndexType    size()            { return _locationLUT.size(); }
        void         reset()           { _locationLUT.resize(0); }
    private:
        VLGraph                        &_vlGraph; 
        std::vector<LocType>       _locationLUT;

};

// Manhattan distance heuristic (for default astar heuristic settings)
template <class Graph, class CostType, typename LocType, typename Directed>
class manhattan_heuristic : public boost::astar_heuristic<Graph, CostType> 
{
    public:
      typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
      using LocMap2 =  LocMap<CostType, Directed, LocType>;
      manhattan_heuristic(LocMap2 &l, Vertex goal)
        : _loc(l), _goal(goal) {}
      CostType operator()(Vertex u)
      {
        CostType dx = ::klib::absDif( _loc.location(_goal).x, _loc.location(u).x);
        CostType dy = ::klib::absDif( _loc.location(_goal).y, _loc.location(u).y);
        CostType dz = absDif( _loc.location(_goal).z, _loc.location(u).z);
        return dx + dy + dz;
      }
    private:
      LocMap2 &_loc;
      Vertex _goal;
};

/// Exception for termination
struct foundGoal {};

/// Vistor that terminates when we find the goal
/// Throw a foundGoal struct if the goal is reached
template <typename Vertex>
class astarGoalVistor : public boost::default_astar_visitor 
{
    public:
        astarGoalVistor(Vertex goal) : _goal(goal) {}
        template<typename Graph>
        void examine_vertex(Vertex u, Graph &g) 
        {
            if (u == _goal) {
                throw foundGoal();
            }
        }
    private:
        Vertex _goal;
};

/// TODO: add new visitor to consider change the edge cost
/// https://www.boost.org/doc/libs/1_55_0/libs/graph/doc/AStarVisitor.html
/// Use those functions to modify the cost of the path

/// A* solver class
template<typename CostType,
         typename Directed = boost::undirectedS, 
         typename LocType = location,
         typename Heuristic = manhattan_heuristic<boost::adjacency_list<boost::vecS, boost::vecS, Directed, boost::property<boost::vertex_name_t, IndexType>, boost::property<boost::edge_weight_t, CostType>>, CostType,LocType, Directed>
         >
class AStarSolver 
{
    using VLGraph = VertexListGraph<CostType, Directed, boost::vecS, boost::vecS>;
    using Graph =boost::adjacency_list<boost::vecS, boost::vecS, Directed, boost::property<boost::vertex_name_t, IndexType>, boost::property<boost::edge_weight_t, CostType>>;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor                                                              Vertex;

    public:
        explicit AStarSolver() : _vlGraph(), _loc(_vlGraph) { _loc.reset();}
        explicit AStarSolver(IndexType size) : _vlGraph(size), _loc(_vlGraph) { _loc.reset();}
        void addVertex(IndexType x, IndexType y, IndexType z)    { _vlGraph.addVertex(); _loc.addVertex(LocType(x, y, z)); }
        void addVertex(IndexType x, IndexType y)    { _vlGraph.addVertex(); _loc.addVertex(LocType(x, y )); }
        void addVertex(LocType &loc)                { _vlGraph.addVertex(); _loc.addVertex(loc); }
        void addEdge(IndexType from, IndexType to, CostType cost)      { _vlGraph.addEdge(from, to, cost); }
        void modifyEdgeWeight(IndexType from, IndexType to, CostType newCost) { _vlGraph.modifyEdgeWeight(from, to, newCost); }
        void removeEdge(IndexType from, IndexType to) { _vlGraph.removeEdge(from, to); }
        void setSource(IndexType source) { _source = source; }
        void setGoal(IndexType goal) 
        { 
            _goal = goal; 
        }
        bool solve() 
        {
            Vertex source = _vlGraph.getVertex(_source);
            Vertex goal = _vlGraph.getVertex(_goal);
            Heuristic _heuristic = Heuristic(_loc, goal); 
            std::vector<Vertex> path(_vlGraph.vertexSize());
            std::vector<CostType> distance(_vlGraph.vertexSize());
            try 
            {
                boost::astar_search_tree(
                        _vlGraph.getGraph(),
                        source,
                        _heuristic,
                        boost::predecessor_map(boost::make_iterator_property_map(path.begin(), boost::get(boost::vertex_index, _vlGraph.getGraph()))).
                        distance_map(boost::make_iterator_property_map(distance.begin(), boost::get(boost::vertex_index, _vlGraph.getGraph()))).
                        visitor(astarGoalVistor<Vertex>(goal))
                        );
            } catch (foundGoal fg) {
                _path.resize(0);
                for (Vertex v = goal; v != source ; v =path[v]) 
                {
                    _path.emplace_back(v);
                }
                _path.emplace_back(source);
                return true;
            }
            return false;
        }

        const std::vector<IndexType> & resultPath() const { return _path; }
        std::vector<IndexType> & resultPath() { return _path; }

    private:
        VLGraph _vlGraph; /// contain the basic graph and vertex/edge descriptor
        IndexType _sourceIdx; /// The index of the source vertex
        IndexType _targetIdx; /// The index of the goal/target vertex
        LocMap<CostType, Directed, LocType>    _loc; /// The collection of locations
        //Heuristic _heuristic; /// The heuristic class
        IndexType _source; /// The source of the search
        IndexType _goal; /// The goal of the search
        CostType  _pathCost; /// The cost of the resulting path
        std::vector<IndexType> _path; /// The path of the searching result
};

}/// namespace klib
#endif ///ZKUTIL_ASTARSOLVER_H_
