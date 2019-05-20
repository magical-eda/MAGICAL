#pragma once

#include <dlx/ExactCoverProblem.hpp>
#include <vector>

class NPieces
{
public:
  enum class Piece {
    None,
    Knight,
    Queen,
  };

  using Solution = std::vector<std::vector<Piece>>;

  NPieces() = default;
  NPieces size(unsigned) const;
  NPieces size(unsigned, unsigned) const;
  NPieces knights(unsigned) const;
  NPieces queens(unsigned) const;

  auto count_solutions() const -> unsigned;
  auto find_solutions() const -> std::vector<Solution>;

private:
  NPieces(unsigned width, unsigned height, unsigned knights, unsigned queens);
  auto attacks(Piece, unsigned x, unsigned y) const -> std::vector<unsigned>;
  auto is_attack(Piece, int x1, int y1, int x2, int y2) const -> bool;

  unsigned width_ = 0;
  unsigned height_ = 0;
  unsigned knights_ = 0;
  unsigned queens_ = 0;

  struct Data {
    Piece piece;
    unsigned x;
    unsigned y;
  };

  ExactCoverProblem problem_;
  std::vector<Data> row_data_;
};
