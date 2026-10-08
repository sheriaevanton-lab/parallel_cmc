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
    case BackendKind::OpenMP:
      backend = std::make_unique<OpenMPBackend>(problem);
      break;
    }
    const auto solver = ConjugateGradient(*backend);

    std::vector<double> w((problem.M - 1) * (problem.N - 1));
    const auto t0 = std::chrono::steady_clock::now();
    const auto result =
        solver.solve(problem.B, w, config.delta, config.max_iter);
    const auto t1 = std::chrono::steady_clock::now();

    const double seconds = std::chrono::duration<double>(t1 - t0).count();
    std::cout << (result.converged ? "converged" : "failed to converge")
              << std::endl
              << "delta_w_norm: " << result.residual << std::endl
              << "iterations: " << result.iterations << std::endl
              << "time: " << seconds << std::endl;
    utils::saveSolutionCsv(config.output_path, w, grid, domain);
  } catch (const std::exception &e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}