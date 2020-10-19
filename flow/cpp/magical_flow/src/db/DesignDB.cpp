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
    if (_rootCkt != INDEX_TYPE_MAX) 
    {
        Assert(false);
        return false;
    }
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

IndexType DesignDB::decoupleSubCkt(IndexType cktIdx, IndexType nodeIdx)
{
    auto &node = this->subCkt(cktIdx).node(nodeIdx);
    IndexType subCktIdx = node.subgraphIdx();
    Assert(_ckts.at(subCktIdx).implType() == ImplType::UNSET);
    IndexType newCktIdx = _ckts.size();
    _ckts.emplace_back(CktGraph());
    _ckts.back() = _ckts.at(subCktIdx);
    auto cktName = _ckts.back().name();
    _ckts.back().setName(_ckts.back().name() + "_" + std::to_string(cktIdx) + "_" + std::to_string(nodeIdx));
    _ckts.back().setRefName(cktName); // Keep reference name unchanged
    return newCktIdx;
}

PROJECT_NAMESPACE_END
