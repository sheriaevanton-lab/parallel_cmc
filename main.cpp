#include "backend/mpi.hpp"
#include "backend/openmp.hpp"
#include "backend/sequential.hpp"
#include "config.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"
#include "problem_data/problem_data.hpp"
#include "solver/solver.hpp"
#include "utils/utils.hpp"
#include <chrono>
#include <iostream>
#include <memory>
#include <mpi.h>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int exec_code = 0;
  try {
    auto config = utils::parseArgs(argc, argv);
    Grid2D grid(config);
    Domain domain;
    ProblemData problem(grid, domain);

    std::unique_ptr<Backend> backend;

    switch (config.backend) {
    case BackendKind::Sequential:
      backend = std::make_unique<SequentialBackend>(problem);
      break;
    case BackendKind::OpenMP:
      backend = std::make_unique<OpenMPBackend>(problem);
      break;
    case BackendKind::MPI:
      backend = std::make_unique<MPIBackend>(problem);
      break;
    }
    const auto solver = ConjugateGradient(*backend);

    std::vector<double> w(problem.size(), 0);
    MPI_Barrier(MPI_COMM_WORLD);
    const double t0 = MPI_Wtime();
    const auto result =
        solver.solve(problem.B, w, config.delta, config.max_iter);
    MPI_Barrier(MPI_COMM_WORLD);
    const double t1 = MPI_Wtime();
    const double local_elapsed = t1 - t0;
    double max_elapsed = 0.0;
    MPI_Reduce(&local_elapsed, &max_elapsed, 1, MPI_DOUBLE, MPI_MAX, 0,
               MPI_COMM_WORLD);

    if (rank == 0) {
      std::cout << (result.converged ? "converged" : "failed to converge")
                << std::endl
                << "delta_w_norm: " << result.residual << std::endl
                << "iterations: " << result.iterations << std::endl
                << "time: " << max_elapsed << std::endl;
      utils::saveSolutionCsv(config.output_path, w, grid, domain);
    }
  } catch (const std::exception &e) {
    if (rank == 0) {
      std::cerr << "error: " << e.what() << "\n";
    }
    exec_code = 1;
  }
  MPI_Finalize();
  return exec_code;
}