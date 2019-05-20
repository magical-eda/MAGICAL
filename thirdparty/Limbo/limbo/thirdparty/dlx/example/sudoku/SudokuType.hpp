#pragma once

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

class SudokuType
{
public:
  // The standard 9x9 Sudoku.
  SudokuType();

  // NxN version of the 9x9.
  explicit SudokuType(unsigned n);

  // Sudoku with rectangle-shaped regions.
  SudokuType(unsigned region_width, unsigned region_height);

  // Sudoku with arbitrarily-shaped regions.
  SudokuType(std::initializer_list<unsigned> regions);
  explicit SudokuType(std::vector<unsigned> regions);

  template <typename... Args>
  static std::shared_ptr<SudokuType> make(Args&&... args) {
    return std::make_shared<SudokuType>(std::forward<Args>(args)...);
  }
  static std::shared_ptr<SudokuType> from_size(unsigned size);
  static std::shared_ptr<SudokuType> guess(const std::string& str);

  unsigned n() const;
  unsigned size() const;
  unsigned region(unsigned pos) const;
  unsigned region(unsigned x, unsigned y) const;

  bool operator==(const SudokuType& other) const;
  bool operator!=(const SudokuType& other) const;

private:
  unsigned n_;
  std::vector<unsigned> region_;

  static std::vector<unsigned> box_regions(unsigned w, unsigned h);
  static std::vector<unsigned> normalize_regions(std::vector<unsigned> regions);
  static unsigned isqrt(unsigned n);
};
