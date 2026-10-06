#include "config.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include <iostream>

using namespace std;

int main(void) {
  Config config;
  config.M = 20;
  config.N = 20;
  config.xmin = -1.1;
  config.xmax = 1.1;
  config.ymin = -0.1;
  config.ymax = 1.1;

  Grid2D grid(config);
  Domain domain;

  double total = 0.0;

  for (int i = 0; i < grid.M; ++i) {
    for (int j = 0; j < grid.N; ++j) {
      Box cell = {{grid.x(i), grid.y(j)}, {grid.x(i + 1), grid.y(j + 1)}};
      total += domain.intersectionArea(cell);
    }
  }

  std::cout << "S_ij sum = " << total << std::endl;
  std::cout << "pi/2 = " << 0.5 * M_PI << std::endl;

  return 0;
}