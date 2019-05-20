#include "../../example/polyomino/Shape.hpp"

#include <gtest/gtest.h>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Shape& shape) {
  os << '<';
  for (auto y = 0u; y < shape.height(); ++y) {
    for (auto x = 0u; x < shape.width(); ++x) {
      os << ".x"[shape[y * shape.width() + x]];
    }
    os << ',';
  }
  os << '>';
  return os;
}

namespace {

TEST(Shape_test, bad_shape) {
  EXPECT_DEATH(Shape({{},{1}}), "Assertion");
}

TEST(Shape_test, shape_rotations) {
  EXPECT_EQ(std::vector<Shape>{Shape()}, Shape().rotations());

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({{1}}),
    }),
    Shape({{1}}).rotations()
  );

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {1,1},
      }),
      Shape({
        {1},
        {1},
      }),
    }),
    Shape({{1,1}}).rotations()
  );

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {1},
        {1},
      }),
      Shape({
        {1,1},
      }),
    }),
    Shape({{1},{1}}).rotations()
  );

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {0,1},
        {1,1},
      }),
      Shape({
        {1,0},
        {1,1},
      }),
      Shape({
        {1,1},
        {1,0},
      }),
      Shape({
        {1,1},
        {0,1},
      }),
    }),
    Shape({
      {0,1},
      {1,1},
    }).rotations()
  );

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {0,1},
        {1,1},
        {1,0},
      }),
      Shape({
        {1,1,0},
        {0,1,1},
      }),
    }),
    Shape({
      {0,1},
      {1,1},
      {1,0},
    }).rotations()
  );
}

TEST(Shape_test, shape_reflections) {
  EXPECT_EQ(std::vector<Shape>{Shape()}, Shape().reflections());

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {0,1},
        {1,1},
      }),
    }),
    Shape({
      {0,1},
      {1,1},
    }).reflections()
  );

  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {0,1},
        {1,1},
        {1,0},
      }),
      Shape({
        {1,0},
        {1,1},
        {0,1},
      }),
    }),
    Shape({
      {0,1},
      {1,1},
      {1,0},
    }).reflections()
  );
}

TEST(Shape_test, shape_variations) {
  EXPECT_EQ(
    std::vector<Shape>({
      Shape({
        {0,1,0},
        {1,1,1},
        {1,0,0},
      }),
      Shape({
        {1,1,0},
        {0,1,1},
        {0,1,0},
      }),
      Shape({
        {0,0,1},
        {1,1,1},
        {0,1,0},
      }),
      Shape({
        {0,1,0},
        {1,1,0},
        {0,1,1},
      }),
      Shape({
        {0,1,0},
        {1,1,1},
        {0,0,1},
      }),
      Shape({
        {0,1,0},
        {0,1,1},
        {1,1,0},
      }),
      Shape({
        {1,0,0},
        {1,1,1},
        {0,1,0},
      }),
      Shape({
        {0,1,1},
        {1,1,0},
        {0,1,0},
      }),
    }),
    Shape({
      {0,1,0},
      {1,1,1},
      {1,0,0},
    }).variations()
  );
}

}
