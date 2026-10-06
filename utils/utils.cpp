#include "utils.hpp"
#include <cmath>

namespace utils {

std::vector<double> solveQuadraticInUnitSegment(double a, double b, double c) {
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

void applyOperator(const ProblemData &problem, const std::vector<double> &w,
                   std::vector<double> &Aw) {
  const int M = problem.M;
  const int N = problem.N;
  const double inv_h1_2 = 1.0 / (problem.h1 * problem.h1);
  const double inv_h2_2 = 1.0 / (problem.h2 * problem.h2);

  for (int j = 1; j <= N - 1; ++j) {
    for (int i = 1; i <= M - 1; ++i) {
      const int k = problem.index(i, j);

      const double wc = w[k];

      const double wl =
          (i > 1) ? w[problem.index(i - 1, j)] : problem.v0_at(i - 1, j);

      const double wr =
          (i < M - 1) ? w[problem.index(i + 1, j)] : problem.v0_at(i + 1, j);

      const double wd =
          (j > 1) ? w[problem.index(i, j - 1)] : problem.v0_at(i, j - 1);

      const double wu =
          (j < N - 1) ? w[problem.index(i, j + 1)] : problem.v0_at(i, j + 1);

      Aw[k] = -(wl - 2.0 * wc + wr) * inv_h1_2 -
              (wd - 2.0 * wc + wu) * inv_h2_2 + problem.Q[k] * wc;
    }
  }
}

} // namespace utils