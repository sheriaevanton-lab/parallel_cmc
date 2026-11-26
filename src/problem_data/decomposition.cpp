#include "decomposition.hpp"
#include <algorithm>
#include <limits>
#include <stdexcept>

Decomposition make_decomposition(std::size_t M, std::size_t N, int num_proc) {
  if (num_proc <= 0 || (num_proc & (num_proc - 1))) {
    throw std::runtime_error("number of processes has to be a power of two");
  }
  if (M < 2 || N < 2) {
    throw std::runtime_error("M and N must be at least 2");
  }

  Decomposition decomp{};
  const std::size_t Kx = M - 1;
  const std::size_t Ky = N - 1;
  const std::size_t size = static_cast<std::size_t>(num_proc);
  double best_score = std::numeric_limits<double>::infinity();

  for (std::size_t px = 1; px <= size; px *= 2) {
    const std::size_t py = size / px;
    if (px > Kx || py > Ky)
      continue;

    const std::size_t nx_min = Kx / px;
    const std::size_t nx_max = nx_min + (Kx % px != 0);
    const std::size_t ny_min = Ky / py;
    const std::size_t ny_max = ny_min + (Ky % py != 0);
    const double score = std::max(static_cast<double>(nx_max) / ny_min,
                                  static_cast<double>(ny_max) / nx_min);

    if (score < best_score) {
      best_score = score;
      decomp = {px, py};
    }
  }

  if (decomp.px == 0) {
    throw std::runtime_error("process grid would contain empty blocks");
  }

  return decomp;
}

LocalGeometry make_local_geometry(const Decomposition &d, int rank,
                                  std::size_t M, std::size_t N) {
  const std::size_t x = rank % d.px;
  const std::size_t y = rank / d.px;
  const std::size_t nx = (M - 1) / d.px;
  const std::size_t ny = (N - 1) / d.py;
  const std::size_t rx = (M - 1) % d.px;
  const std::size_t ry = (N - 1) % d.py;
  const int stride = static_cast<int>(d.px);

  return {rank,
          nx + (x < rx),
          ny + (y < ry),
          x * nx + std::min(x, rx),
          y * ny + std::min(y, ry),
          x > 0 ? rank - 1 : LocalGeometry::no_neighbor,
          x + 1 < d.px ? rank + 1 : LocalGeometry::no_neighbor,
          y > 0 ? rank - stride : LocalGeometry::no_neighbor,
          y + 1 < d.py ? rank + stride : LocalGeometry::no_neighbor};
}
