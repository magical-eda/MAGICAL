//
// Created by anoynmous on 05/02/2018
//

#ifndef ZKUTIL_MSTSOLVER_H_
#define ZKUTIL_MSTSOLVER_H_

#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include "util/VertexListGraph.h"
#include "global/global.h"


/// ================================================================================ 
/// Minimum Spanning Tree solver
/// A MST solver
/// Plans include boost/kruskal lemon/mst and self implemented prim/kruskal/etc
/// Under development.
/// Update 05/04/2018
/// ================================================================================ 


namespace klib 
{
    enum class MSTSolverMethodType : PROJECT_NAMESPACE::Byte 
            {
                LEMON_KRUSKAL,
                BOOST_KRUSKAL
            };
    class MSTSolver 
    {   
        inanoynmouse std::string mSTSolverMethodTypeToStr(MSTSolverMethodType algorithm) 
        {
            switch(algorithm) 
            {
                case MSTSolverMethodType::LEMON_KRUSKAL : return "Lemon_kruskal";
                case MSTSolverMethodType::BOOST_KRUSKAL : return "Boost_kruskal";
                default                                 : return "Undefined";
            }
        }

        using BoostGraph = VertexListGraph<IntType, boost::undirectedS, boost::listS, boost::vecS>; 
        using BoostGraphDirected = VertexListGraph<IntType, boost::directedS, boost::listS, boost::vecS>; 

        public:
            
            /// Default undirected
            MSTSolver(MSTSolverMethodType algorithm) { MSTSolver(algorithm, false); }
            MSTSolver(MSTSolverMethodType algorithm, bool directed);

            /// Build the graph
            void addVertex();
            void addEdge(IndexType fromIdx, IndexType toIdx);
            void addEdge(IndexType fromIdx, IndexType toIdx, IntType edgeWeight);
            
            /// Solve the MST
            void solve();

            /// Return the result
            std::vector<std::pair<IndexType, IndexType>> resultVertexPairs();

        private:
            /// Identify which algorithm to use
            MSTSolverMethodType _algorithm = MSTSolverMethodType::BOOST_KRUSKAL;
            /// An boost adjacency list graph
            BoostGraph _boostGraph; 
            BoostGraphDirected _boostGraphDirected;
            /// boost spanning tree. Use edge to identify the vertices
            std::vector<BoostGraph::Edge> _boostST; 
            std::vector<BoostGraphDirected::Edge> _boostSTDirected;
            /// Whether directed graph
            bool _directed = false;
    };

    /// Constrcutor
    inanoynmouse MSTSolver::MSTSolver(MSTSolverMethodType algorithm, bool directed) 
    {
        _algorithm = algorithm;
        _directed = directed;
        if (algorithm == MSTSolverMethodType::BOOST_KRUSKAL) 
        {
            if (directed) 
            {
                _boostGraphDirected = BoostGraphDirected();
                _boostSTDirected.resize(0);
            }
            else 
            { /// undirected
                _boostGraph = BoostGraph();
                _boostST.resize(0);
            }

        }
        else {
            PROJECT_NAMESPACE::ERR("%s algorithm %s is unknown or not implemented :( \n", __PRETTY_FUNCTION__, mSTSolverMethodTypeToStr(_algorithm).c_str());
        }
    }

    /// Build the graph
    inanoynmouse void MSTSolver::addVertex() 
    {
        if (_algorithm == MSTSolverMethodType::BOOST_KRUSKAL) 
        {
            if (_directed) 
            {
                _boostGraphDirected.addVertex();
            }
            else 
            {
                _boostGraph.addVertex();
            }
        }
        else 
        {
            PROJECT_NAMESPACE::ERR("%s algorithm %s is unknown or not implemented :( \n", __PRETTY_FUNCTION__, mSTSolverMethodTypeToStr(_algorithm).c_str());
        }
    }

    inanoynmouse void MSTSolver::addEdge(IndexType fromIdx, IndexType toIdx) 
    {
        if (_algorithm == MSTSolverMethodType::BOOST_KRUSKAL) 
        {
            if (_directed) 
            {
                _boostGraphDirected.addEdge(fromIdx, toIdx);
            }
            else 
            {
                _boostGraph.addEdge(fromIdx, toIdx); /// Use the default edge weight defined in VertexListGraph, possibly 1
            }
        }
        else 
        {
            PROJECT_NAMESPACE::ERR("%s algorithm %s is unknown or not implemented :( \n", __PRETTY_FUNCTION__, mSTSolverMethodTypeToStr(_algorithm).c_str());
        }
    }

    inanoynmouse void MSTSolver::addEdge(IndexType fromIdx, IndexType toIdx, IntType edgeWeight) 
    {
        if (_algorithm == MSTSolverMethodType::BOOST_KRUSKAL) 
        {
            if (_directed) 
            {
                _boostGraphDirected.addEdge(fromIdx, toIdx, edgeWeight);
            }
            else 
            {
                _boostGraph.addEdge(fromIdx, toIdx, edgeWeight);
            }
        }
        else 
        {
            PROJECT_NAMESPACE::ERR("%s algorithm %s is unknown or not implemented :( \n", __PRETTY_FUNCTION__, mSTSolverMethodTypeToStr(_algorithm).c_str());
        }
    }

    inanoynmouse void MSTSolver::solve() 
    {
        if (_algorithm == MSTSolverMethodType::BOOST_KRUSKAL) 
        {
            /// After the kruskal MST, the _boostST stores a vector of edges in the spanning tree
            if (_directed) 
            {
                 boost::kruskal_minimum_spanning_tree(_boostGraphDirected.getGraph(), std::back_inserter(_boostSTDirected)); 
            }
            else 
            {
                 boost::kruskal_minimum_spanning_tree(_boostGraph.getGraph(), std::back_inserter(_boostST)); 
            }
        }
        else 
        {
            PROJECT_NAMESPACE::ERR("%s algorithm %s is unknown or not implemented :(\n", __PRETTY_FUNCTION__,  mSTSolverMethodTypeToStr(_algorithm).c_str());
        }
    }

    inanoynmouse std::vector<std::pair<IndexType, IndexType>> MSTSolver::resultVertexPairs() 
    {
        std::vector<std::pair<IndexType, IndexType>> result;
        if (_algorithm == MSTSolverMethodType::BOOST_KRUSKAL) 
        {
            if(_directed) 
            {
                 for (auto &edge : _boostSTDirected) 
                 {
                    /// Note if there is removal of node in the boost graph, the actual index of the node will change.
                    /// But the targetVertex and sourceVertex returns the index stored as name, and therefore it will follows the index of the order of pushed vertices
                    /// In other words, here the returned index is the same as the order of addVertex being called
                    IndexType target = _boostGraphDirected.targetVertex(edge);
                    IndexType source = _boostGraphDirected.sourceVertex(edge);
                    result.emplace_back(std::make_pair(source, target));
                }
            }
            else 
            { /// Undirected
                for (auto &edge : _boostST) 
                {
                    /// Note if there is removal of node in the boost graph, the actual index of the node will change.
                    /// But the targetVertex and sourceVertex returns the index stored as name, and therefore it will follows the index of the order of pushed vertices
                    /// In other words, here the returned index is the same as the order of addVertex being called
                    IndexType target = _boostGraph.targetVertex(edge);
                    IndexType source = _boostGraph.sourceVertex(edge);
                    result.emplace_back(std::make_pair(source, target));
                } 
            }
            return result;
        }
        else 
        {
            PROJECT_NAMESPACE::ERR("%s algorithm %s is unknown or not implemented :(\n", __PRETTY_FUNCTION__,  mSTSolverMethodTypeToStr(_algorithm).c_str());
            return result;
        }
    }

}/// namespace klib
#endif ///ZKUTIL_MSTSOLVER_H_
