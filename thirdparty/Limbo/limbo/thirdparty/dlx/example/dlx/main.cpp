#include <limbo/thirdparty/dlx/include/dlx/AlgorithmDLX.hpp>

#include <unistd.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>

namespace {

template <typename T>
void print_range(const T& xs) {
  typename T::const_iterator it = xs.begin();
  typename T::const_iterator end = xs.end();
  for (; it != end;) {
    std::cout << *it;
    std::cout << " \n"[++it == end];
  }
}

}

int main(int argc, char *argv[]) {
  bool opt_print_solutions = false;
  bool opt_verbose = false;
  bool opt_sparse = false;
  bool opt_print_tree = false;

  for (int opt; (opt = getopt(argc, argv, "pvst")) != -1;) {
    switch (opt) {
    case 'p':
      opt_print_solutions = true;
      break;
    case 'v':
      opt_verbose = true;
      opt_print_solutions = true;
      break;
    case 's':
      opt_sparse = true;
      break;
    case 't':
      opt_print_tree = true;
      break;
    default:
      std::cerr << "Unexpected option: '" << opt << "'\n";
      return 1;
    }
  }

  unsigned width = 0;
  unsigned secondary_columns = 0;
  {
    std::string line;
    std::getline(std::cin, line);
    std::istringstream ss(line);
    ss >> width >> secondary_columns;
  }

  std::vector<std::vector<unsigned> > input_rows;
  for (std::string line; std::getline(std::cin, line);) {
    std::istringstream ss(line);

    std::vector<unsigned> row;
    if (opt_sparse) {
      for (unsigned x; ss >> x;) {
        row.push_back(x);
      }
      std::sort(row.begin(), row.end());
    }
    else {
      row.resize(width);
      for (std::vector<unsigned>::iterator it = row.begin(), ite = row.end(); it != ite; ++it) {
        ss >> *it;
      }
    }
    if (!row.empty()) {
      input_rows.push_back(row);
    }
  }

  ExactCoverProblem problem = opt_sparse ? ExactCoverProblem(width, input_rows, secondary_columns)
    : ExactCoverProblem::dense(input_rows, secondary_columns);

  AlgorithmDLX dlx(problem);
  AlgorithmDLX::Result result = dlx.search();
  for (std::vector<AlgorithmDLX::Solution>::const_iterator it = result.solutions.begin(), ite = result.solutions.end(); it != ite; ++it) {
    const AlgorithmDLX::Solution& row_indices = *it; 
    if (opt_print_solutions) {
      if (opt_verbose) {
        for (AlgorithmDLX::Solution::const_iterator it2 = row_indices.begin(), it2e = row_indices.end(); it2 != it2e; ++it2) {
          unsigned i = *it2; 
          print_range(input_rows[i]);
        }
        std::cout << '\n';
      }
      else {
        print_range(row_indices);
      }
    }
  }
  std::cout << "solutions: " << result.number_of_solutions << '\n';

  if (opt_print_tree) {
    std::cout << '\n';
    for (unsigned i = 0u; i < result.profile.size(); ++i) {
      std::cout << "Nodes at depth " << i << ": " << result.profile[i] << '\n';
    }
  }
}
