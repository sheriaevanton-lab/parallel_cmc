#include "backend/backend_factory.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"
#include "problem_data/problem_data.hpp"
#include "solver/solver.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <optional>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int rank = 0;
  int num_procs;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

  try {
    const auto parsed = utils::parseArgs(argc, argv);
    if (!parsed) {
      if (rank == 0)
        utils::printUsage();
      MPI_Finalize();
      return 0;
    }
    const auto &config = *parsed;
    const Grid2D grid(config);
    const Domain domain;

    std::optional<Decomposition> decomposition;
    std::optional<LocalGeometry> local_geometry;
    if (config.backend == BackendKind::MPI) {
      decomposition = make_decomposition(grid.M, grid.N, num_procs);
      local_geometry =
          make_local_geometry(*decomposition, rank, grid.M, grid.N);
    }

    const auto problem = local_geometry
                             ? ProblemData(grid, domain, *local_geometry)
                             : ProblemData(grid, domain);
    const auto backend =
        makeBackend(config.backend, problem,
                    local_geometry ? &*local_geometry : nullptr, MPI_COMM_WORLD);
    const auto solver = ConjugateGradient(*backend);

    std::vector<double> w(problem.size(), 0.0);
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

    if (decomposition && num_procs > 1)
      w = gatherSolution(w, *decomposition, grid, MPI_COMM_WORLD);

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
