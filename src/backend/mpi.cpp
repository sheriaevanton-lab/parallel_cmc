#include "mpi.hpp"
#include <algorithm>

namespace {
enum MessageTag { ToLeft, ToRight, ToBottom, ToTop, Solution };

int mpiRank(int rank) {
  return rank == LocalGeometry::no_neighbor ? MPI_PROC_NULL : rank;
}
} // namespace

MPIBackend::MPIBackend(const ProblemData &p, const LocalGeometry &lg,
                       MPI_Comm comm)
    : problem(p), geometry(lg), comm(comm), left_halo(lg.ly), right_halo(lg.ly),
      bottom_halo(lg.lx), top_halo(lg.lx), column(lg.ly) {}

double MPIBackend::dot(const std::vector<double> &u,
                       const std::vector<double> &v) const {
  double local_sum = 0.0;
  for (std::size_t k = 0; k < u.size(); ++k)
    local_sum += u[k] * v[k];
  double global_sum = 0.0;
  MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, comm);
  return (problem.h1 * problem.h2) * global_sum;
}

void MPIBackend::axpy(double a, const std::vector<double> &x,
                      std::vector<double> &y) const {
  for (std::size_t i = 0; i < x.size(); ++i) {
    y[i] += a * x[i];
  }
}

void MPIBackend::applyOperator(const std::vector<double> &w,
                               std::vector<double> &Aw) const {
  const std::size_t lx = geometry.lx;
  const std::size_t ly = geometry.ly;
  const int row_count = static_cast<int>(lx);
  const int column_count = static_cast<int>(ly);
  const int left = mpiRank(geometry.left);
  const int right = mpiRank(geometry.right);
  const int bottom = mpiRank(geometry.bottom);
  const int top = mpiRank(geometry.top);

  if (left != MPI_PROC_NULL || right != MPI_PROC_NULL) {
    for (std::size_t j = 0; j < ly; ++j)
      column[j] = w[j * lx];
    MPI_Sendrecv(column.data(), column_count, MPI_DOUBLE, left, ToLeft,
                 right_halo.data(), column_count, MPI_DOUBLE, right, ToLeft,
                 comm, MPI_STATUS_IGNORE);

    for (std::size_t j = 0; j < ly; ++j)
      column[j] = w[j * lx + lx - 1];
    MPI_Sendrecv(column.data(), column_count, MPI_DOUBLE, right, ToRight,
                 left_halo.data(), column_count, MPI_DOUBLE, left, ToRight,
                 comm, MPI_STATUS_IGNORE);
  }
  if (bottom != MPI_PROC_NULL || top != MPI_PROC_NULL) {
    MPI_Sendrecv(w.data(), row_count, MPI_DOUBLE, bottom, ToBottom,
                 top_halo.data(), row_count, MPI_DOUBLE, top, ToBottom, comm,
                 MPI_STATUS_IGNORE);
    MPI_Sendrecv(w.data() + (ly - 1) * lx, row_count, MPI_DOUBLE, top, ToTop,
                 bottom_halo.data(), row_count, MPI_DOUBLE, bottom, ToTop, comm,
                 MPI_STATUS_IGNORE);
  }

  const double inv_h1_2 = 1.0 / (problem.h1 * problem.h1);
  const double inv_h2_2 = 1.0 / (problem.h2 * problem.h2);

  for (std::size_t j = 0; j < ly; ++j) {
    for (std::size_t i = 0; i < lx; ++i) {
      const std::size_t k = j * lx + i;

      const double wc = w[k];
      const double wl = i > 0 ? w[k - 1] : left_halo[j];
      const double wr = i + 1 < lx ? w[k + 1] : right_halo[j];
      const double wd = j > 0 ? w[k - lx] : bottom_halo[i];
      const double wu = j + 1 < ly ? w[k + lx] : top_halo[i];

      Aw[k] = -(wl - 2.0 * wc + wr) * inv_h1_2 -
              (wd - 2.0 * wc + wu) * inv_h2_2 + problem.Q[k] * wc;
    }
  }
}

void MPIBackend::applyPreconditioner(const std::vector<double> &r,
                                     std::vector<double> &z) const {
  const double inv_h1_2 = 1.0 / (problem.h1 * problem.h1);
  const double inv_h2_2 = 1.0 / (problem.h2 * problem.h2);

  for (std::size_t k = 0; k < r.size(); ++k) {
    const double diag = 2.0 * inv_h1_2 + 2.0 * inv_h2_2 + problem.Q[k];
    z[k] = r[k] / diag;
  }
}

std::vector<double> gatherSolution(const std::vector<double> &local,
                                   const Decomposition &decomposition,
                                   const Grid2D &grid, MPI_Comm comm) {
  int rank;
  MPI_Comm_rank(comm, &rank);
  const auto block = make_local_geometry(decomposition, rank, grid.M, grid.N);

  if (rank != 0) {
    const int width = static_cast<int>(block.lx);
    MPI_Datatype tile;
    MPI_Type_vector(static_cast<int>(block.ly), width, width, MPI_DOUBLE,
                    &tile);
    MPI_Type_commit(&tile);
    MPI_Send(local.data(), 1, tile, 0, Solution, comm);
    MPI_Type_free(&tile);
    return {};
  }

  const std::size_t nx = grid.M - 1;
  std::vector<double> solution(nx * (grid.N - 1));
  for (std::size_t j = 0; j < block.ly; ++j) {
    std::copy_n(local.data() + j * block.lx, block.lx,
                solution.data() + j * nx);
  }

  int size;
  MPI_Comm_size(comm, &size);
  for (int source = 1; source < size; ++source) {
    const auto remote =
        make_local_geometry(decomposition, source, grid.M, grid.N);
    MPI_Datatype tile;
    MPI_Type_vector(static_cast<int>(remote.ly), static_cast<int>(remote.lx),
                    static_cast<int>(nx), MPI_DOUBLE, &tile);
    MPI_Type_commit(&tile);
    MPI_Recv(solution.data() + remote.offset_y * nx + remote.offset_x, 1, tile,
             source, Solution, comm, MPI_STATUS_IGNORE);
    MPI_Type_free(&tile);
  }
  return solution;
}

