#include "geometry.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <numeric>

namespace {

bool insideUnitCircle(const Point &point) {
  return point.x * point.x + point.y * point.y < 1;
}

void intersectSegmentAndUnitCircle(const Point &start, const Point &end,
                                   std::vector<Point> &poly) {
  double a = (end - start).lengthSquared();
  double b = start * (end - start) * 2;
  double c = start.lengthSquared() - 1;

  for (const auto &root : utils::solveQuadraticInUnitSegment(a, b, c)) {
    Point new_point = start + (end - start) * root;
    poly.push_back(new_point);
  }
}

double gaussShoelaceArea(const std::vector<Point> &points) {
  int n = points.size();

  if (n < 3)
    return 0.0;

  double area = 0.0;

  for (int i = 0; i < n; ++i) {
    int j = (i + 1) % n;

    area += points[i].x * points[j].y;
    area -= points[j].x * points[i].y;
  }

  return std::abs(area) / 2.0;
}

void dedup(std::vector<Point> &poly) {
  std::vector<Point> out;
  out.reserve(poly.size());
  for (const auto &p : poly) {
    bool dup = false;
    for (const auto &r : out) {
      const double dx = r.x - p.x;
      const double dy = r.y - p.y;
      if (dx * dx + dy * dy < utils::kEpsilon) {
        dup = true;
        break;
      }
    }
    if (!dup)
      out.push_back(p);
  }
  poly.swap(out);
}

} // namespace

bool Domain::contains(const Point &point) const {
  return point.y > 0 && insideUnitCircle(point);
}

double Domain::intersectionArea(const Box &cell) const {
  const double xmin = std::max(cell.bottom_left.x, _xmin);
  const double xmax = std::min(cell.top_right.x, _xmax);
  const double ymin = std::max(cell.bottom_left.y, _ymin);
  const double ymax = std::min(cell.top_right.y, _ymax);

  if (xmax <= xmin || ymax <= ymin) {
    return 0.0;
  }

  bool all_points_inside =
      (Domain::contains({xmin, ymin}) && Domain::contains({xmin, ymax}) &&
       Domain::contains({xmax, ymax}) && Domain::contains({xmax, ymin}));

  if (all_points_inside) {
    return (xmax - xmin) * (ymax - ymin);
  }

  std::vector<Point> poly;
  Box clamped_cell = {{xmin, ymin}, {xmax, ymax}};
  std::vector<Point> vertices = clamped_cell.getVertices();
  std::copy_if(vertices.begin(), vertices.end(), std::back_inserter(poly),
               [](Point point) { return insideUnitCircle(point); });

  auto vertices_size = vertices.size();
  for (std::size_t i = 0; i < vertices_size - 1; ++i) {
    intersectSegmentAndUnitCircle(vertices[i], vertices[i + 1], poly);
  }
  intersectSegmentAndUnitCircle(vertices[0], vertices[vertices_size - 1], poly);

  dedup(poly);
  if (poly.size() < 3)
    return 0.0;

  auto center = std::accumulate(poly.begin(), poly.end(), Point{0, 0});
  center = center * (1.0 / static_cast<double>(poly.size()));
  std::sort(poly.begin(), poly.end(),
            [&center](const Point &point, const Point &other_point) {
              return (point - center).getAngle() <
                     (other_point - center).getAngle();
            });
  return gaussShoelaceArea(poly);
}