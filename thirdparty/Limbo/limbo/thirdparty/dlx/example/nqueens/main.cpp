#include "NQueens.hpp"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
  bool opt_print_solutions = false;
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == std::string("-v")) {
      opt_print_solutions = true;
      continue;
    }
    auto n = static_cast<unsigned>(std::stoul(argv[i]));
    auto queens = NQueens(n);
    std::cout << "Solutions for n=" << n << ": " << queens.count_solutions() << '\n';
    if (!opt_print_solutions) {
      continue;
    }

    for (const auto& xs : queens.find_solutions()) {
      for (auto y = 0u; y < n; ++y) {
        for (auto x = 0u; x < n; ++x) {
          std::cout << ".Q"[x == xs[y]];
        }
        std::cout << '\n';
      }
      std::cout << '\n';
    }
  }
}
