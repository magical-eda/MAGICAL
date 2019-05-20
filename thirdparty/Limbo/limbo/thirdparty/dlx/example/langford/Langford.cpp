#include "Langford.hpp"

#include <assert.h>
#include <dlx/AlgorithmDLX.hpp>

Langford::Langford(unsigned n)
  : n_(n),
  row_data_(),
  problem_(3 * n_)
{
  for (auto value = 1u; value <= n_; ++value) {
    for (auto pos = 0u; pos + value + 1 < 2 * n_; ++pos) {
      if (value == 1 && pos + 2 > n) {
        continue;
      }
      row_data_.push_back({value, pos});
      problem_.add_row({value - 1, n_ + pos, n_ + pos + value + 1});
    }
  }
}

auto Langford::problem() const -> const ExactCoverProblem& {
  return problem_;
}

auto Langford::make_solution(const std::vector<unsigned>& used_rows) const -> std::vector<unsigned> {
  auto solution = std::vector<unsigned>(2 * n_);
  for (auto i : used_rows) {
    auto pos = row_data_[i].left_pos;
    auto value = row_data_[i].value;
    solution[pos] = value;
    solution[pos + value + 1] = value;
  }
  return solution;
}
