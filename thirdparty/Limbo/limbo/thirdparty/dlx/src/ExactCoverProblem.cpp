#include <limbo/thirdparty/dlx/include/dlx/ExactCoverProblem.hpp>

#include <algorithm>
#include <stdexcept>

ExactCoverProblem::ExactCoverProblem(unsigned width, unsigned secondary_columns)
  : rows_(),
  width_(width),
  secondary_columns_(secondary_columns)
{
  if (secondary_columns_ > width_) {
    throw std::invalid_argument("secondary_columns > width");
  }
}

ExactCoverProblem::ExactCoverProblem(
  unsigned width,
  std::vector<std::vector<unsigned> > rows,
  unsigned secondary_columns
)
  : rows_(),
  width_(width),
  secondary_columns_(secondary_columns)
{
  if (secondary_columns_ > width_) {
    throw std::invalid_argument("secondary_columns > width");
  }
  for (std::vector<std::vector<unsigned> >::const_iterator it = rows.begin(), ite = rows.end(); it != ite; ++it) {
    add_row(*it);
  }
}

ExactCoverProblem ExactCoverProblem::dense(
  const std::vector<std::vector<unsigned> >& bit_rows,
  unsigned secondary_columns)
{
  if (bit_rows.empty()) {
    return ExactCoverProblem(0, secondary_columns);
  }

  std::size_t width = bit_rows[0].size();
  ExactCoverProblem problem = ExactCoverProblem(width, secondary_columns);

  for (std::vector<std::vector<unsigned> >::const_iterator it = bit_rows.begin(), ite = bit_rows.end(); it != ite; ++it) {
    const std::vector<unsigned>& bits = *it; 
    if (bits.size() != width) {
      throw std::invalid_argument("rows have different lengths");
    }
    std::vector<unsigned> row = std::vector<unsigned>();
    for (unsigned i = 0u; i < bits.size(); ++i) {
      if (bits[i] != 0 && bits[i] != 1) {
        throw std::invalid_argument("dense matrix must contain only 0s and 1s");
      }
      if (bits[i]) {
        row.push_back(i);
      }
    }
    problem.add_row(row);
  }

  return problem;
}

unsigned ExactCoverProblem::width() const {
  return width_;
}

const std::vector<std::vector<unsigned> >& ExactCoverProblem::rows() const {
  return rows_;
}

unsigned ExactCoverProblem::secondary_columns() const {
  return secondary_columns_;
}

void ExactCoverProblem::add_row(std::vector<unsigned> row) {
  std::sort(row.begin(), row.end());
  for (std::vector<unsigned>::const_iterator it = row.begin(), ite = row.end(); it != ite; ++it) {
    unsigned x = *it; 
    if (x >= width_) {
      throw std::invalid_argument("column out of range");
    }
  }
  for (unsigned i = 1u; i < row.size(); ++i) {
    if (row[i - 1] == row[i]) {
      throw std::invalid_argument("duplicate columns");
    }
  }
  // mimic rows_.emplace_back(row)
  rows_.push_back(std::vector<unsigned>()); 
  rows_.back().swap(row); 
}

std::ostream& operator<<(std::ostream& os, const ExactCoverProblem& problem) {
  os << problem.width() << ' ' << problem.secondary_columns() << '\n';
  for (std::vector<std::vector<unsigned> >::const_iterator it = problem.rows().begin(), ite = problem.rows().end(); it != ite; ++it) {
    const std::vector<unsigned>& row = *it; 
    for (unsigned i = 0u; i < row.size(); ++i) {
      os << row[i] << " \n"[i + 1 == row.size()];
    }
  }
  return os;
}
