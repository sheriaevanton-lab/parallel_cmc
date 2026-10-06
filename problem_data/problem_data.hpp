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

  std::vector<double> v0_bottom; // j = 0,     i = 0..M
  std::vector<double> v0_top;    // j = N,     i = 0..M
  std::vector<double> v0_left;   // i = 0,     j = 0..N
  std::vector<double> v0_right;  // i = M,     j = 0..N

  int index(int i, int j) const { return (i - 1) + (j - 1) * (M - 1); }

  double v0_at(int i, int j) const {
    if (i == 0)
      return v0_left[j];
    if (i == M)
      return v0_right[j];
    if (j == 0)
      return v0_bottom[i];
    if (j == N)
      return v0_top[i];
    return v0[index(i, j)];
  }

  std::size_t size() const { return Q.size(); }

private:
  void build(const Grid2D &grid, const Domain &domain);
};