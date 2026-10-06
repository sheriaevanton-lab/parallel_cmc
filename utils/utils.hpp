#pragma once
#include "problem_data.hpp"
#include <vector>

namespace utils {
constexpr double kEpsilon = 1e-15;
std::vector<double> solveQuadraticInUnitSegment(double a, double b, double c);
void applyOperator(const ProblemData &problem, const std::vector<double> &w,
                   std::vector<double> &Aw);
} // namespace utils