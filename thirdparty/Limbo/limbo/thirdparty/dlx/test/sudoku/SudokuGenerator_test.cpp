#include "../../example/sudoku/SudokuGenerator.hpp"

#include <gtest/gtest.h>

namespace {

TEST(SudokuGenerator_test, small_sudoku) {
  auto generator = SudokuGenerator();
  auto sudoku = generator(SudokuType::make(4));
  EXPECT_EQ(4, sudoku.type().n());
  EXPECT_TRUE(sudoku.is_valid());
  EXPECT_FALSE(sudoku.is_solved());
}

TEST(SudokuGenerator_test, standard_sudoku) {
  auto generator = SudokuGenerator();
  auto sudoku = generator(SudokuType::make());
  EXPECT_EQ(9, sudoku.type().n());
  EXPECT_TRUE(sudoku.is_valid());
  EXPECT_FALSE(sudoku.is_solved());
}

}
