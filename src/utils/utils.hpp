#pragma once

#include "config.hpp"
#include <vector>

class Grid2D;
struct Domain;

namespace utils {
constexpr double kEpsilon = 1e-15;
std::vector<double> solveQuadraticInUnitSegment(double a, double b, double c);

Config parseArgs(int argv, char *argc[]);

void saveSolutionCsv(const std::string &path, const std::vector<double> &w,
                     const Grid2D &grid, const Domain &domain);

} // namespace utils