#include "problem_data.hpp"

ProblemData::ProblemData(const Grid2D &grid, const Domain &domain)
    : M(grid.M), N(grid.N), h1(grid.h1), h2(grid.h2), eps(grid.getEpsilon()) {
  build(grid, domain);
}

void ProblemData::build(const Grid2D &grid, const Domain &domain) {
  const int nx = M - 1;
  const int ny = N - 1;
  const std::size_t n = static_cast<std::size_t>(nx) * ny;

  Q.assign(n, 0.0);
  F.assign(n, 0.0);
  v0.assign(n, 0.0);
  B.assign(n, 0.0);

  const double cell_area = h1 * h2;
  const double inv_cell_area = 1.0 / cell_area;
  const double inv_eps2 = 1.0 / (eps * eps);

  for (int j = 1; j <= N - 1; ++j) {
    for (int i = 1; i <= M - 1; ++i) {
      Box cell = {{grid.xHalf(i - 1), grid.yHalf(j - 1)},
                  {grid.xHalf(i), grid.yHalf(j)}};

      const double S = domain.intersectionArea(cell);

      const int k = idx(i, j);
      Q[k] = (cell_area - S) * inv_cell_area * inv_eps2;
      F[k] = S * inv_cell_area;
      v0[k] = domain.v0({grid.x(i), grid.y(j)});
      B[k] = F[k] + Q[k] * v0[k];
    }
  }
}