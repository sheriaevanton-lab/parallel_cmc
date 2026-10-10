#pragma once

#include "backend/backend.hpp"
#include <vector>

class ConjugateGradient {
public:
  struct Result {
    std::size_t iterations;
    double residual;
    bool converged;
  };

  explicit ConjugateGradient(const Backend &backend) : backend(backend) {}

  Result solve(const std::vector<double> &B, std::vector<double> &w,
               double delta, std::size_t max_iter) const;

private:
  const Backend &backend;
};
