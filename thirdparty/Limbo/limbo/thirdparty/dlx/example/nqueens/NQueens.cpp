#include "NQueens.hpp"

#include <dlx/AlgorithmDLX.hpp>
#include <assert.h>

NQueens::NQueens(unsigned n)
  : n_(n),
  problem_(6 * n_ - 2, 4 * n_ - 2)
{
  assert(n_ > 0);
  auto D = n + n - 1;
  for (auto y = 0u; y < n; ++y) {
    for (auto x = 0u; x < n; ++x) {
      row_data_.push_back({x, y});
      auto d1 = x + y;
      auto d2 = x + n - y - 1;
      problem_.add_row({
        d1,
        D + d2,
        D + D + x,
        D + D + n + y,
      });
    }
  }
}

const ExactCoverProblem& NQueens::problem() const {
  return problem_;
}

unsigned NQueens::count_solutions() const {
  auto dlx = AlgorithmDLX(problem_);
  return dlx.count_solutions();
}

std::vector<std::vector<unsigned>> NQueens::find_solutions() const {
  auto solutions = std::vector<std::vector<unsigned>>();
  auto dlx = AlgorithmDLX(problem_);
  for (const auto& used_rows : dlx.find_solutions()) {
    auto solution = std::vector<unsigned>(n_);
    for (auto i : used_rows) {
      solution[row_data_[i].y] = row_data_[i].x;
    }
    solutions.emplace_back(std::move(solution));
  }
  return solutions;
}
