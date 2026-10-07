#pragma once

#include "config.hpp"
#include <algorithm>

struct Grid2D {
  std::size_t M, N;
  double xmin, ymin;
  double h1, h2;

  double x(std::size_t i) const { return xmin + i * h1; }
  double y(std::size_t j) const { return ymin + j * h2; }

  double xHalf(std::size_t i) const { return xmin + (i + 0.5) * h1; }
  double yHalf(std::size_t j) const { return ymin + (j + 0.5) * h2; }

  std::size_t index(std::size_t i, std::size_t j) const {
    return i + j * (M + 1);
  }
  std::size_t size() const { return (M + 1) * (N + 1); }

  double getEpsilon() const { return std::max(h1, h2) * std::max(h1, h2); }

  Grid2D(Config config) {
    this->M = config.M;
    this->N = config.N;
    this->xmin = config.xmin;
    this->ymin = config.ymin;
    this->h1 = (config.xmax - config.xmin) / static_cast<double>(M);
    this->h2 = (config.ymax - config.ymin) / static_cast<double>(N);
  }
};