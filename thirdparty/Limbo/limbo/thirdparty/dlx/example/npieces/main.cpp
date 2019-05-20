#include "NPieces.hpp"

#include <iostream>
#include <map>
#include <string>

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0] << " <width> <height> <knights> <queens>\n";
    return 1;
  }

  auto width = static_cast<unsigned>(std::stoi(argv[1]));
  auto height = static_cast<unsigned>(std::stoi(argv[2]));
  auto knights = static_cast<unsigned>(std::stoi(argv[3]));
  auto queens = static_cast<unsigned>(std::stoi(argv[4]));

  auto npieces = NPieces().size(width, height).knights(knights).queens(queens);
  auto solutions = npieces.find_solutions();
  std::cout << "Solutions for " << width << 'x' << height << ", N=" <<
    knights << ", Q=" << queens << ": " << solutions.size() << '\n';

  auto chr = std::map<NPieces::Piece, char>{
    {NPieces::Piece::None, '.'},
    {NPieces::Piece::Knight, 'N'},
    {NPieces::Piece::Queen, 'Q'},
  };

  for (const auto& solution : solutions) {
    for (const auto& row : solution) {
      for (auto piece : row) {
        std::cout << chr[piece];
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
}
