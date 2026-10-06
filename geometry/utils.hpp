#pragma once
#include <vector>

namespace utils {
constexpr double kEpsilon = 1e-15;
std::vector<double> solveQuadraticInUnitSegment(double a, double b, double c);
} // namespace utils