#include "backend/backend_factory.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"
#include "problem_data/problem_data.hpp"
#include "solver/solver.hpp"
#include "utils/utils.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int rank = 0;
  int num_procs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  try {
    auto config = utils::parseArgs(argc, argv);
    Grid2D grid(config);
    Domain domain;

    std::unique_ptr<ProblemData> problem;
    std::unique_ptr<Backend> backend;
    std::unique_ptr<Decomposition> dec;
    std::unique_ptr<LocalGeometry> lg;

    if (!config.backend.has_value()) {
      throw std::runtime_error("unknown backend type");
    } else if (*config.backend == BackendKind::MPI) {
      dec = std::make_unique<Decomposition>(
          make_decomposition(config.M, config.N, num_procs));
      lg = std::make_unique<LocalGeometry>(
          make_local_geometry(*dec, rank, config.M, config.N));
    } else {
      problem = std::make_unique<ProblemData>(grid, domain);
    }

    backend = makeBackend(config.backend, *problem, dec.get(), lg.get(),
                          MPI_COMM_WORLD);
    const auto solver = ConjugateGradient(*backend);

    std::vector<double> w(problem->size(), 0);
    MPI_Barrier(MPI_COMM_WORLD);
    const double t0 = MPI_Wtime();
    const auto result =
        solver.solve(problem->B, w, config.delta, config.max_iter);
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
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  MPI_Finalize();
  return 0;
}