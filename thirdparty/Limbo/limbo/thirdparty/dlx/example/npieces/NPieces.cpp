#include "NPieces.hpp"

#include <dlx/AlgorithmDLX.hpp>
#include <algorithm>
#include <set>

NPieces NPieces::size(unsigned n) const {
  return size(n, n);
}

NPieces NPieces::size(unsigned width, unsigned height) const {
  return NPieces(width, height, knights_, queens_);
}

NPieces NPieces::knights(unsigned n) const {
  return NPieces(width_, height_, n, queens_);
}

NPieces NPieces::queens(unsigned n) const {
  return NPieces(width_, height_, knights_, n);
}

NPieces::NPieces(unsigned width, unsigned height, unsigned knights, unsigned queens)
  : width_(width),
  height_(height),
  knights_(knights),
  queens_(queens)
{
  // Columns
  //   P*A: (x,y) attacked by piece i?
  //   P*A: placing piece i at (x,y) ok?
  //   P: piece i used?
  //
  // Total: (2A+1)P
  // Secondary: 2AP

  auto A = width_ * height_;
  auto P = knights_ + queens_;

  problem_ = ExactCoverProblem((2 * A + 1) * P, 2 * A * P);

  auto piece_type = [&](unsigned p) -> Piece {
    return p < knights_ ? Piece::Knight : Piece::Queen;
  };
  auto col_attack = [&](unsigned p, unsigned yx) -> unsigned {
    return p * A + yx;
  };
  auto col_put = [&](unsigned p, unsigned yx) -> unsigned {
    return (P + p) * A + yx;
  };
  auto col_piece = [&](unsigned p) -> unsigned {
    return 2 * P * A + p;
  };

  for (auto yx = 0u; yx < A; ++yx) {
    auto x = yx % width_;
    auto y = yx / width_;
    for (auto p = 0u; p < P; ++p) {
      auto piece = piece_type(p);
      row_data_.push_back({piece, x, y});
      auto columns = std::vector<unsigned>();
      for (auto pos : attacks(piece, x, y)) {
        columns.push_back(col_attack(p, pos));
      }
      columns.push_back(col_piece(p));
      columns.push_back(col_put(p, yx));
      for (auto p2 = 0u; p2 < P; ++p2) {
        if (p2 != p) {
          columns.push_back(col_attack(p2, yx));
        }
      }
      if (p > 0 && piece_type(p - 1) == piece) {
        for (auto yx2 = yx + 1; yx2 < A; ++yx2) {
          columns.push_back(col_put(p - 1, yx2));
        }
      }
      std::sort(columns.begin(), columns.end());
      problem_.add_row(std::move(columns));
    }
  }
}

unsigned NPieces::count_solutions() const {
  auto dlx = AlgorithmDLX(problem_);
  return dlx.count_solutions();
}

auto NPieces::find_solutions() const -> std::vector<Solution> {
  auto dlx = AlgorithmDLX(problem_);
  auto solutions = std::vector<Solution>();
  for (const auto& used_rows : dlx.find_solutions()) {
    auto solution = Solution(height_, std::vector<Piece>(width_, Piece::None));
    for (auto i : used_rows) {
      auto data = row_data_[i];
      solution[data.y][data.x] = data.piece;
    }
    solutions.push_back(std::move(solution));
  }
  return solutions;
}

std::vector<unsigned> NPieces::attacks(Piece piece, unsigned x0, unsigned y0) const {
  auto points = std::set<unsigned>{y0 * width_ + x0};
  for (auto y = 0u; y < height_; ++y) {
    for (auto x = 0u; x < width_; ++x) {
      if (is_attack(piece, x0, y0, x, y)) {
        points.insert(y * width_ + x);
      }
    }
  }

  auto result = std::vector<unsigned>();
  result.assign(points.begin(), points.end());
  return result;
}

bool NPieces::is_attack(Piece piece, int x1, int y1, int x2, int y2) const {
  if (piece == Piece::Knight) {
    int dx = x1 - x2;
    int dy = y1 - y2;
    return dx * dx + dy * dy == 5;
  }
  if (piece == Piece::Queen) {
    return x1 == x2 || y1 == y2 || y1 - x1 == y2 - x2 || y1 + x1 == y2 + x2;
  }
  throw std::logic_error("NPieces::is_attack(): Unknown piece");
}
