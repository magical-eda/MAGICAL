#pragma once

#include <limbo/thirdparty/dlx/include/dlx/LinkedMatrix.hpp>

#include <stdint.h>
#include <functional>
#include <limits>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

class AlgorithmDLX {
public:
  typedef std::vector<unsigned> Solution;

  struct Options
  {
    bool choose_random_column;
    bool get_solutions;
    unsigned max_solutions;
    boost::mt19937 *random_engine;

    Options() {
        choose_random_column = false; 
        get_solutions = true; 
        max_solutions = std::numeric_limits<unsigned>::max();
        random_engine = NULL; 
    }
  };

  struct Result
  {
    unsigned number_of_solutions;
    std::vector<Solution> solutions;
    std::vector<unsigned> profile;

    Result() {
        number_of_solutions = 0; 
    }
  };

  explicit AlgorithmDLX(const ExactCoverProblem& problem);

  Result search();
  Result search(const Options&);
  unsigned count_solutions();
  std::vector<Solution> find_solutions(unsigned max = ~0u);

private:
  LinkedMatrix A_;

  struct SearchState;
  void search(Result& result, const Options& options, SearchState& state);

  typedef LinkedMatrix::NodeId NodeId; 
  void cover_column(NodeId id) { A_.cover_column(id); }
  void uncover_column(NodeId id) { A_.uncover_column(id); }
  unsigned Y(NodeId id) { return A_.Y(id); }
  unsigned S(NodeId id) { return A_.S(id); }
  NodeId L(NodeId id) { return A_.L(id); }
  NodeId R(NodeId id) { return A_.R(id); }
  NodeId U(NodeId id) { return A_.U(id); }
  NodeId D(NodeId id) { return A_.D(id); }
};

struct AlgorithmDLX::SearchState
{
  std::vector<unsigned> stack;
  bool stopped;

  SearchState() {
      stopped = false; 
  }
};
