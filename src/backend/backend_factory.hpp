#pragma once
#include "mpi.hpp"
#include "openmp.hpp"
#include "sequential.hpp"
#include <memory>
#include <mpi.h>
#include <optional>
#include <stdexcept>
#include <string>

inline std::unique_ptr<Backend> makeBackend(BackendKind kind,
                                            ProblemData &problem,
                                            LocalGeometry *lg, MPI_Comm comm) {
  switch (kind) {
  case BackendKind::Sequential:
    return std::make_unique<SequentialBackend>(problem);
  case BackendKind::OpenMP:
    return std::make_unique<OpenMPBackend>(problem);
  case BackendKind::MPI:
    return std::make_unique<MPIBackend>(problem, *lg, comm);
  }
  throw std::runtime_error("unknown backend type");
}
