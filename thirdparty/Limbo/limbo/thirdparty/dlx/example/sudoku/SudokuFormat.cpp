#include "SudokuFormat.hpp"

#include <assert.h>
#include <set>
#include <unordered_set>

SudokuFormat::SudokuFormat(std::string format)
  : SudokuFormat{SudokuType::guess(format), std::move(format)}
{
}

SudokuFormat::SudokuFormat(std::shared_ptr<SudokuType> type)
  : SudokuFormat(type, default_template(*type))
{
}

SudokuFormat::SudokuFormat(std::shared_ptr<SudokuType> type, std::string format)
  : type_(std::move(type)),
  template_(std::move(format)),
  labels_(choose_labels(template_))
{
  unsigned cells = 0;
  for (char& c : template_) {
    if (is_cell(c)) {
      c = '.';
      ++cells;
    }
  }
  if (cells != type_->size()) {
    throw std::invalid_argument("");
  }
}

SudokuFormat SudokuFormat::compact(std::shared_ptr<SudokuType> type) {
  std::string line(type->n(), '.');
  line += '\n';
  std::string result;
  for (unsigned y = 0; y < type->n(); ++y) {
    result += line;
  }
  return SudokuFormat(std::move(type), result);
}

SudokuFormat SudokuFormat::oneline(std::shared_ptr<SudokuType> type) {
  return SudokuFormat(type, std::string(type->size(), '.') + '\n');
}

SudokuFormat SudokuFormat::with_labels(const std::string& str) const {
  auto labels = choose_labels(str, type_->n());
  auto format = *this;
  format.labels_ = std::move(labels);
  return format;
}

unsigned SudokuFormat::count_cells(const std::string& str) {
  unsigned count = 0;
  for (char c : str) {
    if (is_valid_label(c) || is_empty(c)) {
      ++count;
    }
  }
  return count;
}

bool SudokuFormat::is_cell(char c) const {
  return is_empty(c) || labels_.find(c) != std::string::npos;
}

unsigned SudokuFormat::value(char c) const {
  auto pos = labels_.find(c);
  return pos == std::string::npos ? 0 : pos + 1;
}

char SudokuFormat::label(unsigned i) const {
  assert(i <= labels_.size());
  return i == 0 ? '.' : labels_[i - 1];
}

const std::string& SudokuFormat::labels() const {
  return labels_;
}

std::vector<unsigned> SudokuFormat::get_values(const std::string& str) {
  auto labels = choose_labels(str);
  auto values = std::vector<unsigned>();
  for (char c : str) {
    auto pos = labels.find(c);
    if (pos != std::string::npos) {
      values.push_back(pos + 1);
    }
    else if (is_empty(c)) {
      values.push_back(0);
    }
  }
  return values;
}

std::string SudokuFormat::to_string() const {
  return to_string(std::vector<unsigned>(type_->size(), 0));
}

std::string SudokuFormat::to_string(const std::vector<unsigned>& values) const {
  if (values.size() != type_->size()) {
    throw std::invalid_argument("to_string(): wrong number of values ("
      + std::to_string(values.size()) + " vs. " + std::to_string(type_->size()) + ')');
  }

  std::string result = template_;
  unsigned j = 0;
  for (unsigned i = 0; i < result.size(); ++i) {
    if (is_cell(result[i])) {
      if (j >= values.size()) {
        throw std::logic_error("");
      }
      result[i] = label(values[j]);
      ++j;
    }
  }
  if (j != values.size()) {
    throw std::logic_error("");
  }
  return result;
}

const SudokuType& SudokuFormat::type() const {
  return *type_;
}

bool SudokuFormat::is_empty(char c) {
  return c == '.' || c == '0';
}

bool SudokuFormat::is_valid_label(char c) {
  return valid_labels().find(c) != std::string::npos;
}

bool SudokuFormat::is_valid_cell(char c) {
  return is_valid_label(c) || is_empty(c);
}

const std::string& SudokuFormat::valid_labels() {
  static const std::string chars(
    "123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
  );
  return chars;
}

std::string SudokuFormat::choose_labels(const std::string& str) {
  auto size = 0u;
  for (char c : str) {
    if (is_valid_cell(c)) {
      ++size;
    }
  }
  auto n = 0u;
  while ((n + 1) * (n + 1) <= size) {
    ++n;
  }
  if (n * n != size) {
    throw std::invalid_argument(":(");
  }
  return choose_labels(str, n);
}

std::string SudokuFormat::choose_labels(const std::string& str, unsigned n) {
  auto used = std::set<char>();
  for (char c : str) {
    if (is_valid_label(c)) {
      used.insert(c);
    }
  }

  if (used.size() > n) {
    throw std::invalid_argument("Too many different labels");
  }

  auto has_digit = false;
  auto has_upper = false;
  auto has_lower = false;
  for (char c : used) {
    if (::isdigit(c)) {
      has_digit = true;
    }
    else if (::isupper(c)) {
      has_upper = true;
    }
    else if (::islower(c)) {
      has_lower = true;
    }
  }

  for (char c : valid_labels()) {
    if (used.size() >= n) {
      continue;
    }
    if ((::isdigit(c) && has_digit)
        || (::islower(c) && has_lower)
        || (::isupper(c) && has_upper))
    {
      used.insert(c);
    }
  }

  for (char c : valid_labels()) {
    if (used.size() < n) {
      used.insert(c);
    }
  }

  if (used.size() < n) {
    throw std::invalid_argument("Sudoku too large, not enough labels");
  }

  std::string labels;
  for (char c : used) {
    labels += c;
  }
  return labels;
}

std::string SudokuFormat::default_template(const SudokuType& type) {
  /*
   * The template is built in four steps.
   *
   * 1) empty
   *
   * +-------+
   * |. . . .|
   * |       |
   * |. . . .|
   * |       |
   * |. . . .|
   * |       |
   * |. . . .|
   * +-------+
   *
   * 2) add vertical lines
   *
   * +-----+-+
   * |. . .|.|
   * | |   | |
   * |.|. . .|
   * | | |   |
   * |. .|. .|
   * |   |   |
   * |. .|. .|
   * +---+---+
   *
   * 3) add horizontal lines
   *
   * +-----+-+
   * |. . .|.|
   * | +---+ |
   * |.|. . .|
   * +-+-+---+
   * |. .|. .|
   * |   |   |
   * |. .|. .|
   * +---+---+
   *
   * 4) collapse uninteresting rows and columns
   *
   * +-----+-+
   * |. . .|.|
   * | +---+ |
   * |.|. . .|
   * +-+-+---+
   * |. .|. .|
   * |. .|. .|
   * +---+---+
   *
   */

  // step 1: empty
  std::vector<std::string> lines;
  unsigned n = type.n();
  std::string header(n * 2 + 1, '-');
  std::string empty(n * 2 + 1, ' ');
  *header.begin() = '+';
  *header.rbegin() = '+';
  *empty.begin() = '|';
  *empty.rbegin() = '|';

  lines.push_back(header);
  for (unsigned i = 0; i < 2 * n - 1; ++i) {
    lines.push_back(empty);
  }
  lines.push_back(header);

  for (unsigned y = 0; y < n; ++y) {
    for (unsigned x = 0; x < n; ++x) {
      lines[2 * y + 1][2 * x + 1] = '.';
    }
  }

  // step 2: add vertical lines
  auto set = [&](unsigned x, unsigned y, char c) {
    if (lines[y][x] == ' ') {
      lines[y][x] = c;
    }
    else if (lines[y][x] != c) {
      lines[y][x] = '+';
    }
  };

  for (unsigned y = 0; y < n; ++y) {
    for (unsigned x = 0; x + 1 < n; ++x) {
      if (type.region(x, y) != type.region(x + 1, y)) {
        set(2 * x + 2, 2 * y, '|');
        set(2 * x + 2, 2 * y + 1, '|');
        set(2 * x + 2, 2 * y + 2, '|');
      }
    }
  }

  // step 3: add horizontal lines
  for (unsigned y = 0; y + 1 < n; ++y) {
    for (unsigned x = 0; x < n; ++x) {
      if (type.region(x, y) != type.region(x, y + 1)) {
        set(2 * x, 2 * y + 2, '-');
        set(2 * x + 1, 2 * y + 2, '-');
        set(2 * x + 2, 2 * y + 2, '-');
      }
    }
  }

  // step 4: collapse uninteresting rows and columns
  unsigned Y = lines.size();
  unsigned X = lines[0].size();
  std::vector<bool> keep_row(Y);
  std::vector<bool> keep_col(X);
  for (unsigned y = 0; y < Y; ++y) {
    for (unsigned x = 0; x < X; ++x) {
      char c = lines[y][x];
      if (c != ' ' && c != '|') {
        keep_row[y] = true;
      }
      if (c != ' ' && c != '-') {
        keep_col[x] = true;
      }
    }
  }

  std::string result;
  for (unsigned y = 0; y < Y; ++y) {
    if (!keep_row[y]) {
      continue;
    }
    for (unsigned x = 0; x < X; ++x) {
      if (keep_col[x]) {
        result += lines[y][x];
      }
    }
    result += '\n';
  }
  return result;
}
