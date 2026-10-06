#include "utils.hpp"
#include <cmath>

using namespace utils;

std::vector<double> utils::solveQuadraticInUnitSegment(double a, double b,
                                                       double c) {
  std::vector<double> roots;

  auto addRoot = [&](double x) {
    if (x < 0.0 && x >= -kEpsilon)
      x = 0.0;
    if (x > 1.0 && x <= 1.0 + kEpsilon)
      x = 1.0;

    if (x < -kEpsilon || x > 1.0 + kEpsilon)
      return;

    for (double r : roots) {
      if (std::fabs(r - x) <= kEpsilon)
        return;
    }

    roots.push_back(x);
  };

  double D = b * b - 4.0 * a * c;

  if (D < -kEpsilon)
    return roots;

  if (D < 0.0)
    D = 0.0;

  if (D == 0.0) {
    addRoot(-b / (2.0 * a));
    return roots;
  }

  double sqrtD = std::sqrt(D);

  double q = -0.5 * (b + std::copysign(sqrtD, b));

  addRoot(q / a);
  addRoot(c / q);

  return roots;
}