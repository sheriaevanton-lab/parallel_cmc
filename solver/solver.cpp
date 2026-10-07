#include "solver.hpp"

ConjugateGradient::Result ConjugateGradient::solve(const std::vector<double> &B,
                                                   std::vector<double> &w,
                                                   double delta,
                                                   std::size_t max_iter) const {
  const std::size_t n = w.size();
  std::vector<double> r(n), z(n), p(n), Ap(n);

  backend.applyOperator(w, Ap);
  for (std::size_t k = 0; k < n; ++k) {
    r[k] = B[k] - Ap[k];
  }
  backend.applyPreconditioner(r, z);
  p = z;

  double rz = backend.dot(r, z);

  double r_norm = 0;

  for (std::size_t iter = 1; iter < max_iter; ++iter) {
    backend.applyOperator(p, Ap);
    const double alpha = rz / backend.dot(Ap, p);

    backend.axpy(alpha, p, w);
    backend.axpy(-alpha, Ap, r);

    r_norm = backend.norm(r);
    if (r_norm < delta) {
      return {iter, r_norm, true};
    }

    backend.applyPreconditioner(r, z);
    const double rz_new = backend.dot(z, r);
    const double beta = rz_new / rz;
    backend.axpy(beta, p, z);
    p.swap(z);
    rz = rz_new;
  }

  return {max_iter, r_norm, false};
}