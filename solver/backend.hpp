#pragma once

#include <cmath>
#include <vector>

class Backend {
public:
  virtual ~Backend() = default;

  virtual double dot(const std::vector<double> &u,
                     const std::vector<double> &v) const = 0;

  virtual void axpy(double a, const std::vector<double> &x,
                    std::vector<double> &y) const = 0;

  virtual void applyOperator(const std::vector<double> &w,
                             std::vector<double> &Aw) const = 0;

  virtual void applyPreconditioner(const std::vector<double> &r,
                                   std::vector<double> &z) const = 0;

  double norm(const std::vector<double> &u) const {
    return std::sqrt(dot(u, u));
  }
};