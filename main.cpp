#include "backend/sequential.hpp"
#include "config.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"
#include "problem_data/problem_data.hpp"
#include "solver/solver.hpp"
#include "utils/utils.hpp"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
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
    }
    const auto solver = ConjugateGradient(*backend);

    std::vector<double> w((problem.M - 1) * (problem.N - 1));
    const auto result =
        solver.solve(problem.B, w, config.delta, config.max_iter);
    std::cout << (result.converged ? "converged" : "failed to converge")
              << std::endl
              << "delta_w_norm: " << result.residual
              << " iterations: " << result.iterations << std::endl;
    utils::saveSolutionCsv(config.output_path, w, grid, domain);
  } catch (const std::exception &e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}