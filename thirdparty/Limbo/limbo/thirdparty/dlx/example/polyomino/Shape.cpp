#include "Shape.hpp"

#include <assert.h>
#include <algorithm>

Shape::Shape()
  : Shape(std::initializer_list<std::vector<unsigned>>{})
{
}

Shape::Shape(std::initializer_list<std::vector<unsigned>> bits)
  : Shape('#', bits)
{
}

Shape::Shape(char name, std::vector<std::vector<unsigned>> bs)
  : name_(name),
  bits_(std::move(bs)),
  width_(bits_.empty() ? 0 : bits_[0].size()),
  height_(bits_.size())
{
  for (const auto& row : bits_) {
    assert(row.size() == width_);
  }
}

std::vector<Shape> Shape::pentominoes() {
  return {
    Shape('I', {
      {1, 1, 1, 1, 1}
    }),

    Shape('N', {
      {1, 1, 1, 0},
      {0, 0, 1, 1},
    }),
    Shape('L', {
      {1, 1, 1, 1},
      {1, 0, 0, 0},
    }),
    Shape('Y', {
      {1, 1, 1, 1},
      {0, 1, 0, 0},
    }),

    Shape('P', {
      {1, 1, 1},
      {1, 1, 0},
    }),
    Shape('C', {
      {1, 1, 1},
      {1, 0, 1},
    }),

    Shape('V', {
      {1, 1, 1},
      {1, 0, 0},
      {1, 0, 0},
    }),
    Shape('T', {
      {1, 1, 1},
      {0, 1, 0},
      {0, 1, 0},
    }),
    Shape('F', {
      {1, 1, 0},
      {0, 1, 1},
      {0, 1, 0},
    }),
    Shape('Z', {
      {1, 1, 0},
      {0, 1, 0},
      {0, 1, 1},
    }),
    Shape('W', {
      {1, 1, 0},
      {0, 1, 1},
      {0, 0, 1},
    }),
    Shape('X', {
      {0, 1, 0},
      {1, 1, 1},
      {0, 1, 0},
    }),
  };
}

Shape Shape::rotate() const {
  auto rows = std::vector<std::vector<unsigned>>(width_, std::vector<unsigned>(height_));
  for (auto y = 0u; y < height_; ++y) {
    for (auto x = 0u; x < width_; ++x) {
      rows[x][height_ - y - 1] = bits_[y][x];
    }
  }
  return Shape(name_, rows);
}

Shape Shape::reflect() const {
  auto rows = bits_;
  for (auto& row : rows) {
    std::reverse(row.begin(), row.end());
  }
  return Shape(name_, rows);
}

std::vector<Shape> Shape::rotations() const {
  auto result = std::vector<Shape>{*this};
  for (auto shape = rotate(); shape != result[0]; shape = shape.rotate()) {
    result.push_back(shape);
  }
  return result;
}

std::vector<Shape> Shape::reflections() const {
  auto refl = reflect();
  for (const auto& rot : rotations()) {
    if (rot == refl) {
      return {*this};
    }
  }
  return {*this, refl};
}

std::vector<Shape> Shape::variations() const {
  auto vars = std::vector<Shape>();
  for (const auto& refl : reflections()) {
    for (auto&& rot : refl.rotations()) {
      vars.push_back(std::move(rot));
    }
  }
  return vars;
}

char Shape::name() const {
  return name_;
}

unsigned Shape::width() const {
  return width_;
}

unsigned Shape::height() const {
  return height_;
}

unsigned Shape::size() const {
  return width_ * height_;
}

bool Shape::operator[](unsigned yx) const {
  return bits_[yx / width_][yx % width_];
}

bool Shape::operator==(const Shape& rhs) const {
  return bits_ == rhs.bits_;
}

bool Shape::operator!=(const Shape& rhs) const {
  return !(*this == rhs);
}

bool Shape::operator<(const Shape& rhs) const {
  return bits_ < rhs.bits_;
}
