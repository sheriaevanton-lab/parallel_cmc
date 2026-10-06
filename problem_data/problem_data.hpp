#pragma once

#include "config.hpp"
#include "geometry.hpp"
#include "grid.hpp"

#include <vector>

class ProblemData {
public:
  ProblemData(const Grid2D &grid, const Domain &domain);

  int M, N;
  double h1, h2;
  double eps;

  std::vector<double> Q;
  std::vector<double> F;
  std::vector<double> v0;
  std::vector<double> B;

  int idx(int i, int j) const { return (i - 1) + (j - 1) * (M - 1); }

  std::size_t size() const { return Q.size(); }

private:
  void build(const Grid2D &grid, const Domain &domain);
};