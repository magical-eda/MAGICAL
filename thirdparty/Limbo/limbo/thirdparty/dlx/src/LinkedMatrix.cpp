#include <limbo/thirdparty/dlx/include/dlx/LinkedMatrix.hpp>

#include <assert.h>
#include <algorithm>

LinkedMatrix::LinkedMatrix(const ExactCoverProblem& problem)
  : col_ids_(problem.width()), sizes_(problem.width())
{
  NodeId root = create_node(~0, ~0);
  assert(root == root_id());

  for (unsigned x = 0u; x < col_ids_.size(); ++x) {
    NodeId id = create_node(x, ~0);
    col_ids_[x] = id;
    if (x >= problem.secondary_columns()) {
      nodes_[id].r = root;
      nodes_[id].l = L(root);
      nodes_[L(root)].r = id;
      nodes_[root].l = id;
    }
  }

  for (unsigned y = 0u; y < problem.rows().size(); ++y) {
    add_row(y, problem.rows()[y]);
  }
}

void LinkedMatrix::add_row(unsigned y, const std::vector<unsigned>& xs) {
  NodeId first_id = 0;
  for (std::vector<unsigned>::const_iterator it = xs.begin(), ite = xs.end(); it != ite; ++it) {
    unsigned x = *it; 
    NodeId id = create_node(x, y);
    nodes_[id].d = C(id);
    nodes_[id].u = U(C(id));
    nodes_[U(C(id))].d = id;
    nodes_[C(id)].u = id;
    ++sizes_[x];
    if (first_id == 0) {
      first_id = id;
    }
    else {
      nodes_[id].r = first_id;
      nodes_[id].l = L(first_id);
      nodes_[L(first_id)].r = id;
      nodes_[first_id].l = id;
    }
  }
}

void LinkedMatrix::cover_column(NodeId c) {
  c = C(c);
  nodes_[L(c)].r = R(c);
  nodes_[R(c)].l = L(c);
  for (NodeId i = D(c); i != c; i = D(i)) {
    for (NodeId j = R(i); j != i; j = R(j)) {
      nodes_[U(j)].d = D(j);
      nodes_[D(j)].u = U(j);
      --sizes_[X(j)];
    }
  }
}

void LinkedMatrix::uncover_column(NodeId c) {
  c = C(c);
  for (NodeId i = U(c); i != c; i = U(i)) {
    for (NodeId j = L(i); j != i; j = L(j)) {
      nodes_[U(j)].d = j;
      nodes_[D(j)].u = j;
      ++sizes_[X(j)];
    }
  }
  nodes_[L(c)].r = c;
  nodes_[R(c)].l = c;
}

LinkedMatrix::NodeId LinkedMatrix::create_node(unsigned x, unsigned y) {
  assert(x <= width() || x == ~0u);
  unsigned id = nodes_.size();
  nodes_.push_back(Node(id, x, y));
  return id;
}
