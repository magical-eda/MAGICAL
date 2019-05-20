#pragma once

#include <limbo/thirdparty/dlx/include/dlx/ExactCoverProblem.hpp>
#include <vector>

class LinkedMatrix {
public:
  typedef unsigned NodeId; 

  explicit LinkedMatrix(const ExactCoverProblem& problem);

  void cover_column(NodeId id);
  void uncover_column(NodeId id);

  unsigned width() const;
  NodeId root_id() const;

  unsigned X(NodeId id) const;
  unsigned Y(NodeId id) const;
  unsigned S(NodeId id) const;
  NodeId C(NodeId id) const;
  NodeId L(NodeId id) const;
  NodeId R(NodeId id) const;
  NodeId U(NodeId id) const;
  NodeId D(NodeId id) const;

private:
  void add_row(unsigned y, const std::vector<unsigned>& xs);

  NodeId create_node(unsigned x, unsigned y);

  struct Node;
  std::vector<NodeId> col_ids_;
  std::vector<unsigned> sizes_;
  std::vector<Node> nodes_;
};

struct LinkedMatrix::Node
{
  NodeId id;
  unsigned x, y;
  NodeId l, r, u, d;
  explicit Node(NodeId id_, unsigned x_, unsigned y_)
    : id(id_), x(x_), y(y_),
    l(id), r(id), u(id), d(id)
  {
  }
};

inline unsigned LinkedMatrix::width() const { return col_ids_.size(); }
inline LinkedMatrix::NodeId LinkedMatrix::root_id() const{ return 0; }
inline unsigned LinkedMatrix::X(NodeId id) const { return nodes_[id].x; }
inline unsigned LinkedMatrix::Y(NodeId id) const { return nodes_[id].y; }
inline unsigned LinkedMatrix::S(NodeId id) const { return sizes_[X(id)]; }
inline LinkedMatrix::NodeId LinkedMatrix::C(NodeId id) const { return col_ids_[X(id)]; }
inline LinkedMatrix::NodeId LinkedMatrix::L(NodeId id) const { return nodes_[id].l; }
inline LinkedMatrix::NodeId LinkedMatrix::R(NodeId id) const { return nodes_[id].r; }
inline LinkedMatrix::NodeId LinkedMatrix::U(NodeId id) const { return nodes_[id].u; }
inline LinkedMatrix::NodeId LinkedMatrix::D(NodeId id) const { return nodes_[id].d; }
