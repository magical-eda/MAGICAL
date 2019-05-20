#include <dlx/AlgorithmDLX.hpp>

#include <gtest/gtest.h>

namespace {

TEST(AlgorithmDLX_test, no_rows) {
  auto rows = std::vector<std::vector<unsigned>>();
  auto dlx = AlgorithmDLX(ExactCoverProblem(1, rows));
  EXPECT_EQ(0u, dlx.count_solutions());
}

}
