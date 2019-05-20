#include "../../example/nqueens/NQueens.hpp"

#include <gtest/gtest.h>
#include <algorithm>

namespace {

TEST(NQueens_test, zero) {
  EXPECT_DEATH(NQueens(0), "Assertion");
}

TEST(NQueens_test, count_solutions) {
  EXPECT_EQ(1, NQueens(1).count_solutions());
  EXPECT_EQ(0, NQueens(2).count_solutions());
  EXPECT_EQ(0, NQueens(3).count_solutions());
  EXPECT_EQ(2, NQueens(4).count_solutions());
  EXPECT_EQ(10, NQueens(5).count_solutions());
  EXPECT_EQ(4, NQueens(6).count_solutions());
  EXPECT_EQ(40, NQueens(7).count_solutions());
  EXPECT_EQ(92, NQueens(8).count_solutions());
  EXPECT_EQ(352, NQueens(9).count_solutions());
  EXPECT_EQ(724, NQueens(10).count_solutions());
}

TEST(NQueens_test, find_solutions) {
  EXPECT_TRUE(NQueens(2).find_solutions().empty());
  EXPECT_EQ(std::vector<std::vector<unsigned>>{{0}}, NQueens(1).find_solutions());

  auto n4 = NQueens(4).find_solutions();
  std::sort(n4.begin(), n4.end());
  EXPECT_EQ(
    (std::vector<std::vector<unsigned>>{
      {1,3,0,2},
      {2,0,3,1}
    }),
    n4
  );
}

}
