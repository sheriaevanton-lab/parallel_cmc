#pragma once
#include "config.hpp"
#include <cmath>
#include <vector>

struct Point {
  double x, y;

  double operator*(const Point &other) const {
    return x * other.x + y * other.y;
  };
  Point operator*(double value) const { return Point{value * x, value * y}; };
  Point operator+(const Point &other) const {
    return Point{x + other.x, y + other.y};
  };
  Point operator-(const Point &other) const { return *this + other * (-1.0); };
  double lengthSquared() const { return *this * *this; };
  double getAngle() const { return std::atan2(y, x); }
};

struct Box {
  Point bottom_left, top_right;

  std::vector<Point> getVertices() const {
    return {bottom_left, Point{bottom_left.x, top_right.y}, top_right,
            Point{top_right.x, bottom_left.y}};
  };
};

class Domain {
public:
  bool contains(const Point &point) const;
  double intersectionArea(const Box &cell) const;
  double phi(const Point &point) const { return std::sin(point.x + point.y); }
  double v0(const Point &point) const { return std::sin(point.x + point.y); }

private:
  double _xmin = -1.0, _xmax = 1.0, _ymin = 0.0, _ymax = 1.0;
};