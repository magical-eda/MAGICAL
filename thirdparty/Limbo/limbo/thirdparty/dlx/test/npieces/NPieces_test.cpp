#include "../../example/npieces/NPieces.hpp"

#include <gtest/gtest.h>

namespace {

TEST(NPieces_test, zero_pieces) {
  EXPECT_EQ(1, NPieces().count_solutions());
  EXPECT_EQ(1, NPieces().size(1).count_solutions());
  EXPECT_EQ(1, NPieces().size(2, 3).count_solutions());
}

TEST(NPieces_test, knights) {
  EXPECT_EQ(1, NPieces().size(1).knights(0).count_solutions());
  EXPECT_EQ(1, NPieces().size(1).knights(1).count_solutions());
  EXPECT_EQ(0, NPieces().size(1).knights(2).count_solutions());

  EXPECT_EQ(1, NPieces().size(2, 1).knights(0).count_solutions());
  EXPECT_EQ(2, NPieces().size(2, 1).knights(1).count_solutions());
  EXPECT_EQ(1, NPieces().size(2, 1).knights(2).count_solutions());
  EXPECT_EQ(0, NPieces().size(2, 1).knights(3).count_solutions());

  EXPECT_EQ(1, NPieces().size(1, 2).knights(0).count_solutions());
  EXPECT_EQ(2, NPieces().size(1, 2).knights(1).count_solutions());
  EXPECT_EQ(1, NPieces().size(1, 2).knights(2).count_solutions());
  EXPECT_EQ(0, NPieces().size(1, 2).knights(3).count_solutions());

  EXPECT_EQ(1, NPieces().size(3, 1).knights(0).count_solutions());
  EXPECT_EQ(3, NPieces().size(3, 1).knights(1).count_solutions());
  EXPECT_EQ(3, NPieces().size(3, 1).knights(2).count_solutions());
  EXPECT_EQ(1, NPieces().size(3, 1).knights(3).count_solutions());
  EXPECT_EQ(0, NPieces().size(3, 1).knights(4).count_solutions());

  EXPECT_EQ(1, NPieces().size(4, 1).knights(0).count_solutions());
  EXPECT_EQ(4, NPieces().size(4, 1).knights(1).count_solutions());
  EXPECT_EQ(6, NPieces().size(4, 1).knights(2).count_solutions());
  EXPECT_EQ(4, NPieces().size(4, 1).knights(3).count_solutions());
  EXPECT_EQ(1, NPieces().size(4, 1).knights(4).count_solutions());
  EXPECT_EQ(0, NPieces().size(4, 1).knights(5).count_solutions());

  EXPECT_EQ(1, NPieces().size(2).knights(0).count_solutions());
  EXPECT_EQ(4, NPieces().size(2).knights(1).count_solutions());
  EXPECT_EQ(6, NPieces().size(2).knights(2).count_solutions());
  EXPECT_EQ(4, NPieces().size(2).knights(3).count_solutions());
  EXPECT_EQ(1, NPieces().size(2).knights(4).count_solutions());
  EXPECT_EQ(0, NPieces().size(2).knights(5).count_solutions());

  EXPECT_EQ(1, NPieces().size(2, 3).knights(0).count_solutions());
  EXPECT_EQ(6, NPieces().size(2, 3).knights(1).count_solutions());
  EXPECT_EQ(13, NPieces().size(2, 3).knights(2).count_solutions());

  EXPECT_EQ(64, NPieces().size(8).knights(1).count_solutions());
}

TEST(NPieces_test, queens) {
  EXPECT_EQ(1, NPieces().size(1).queens(0).count_solutions());
  EXPECT_EQ(1, NPieces().size(1).queens(1).count_solutions());
  EXPECT_EQ(0, NPieces().size(1).queens(2).count_solutions());

  EXPECT_EQ(1, NPieces().size(2).queens(0).count_solutions());
  EXPECT_EQ(4, NPieces().size(2).queens(1).count_solutions());
  EXPECT_EQ(0, NPieces().size(2).queens(2).count_solutions());

  EXPECT_EQ(1, NPieces().size(3).queens(0).count_solutions());
  EXPECT_EQ(9, NPieces().size(3).queens(1).count_solutions());

  EXPECT_EQ(2, NPieces().size(3, 2).queens(2).count_solutions());
  /*
  EXPECT_EQ(8, NPieces().size(3).queens(2).count_solutions());
  EXPECT_EQ(0, NPieces().size(3).queens(3).count_solutions());

  EXPECT_EQ(44, NPieces().size(4).queens(2).count_solutions());

  EXPECT_EQ(2, NPieces().size(4).queens(4).count_solutions());
  EXPECT_EQ(10, NPieces().size(5).queens(5).count_solutions());
  */
}

}
