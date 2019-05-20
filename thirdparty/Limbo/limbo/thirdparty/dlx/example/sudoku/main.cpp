#include "SudokuGenerator.hpp"
#include "SudokuSolver.hpp"
#include "SudokuType.hpp"

#include <stdlib.h>
#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <string>

static void show_usage_and_exit() {
  std::cout << "Help message goes here\n";
  ::exit(1);
}

static std::vector<std::string> lines(const std::string& str) {
  auto result = std::vector<std::string>();
  auto line = std::string();
  for (char c : str) {
    if (c == '\n') {
      result.push_back(std::move(line));
      line.clear();
    }
    else {
      line += c;
    }
  }
  if (!line.empty()) {
    result.push_back(std::move(line));
  }
  return result;
}

static void print_side_by_side(const std::string& left, const std::string& right) {
  auto ls = lines(left);
  auto rs = lines(right);
  auto max_left = 0u;
  for (const auto& l : ls) {
    max_left = std::max<unsigned>(max_left, l.size());
  }
  for (auto y = 0u; y < std::max(ls.size(), rs.size()); ++y) {
    auto pos = 0u;
    if (y < ls.size()) {
      std::cout << ls[y];
      pos = ls[y].size();
    }
    if (y < rs.size()) {
      std::cout << std::string(4 + max_left - pos, ' ');
      std::cout << rs[y];
    }
    std::cout << '\n';
  }
}

int main(int argc, char **argv) {
  enum class Format { DEFAULT, ONELINE, COMPACT, PRESERVE };
  std::map<std::string, Format> format_names{
    {"default", Format::DEFAULT},
    {"oneline", Format::ONELINE},
    {"compact", Format::COMPACT},
    {"preserve", Format::PRESERVE},
  };

  auto opt_format = Format::PRESERVE;
  auto opt_one_sudoku_per_line = false;
  auto opt_side_by_side = true;
  auto opt_print_solved_only = false;

  for (int opt; (opt = ::getopt(argc, argv, "f:hlsv")) != -1;) {
    switch (opt) {
      case 'h':
        show_usage_and_exit();
        break;

      case 'l':
        opt_one_sudoku_per_line = true;
        break;

      case 's':
        opt_print_solved_only = true;
        break;

      case 'v': // vertically
        opt_side_by_side = false;
        break;

      case 'f': {
        auto it = format_names.find(::optarg);
        if (it == format_names.end()) {
          std::cerr << "Invalid argument for -f '" << ::optarg << "'\n";
          show_usage_and_exit();
        }
        opt_format = it->second;
        break;
      }

      default:
        show_usage_and_exit();
    }
  }

  auto generator = SudokuGenerator();
  std::string input;
  bool first = true;
  for (std::string line; std::cin;) {
    if (std::getline(std::cin, line) && !line.empty()) {
      input += line;
      if (!opt_one_sudoku_per_line) {
        input += '\n';
        continue;
      }
    }

    if (input.empty()) {
      continue;
    }

    try {
      auto type = SudokuType::guess(input);
      auto format = (
        opt_format == Format::PRESERVE ? SudokuFormat(type, input) :
        opt_format == Format::COMPACT ? SudokuFormat::compact(type) :
        opt_format == Format::ONELINE ? SudokuFormat::oneline(type) :
        SudokuFormat(type)
      );

      if (!first && opt_format != Format::ONELINE) {
        std::cout << '\n';
      }
      first = false;

      auto sudoku = Sudoku(type, input);
      if (sudoku.is_empty()) {
        sudoku = generator(type);
      }

      auto solved = SudokuSolver().solve(sudoku);
      if (!opt_print_solved_only && opt_side_by_side) {
        print_side_by_side(sudoku.to_string(format), solved.to_string(format));
        input.clear();
        continue;
      }

      if (!opt_print_solved_only) {
        std::cout << sudoku.to_string(format);
        if (opt_format != Format::ONELINE) {
          std::cout << '\n';
        }
      }
      std::cout << solved.to_string(format);
    }
    catch (const std::exception& ex) {
      std::cout << '\n';
      std::cout << "ERROR! " << ex.what() << '\n';
      std::cout << input;
    }

    input.clear();
  }
}
