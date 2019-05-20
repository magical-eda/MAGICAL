#include "Sudoku.hpp"
#include "SudokuFormat.hpp"

#include <ctype.h>
#include <algorithm>

Sudoku::Sudoku()
  : Sudoku(std::make_shared<SudokuType>())
{
}

Sudoku::Sudoku(std::shared_ptr<SudokuType> type)
  : Sudoku(type, std::vector<unsigned>(type->size(), 0))
{
}

Sudoku::Sudoku(const std::string& str)
  : Sudoku(SudokuType::guess(str), str)
{
}

Sudoku::Sudoku(std::shared_ptr<SudokuType> type, const std::string& str)
  : Sudoku(std::move(type), SudokuFormat::get_values(str))
{
}

Sudoku::Sudoku(std::shared_ptr<SudokuType> type, std::vector<unsigned> values)
  : type_(std::move(type)),
  values_(std::move(values))
{
}

const SudokuType& Sudoku::type() const {
  return *type_;
}

unsigned Sudoku::size() const {
  return type_->size();
}

bool Sudoku::is_empty() const {
  for (auto v : values_) {
    if (v > 0) {
      return false;
    }
  }
  return true;
}

bool Sudoku::is_valid() const {
  unsigned n = type_->n();
  for (unsigned i = 0; i < type_->size(); ++i) {
    for (unsigned j = i + 1; j < type_->size(); ++j) {
      unsigned a = values_[i];
      unsigned b = values_[j];
      if (a == 0 || a != b) {
        continue;
      }
      if (i % n == j % n) {
        return false;
      }
      if (i / n == j / n) {
        return false;
      }
      if (type_->region(i) == type_->region(j)) {
        return false;
      }
    }
  }
  return true;
}

bool Sudoku::is_solved() const {
  return is_valid() && *std::min_element(values_.begin(), values_.end()) > 0;
}

unsigned& Sudoku::operator[](unsigned pos) {
  return values_[pos];
}

unsigned Sudoku::operator[](unsigned pos) const {
  return values_[pos];
}

bool Sudoku::operator==(const Sudoku& other) const {
  return *type_ == *other.type_ && values_ == other.values_;
}

bool Sudoku::operator!=(const Sudoku& other) const {
  return !(*this == other);
}

std::string Sudoku::to_string() const {
  return to_string(SudokuFormat(type_));
}

std::string Sudoku::to_string(const SudokuFormat& format) const {
  if (format.type() != *type_) {
    throw std::invalid_argument("");
  }
  return format.to_string(values_);
}
