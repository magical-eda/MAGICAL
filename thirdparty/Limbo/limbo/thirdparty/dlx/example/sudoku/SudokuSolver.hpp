#pragma once

#include "Sudoku.hpp"

class SudokuSolver
{
public:
  Sudoku solve(const Sudoku&) const;
  Sudoku random_solution(const Sudoku&) const;

private:
  Sudoku solve_impl(const Sudoku&, bool randomized) const;
};
