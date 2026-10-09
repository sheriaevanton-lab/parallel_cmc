#pragma once
#include <string>

struct Decomposition {
  std::size_t px, py;
  std::size_t P_eff;
  std::size_t local_nx, local_ny;
  std::size_t offset_x, offset_y;
};

Decomposition make_decomposition(std::size_t M, std::size_t N, int size);

struct LocalGeometry {
  int rank;
  std::size_t lx, ly;
  std::size_t offset_x, offset_y;
  std::size_t left, right, bottom, top;
};

LocalGeometry make_local_geometry(const Decomposition &d, int rank,
                                  std::size_t M, std::size_t N);