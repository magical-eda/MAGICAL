#pragma once

#include "Shape.hpp"
#include <dlx/ExactCoverProblem.hpp>
#include <string>
#include <vector>

class Polyomino
{
public:
  Polyomino();
  Polyomino(std::vector<Shape> pieces, std::vector<std::vector<unsigned>> area);

  static auto area(unsigned width, unsigned height) -> std::vector<std::vector<unsigned>>;

  auto problem() const -> const ExactCoverProblem&;
  auto make_solution(const std::vector<unsigned>& used_rows) const -> std::vector<std::string>;

  auto size() const -> unsigned;
  auto width() const -> unsigned;
  auto height() const -> unsigned;

private:
  std::vector<std::vector<unsigned>> area_;
  std::vector<std::vector<unsigned>> index_;
  std::vector<std::vector<Shape>> shapes_;
  unsigned size_;

  static auto get_size(const std::vector<std::vector<unsigned>>& area) -> unsigned;
  auto can_put(const Shape& shape, unsigned x, unsigned y) const -> bool;
  
  struct RowData
  {
    unsigned shape;
    unsigned variation;
    unsigned x;
    unsigned y;
  };

  std::vector<RowData> row_data_;
  ExactCoverProblem problem_;
};
