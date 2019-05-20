#include "SudokuSolver.hpp"

#include <dlx/AlgorithmDLX.hpp>

#include <assert.h>
#include <algorithm>
#include <unordered_map>
#include <vector>

Sudoku SudokuSolver::solve(const Sudoku& sudoku) const {
  return solve_impl(sudoku, false);
}

Sudoku SudokuSolver::random_solution(const Sudoku& sudoku) const {
  return solve_impl(sudoku, true);
}

Sudoku SudokuSolver::solve_impl(const Sudoku& sudoku, bool randomized) const {
  if (!sudoku.is_valid()) {
    throw std::runtime_error("solve(): Invalid sudoku");
  }

  const SudokuType& type = sudoku.type();
  unsigned n = type.n();
  auto pack = [&](unsigned a, unsigned b) -> unsigned { return a * n + b; };
  auto id_cell = [&](unsigned x, unsigned y) -> unsigned { return pack(x, y); };
  auto id_col = [&](unsigned x, unsigned d) -> unsigned { return type.size() + pack(x, d); };
  auto id_row = [&](unsigned y, unsigned d) -> unsigned { return 2 * type.size() + pack(y, d); };
  auto id_region = [&](unsigned i, unsigned d) -> unsigned { return 3 * type.size() + pack(i, d); };

  std::vector<unsigned> cell_taken(type.size());
  std::vector<unsigned> col_taken(type.size());
  std::vector<unsigned> row_taken(type.size());
  std::vector<unsigned> region_taken(type.size());
  for (unsigned i = 0; i < type.size(); ++i) {
    if (sudoku[i] != 0) {
      unsigned x = i % n;
      unsigned y = i / n;
      unsigned d = sudoku[i] - 1;
      ++cell_taken[pack(x, y)];
      ++col_taken[pack(x, d)];
      ++row_taken[pack(y, d)];
      ++region_taken[pack(type.region(x, y), d)];
    }
  }

  std::vector<std::vector<unsigned>> matrix;
  for (unsigned i = 0; i < n; ++i) {
    for (unsigned j = 0; j < n; ++j) {
      if (cell_taken[pack(i, j)]) matrix.push_back({id_cell(i, j)});
      if (col_taken[pack(i, j)]) matrix.push_back({id_col(i, j)});
      if (row_taken[pack(i, j)]) matrix.push_back({id_row(i, j)});
      if (region_taken[pack(i, j)]) matrix.push_back({id_region(i, j)});
    }
  }

  std::unordered_map<unsigned, unsigned> row_position, row_digit;
  for (unsigned y = 0; y < n; ++y) {
    for (unsigned x = 0; x < n; ++x) {
      for (unsigned d = 0; d < n; ++d) {
        if (cell_taken[pack(x, y)]
            || col_taken[pack(x, d)]
            || row_taken[pack(y, d)]
            || region_taken[pack(type.region(x, y), d)])
        {
          continue;
        }
        unsigned row_index = matrix.size();
        row_position[row_index] = y * n + x;
        row_digit[row_index] = d;
        matrix.push_back({
          id_cell(x, y),
          id_col(x, d),
          id_row(y, d),
          id_region(type.region(x, y), d)
        });
      }
    }
  }

  AlgorithmDLX dlx(ExactCoverProblem(4 * type.size(), matrix));
  auto options = AlgorithmDLX::Options();
  if (randomized) {
    static std::random_device rd;
    static auto engine = std::mt19937(rd());
    options.choose_random_column = randomized;
    options.random_engine = &engine;
  }
  options.max_solutions = randomized ? 1 : 2;
  auto result = dlx.search(options);
  auto solutions = std::vector<Sudoku>();
  for (const auto& rows : result.solutions) {
    Sudoku solved(sudoku);
    for (auto i : rows) {
      auto pos = row_position.find(i);
      if (pos != row_position.end()) {
        solved[pos->second] = row_digit[i] + 1;
      }
    }
    solutions.push_back(std::move(solved));
  }

  if (solutions.empty()) {
    throw std::runtime_error("No solution");
  }
  if (solutions.size() > 1) {
    throw std::runtime_error("Multiple solutions");
  }
  return solutions[0];
}
