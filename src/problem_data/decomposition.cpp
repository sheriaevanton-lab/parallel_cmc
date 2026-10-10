#include "decomposition.hpp"
#include <limits>
#include <stdexcept>

Decomposition make_decomposition(std::size_t M, std::size_t N, int num_proc) {
  if (num_proc & (num_proc - 1)) {
    throw std::runtime_error("number of processes has to be a power of two");
  }

  Decomposition decomp;
  const std::size_t Kx = M - 1;
  const std::size_t Ky = N - 1;
  const std::size_t size = static_cast<std::size_t>(num_proc);
  double best_score = std::numeric_limits<double>::infinity();

  bool found = false;

  for (std::size_t px = 1; px <= size; ++px) {
    if (size % px)
      continue;

    std::size_t py = size / px;

    std::size_t sx_base = Kx / px, sx_rem = Kx % px;
    std::size_t sy_base = Ky / py, sy_rem = Ky % py;

    std::size_t nx_min = sx_base;
    std::size_t nx_max = sx_base + (sx_rem > 0 ? 1 : 0);
    std::size_t ny_min = sy_base;
    std::size_t ny_max = sy_base + (sy_rem > 0 ? 1 : 0);

    double ratio_max = double(nx_max) / double(ny_min);
    double ratio_min = double(nx_min) / double(ny_max);

    if (ratio_min < 0.5 || ratio_max > 2.0)
      continue;

    double score = std::max(ratio_max, 1.0 / ratio_min);

    if (!found || score < best_score) {
      best_score = score;
      decomp = {px, py};
      found = true;
    }
  }

  if (!found) {
    std::size_t px = 1;
    for (std::size_t d = 1; d * d <= size; ++d)
      if (size % d == 0)
        px = d;
    std::size_t py = size / px;
    decomp = {px, py};
  }

  return decomp;
}

LocalGeometry make_local_geometry(const Decomposition &d, int rank,
                                  std::size_t M, std::size_t N) {
  LocalGeometry lg;
  return lg;
}