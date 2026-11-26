#pragma once

#include "decomposition.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"

#include <vector>

class ProblemData {
public:
  ProblemData(const Grid2D &grid, const Domain &domain);
  ProblemData(const Grid2D &grid, const Domain &domain,
              const LocalGeometry &lg);

  std::size_t M, N;
  double h1, h2;
  double eps;

  std::vector<double> Q;
  std::vector<double> F;
  std::vector<double> v0;
  std::vector<double> B;

  // Segments of the physical boundary adjacent to this block.
  std::vector<double> v0_bottom;
  std::vector<double> v0_top;
  std::vector<double> v0_left;
  std::vector<double> v0_right;

  // Map global interior coordinates in this block to a local vector index.
  std::size_t index(std::size_t i, std::size_t j) const {
    return (i - 1 - offset_x) + (j - 1 - offset_y) * nx;
  }

  double v0_at(std::size_t i, std::size_t j) const {
    if (i == 0)
      return v0_left[j - offset_y];
    if (i == M)
      return v0_right[j - offset_y];
    if (j == 0)
      return v0_bottom[i - offset_x];
    if (j == N)
      return v0_top[i - offset_x];
    return v0[index(i, j)];
  }

  std::size_t size() const { return Q.size(); }

private:
  std::size_t nx, ny, offset_x, offset_y;

  void build(const Grid2D &grid, const Domain &domain);
};
