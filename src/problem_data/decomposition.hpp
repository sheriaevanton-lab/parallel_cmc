#pragma once
#include <cstddef>

struct Decomposition {
  std::size_t px, py;
};

Decomposition make_decomposition(std::size_t M, std::size_t N, int num_proc);

struct LocalGeometry {
  static constexpr int no_neighbor = -1;

  int rank;
  std::size_t lx, ly;
  // Number of interior nodes before this block on each axis.
  std::size_t offset_x, offset_y;
  int left, right, bottom, top;
};

LocalGeometry make_local_geometry(const Decomposition &d, int rank,
                                  std::size_t M, std::size_t N);
