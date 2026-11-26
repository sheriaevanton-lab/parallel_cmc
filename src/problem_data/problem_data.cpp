#include "problem_data.hpp"

ProblemData::ProblemData(const Grid2D &grid, const Domain &domain)
    : ProblemData(grid, domain,
                  make_local_geometry({1, 1}, 0, grid.M, grid.N)) {}

ProblemData::ProblemData(const Grid2D &grid, const Domain &domain,
                         const LocalGeometry &lg)
    : M(grid.M), N(grid.N), h1(grid.h1), h2(grid.h2), eps(grid.getEpsilon()),
      nx(lg.lx), ny(lg.ly), offset_x(lg.offset_x), offset_y(lg.offset_y) {
  build(grid, domain);
}

void ProblemData::build(const Grid2D &grid, const Domain &domain) {
  const std::size_t n = nx * ny;

  Q.assign(n, 0.0);
  F.assign(n, 0.0);
  v0.assign(n, 0.0);
  B.assign(n, 0.0);

  const double cell_area = h1 * h2;
  const double inv_cell_area = 1.0 / cell_area;
  const double inv_eps2 = 1.0 / (eps * eps);
  const double inv_h1_2 = 1.0 / (h1 * h1);
  const double inv_h2_2 = 1.0 / (h2 * h2);

  const std::size_t last_x = offset_x + nx;
  const std::size_t last_y = offset_y + ny;
  if (offset_y == 0) {
    v0_bottom.resize(nx + 2);
    for (std::size_t i = 0; i < v0_bottom.size(); ++i)
      v0_bottom[i] = domain.v0({grid.x(offset_x + i), grid.y(0)});
  }
  if (last_y == N - 1) {
    v0_top.resize(nx + 2);
    for (std::size_t i = 0; i < v0_top.size(); ++i)
      v0_top[i] = domain.v0({grid.x(offset_x + i), grid.y(N)});
  }
  if (offset_x == 0) {
    v0_left.resize(ny + 2);
    for (std::size_t j = 0; j < v0_left.size(); ++j)
      v0_left[j] = domain.v0({grid.x(0), grid.y(offset_y + j)});
  }
  if (last_x == M - 1) {
    v0_right.resize(ny + 2);
    for (std::size_t j = 0; j < v0_right.size(); ++j)
      v0_right[j] = domain.v0({grid.x(M), grid.y(offset_y + j)});
  }

  for (std::size_t j = offset_y + 1; j <= last_y; ++j) {
    for (std::size_t i = offset_x + 1; i <= last_x; ++i) {
      Box cell = {{grid.xHalf(i - 1), grid.yHalf(j - 1)},
                  {grid.xHalf(i), grid.yHalf(j)}};

      const double S = domain.intersectionArea(cell);

      const std::size_t k = index(i, j);
      Q[k] = (cell_area - S) * inv_cell_area * inv_eps2;
      F[k] = S * inv_cell_area;
      v0[k] = domain.v0({grid.x(i), grid.y(j)});
      B[k] = F[k] + Q[k] * v0[k];
      if (i == 1)
        B[k] += v0_at(0, j) * inv_h1_2;
      if (i == M - 1)
        B[k] += v0_at(M, j) * inv_h1_2;
      if (j == 1)
        B[k] += v0_at(i, 0) * inv_h2_2;
      if (j == N - 1)
        B[k] += v0_at(i, N) * inv_h2_2;
    }
  }
}
