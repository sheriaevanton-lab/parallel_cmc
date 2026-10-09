#pragma once

#include "backend/backend.hpp"
#include <string>

struct Config {
  int M, N;
  double xmin, xmax, ymin, ymax;
  double delta;
  int max_iter;
  std::string output_path;
  std::optional<BackendKind> backend;
};