#include "../../example/langford/Langford.hpp"

#include <dlx/AlgorithmDLX.hpp>
#include <gtest/gtest.h>

namespace {

TEST(Langford_test, count_solutions) {
  EXPECT_EQ(0, AlgorithmDLX(Langford(1).problem()).count_solutions());
  EXPECT_EQ(0, AlgorithmDLX(Langford(2).problem()).count_solutions());
  EXPECT_EQ(1, AlgorithmDLX(Langford(3).problem()).count_solutions());
  EXPECT_EQ(1, AlgorithmDLX(Langford(4).problem()).count_solutions());
  EXPECT_EQ(0, AlgorithmDLX(Langford(5).problem()).count_solutions());
  EXPECT_EQ(0, AlgorithmDLX(Langford(6).problem()).count_solutions());
  EXPECT_EQ(26, AlgorithmDLX(Langford(7).problem()).count_solutions());
  EXPECT_EQ(150, AlgorithmDLX(Langford(8).problem()).count_solutions());
  EXPECT_EQ(0, AlgorithmDLX(Langford(9).problem()).count_solutions());
  EXPECT_EQ(0, AlgorithmDLX(Langford(10).problem()).count_solutions());
  EXPECT_EQ(17792, AlgorithmDLX(Langford(11).problem()).count_solutions());
}

TEST(Langford_test, find_solutions) {
  EXPECT_TRUE(AlgorithmDLX(Langford(1).problem()).find_solutions().empty());

  auto langford = Langford(3);
  auto used_rows = AlgorithmDLX(langford.problem()).find_solutions();
  ASSERT_EQ(1, used_rows.size());
  EXPECT_EQ((std::vector<unsigned>{3,1,2,1,3,2}), langford.make_solution(used_rows[0]));
}

}
