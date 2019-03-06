#ifndef _DISJOINTSET_H_
#define _DISJOINTSET_H_

#include <algorithm>
#include <vector>

struct disjointSet
{
    disjointSet(int nodes) { 
        m_rank.clear();
        for (int i=0; i < nodes; i++)
            m_rank.push_back(0);

        m_parent.clear();
        for (int i=0; i < nodes; i++)
            m_parent.push_back(i);
    }   
    ~disjointSet() { m_parent.clear();  m_rank.clear();}

    int find (int x)
    {
        if (m_parent[x] == x)   return x;
        else
        {
            m_parent[x] = find (m_parent[x]);
            return m_parent[x];
        }
    }

    void unionSet (int x, int y)
    {
        if (m_parent[x] == m_parent[y]) return;
        int fx = find (x);
        int fy = find (y);
        
        // union by rank
        if (m_rank[fx] < m_rank[fy])
        {
            m_parent[fx] = fy;
        }
        else
        {
            m_parent[fy] = fx;
            if (m_rank[fx] == m_rank[fy])
                m_rank[fx] ++;
        }

    }

    std::vector<int>    m_parent;
    std::vector<int>    m_rank;
};

#endif
