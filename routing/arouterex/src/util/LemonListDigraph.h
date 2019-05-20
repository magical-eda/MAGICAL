//
// Created by Keren on 08/20/2018
//

#ifndef ZKUTIL_LEMONLISTDIGRAPH_H_
#define ZKUTIL_LEMONLISTDIGRAPH_H_

// ================================================================================ 
// LemonGraph.h
// 
// Wrapper of the lemon library ListDigraph
// ListDigraph/listGraph/SmartDigraph/SmartGraph can do similar things, but lemon
// did not make them into general boost/std-like library. Therefore it might not
// proper to write them in a general LemonGraph class.
//
// Therefore Here is only ListDigraph
//
// And this is very application-specified, may be only have the simplest functions
//
// ================================================================================  

#include <lemon/list_graph.h>
//#include <lemon/smart_graph.h>
#include "global/global.h"

namespace klib {
    /// @class klib::LemonListDigraph
    /// @breif A wrapper for the lemon library ListDigraph
    class LemonListDigraph
    {
        public:
            /// define types
            typedef typename lemon::ListDigraph Graph;
            typedef typename lemon::ListDigraph::Node Node;
            typedef typename lemon::ListDigraph::Arc Edge;

            explicit LemonListDigraph() = default;

            // Getters
            const Graph & graph() const { return _graph; }
            Graph &       graph()       { return _graph; }

        private:
            Graph   _graph;
    };
} // namespace klib
#endif // ZKUTIL_LEMONLISTDIGRAPH_H_

