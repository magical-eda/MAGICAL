#pragma once

#include <vector>

class Shape
{
public:
  Shape();
  Shape(std::initializer_list<std::vector<unsigned>> bits);
  Shape(char name, std::vector<std::vector<unsigned>> bits);

  static auto pentominoes() -> std::vector<Shape>;

  auto rotate() const -> Shape;
  auto reflect() const -> Shape;
  auto rotations() const -> std::vector<Shape>;
  auto reflections() const -> std::vector<Shape>;
  auto variations() const -> std::vector<Shape>;

  auto name() const -> char;
  auto width() const -> unsigned;
  auto height() const -> unsigned;
  auto size() const -> unsigned;

  bool operator[](unsigned yx) const;
  bool operator==(const Shape&) const;
  bool operator!=(const Shape&) const;
  bool operator<(const Shape&) const;

private:
  char name_;
  std::vector<std::vector<unsigned>> bits_;
  unsigned width_;
  unsigned height_;
};
