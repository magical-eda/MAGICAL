#include <limbo/thirdparty/dlx/include/dlx/AlgorithmDLX.hpp>

#include <assert.h>
#include <stdexcept>

AlgorithmDLX::AlgorithmDLX(const ExactCoverProblem& problem)
  : A_(problem)
{
}

unsigned AlgorithmDLX::count_solutions(){
  Options options = Options();
  options.get_solutions = false;
  return search(options).number_of_solutions;
}

std::vector<AlgorithmDLX::Solution> AlgorithmDLX::find_solutions(unsigned max) {
  Options options = Options();
  options.max_solutions = max;
  return search(options).solutions;
}

AlgorithmDLX::Result AlgorithmDLX::search() {
  return search(Options());
}

AlgorithmDLX::Result AlgorithmDLX::search(const Options& options) {
  if (options.choose_random_column && options.random_engine == NULL) {
    throw std::runtime_error("Random engine must be given");
  }
  Result result = Result();
  SearchState state = SearchState();
  search(result, options, state);
  return result;
}

void AlgorithmDLX::search(Result& result, const Options& options, SearchState& state) {
  if (state.stopped) {
    return;
  }

  while (result.profile.size() <= state.stack.size()) {
    result.profile.push_back(0);
  }
  ++result.profile[state.stack.size()];

  NodeId h = A_.root_id();
  if (R(h) == h) {
    ++result.number_of_solutions;
    if (options.get_solutions) {
      result.solutions.push_back(state.stack);
    }
    if (result.number_of_solutions >= options.max_solutions) {
      state.stopped = true;
    }
    return;
  }

  std::vector<NodeId> cs = std::vector<NodeId>();
  for (NodeId j = R(h); j != h; j = R(j)) {
    if (!cs.empty() && S(j) < S(cs[0])) {
      cs.clear();
    }
    if (cs.empty() || S(j) == S(cs[0])) {
      cs.push_back(j);
    }
  }
  assert(!cs.empty());
  if (S(cs[0]) < 1) {
    return;
  }

  NodeId c = cs[0];
  if (options.choose_random_column) {
    boost::uniform_int<unsigned> randint = boost::uniform_int<unsigned>(0, cs.size() - 1);
    c = cs[randint(*options.random_engine)];
  }

  cover_column(c);
  for (NodeId r = D(c); r != c; r = D(r)) {
    state.stack.push_back(Y(r));
    for (NodeId j = R(r); j != r; j = R(j))
      cover_column(j);
    search(result, options, state);
    for (NodeId j = L(r); j != r; j = L(j))
      uncover_column(j);
    state.stack.pop_back();
  }
  uncover_column(c);
}
