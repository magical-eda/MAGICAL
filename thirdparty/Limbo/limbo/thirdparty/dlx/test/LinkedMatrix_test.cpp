#include <dlx/LinkedMatrix.hpp>

#include <gtest/gtest.h>

namespace {

TEST(LinkedMatrix_test, empty_matrix) {
  auto M = LinkedMatrix(ExactCoverProblem());
  EXPECT_EQ(M.root_id(), M.L(M.root_id()));
  EXPECT_EQ(M.root_id(), M.R(M.root_id()));
  EXPECT_EQ(M.root_id(), M.U(M.root_id()));
  EXPECT_EQ(M.root_id(), M.D(M.root_id()));
}

TEST(LinkedMatrix_test, positive_width_without_rows) {
  auto M = LinkedMatrix(ExactCoverProblem(1u, {}));
  EXPECT_NE(M.root_id(), M.L(M.root_id()));
}

TEST(LinkedMatrix_test, small_matrix_0) {
  //     v          v
  // > [root] <-> [col] <
  //     ^          ^

  auto M = LinkedMatrix(ExactCoverProblem::dense({{0}}));
  auto root = M.root_id();
  auto col = M.R(root);
  EXPECT_NE(root, col);

  EXPECT_EQ(col, M.L(root));
  EXPECT_EQ(col, M.R(root));
  EXPECT_EQ(root, M.U(root));
  EXPECT_EQ(root, M.D(root));

  EXPECT_EQ(root, M.L(col));
  EXPECT_EQ(root, M.R(col));
  EXPECT_EQ(col, M.U(col));
  EXPECT_EQ(col, M.D(col));

  EXPECT_EQ(0, M.S(col));
  EXPECT_EQ(col, M.C(col));
}

TEST(LinkedMatrix_test, small_matrix_1) {
  //     v          v
  // > [root] <-> [col ] <
  //     ^          ^
  //                |
  //                v
  //            > [node] <
  //                ^

  auto M = LinkedMatrix(ExactCoverProblem::dense({{1}}));
  auto root = M.root_id();
  auto col = M.R(root);
  auto node = M.D(col);
  EXPECT_NE(col, root);
  EXPECT_NE(col, node);
  EXPECT_NE(node, root);

  EXPECT_EQ(col, M.L(root));
  EXPECT_EQ(col, M.R(root));
  EXPECT_EQ(root, M.U(root));
  EXPECT_EQ(root, M.D(root));

  EXPECT_EQ(root, M.L(col));
  EXPECT_EQ(root, M.R(col));
  EXPECT_EQ(node, M.U(col));
  EXPECT_EQ(node, M.D(col));

  EXPECT_EQ(node, M.L(node));
  EXPECT_EQ(node, M.R(node));
  EXPECT_EQ(col, M.U(node));
  EXPECT_EQ(col, M.D(node));

  EXPECT_EQ(1, M.S(col));
  EXPECT_EQ(col, M.C(col));
  EXPECT_EQ(1, M.S(node));
  EXPECT_EQ(0, M.Y(node));
  EXPECT_EQ(col, M.C(node));
}

TEST(LinkedMatrix_test, small_matrix_11) {
  //     v          v           v
  // > [root] <-> [col1 ] <-> [col2 ] <
  //     ^          ^           ^
  //                |           |
  //                v           v
  //            > [node1] <-> [node2] <
  //                ^           ^

  auto M = LinkedMatrix(ExactCoverProblem::dense({{1, 1}}));
  auto root = M.root_id();
  auto col1 = M.R(root);
  auto col2 = M.R(col1);
  auto node1 = M.D(col1);
  auto node2 = M.D(col2);
  EXPECT_NE(col1, col2);
  EXPECT_NE(col1, node1);
  EXPECT_NE(col2, node2);

  EXPECT_EQ(1, M.S(col1));
  EXPECT_EQ(1, M.S(col2));
  EXPECT_EQ(col1, M.C(node1));
  EXPECT_EQ(col2, M.C(node2));
  EXPECT_EQ(0, M.Y(node1));
  EXPECT_EQ(0, M.Y(node2));

  EXPECT_EQ(col2, M.L(root));
  EXPECT_EQ(col1, M.R(root));
  EXPECT_EQ(root, M.U(root));
  EXPECT_EQ(root, M.D(root));

  EXPECT_EQ(root, M.L(col1));
  EXPECT_EQ(col2, M.R(col1));
  EXPECT_EQ(node1, M.U(col1));
  EXPECT_EQ(node1, M.D(col1));

  EXPECT_EQ(col1, M.L(col2));
  EXPECT_EQ(root, M.R(col2));
  EXPECT_EQ(node2, M.U(col2));
  EXPECT_EQ(node2, M.D(col2));

  EXPECT_EQ(node2, M.L(node1));
  EXPECT_EQ(node2, M.R(node1));
  EXPECT_EQ(col1, M.U(node1));
  EXPECT_EQ(col1, M.D(node1));

  EXPECT_EQ(node1, M.L(node2));
  EXPECT_EQ(node1, M.R(node2));
  EXPECT_EQ(col2, M.U(node2));
  EXPECT_EQ(col2, M.D(node2));
}

}
