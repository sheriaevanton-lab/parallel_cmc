#include "utils.hpp"
#include "geometry/geometry.hpp"
#include "geometry/grid.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace utils {

namespace {
  constexpr std::size_t kM = 20;
  constexpr std::size_t kN = 20;
  constexpr double kDefaultXMin = -1.1;
  constexpr double kDefaultXMax = 1.1;
  constexpr double kDefaultYMin = -0.1;
  constexpr double kDefaultYMax = 1.1;
  constexpr double kDelta = 1e-5;
  constexpr double kMaxIter = 1000;
  constexpr int kPrecision = 17;
  const auto kDefaultBackend = BackendKind::Sequential;
  const auto kDefaultPath = "./solution.csv"; 
} // namespace

std::vector<double> solveQuadraticInUnitSegment(double a, double b, double c) {
  std::vector<double> roots;

  auto addRoot = [&](double x) {
    if (x < 0.0 && x >= -kEpsilon)
      x = 0.0;
    if (x > 1.0 && x <= 1.0 + kEpsilon)
      x = 1.0;

    if (x < -kEpsilon || x > 1.0 + kEpsilon)
      return;

    for (double r : roots) {
      if (std::fabs(r - x) <= kEpsilon)
        return;
    }

    roots.push_back(x);
  };

  double D = b * b - 4.0 * a * c;

  if (D < -kEpsilon)
    return roots;

  if (D < 0.0)
    D = 0.0;

  if (D == 0.0) {
    addRoot(-b / (2.0 * a));
    return roots;
  }

  double sqrtD = std::sqrt(D);

  double q = -0.5 * (b + std::copysign(sqrtD, b));

  addRoot(q / a);
  addRoot(c / q);

  return roots;
}

void fail(const std::string &msg) { throw std::runtime_error(msg); }

Config parseArgs(int argc, char *argv[]) {
  Config config;
  config.M = kM;
  config.N = kN;
  config.xmin = kDefaultXMin;
  config.xmax = kDefaultXMax;
  config.ymin = kDefaultYMin;
  config.ymax = kDefaultYMax;
  config.delta = kDelta;
  config.max_iter = kMaxIter;
  config.backend = kDefaultBackend;
  config.output_path = kDefaultPath;

  for (int i = 1; i < argc; ++i) {
    std::string key = argv[i];
    auto next = [&]() -> std::string {
      if (i + 1 >= argc)
        fail("missing value for " + key);
      return argv[++i];
    };

    if (key == "--backend" || key == "-b") {
      auto k = parseBackendKind(next());
      if (!k.has_value())
        fail("unknown backend type");
      config.backend = *k;
    } else if (key == "--M")
      config.M = std::stoi(next());
    else if (key == "--N")
      config.N = std::stoi(next());
    else if (key == "--xmin")
      config.xmin = std::stod(next());
    else if (key == "--xmax")
      config.xmax = std::stod(next());
    else if (key == "--ymin")
      config.ymin = std::stod(next());
    else if (key == "--ymax")
      config.ymax = std::stod(next());
    else if (key == "--delta")
      config.delta = std::stod(next());
    else if (key == "--max-iter")
      config.max_iter = std::stoi(next());
    else if (key == "--output" || key == "-o")
      config.output_path = next();
    else if (key == "--help" || key == "-h") {
      std::cout << "usage: prog [options]\n"
                   "  -b, --backend seq|omp|mpi|hybrid\n"
                   "      --M <int>   --N <int>\n"
                   "      --xmin --xmax --ymin --ymax <double>\n"
                   "      --delta <double>   --max-iter <int>\n"
                   "  -o, --output <file>\n";
      std::exit(0);
    } else
      fail("unknown option: " + key);
  }

  return config;
}

void saveSolutionCsv(const std::string &path, const std::vector<double> &w,
                     const Grid2D &grid, const Domain &domain) {
  std::ofstream out(path);
  if (!out)
    fail("cannot open " + path);

  out << std::setprecision(kPrecision);
  out << "x,y,w\n";

  const std::size_t nx = grid.M - 1;

  for (std::size_t j = 0; j <= grid.N; ++j) {
    for (std::size_t i = 0; i <= grid.M; ++i) {
      double value;
      if (i == 0 || i == grid.M || j == 0 || j == grid.N) {
        value = domain.v0({grid.x(i), grid.y(j)});
      } else {
        const std::size_t k = (i - 1) + (j - 1) * nx;
        value = w[k];
      }
      out << grid.x(i) << ',' << grid.y(j) << ',' << value << '\n';
    }
  }

  out.flush();
  if (!out)
    throw std::runtime_error("write failed: " + path);
}

} // namespace utils