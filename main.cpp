#include "backend/sequential.hpp"
#include "config.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"
#include "problem_data/problem_data.hpp"
#include "solver/solver.hpp"
#include <iostream>

int main(void) {
  Config config;
  config.M = 20;
  config.N = 20;
  config.xmin = -1.1;
  config.xmax = 1.1;
  config.ymin = -0.1;
  config.ymax = 1.1;
  config.delta = 1e-5;
  config.max_iter = 1e5;

  Grid2D grid(config);
  Domain domain;
  ProblemData problem(grid, domain);

  SequentialBackend backend(problem);
  const auto solver = ConjugateGradient(backend);

  std::vector<double> w((problem.M - 1) * (problem.N - 1));
  const auto result = solver.solve(problem.B, w, config.delta, config.max_iter);
  std::cout << "delta_w_norm: " << result.residual
            << " iterations: " << result.iterations << std::endl;
  return 0;
}