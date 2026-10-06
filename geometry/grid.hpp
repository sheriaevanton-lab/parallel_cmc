#pragma once
#include "config.hpp"

struct Grid2D {
  int M, N;
  double xmin, ymin;
  double h1, h2;

  double x(int i) const { return xmin + i * h1; }
  double y(int j) const { return ymin + j * h2; }

  double xHalf(int i) const { return xmin + (i + 0.5) * h1; }
  double yHalf(int j) const { return ymin + (j + 0.5) * h2; }

  int index(int i, int j) const { return i + j * (M + 1); }
  int size() const { return (M + 1) * (N + 1); }

  Grid2D(Config config) {
    this->M = config.M;
    this->N = config.N;
    this->xmin = config.xmin;
    this->ymin = config.ymin;
    this->h1 = (config.xmax - config.xmin) / static_cast<double>(M);
    this->h2 = (config.ymax - config.ymin) / static_cast<double>(N);
  }
};