/**
 * @file   disjointSet.hpp
 * @brief  Datastructure - Disjoint set
 * @author Hao Chen
 * @date   09/30/2019
 *
 **/

#ifndef _DS_DISJOINT_SET_HPP_
#define _DS_DISJOINT_SET_HPP_

#include "src/global/global.hpp"

PROJECT_NAMESPACE_START

class DisjointSet {
 public:
  DisjointSet()
    : parent(0), rnk(0), n(0) {}
  DisjointSet(Int_t n) { init(n); }
  ~DisjointSet() {
    delete [] parent;
    delete [] rnk;
  }

  void init(Int_t n) {
    this->n = n;
    parent = new Int_t[n];
    rnk = new Int_t[n];
    for (Int_t i = 0; i < n; ++i) {
      rnk[i] = 0;
      parent[i] = i;
    }
  }
  // Find set
  Int_t find(Int_t u) {
    return (u == parent[u] ? u : parent[u] = find(parent[u]));
  }

  // Union by rank
  void merge(Int_t x, Int_t y) {
    x = find(x), y = find(y);
    if (x == y)
      return;
    if (rnk[x] > rnk[y])
      parent[y] = x;
    else // If rnk[x] <= rnk[y]
      parent[x] = y;
    if (rnk[x] == rnk[y])
      rnk[y]++;
  }

  // Number of disjoInt_t sets
  Int_t nSets() {
    Int_t nSets = 0;
    for (Int_t i = 0; i < n; ++i)
      if (parent[i] == i) ++nSets;
    return nSets;
  }

 private:
  Int_t *parent, *rnk;
  Int_t n;
};

PROJECT_NAMESPACE_END

#endif /// _DS_DISJOINT_SET_HPP_
