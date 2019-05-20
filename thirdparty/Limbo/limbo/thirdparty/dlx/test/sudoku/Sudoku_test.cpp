#include "../../example/sudoku/Sudoku.hpp"
#include "../../example/sudoku/SudokuFormat.hpp"

#include <gtest/gtest.h>

namespace {

TEST(Sudoku_test, to_string) {
  auto type = SudokuType::make(4);
  auto format = SudokuFormat(type,
    "..|..\n"
    "..|..\n"
    "--|--\n"
    "..|..\n"
    "..|..\n"
  );

  Sudoku sudoku(type);
  sudoku[0] = 0;
  sudoku[3] = 4;
  sudoku[4] = 2;
  sudoku[9] = 1;
  sudoku[14] = 1;

  EXPECT_EQ(
    "..|.4\n"
    "2.|..\n"
    "--|--\n"
    ".1|..\n"
    "..|1.\n",
    sudoku.to_string(format)
  );
}

TEST(Sudoku_test, from_string) {
  EXPECT_THROW(Sudoku(""), std::invalid_argument);
  EXPECT_NO_THROW(Sudoku("."));
  EXPECT_NO_THROW(Sudoku("0"));
  EXPECT_NO_THROW(Sudoku("1"));
  EXPECT_NO_THROW(Sudoku("( 1 )"));

  EXPECT_NO_THROW(Sudoku("2"));
  EXPECT_NO_THROW(Sudoku("A"));
  EXPECT_NO_THROW(Sudoku("z"));

  EXPECT_ANY_THROW(Sudoku("ABCD .... .... ...E"));
  EXPECT_NO_THROW(Sudoku("ABCD .... .... ...A"));

  Sudoku sudoku(
    "1.|.."
    "..|.2"
    "-----"
    ".1|.."
    "..|3."
  );

  EXPECT_EQ(1, sudoku[0]);
  EXPECT_EQ(0, sudoku[1]);
  EXPECT_EQ(0, sudoku[2]);
  EXPECT_EQ(0, sudoku[3]);
  EXPECT_EQ(0, sudoku[4]);
  EXPECT_EQ(0, sudoku[5]);
  EXPECT_EQ(0, sudoku[6]);
  EXPECT_EQ(2, sudoku[7]);
  EXPECT_EQ(0, sudoku[8]);
  EXPECT_EQ(1, sudoku[9]);
  EXPECT_EQ(0, sudoku[10]);
  EXPECT_EQ(0, sudoku[11]);
  EXPECT_EQ(0, sudoku[12]);
  EXPECT_EQ(0, sudoku[13]);
  EXPECT_EQ(3, sudoku[14]);
  EXPECT_EQ(0, sudoku[15]);
}

TEST(Sudoku_test, is_valid) {
  EXPECT_FALSE(
    Sudoku(
      "1.|1."
      "..|.."
      "-----"
      "..|.."
      "..|.."
    ).is_valid()
  );

  EXPECT_FALSE(
    Sudoku(
      "1.|.."
      "..|.."
      "-----"
      "1.|.."
      "..|.."
    ).is_valid()
  );

  EXPECT_FALSE(
    Sudoku(
      "1.|.."
      ".1|.."
      "-----"
      "..|.."
      "..|.."
    ).is_valid()
  );

  EXPECT_TRUE(
    Sudoku(
      "1.|.."
      "..|1."
      "-----"
      ".1|.."
      "..|.1"
    ).is_valid()
  );
}

TEST(Sudoku_test, is_solved) {
  EXPECT_FALSE(Sudoku(".").is_solved());
  EXPECT_TRUE(Sudoku("1").is_solved());
  EXPECT_FALSE(
    Sudoku(
      "12|3."
      "34|12"
      "-----"
      "43|21"
      "21|43"
    ).is_solved()
  );
  EXPECT_TRUE(
    Sudoku(
      "12|34"
      "34|12"
      "-----"
      "43|21"
      "21|43"
    ).is_solved()
  );
}

}
