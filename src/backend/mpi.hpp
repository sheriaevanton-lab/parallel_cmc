#pragma once

#include "backend.hpp"
#include "problem_data/decomposition.hpp"
#include "problem_data/problem_data.hpp"
#include <mpi.h>

class MPIBackend : public Backend {
  const ProblemData &problem;
  LocalGeometry geometry;
  MPI_Comm comm;

  mutable std::vector<double> left_halo, right_halo, bottom_halo, top_halo;
  mutable std::vector<double> column;

public:
  explicit MPIBackend(const ProblemData &p, const LocalGeometry &lg,
                      MPI_Comm comm);

  double dot(const std::vector<double> &u,
             const std::vector<double> &v) const override;

  void axpy(double a, const std::vector<double> &x,
            std::vector<double> &y) const override;

  void applyOperator(const std::vector<double> &w,
                     std::vector<double> &Aw) const override;

  void applyPreconditioner(const std::vector<double> &r,
                           std::vector<double> &z) const override;
};

// Reassemble the interior in global row order on rank 0.
std::vector<double> gatherSolution(const std::vector<double> &local,
                                   const Decomposition &decomposition,
                                   const Grid2D &grid, MPI_Comm comm);
