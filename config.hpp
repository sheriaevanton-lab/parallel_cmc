#pragma once

#include <cstddef>

struct Config {
  std::size_t M, N;
  double xmin, xmax, ymin, ymax;
  double delta;
  std::size_t max_iter;
};