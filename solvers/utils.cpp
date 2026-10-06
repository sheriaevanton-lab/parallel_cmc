#include "utils.hpp"

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