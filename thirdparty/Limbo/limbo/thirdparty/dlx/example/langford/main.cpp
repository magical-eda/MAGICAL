#include "Langford.hpp"

#include <dlx/AlgorithmDLX.hpp>

#include <iostream>
#include <string>

int main(int argc, char **argv) {
  bool opt_verbose = false;
  for (int i = 1; i < argc; ++i) {
    if (argv[i] == std::string("-v")) {
      opt_verbose = true;
      continue;
    }

    auto n = static_cast<unsigned>(std::stoi(argv[i]));
    auto langford = Langford(n);
    auto dlx = AlgorithmDLX(langford.problem());
    auto options = AlgorithmDLX::Options();
    options.get_solutions = opt_verbose;
    auto result = dlx.search(options);

    std::cout
      << "Solutions for n=" << n << ": "
      << result.number_of_solutions << '\n';
    if (opt_verbose) {
      for (const auto& used_rows : result.solutions) {
        auto solution = langford.make_solution(used_rows);
        for (auto i = 0u; i < solution.size(); ++i) {
          std::cout << solution[i] << " \n"[i == solution.size() - 1];
        }
      }
    }
  }
}
