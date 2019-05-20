#include "../include/dlx/ExactCoverProblem.hpp"

#include <gtest/gtest.h>

namespace {

TEST(ExactCoverProblem_test, ctor) {
  EXPECT_ANY_THROW(ExactCoverProblem(0, {{0}}));
  EXPECT_ANY_THROW(ExactCoverProblem(1, {{1}}));
  EXPECT_ANY_THROW(ExactCoverProblem(5, {{5}}));
  EXPECT_ANY_THROW(ExactCoverProblem(1, {{0, 0}}));

  EXPECT_NO_THROW(ExactCoverProblem(1, {{0}}));
  EXPECT_NO_THROW(ExactCoverProblem(2, {{1}}));
  EXPECT_NO_THROW(ExactCoverProblem(6, {{5}}));
}

TEST(ExactCoverProblem_test, dense_matrix) {
  EXPECT_ANY_THROW(ExactCoverProblem::dense({{0}, {}}));
  EXPECT_ANY_THROW(ExactCoverProblem::dense({{2}}));
  EXPECT_ANY_THROW(ExactCoverProblem::dense({{0}}, 2));

  EXPECT_NO_THROW(ExactCoverProblem::dense({}));
  EXPECT_NO_THROW(ExactCoverProblem::dense({{}, {}}));
  EXPECT_NO_THROW(ExactCoverProblem::dense({{0}, {1}}));
  EXPECT_NO_THROW(ExactCoverProblem::dense({{0}}, 1));

  EXPECT_EQ(0, ExactCoverProblem::dense({{}}).width());
  EXPECT_EQ(2, ExactCoverProblem::dense({{0, 0}}).width());
}

}
