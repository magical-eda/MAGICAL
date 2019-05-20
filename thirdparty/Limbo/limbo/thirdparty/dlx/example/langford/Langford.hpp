#pragma once

#include <dlx/ExactCoverProblem.hpp>
#include <vector>

class Langford
{
public:
  explicit Langford(unsigned n);

  auto problem() const -> const ExactCoverProblem&;
  auto make_solution(const std::vector<unsigned>& used_rows) const -> std::vector<unsigned>;

private:
  struct Placement {
    unsigned value;
    unsigned left_pos;
  };

  unsigned n_;
  std::vector<Placement> row_data_;
  ExactCoverProblem problem_;
};
