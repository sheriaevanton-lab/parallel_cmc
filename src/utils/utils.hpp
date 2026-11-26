#pragma once

#include "config.hpp"
#include <optional>
#include <vector>

struct Grid2D;
class Domain;

namespace utils {
constexpr double kEpsilon = 1e-15;
std::vector<double> solveQuadraticInUnitSegment(double a, double b, double c);

std::optional<Config> parseArgs(int argc, char *argv[]);
void printUsage();

void saveSolutionCsv(const std::string &path, const std::vector<double> &w,
                     const Grid2D &grid, const Domain &domain);

} // namespace utils
