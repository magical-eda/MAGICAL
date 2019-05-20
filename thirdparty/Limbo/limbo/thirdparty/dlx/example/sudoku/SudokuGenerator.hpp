#pragma once

#include "Sudoku.hpp"
#include "SudokuType.hpp"

#include <random>

class SudokuGenerator
{
public:
  SudokuGenerator();

  Sudoku operator()(std::shared_ptr<SudokuType> type);

private:
  std::mt19937 engine_;

  static unsigned count_solutions(const Sudoku& sudoku);
};
