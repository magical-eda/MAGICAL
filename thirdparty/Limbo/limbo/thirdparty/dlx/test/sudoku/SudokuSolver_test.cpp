#include "../example/sudoku/SudokuSolver.hpp"
#include "../example/sudoku/Sudoku.hpp"
#include "../example/sudoku/SudokuFormat.hpp"
#include "../example/sudoku/SudokuType.hpp"

#include <gtest/gtest.h>

namespace {

TEST(SudokuSolver_test, already_solved) {
  Sudoku sudoku("846937152319625847752184963285713694463859271971246385127598436638471529594362718");
  ASSERT_TRUE(sudoku.is_solved());
  EXPECT_EQ(sudoku, SudokuSolver().solve(sudoku));
}

TEST(SudokuSolver_test, invalid) {
  Sudoku sudoku(std::string(81, '1'));
  ASSERT_FALSE(sudoku.is_valid());
  EXPECT_THROW(SudokuSolver().solve(sudoku), std::exception);

  EXPECT_ANY_THROW(SudokuSolver().solve(Sudoku(
    "+--+--+\n"
    "|10|00|\n"
    "|01|00|\n"
    "+--+--+\n"
    "|00|00|\n"
    "|00|00|\n"
    "+--+--+\n"
  )));

  EXPECT_ANY_THROW(SudokuSolver().solve(Sudoku(
    "+--+--+\n"
    "|10|01|\n"
    "|00|00|\n"
    "+--+--+\n"
    "|00|00|\n"
    "|00|00|\n"
    "+--+--+\n"
  )));

  EXPECT_ANY_THROW(SudokuSolver().solve(Sudoku(
    "+--+--+\n"
    "|10|00|\n"
    "|00|00|\n"
    "+--+--+\n"
    "|00|00|\n"
    "|10|00|\n"
    "+--+--+\n"
  )));
}

TEST(SudokuSolver_test, almost_solved) {
  Sudoku sudoku(
    "+---+---+---+\n"
    "|681|739|245|\n"
    "|497|562|813|\n"
    "|523|841|769|\n"
    "+---+---+---+\n"
    "|172|954|386|\n"
    "|865|317|924|\n"
    "|349|628|571|\n"
    "+---+---+---+\n"
    "|916|283|457|\n"
    "|234|175|698|\n"
    "|758|496|132|\n"
    "+---+---+---+\n"
  );

  ASSERT_NO_THROW(SudokuSolver().solve(sudoku));

  for (unsigned i = 0; i < sudoku.size(); ++i) {
    for (unsigned digit = 1; digit <= 9; ++digit) {
      if (digit == sudoku[i]) {
        continue;
      }
      unsigned old = sudoku[i];
      sudoku[i] = digit;
      ASSERT_ANY_THROW(SudokuSolver().solve(sudoku));
      sudoku[i] = old;
    }
  }

  ASSERT_NO_THROW(SudokuSolver().solve(sudoku));
}

TEST(SudokuSolver_test, tenpai) {
  Sudoku sudoku(
    "+---+---+---+\n"
    "|681|739|245|\n"
    "|497|562|813|\n"
    "|523|841|769|\n"
    "+---+---+---+\n"
    "|172|954|386|\n"
    "|865|317|924|\n"
    "|349|628|571|\n"
    "+---+---+---+\n"
    "|916|283|457|\n"
    "|234|175|698|\n"
    "|758|496|132|\n"
    "+---+---+---+\n"
  );

  auto solved = sudoku;
  SudokuSolver solver;

  for (unsigned i = 0; i < sudoku.size(); ++i) {
    unsigned digit = sudoku[i];
    sudoku[i] = 0;
    ASSERT_EQ(solved, solver.solve(sudoku));
    sudoku[i] = digit;
  }
}

TEST(SudokuSolver_test, easy) {
  Sudoku easy(
    "+---+---+---+\n"
    "|14.|8..|97.|\n"
    "|..6|75.|...|\n"
    "|7..|...|..8|\n"
    "+---+---+---+\n"
    "|5..|4.2|.93|\n"
    "|93.|.7.|.82|\n"
    "|62.|9.8|..4|\n"
    "+---+---+---+\n"
    "|4..|...|..9|\n"
    "|...|.15|8..|\n"
    "|.72|..4|.15|\n"
    "+---+---+---+\n"
  );

  Sudoku solved(
    "+---+---+---+\n"
    "|145|823|976|\n"
    "|286|759|431|\n"
    "|793|146|528|\n"
    "+---+---+---+\n"
    "|518|462|793|\n"
    "|934|571|682|\n"
    "|627|938|154|\n"
    "+---+---+---+\n"
    "|451|387|269|\n"
    "|369|215|847|\n"
    "|872|694|315|\n"
    "+---+---+---+\n"
  );

  ASSERT_NE(solved, easy);
  EXPECT_EQ(solved, SudokuSolver().solve(easy));
}

TEST(SudokuSolver_test, hard) {
  Sudoku hard(
    "050|002|000"
    "000|100|400"
    "700|000|000"
    "---|---|---"
    "010|700|080"
    "004|030|060"
    "000|500|000"
    "---|---|---"
    "308|060|000"
    "000|000|100"
    "600|000|000"
  );

  Sudoku solved(
    "159|482|673"
    "836|175|429"
    "742|693|518"
    "---|---|---"
    "213|746|985"
    "584|239|761"
    "967|518|342"
    "---|---|---"
    "378|961|254"
    "495|827|136"
    "621|354|897"
  );

  EXPECT_EQ(solved, SudokuSolver().solve(hard));
}

TEST(SudokuSolver_test, region_2x2) {
  Sudoku puzzle(
    "21|.."
    ".3|2."
    "--+--"
    "..|.4"
    "1.|.."
  );
  Sudoku solved(
    "21|43"
    "43|21"
    "--+--"
    "32|14"
    "14|32"
  );
  EXPECT_EQ(solved, SudokuSolver().solve(puzzle));
}

TEST(SudokuSolver_test, region_3x2) {
  Sudoku puzzle(
    "5.6|...\n"
    "...|.2.\n"
    "---+---\n"
    ".6.|...\n"
    "...|5.1\n"
    "---+---\n"
    "..4|...\n"
    "...|1.3\n"
  );
  Sudoku solved(
    "526|314\n"
    "431|625\n"
    "---+---\n"
    "165|432\n"
    "243|561\n"
    "---+---\n"
    "314|256\n"
    "652|143\n"
  );

  EXPECT_EQ(solved, SudokuSolver().solve(puzzle));
}

TEST(SudokuSolver_test, region_5x2) {
  Sudoku puzzle(
    ".47..|..65.\n"
    "9...5|7...2\n"
    "-----+-----\n"
    "6..41|85..9\n"
    "..92.|.63..\n"
    "-----+-----\n"
    ".92.8|6.47.\n"
    ".A6.4|5.93.\n"
    "-----+-----\n"
    "..36.|.91..\n"
    "8..79|3A..6\n"
    "-----+-----\n"
    "A...6|4...1\n"
    ".14..|..86.\n"
  );
  Sudoku solved(
    "147A2|98653\n"
    "96835|74A12\n"
    "-----+-----\n"
    "63A41|85729\n"
    "58927|163A4\n"
    "-----+-----\n"
    "39258|6147A\n"
    "7A614|52938\n"
    "-----+-----\n"
    "4536A|29187\n"
    "82179|3A546\n"
    "-----+-----\n"
    "A7586|43291\n"
    "21493|A7865\n"
  );

  EXPECT_EQ(solved, SudokuSolver().solve(puzzle));
}

TEST(SudokuSolver_test, custom_labels) {
  Sudoku puzzle(
    ".P.|K.R|I.D"
    "D..|B..|..R"
    ".B.|E..|PA."
    "---+---+---"
    "P..|.KW|A.B"
    "...|...|RK."
    ".AD|...|..."
    "---+---+---"
    "B..|.E.|..P"
    "A..|...|E.."
    "ER.|P.K|B.."
  );

  Sudoku solved(
    "WPE|KAR|IBD"
    "DIA|BWP|KER"
    "RBK|EID|PAW"
    "---+---+---"
    "PER|IKW|ADB"
    "IWB|DPA|RKE"
    "KAD|RBE|WPI"
    "---+---+---"
    "BKW|AEI|DRP"
    "ADP|WRB|EIK"
    "ERI|PDK|BWA"
  );

  EXPECT_EQ(solved, SudokuSolver().solve(puzzle));
}

TEST(SudokuSolver_test, custom_regions) {
  Sudoku puzzle1(
    "---------------\n"
    "|. .|5 . . 6|4|\n"
    "|   ---     | |\n"
    "|2 . 1|. . .|7|\n"
    "|--   |------ |\n"
    "|.|. .|6 .|. .|\n"
    "| -----   |   |\n"
    "|. .|. . 5|. .|\n"
    "|   |   ----- |\n"
    "|4 3|. .|. .|.|\n"
    "| -------   --|\n"
    "|.|. . 5|4 . .|\n"
    "| |     ---   |\n"
    "|.|4 . 7 .|2 .|\n"
    "---------------\n"
  );

  Sudoku puzzle2(
    "---------------\n"
    "|. 1|3 . . 2|.|\n"
    "|   ---     | |\n"
    "|6 2 .|. . 5|.|\n"
    "|--   |------ |\n"
    "|.|. .|7 .|6 .|\n"
    "| -----   |   |\n"
    "|. .|. 4 .|. .|\n"
    "|   |   ----- |\n"
    "|1 .|. .|. .|.|\n"
    "| -------   --|\n"
    "|.|. . .|. . 6|\n"
    "| |     ---   |\n"
    "|2|. . . 6|. 7|\n"
    "---------------\n"
  );

  Sudoku solved2(
    "---------------\n"
    "|7 1|3 6 4 2|5|\n"
    "|   ---     | |\n"
    "|6 2 4|1 7 5|3|\n"
    "|--   |------ |\n"
    "|4|3 5|7 1|6 2|\n"
    "| -----   |   |\n"
    "|5 6|2 4 3|7 1|\n"
    "|   |   ----- |\n"
    "|1 7|6 5|2 3|4|\n"
    "| -------   --|\n"
    "|3|4 7 2|5 1 6|\n"
    "| |     ---   |\n"
    "|2|5 1 3 6|4 7|\n"
    "---------------\n"
  );

  EXPECT_ANY_THROW(SudokuSolver().solve(puzzle1));
  EXPECT_EQ(solved2, SudokuSolver().solve(puzzle2));
}

TEST(SudokuSolver_test, no_solution) {
  EXPECT_ANY_THROW(SudokuSolver().solve(Sudoku(
    "12|3."
    "..|.."
    "-----"
    "23|1."
    "..|.."
  )));
}

TEST(SudokuSolver_test, multiple_solutions) {
  EXPECT_NO_THROW(SudokuSolver().solve(Sudoku(
    "12|34"
    ".3|21"
    "-----"
    "..|12"
    "21|43"
  )));

  EXPECT_ANY_THROW(SudokuSolver().solve(Sudoku(
    "12|34"
    "..|21"
    "-----"
    "..|12"
    "21|43"
  )));
}

}
