#include "solver.hpp"
// #include <cstdio>

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

  double dw_norm = 0;

  // const double B_norm = backend.norm(B);

  for (std::size_t iter = 1; iter < max_iter; ++iter) {
    backend.applyOperator(p, Ap);
    const double alpha = rz / backend.dot(Ap, p);

    backend.axpy(alpha, p, w);
    backend.axpy(-alpha, Ap, r);

    // const double r_rel  = backend.norm(r) / B_norm;
    // const double dw_rel = std::abs(alpha) * backend.norm(p);

    // std::printf("iter %zu  |r|/|B| = %g  |dw| = %g\n",
    //             iter, r_rel, dw_rel);

    // dw_norm = alpha * backend.norm(p);
    if (dw_norm < delta) {
      return {iter, dw_norm, true};
    }

    backend.applyPreconditioner(r, z);
    const double rz_new = backend.dot(z, r);
    const double beta = rz_new / rz;
    backend.axpy(beta, p, z);
    p.swap(z);
    rz = rz_new;
  }

  return {max_iter, dw_norm, false};
}