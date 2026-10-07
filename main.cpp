#include "config.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include "problem_data.hpp"
#include <cstdio>

using namespace std;

int main(void) {
  Config config;
  config.M = 20;
  config.N = 20;
  config.xmin = -1.1;
  config.xmax = 1.1;
  config.ymin = -0.1;
  config.ymax = 1.1;

  Grid2D grid(config);
  Domain domain;

  double total = 0.0;

  for (std::size_t i = 0; i < grid.M; ++i) {
    for (std::size_t j = 0; j < grid.N; ++j) {
      Box cell = {{grid.x(i), grid.y(j)}, {grid.x(i + 1), grid.y(j + 1)}};
      total += domain.intersectionArea(cell);
    }
  }

  std::printf("S_ij sum = %g\n", total);
  std::printf("pi / 2   = %g\n", M_PI_2);

  ProblemData problem(grid, domain);

  std::printf("M x N    = %ld x %ld\n", problem.M, problem.N);
  std::printf("h1, h2   = %.3e, %.3e\n", problem.h1, problem.h2);
  std::printf("eps      = %.3e\n", problem.eps);
  std::printf("size     = %zu\n", problem.size());
  std::printf("Q[0]     = %.6e\n", problem.Q[0]);
  std::printf("B[0]     = %.6e\n", problem.B[0]);

  return 0;
}