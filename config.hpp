#pragma once

#include "backend/backend_kind.hpp"
#include <cstddef>
#include <string>

struct Config {
  std::size_t M, N;
  double xmin, xmax, ymin, ymax;
  double delta;
  std::size_t max_iter;
  std::string output_path;
  BackendKind backend;
};