#pragma once

#include "SudokuFormat.hpp"
#include "SudokuType.hpp"

#include <memory>
#include <string>
#include <vector>

class Sudoku
{
public:
  Sudoku();
  explicit Sudoku(std::shared_ptr<SudokuType> type);
  explicit Sudoku(const std::string& str);
  explicit Sudoku(const std::vector<unsigned>& values);
  Sudoku(std::shared_ptr<SudokuType> type, const std::string& str);
  Sudoku(std::shared_ptr<SudokuType> type, std::vector<unsigned> values);

  const SudokuType& type() const;
  unsigned size() const;

  bool is_empty() const;
  bool is_valid() const;
  bool is_solved() const;

  unsigned& operator[](unsigned pos);
  unsigned  operator[](unsigned pos) const;

  bool operator==(const Sudoku& other) const;
  bool operator!=(const Sudoku& other) const;

  std::string to_string() const;
  std::string to_string(const SudokuFormat& format) const;

private:
  std::shared_ptr<SudokuType> type_;
  std::vector<unsigned> values_;
};
