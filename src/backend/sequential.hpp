#pragma once

#include "backend.hpp"
#include "problem_data/problem_data.hpp"

class SequentialBackend : public Backend {
  const ProblemData &problem;

public:
  explicit SequentialBackend(const ProblemData &p);

  double dot(const std::vector<double> &u,
             const std::vector<double> &v) const override;

  void axpy(double a, const std::vector<double> &x,
            std::vector<double> &y) const override;

  void applyOperator(const std::vector<double> &w,
                     std::vector<double> &Aw) const override;

  void applyPreconditioner(const std::vector<double> &r,
                           std::vector<double> &z) const override;
};