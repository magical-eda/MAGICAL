#include "Polyomino.hpp"

Polyomino::Polyomino()
  : Polyomino(Shape::pentominoes(), area(10, 6))
{
}

Polyomino::Polyomino(std::vector<Shape> pieces, std::vector<std::vector<unsigned>> area)
  : area_(std::move(area)),
  index_(),
  shapes_(),
  size_(get_size(area_))
{
  for (const auto& piece : pieces) {
    shapes_.emplace_back(piece.variations());
  }

  index_ = std::vector<std::vector<unsigned>>(height(), std::vector<unsigned>(width()));
  for (auto y = 0u, i = 0u; y < height(); ++y) {
    for (auto x = 0u; x < width(); ++x) {
      if (area_[y][x]) {
        index_[y][x] = i++;
      }
    }
  }

  problem_ = ExactCoverProblem(shapes_.size() + size_);
  for (auto s = 0u; s < shapes_.size(); ++s) {
    for (auto v = 0u; v < shapes_[s].size(); ++v) {
      const auto& shape = shapes_[s][v];
      for (auto yx = 0u; yx < size_; ++yx) {
        auto y = yx / width();
        auto x = yx % width();
        if (!can_put(shape, x, y)) {
          continue;
        }
        row_data_.push_back({s, v, x, y});
        auto row = std::vector<unsigned>();
        for (auto dyx = 0u; dyx < shape.size(); ++dyx) {
          if (!shape[dyx]) {
            continue;
          }
          auto dy = dyx / shape.width();
          auto dx = dyx % shape.width();
          row.push_back(index_[y + dy][x + dx]);
        }
        row.push_back(size_ + s);
        problem_.add_row(std::move(row));
      }
    }
  }
}

auto Polyomino::area(unsigned width, unsigned height) -> std::vector<std::vector<unsigned>> {
  return std::vector<std::vector<unsigned>>(height, std::vector<unsigned>(width, 1));
}

auto Polyomino::problem() const -> const ExactCoverProblem& {
  return problem_;
}

auto Polyomino::size() const -> unsigned {
  return size_;
}

auto Polyomino::width() const -> unsigned {
  return area_.empty() ? 0 : area_[0].size();
}

auto Polyomino::height() const -> unsigned {
  return area_.size();
}

auto Polyomino::make_solution(const std::vector<unsigned>& used_rows) const -> std::vector<std::string> {
  auto lines = std::vector<std::string>(height(), std::string(width(), ' '));
  for (auto i : used_rows) {
    const auto& data = row_data_[i];
    const auto& shape = shapes_[data.shape][data.variation];
    for (auto y = 0u; y < shape.height(); ++y) {
      for (auto x = 0u; x < shape.width(); ++x) {
        if (shape[y * shape.width() + x]) {
          lines[data.y + y][data.x + x] = shape.name();
        }
      }
    }
  }
  return lines;
}

auto Polyomino::get_size(const std::vector<std::vector<unsigned>>& area) -> unsigned {
  auto result = 0u;
  for (const auto& row : area) {
    for (auto value : row) {
      if (value) {
        ++result;
      }
    }
  }
  return result;
}

auto Polyomino::can_put(const Shape& shape, unsigned x, unsigned y) const -> bool {
  for (auto dy = 0u; dy < shape.height(); ++dy) {
    for (auto dx = 0u; dx < shape.width(); ++dx) {
      if (!shape[dy * shape.width() + dx]) {
        continue;
      }
      if (x + dx >= width() || y + dy >= height() || !area_[y + dy][x + dx]) {
        return false;
      }
    }
  }
  return true;
}
