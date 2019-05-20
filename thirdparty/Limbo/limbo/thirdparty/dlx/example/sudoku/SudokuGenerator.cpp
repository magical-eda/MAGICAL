#include "SudokuGenerator.hpp"
#include "SudokuSolver.hpp"

#include <algorithm>
#include <string>

SudokuGenerator::SudokuGenerator()
  : engine_()
{
  std::random_device rd;
  engine_ = std::mt19937(rd());
}

Sudoku SudokuGenerator::operator()(std::shared_ptr<SudokuType> type) {
  auto sudoku = SudokuSolver().random_solution(Sudoku(type));

  auto yxs = std::vector<unsigned>();
  for (auto i = 0u; i < type->size(); ++i) {
    yxs.push_back(i);
  }

  for (;;) {
    std::shuffle(yxs.begin(), yxs.end(), engine_);
    auto deletions = 0u;
    for (auto yx : yxs) {
      auto d = sudoku[yx];
      if (d != 0) {
        sudoku[yx] = 0;
        if (count_solutions(sudoku) != 1) {
          sudoku[yx] = d;
        }
        else {
          ++deletions;
        }
      }
    }
    if (deletions == 0) {
      return sudoku;
    }
  }
}

unsigned SudokuGenerator::count_solutions(const Sudoku& sudoku) {
  try {
    auto solved = SudokuSolver().solve(sudoku);
    return 1;
  }
  catch (const std::exception& ex) {
    // TODO: Different types of exceptions.
    if (ex.what() == std::string("No solution")) {
      return 0;
    }
    if (ex.what() == std::string("Multiple solutions")) {
      return 2;
    }
    throw;
  }
}
