#include "sequential.hpp"

SequentialBackend::SequentialBackend(const ProblemData &p) : problem(p) {}

double SequentialBackend::dot(const std::vector<double> &u,
                              const std::vector<double> &v) const {
  const double weight = problem.h1 * problem.h2;
  double s = 0.0;
  for (std::size_t k = 0; k < u.size(); ++k)
    s += u[k] * v[k];
  return weight * s;
}

void SequentialBackend::axpy(double a, const std::vector<double> &x,
                             std::vector<double> &y) const {
  for (std::size_t i = 0; i < x.size(); ++i) {
    y[i] += a * x[i];
  }
}

void SequentialBackend::applyOperator(const std::vector<double> &w,
                                      std::vector<double> &Aw) const {
  const std::size_t M = problem.M;
  const std::size_t N = problem.N;
  const double inv_h1_2 = 1.0 / (problem.h1 * problem.h1);
  const double inv_h2_2 = 1.0 / (problem.h2 * problem.h2);

  for (std::size_t j = 1; j <= N - 1; ++j) {
    for (std::size_t i = 1; i <= M - 1; ++i) {
      const std::size_t k = problem.index(i, j);

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

void SequentialBackend::applyPreconditioner(const std::vector<double> &r,
                                            std::vector<double> &z) const {
  const double inv_h1_2 = 1.0 / (problem.h1 * problem.h1);
  const double inv_h2_2 = 1.0 / (problem.h2 * problem.h2);

  for (std::size_t k = 0; k < r.size(); ++k) {
    const double diag = 2.0 * inv_h1_2 + 2.0 * inv_h2_2 + problem.Q[k];
    z[k] = r[k] / diag;
  }
}
