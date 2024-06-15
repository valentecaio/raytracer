#pragma once

#include "utils/common.hpp"

namespace raytracer {

// The Ray class represents a ray in 3D space.
class Ray {
  public:
    Ray() {}
    Ray(const Point& _origin, const Vec& _direction)
      : orig(_origin), dir(glm::normalize(_direction)) {}

    // already normalized
    Vec direction() const { return dir; }
    Point origin() const { return orig; }

    Point at(double t) const {
      return orig + t*dir;
    }

  private:
    Point orig; // origin
    Vec dir;    // direction, normalized
};

} // namespace raytracer
