/*
 * @file DesignDB.cpp
 * @author Keren Zhu
 * @date 06/26/2019
 */

#include "db/DesignDB.h"
#include <unordered_map>

PROJECT_NAMESPACE_BEGIN


bool DesignDB::findRootCkt()
{
    if (this->numCkts() == 0) { return true; }

    std::stack<IndexType> sortStack; //< Store the sorted indices here
    std::vector<IntType> visited; //< Mark whether one sub circuit has been visited
    std::stack<IndexType> stack; //< for DFS

    // Initialize the variables
    visited.resize(this->numCkts(), 0);

    // DFS
    for (IndexType startIdx = 0;  startIdx < this->numCkts(); ++startIdx)
    {
        if (visited[startIdx] == 1)
        {
            continue;
        }
        stack.push(startIdx);
        std::stack<IndexType> innerStack; //< Store the sorting of the current DFS
        while (!stack.empty())
        {
            auto nodeIdx = stack.top();
            stack.pop();
            visited.at(nodeIdx) = 1;
            for (const auto & childNode : _ckts.at(nodeIdx).nodeArray())
            {
                if (childNode.isLeaf())
                {
                    continue;
                }
                IndexType graphIdx = childNode.subgraphIdx();
                if (visited[graphIdx] == 1)
                {
                    continue;
                }
                stack.push(graphIdx);
            }
            innerStack.push(nodeIdx); // Add to the vector (used as stack)
        }
        // Add the DFS result to the overall order
        while (!innerStack.empty())
        {
            sortStack.push(innerStack.top());
            innerStack.pop();
        }
    }

    _rootCkt = sortStack.top();
    return true;
}

PROJECT_NAMESPACE_END
