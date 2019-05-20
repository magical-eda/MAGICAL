#include "SudokuType.hpp"
#include "SudokuFormat.hpp"

#include <assert.h>
#include <functional>
#include <unordered_map>
#include <unordered_set>

SudokuType::SudokuType()
  : SudokuType(9)
{
}

SudokuType::SudokuType(unsigned size)
  : SudokuType(isqrt(size), isqrt(size))
{
}

SudokuType::SudokuType(unsigned region_width, unsigned region_height)
  : SudokuType(box_regions(region_width, region_height))
{
}

SudokuType::SudokuType(std::initializer_list<unsigned> regions)
  : SudokuType(std::vector<unsigned>(regions))
{
}

SudokuType::SudokuType(std::vector<unsigned> regions)
  : n_(isqrt(regions.size())),
  region_(normalize_regions(std::move(regions)))
{
  if (n_ == 0) {
    throw std::invalid_argument("Sudoku must have non-zero size");
  }
}

std::shared_ptr<SudokuType> SudokuType::from_size(unsigned size) {
  return std::make_shared<SudokuType>(isqrt(size));
}

std::shared_ptr<SudokuType> SudokuType::guess(const std::string& str) {
  auto size = SudokuFormat::count_cells(str);
  auto n = isqrt(size);
  auto lines = std::vector<std::string>();
  auto line = std::string();
  auto cells = 0u;

  for (char c : str) {
    if (c == '\n') {
      if (cells != 0 && cells != n) {
        return SudokuType::from_size(size);
      }
      lines.push_back(std::move(line));
      cells = 0;
      line.clear();
      continue;
    }
    if (SudokuFormat::is_valid_cell(c)) {
      ++cells;
    }
    if (cells > n) {
      lines.push_back(std::move(line));
      line.clear();
      cells %= n;
    }
    line += c;
  }
  lines.push_back(std::move(line));

  auto region = std::vector<std::vector<unsigned>>();
  std::function<unsigned(unsigned, int, int)> find_region;
  find_region = [&](unsigned id, int x, int y) -> unsigned {
    if (x < 0 || y < 0 || y >= int(lines.size()) || x >= int(lines[y].size())) {
      return 0;
    }
    while (region.size() <= unsigned(y)) {
      region.emplace_back();
    }
    while (region[y].size() <= unsigned(x)) {
      region[y].push_back(0);
    }
    if (region[y][x] != 0) {
      return 0;
    }
    char c = lines[y][x];
    if (!SudokuFormat::is_valid_cell(c) && c != ' ') {
      return 0;
    }
    region[y][x] = id;
    auto region_size = 1u;
    region_size += find_region(id, x, y - 1);
    region_size += find_region(id, x, y + 1);
    region_size += find_region(id, x - 1, y);
    region_size += find_region(id, x + 1, y);
    return region_size;
  };

  auto next_id = 1u;
  for (auto y = 0u; y < lines.size(); ++y) {
    for (auto x = 0u; x < lines[y].size(); ++x) {
      if (!SudokuFormat::is_valid_cell(lines[y][x])) {
        continue;
      }
      auto region_size = find_region(next_id, x, y);
      if (region_size > 0) {
        ++next_id;
      }
    }
  }

  auto region_size = std::unordered_map<unsigned, unsigned>();
  auto final_regions = std::vector<unsigned>();
  auto total_size = 0u;
  for (auto y = 0u; y < lines.size(); ++y) {
    for (auto x = 0u; x < lines[y].size(); ++x) {
      if (SudokuFormat::is_valid_cell(lines[y][x])) {
        ++total_size;
        ++region_size[region[y][x]];
        final_regions.push_back(region[y][x]);
      }
    }
  }

  assert(total_size == size);
  for (auto p : region_size) {
    if (p.second != n) {
      return SudokuType::from_size(size);
    }
  }

  return SudokuType::make(final_regions);
}

bool SudokuType::operator==(const SudokuType& other) const {
  return region_ == other.region_;
}

bool SudokuType::operator!=(const SudokuType& other) const {
  return !(region_ == other.region_);
}

unsigned SudokuType::n() const {
  return n_;
}

unsigned SudokuType::size() const {
  return n_ * n_;
}

unsigned SudokuType::region(unsigned pos) const {
  assert(pos < size());
  return region_[pos];
}

unsigned SudokuType::region(unsigned x, unsigned y) const {
  assert(x < n_ && y < n_);
  return region_[y * n_ + x];
}

std::vector<unsigned> SudokuType::box_regions(unsigned w, unsigned h) {
  std::vector<unsigned> regions;
  unsigned n = w * h;
  for (unsigned y = 0; y < n; ++y) {
    for (unsigned x = 0; x < n; ++x) {
      regions.push_back(y / h * h + x / w);
    }
  }
  return regions;
}

std::vector<unsigned> SudokuType::normalize_regions(std::vector<unsigned> regions) {
  unsigned n = isqrt(regions.size());
  std::unordered_map<unsigned, unsigned> ids;
  std::unordered_map<unsigned, unsigned> areas;
  for (unsigned id : regions) {
    if (ids.find(id) == ids.end()) {
      unsigned next_id = ids.size();
      ids[id] = next_id;
    }
    if (++areas[id] > n) {
      throw std::invalid_argument("Region has wrong size");
    }
  }
  if (ids.size() != n) {
    throw std::invalid_argument("Too many regions");
  }
  for (unsigned& id : regions) {
    id = ids[id];
  }
  return regions;
}

unsigned SudokuType::isqrt(unsigned n) {
  unsigned k = 0;
  while ((k + 1) * (k + 1) <= n) ++k;
  if (k * k != n) {
    throw std::invalid_argument("Not a square");
  }
  return k;
}
