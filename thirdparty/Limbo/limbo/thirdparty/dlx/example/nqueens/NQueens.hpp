#pragma once

#include <dlx/ExactCoverProblem.hpp>
#include <vector>

class NQueens
{
public:
  explicit NQueens(unsigned n);

  auto count_solutions() const -> unsigned;
  auto find_solutions() const -> std::vector<std::vector<unsigned>>;

  auto problem() const -> const ExactCoverProblem&;

private:
  struct Position {
    unsigned x;
    unsigned y;
  };

  unsigned n_;
  ExactCoverProblem problem_;
  std::vector<Position> row_data_;
};
