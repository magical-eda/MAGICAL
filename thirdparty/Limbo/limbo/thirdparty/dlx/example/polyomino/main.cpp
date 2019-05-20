#include "Polyomino.hpp"

#include <dlx/AlgorithmDLX.hpp>

int main() {
  auto scotts_problem = Polyomino(Shape::pentominoes(), {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
  });
  auto result = AlgorithmDLX(scotts_problem.problem()).search();
  for (const auto& used_rows : result.solutions) {
    auto solution = scotts_problem.make_solution(used_rows);
    for (const auto& line : solution) {
      std::cout << line << '\n';
    }
    std::cout << '\n';
  }
  std::cout << result.number_of_solutions << '\n';
}
