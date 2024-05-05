#ifndef RAY_H
#define RAY_H

#include "utils/common.hpp"

namespace raytracer {

// The Ray class represents a ray in 3D space.
class Ray {
  public:
    Ray() {}
    Ray(const Point& _origin, const Vec& _direction)
      : orig(_origin), dir(glm::normalize(_direction)) {}

    Point origin() const { return orig; }
    Vec direction() const { return dir; }

    Point at(double t) const {
      return orig + t*dir;
    }

  private:
    Point orig; // origin
    Vec dir;    // direction, normalized
};

} // namespace raytracer

#endif // RAY_H
