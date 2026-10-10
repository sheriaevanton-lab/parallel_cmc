#pragma once
#include <string>

struct Decomposition {
  std::size_t px, py;
};

Decomposition make_decomposition(std::size_t M, std::size_t N, int num_proc);

struct LocalGeometry {
  int rank;
  std::size_t lx, ly;
  std::size_t offset_x, offset_y;
  std::size_t left, right, bottom, top;
};

LocalGeometry make_local_geometry(const Decomposition &d, int rank,
                                  std::size_t M, std::size_t N);
